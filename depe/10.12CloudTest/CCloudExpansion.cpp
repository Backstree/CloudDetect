#include "CCloudExpansion.h"
#include <iostream>
#include "gdal_priv.h"
CloudExpansion::CloudExpansion()
{
	string m_strInputRFileName=string();
	m_bInit = false;
}
CloudExpansion::~CloudExpansion()
{
}
//初始化
bool CloudExpansion::Initilization(const string & strInputRFileName,const string & strInputGFileName)
{
	if (strInputRFileName.empty()||strInputGFileName.empty())
	{
		std::cout<<"文件名为空！";
		system("pause");
		return false;
	}
	m_strInputGFileName=strInputGFileName;
	m_strInputRFileName=strInputRFileName;
	//注册所有驱动
	GDALAllRegister();
	GDALDataset *pDatasetR=(GDALDataset*)GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	GDALDataset *pDatasetG=(GDALDataset*)GDALOpen(m_strInputGFileName.c_str(),GA_Update);
	if (pDatasetG==0||pDatasetR==0)
	{
		m_strInputGFileName.erase(m_strInputGFileName.begin(),m_strInputGFileName.end());
		m_strInputRFileName.erase(m_strInputRFileName.begin(),m_strInputRFileName.end());
		std::cout<<"文件打开错误！"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pDatasetR);
	GDALClose(pDatasetG);
	m_bInit=true;
	return true;
}
//执行
bool CloudExpansion::Execute(Float32 *pRBuf,Float32 *pGBuf,const string & strOutputHFileName,Float32 k)
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
	GDALDataset * pSrcDatasetG = (GDALDataset *) GDALOpen(m_strInputGFileName.c_str(),GA_Update);
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
	if (false==cloudExpansion(pSrcDatasetR,pSrcDatasetG,pDrcDatasetH,pRBuf,pGBuf,k))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"计算膨胀出错！"<<std::endl;
		system("pause");
		return false;
	}
	delete [] pDsdBuf;
	pDsdBuf = 0;
	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}
bool CloudExpansion::cloudExpansion(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG, 
									GDALDataset * pDrcDatasetR, Float32 *pRBuf ,Float32 *pGBuf,Float32 k)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pSrcBandG = pSrcDatasetG->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32||pSrcBandG->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"格式不是GDT_Float32"<<std::endl;
		system("pause");
		return false;
	}
	int nXBlockSize,nYBlockSize;
	int iXBlock=0,iYBlock=0;
	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	
	//设置块的大小为3
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
	Float32 *pDRBuf = new Float32[nWidth*nHeight];
	Float32 *pDDDRBuf = new Float32[nWidth*nHeight];
	Float32 **psdBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		psdBuf[i]=new Float32 [nWidth];
	}
	pRBuf = new Float32[nWidth*nHeight]; 
	if (pSrcBandR->RasterIO(GF_Read,0,0,nWidth,nHeight,pRBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	{
		delete [] pRBuf;
		pRBuf = 0;
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
			pDDsdBuf[ii][cc]=pRBuf[jj++];
			if (jj>nWidth*nHeight)
			{
				break;
			}
		}
	}
	jj=0;
	pGBuf = new Float32[nWidth*nHeight]; 
	if (pSrcBandG->RasterIO(GF_Read,0,0,nWidth,nHeight,pGBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	{
		delete [] pGBuf;
		pGBuf = 0;
		std::cout<<"读取文件出错！"<<std::endl;
		return false;
	}
	//把读取影像转为二维数组
	Float32 **pDDsdGBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		pDDsdGBuf[i]=new Float32 [nWidth];
	}
	//把整幅影像转换为二维数组
	for (int ii =0; ii <nHeight; ii++)
	{
		for (int cc =0; cc <nWidth; cc++)
		{
			pDDsdGBuf[ii][cc]=pGBuf[jj++];
			if (jj>nWidth*nHeight)
			{
				break;
			}
		}
	}
	jj=0;
	Float32 **pdDsdBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		pdDsdBuf[i]=new Float32 [nWidth];
	}
	int D=0,T=0;
	while (1)
	{
		for (iYBlock=0; iYBlock<nHeight; iYBlock++)
		{
			for (iXBlock=0; iXBlock<nWidth; iXBlock++)
			{
				//云像素处理
				if (pDDsdGBuf[iYBlock][iXBlock]==1)
				{
					//以云像素为中心的3*3窗口块
					if (iXBlock+1<nWidth&&iYBlock+1<nHeight&&iXBlock-1>=0&&iYBlock-1>=0)
					{
						if (pSrcBandR->RasterIO(GF_Read,iXBlock-1,iYBlock-1,3,3,pDRBuf,3,3,GDT_Float64,0,0) != CE_None)
						{
							delete [] pDRBuf;
							pDRBuf = 0;
							std::cout<<"读取文件出错！"<<std::endl;
							return false;
						}
						//把3的影像块转换成二维数组->psdBuf
						int j=0;
						for (int i = iYBlock-1; i <=iYBlock+1; i++)
						{
							for (int c = iXBlock-1; c <=iXBlock+1; c++)
							{
								psdBuf[i][c]=pDRBuf[j++];
							}
						}
						//
						for (int i = iYBlock-1; i <=iYBlock+1; i++)
						{
							for (int c = iXBlock-1; c <=iXBlock+1; c++)
							{
								if(psdBuf[i][c]==0&&i!=iYBlock&&c!=iXBlock)
								{
									if(abs(pDDsdBuf[iYBlock][iXBlock]-psdBuf[i][c])<k*pDDsdBuf[iYBlock][iXBlock])
									{
										pDDsdGBuf[i][c]=2;
										D++;
									}
								}
							}
						}
					}
				}			
			}
		}
		T++;
		if (D<200)
		{
			break;
		}
		D=0;
		if (T<3)
		{
			break;
		}
	}
	int hhh=0;
	for (int i=0;i<nHeight;i++)
	{
		for (int j=0;j<nWidth;j++)
		{
			pDDDRBuf[hhh++]=pDDsdGBuf[i][j];
		}
	}
	if (pDstBandR->RasterIO(GF_Write,0,0,nWidth,nHeight,pDDDRBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	{
		delete [] pRBuf;
		pRBuf = 0;
		for(int i = 0; i <nHeight; i++)
		{
			delete [] psdBuf[i];
		}
		delete [] psdBuf;
		delete [] pDRBuf;
		pDRBuf = 0;
		for(int i = 0; i <nHeight; i++)
		{
			delete [] pDDsdBuf[i];
		}
		delete [] pDDsdBuf;
		delete [] pDDDRBuf;
		pDDDRBuf = 0;
		std::cout<<"写入文件出错！"<<std::endl;
		return false;
	}
	//释放内存
	delete [] pRBuf;
	pRBuf = 0;
	for(int i = 0; i <nHeight; i++)
	{
		delete [] psdBuf[i];
	}
	delete [] psdBuf;
	delete [] pDRBuf;
	pDRBuf = 0;
	for(int i = 0; i <nHeight; i++)
	{
		delete [] pDDsdBuf[i];
	}
	delete [] pDDsdBuf;
	delete [] pDDDRBuf;
	pDDDRBuf = 0;
	return true;
}