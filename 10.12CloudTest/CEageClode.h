#include "CEageCloude.h"
#include <iostream>
#include "gdal_priv.h"
EageCloude::EageCloude()
{
	string m_strInputRFileName=string();
	Float32 adfRmaxmin[2]={0.0,0.0};
	m_bInit = false;
}
EageCloude::~EageCloude()
{
}
//初始化
bool EageCloude::Initilization(const string & strInputRFileName)
{
	if (strInputRFileName.empty())
	{
		std::cout<<"文件名为空！";
		system("pause");
		return false;
	}
	m_strInputRFileName=strInputRFileName;
	//注册所有驱动
	GDALAllRegister();
	GDALDataset *pDatasetR=(GDALDataset*)GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	if (pDatasetR==0)
	{
		m_strInputRFileName.erase(m_strInputRFileName.begin(),m_strInputRFileName.end());
		std::cout<<"文件打开错误！"<<std::endl;
		system("pause");
		return false;
	}
	GDALClose(pDatasetR);
	m_bInit=true;
	return true;
}
//执行
bool EageCloude::Execute(Float32 *pRBuf,const string & strOutputHFileName)
{
	if (strOutputHFileName.empty())
	{
		std::cout<<"输出文件名为空！";
		system("pause");
		return false;
	}
	m_strOutputHFileName=strOutputHFileName;
	if (m_bInit==false)
	{
		std::cout<<"未执行初始化！"<<std::endl;
		system("pause");
		return false;
	}
	GDALAllRegister();
	GDALDataset * pSrcDatasetR = (GDALDataset *) GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	//获取影像宽度
	int nWidth, nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();

	double HRGeoTransForm[6];
	pSrcDatasetR->GetGeoTransform(HRGeoTransForm);

	//创建目标文件
	const char *pszFormat = "GTiff";
	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		return false;
	}	
	int nBandCount = pSrcDatasetR->GetRasterCount();
	GDALDataset *pDrcDatasetH = poDriver->Create(m_strOutputHFileName.c_str(), nWidth, nHeight, nBandCount, GDT_Float32, NULL);
	if (pDrcDatasetH == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		return false;
	}
	const char *  pszSRS_WKT =  pSrcDatasetR->GetProjectionRef();	
	pDrcDatasetH->SetGeoTransform(HRGeoTransForm);
	pDrcDatasetH->SetProjection(pszSRS_WKT);
	pDrcDatasetH->SetMetadata(pSrcDatasetR->GetMetadata());
	GDALRasterBand * pDSrcBandR = pSrcDatasetR->GetRasterBand(1);
	//存放整幅影像的数组，读取使用
	Float32 *pDsdBuf=new Float32 [nWidth*nHeight];
	//读取整幅影像->pDstBuf
	if (pDSrcBandR->RasterIO(GF_Read,0,0,nWidth,nHeight,pDsdBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	{
		delete [] pDsdBuf;
		pDsdBuf = 0;
		std::cout<<"读取文件出错！"<<std::endl;
		return false;
	}
	//把读取影像转为二维数组
	Float32 **pDDsdBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		pDDsdBuf[i]=new Float32 [nWidth];
	}
	int jj=0;
	//把整幅影像转换为二维数组
	for (int ii =0; ii <nHeight; ii++)
	{
		for (int cc =0; cc <nWidth; cc++)
		{

			pDDsdBuf[ii][cc]=pDsdBuf[jj++];
			if (jj>nWidth*nHeight)
			{
				break;
			}
		}
	}
	jj=0;
	if (false==eageCloude(pSrcDatasetR,pDrcDatasetH,pRBuf,pDDsdBuf,r))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"计算双边滤波出错！"<<std::endl;
		system("pause");
		return false;
	}
	for(int i = 0; i < nHeight; i++)
	{
		delete [] pDDsdBuf[i];
	}
	delete [] pDDsdBuf;
	delete [] pDsdBuf;
	pDsdBuf = 0;
	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}

//双边滤波
bool EageCloude::eageCloude(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR, Float32 *pRBuf,Float32 **pDDsdBuf)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"格式不是GDT_Float32"<<std::endl;
		system("pause");
		return false;
	}
	int nXBlockSize,nYBlockSize;
	int iXBlock=0,iYBlock=0;
	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	
	//设置块的大小为2*r+1
	if (nXBlockSize<64||nXBlockSize>512)		
	{
		nXBlockSize = 3;
	}
	if (nYBlockSize<64||nYBlockSize>512)
	{
		nYBlockSize = 3;
	}
	int nXBlocks = (nWidth + nXBlockSize - 1)/nXBlockSize;
	int nYBlocks = (nHeight + nYBlockSize - 1)/nYBlockSize;
	long int nBlockSize = nXBlockSize * nYBlockSize;
	pRBuf = new Float32[nBlockSize]; 
	//存放整幅影像的数组，写入使用
	Float32 *pDRBuf = new Float32[nWidth*nHeight]; 
	//读取2*r+1的图像数组
	Float32 **psdBuf=new Float32*[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		psdBuf[i]=new Float32[nWidth];

	}
	//处理的结果数组
	Float32 **pDDDsdBuf=new Float32 *[nHeight];
	for (int ii=0;ii<nHeight;ii++)
	{
		pDDDsdBuf[ii]=new Float32 [nWidth];
	}
	//统计影像最大值和最小值
	int bRGoMin=0,bRGoMax=0;
	adfRmaxmin[0]=pSrcBandR->GetMaximum(&bRGoMax);
	adfRmaxmin[1]=pSrcBandR->GetMinimum(&bRGoMax);
	if (!(bRGoMax&&bRGoMin))
	{
		GDALComputeRasterMinMax(pSrcBandR,FALSE,adfRmaxmin);
	}
	std::cout<<adfRmaxmin[0]<<"	"<<adfRmaxmin[1]<<std::endl;
	//循环整幅影像
	for (iYBlock=0; iYBlock<nHeight; iYBlock++)
	{
		for (iXBlock=0; iXBlock<nWidth; iXBlock++)
		{			
			double k = 0;
			double f = 0;
			//读取一个2*r+1的影像块
			if (iXBlock+1<nWidth&&iYBlock+1<nHeight&&iXBlock-1>=0&&iYBlock-1>=0)
			{
				if (pSrcBandR->RasterIO(GF_Read,iXBlock-1,iYBlock-1,3,3,pRBuf,3,3,GDT_Float64,0,0) != CE_None)
				{
					delete [] pRBuf;
					pRBuf = 0;
					std::cout<<"读取文件出错！"<<std::endl;
					return false;
				}
				//把2*r+1的影像块转换成二维数组->psdBuf
				int j=0;
				for (int i = iYBlock-1; i <=iYBlock+1; i++)
				{
					for (int c = iXBlock-1; c <=iXBlock+1; c++)
					{
						psdBuf[i][c]=pRBuf[j++];
					}
				}
				//双边滤波数据处理
				for (int i = iYBlock-1; i <=iYBlock+1; i++)
				{
					for (int c = iXBlock-1; c <=iXBlock+1; c++)
					{
						if (i!=iYBlock && c!=iXBlock)
						{
							
						}
					}
				}
				if (k!=0.0)
				{
					double value = f / k;
					if (value < 0) 
					{
						value = 0;
					}
					else if (value > 255)
					{
						value = 255;
					}
					//二维数组
					pDDDsdBuf[iYBlock][iXBlock]=value;	
				}else
				{
					pDDDsdBuf[iYBlock][iXBlock]=pDDsdBuf[iYBlock][iXBlock];
				}			
			}
			else
			{
				pDDDsdBuf[iYBlock][iXBlock]=pDDsdBuf[iYBlock][iXBlock];
			}
		}
	}
	//把处理结果转换成一维数组
	int hhh=0;
	for (int i=0;i<nHeight;i++)
	{
		for (int j=0;j<nWidth;j++)
		{
			pDRBuf[hhh++]=pDDDsdBuf[i][j];
		}
	}
	if (pDstBandR->RasterIO(GF_Write,0,0,nWidth,nHeight,pDRBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	{
		delete [] pRBuf;
		pRBuf = 0;
		delete [] pDRBuf;
		pDRBuf = 0;
		for(int i = 0; i < 3; i++)
		{
			delete [] psdBuf[i];
		}
		delete [] psdBuf;
		for(int i = 0; i < nHeight; i++)
		{
			delete [] pDDDsdBuf[i];
		}
		delete [] pDDDsdBuf;
		std::cout<<"写入文件出错！"<<std::endl;
		return false;
	}
	//释放内存
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pDRBuf;
	pDRBuf=0;
	for(int i = 0; i <3; i++)
	{
		delete [] psdBuf[i];
	}
	delete [] psdBuf;
	for(int i = 0; i < nHeight; i++)
	{
		delete [] pDDDsdBuf[i];
	}
	delete [] pDDDsdBuf;
	return true;
}