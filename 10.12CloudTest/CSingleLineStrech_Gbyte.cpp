#include "CSingleLineStrech_Gbyte.h"
#include <iostream>
#include "gdal_priv.h"
CSingleLineStreach_GByte::CSingleLineStreach_GByte()
{
	string m_strInputRFileName=string();
	Float32 adfRmaxmin[2]={0.0,0.0};
	m_bInit = false;
}
CSingleLineStreach_GByte::~CSingleLineStreach_GByte()
{
}
//初始化
bool CSingleLineStreach_GByte::Initilization(const string & strInputRFileName)
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
bool CSingleLineStreach_GByte::Execute(const string & strOutputHFileName,int AimsMin,int AimsMax)
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

	if (false==LineStrecth(pSrcDatasetR,pDrcDatasetH,AimsMin,AimsMax))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"拉伸到01时出错！"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}

//线性拉伸
bool CSingleLineStreach_GByte::LineStrecth(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR,int AimsMin,int AimsMax)
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
	int iXBlock,iYBlock;
	int iXOffset,iYOffset;
	int w = 0;
	int h =	0;

	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	

	if (nXBlockSize<64||nXBlockSize>512)		
	{
		nXBlockSize = 512;
	}
	if (nYBlockSize<64||nYBlockSize>512)
	{
		nYBlockSize = 512;
	}
	int nXBlocks = (nWidth + nXBlockSize - 1)/nXBlockSize;
	int nYBlocks = (nHeight + nYBlockSize - 1)/nYBlockSize;
	long int nBlockSize = nXBlockSize * nYBlockSize;
	GByte* pRBuf = new GByte[nBlockSize]; 
	GByte *pDRBuf = new GByte[nBlockSize]; 
	int bRGoMin=0,bRGoMax=0;
	adfRmaxmin[0]=pSrcBandR->GetMaximum(&bRGoMax);
	adfRmaxmin[1]=pSrcBandR->GetMinimum(&bRGoMax);
	if (!(bRGoMax&&bRGoMin))
	{
		GDALComputeRasterMinMax(pSrcBandR,FALSE,adfRmaxmin);
	}
	std::cout<<"R->Max="<<adfRmaxmin[1]<<std::endl;
	std::cout<<"R->Min="<<adfRmaxmin[0]<<std::endl;
	for (iYBlock=0; iYBlock<nYBlocks; iYBlock++)
	{
		iYOffset = iYBlock*nYBlockSize;
		for (iXBlock=0; iXBlock<nXBlocks; iXBlock++)
		{
			iXOffset = iXBlock*nXBlockSize;
			w = nXBlockSize;
			h = nYBlockSize;
			//影像边缘不足一个块的宽和长时的操作
			if (iXBlock == nXBlocks - 1)
			{
				w = nWidth - iXBlock*nXBlockSize;				
			}
			if (iYBlock == nYBlocks - 1)
			{
				h = nHeight - iYBlock*nYBlockSize;				
			}
			nBlockSize = w*h;	
			if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pRBuf,w,h,GDT_Byte,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				std::cout<<"读取文件出错！"<<std::endl;
				return false;
			}
			for (int i=0;i<nBlockSize;i++)
			{
				///////////////////////////////////////////////////////////
				//-------------------------------------------------------->
				//拉伸到01
				//-------------------------------------------------------->
				///////////////////////////////////////////////////////////
				pDRBuf[i]=AimsMin+(pRBuf[i]-adfRmaxmin[0])/(adfRmaxmin[1]-adfRmaxmin[0])*AimsMax;
			}
			//写入失败
			if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDRBuf,w,h,GDT_Byte,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				std::cout<<"写入文件出错！"<<std::endl;
				return false;
			}
		}
	}
	delete [] pRBuf;
	pRBuf = 0;
	return true;
}