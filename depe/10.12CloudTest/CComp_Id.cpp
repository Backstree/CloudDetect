#include "CComp_Id.h"
#include <iostream>
#include "gdal_priv.h"
Comp_Id::Comp_Id()
{
	string m_strInputRFileName=string();
	string m_strInputGFileName=string();
	m_bInit = false;
}
Comp_Id::~Comp_Id()
{
}
//初始化
bool Comp_Id::Initilization(const string & strInputRFileName, const string & strInputGFileName)
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
bool Comp_Id::Execute(Float32 *pRBuf,Float32 *pGBuf,const string & strOutputRFileName)
{
	if (strOutputRFileName.empty())
	{
		std::cout<<"输出文件名为空！";
		system("pause");
		return false;
	}
	m_strOutputRFileName=strOutputRFileName;

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
	//创建目标文件
	const char *pszFormat = "GTiff";
	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		return false;
	}	
	int nBandCount = pSrcDatasetR->GetRasterCount();
	GDALDataset *pDstDatsetR = poDriver->Create(m_strOutputRFileName.c_str(), nWidth, nHeight, nBandCount, GDT_Float32, NULL);
	if (pDstDatsetR == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		return false;
	}
	double HRGeoTransForm[6];
	pSrcDatasetR->GetGeoTransform(HRGeoTransForm);
	const char *  pszSRS_WKT =  pSrcDatasetR->GetProjectionRef();	
	pDstDatsetR->SetGeoTransform(HRGeoTransForm);
	pDstDatsetR->SetProjection(pszSRS_WKT);
	pDstDatsetR->SetMetadata(pDstDatsetR->GetMetadata());
	if (false==comp_Id(pSrcDatasetR,pSrcDatasetG,pDstDatsetR,pRBuf,pGBuf))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDstDatsetR);
		pDstDatsetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		std::cout<<"计算J出错！"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDstDatsetR);
	pDstDatsetR = 0;
	GDALClose(pSrcDatasetG);
	pSrcDatasetG = 0;
	return true;
}

//重新排序RGB
bool Comp_Id::comp_Id(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,
				GDALDataset * pDstDatsetR, Float32 *pRBuf,Float32 *pGBuf)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDstDatsetR->GetRasterBand(1);;
	GDALRasterBand * pSrcBandG = pSrcDatasetG->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"格式错误"<<std::endl;
		return false;
	}

	int nXBlockSize,nYBlockSize;
	int iXBlock,iYBlock;
	int iXOffset,iYOffset;
	long int nRealBlockSize = 0;
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
	pRBuf = new Float32[nBlockSize]; 
	pGBuf = new Float32[nBlockSize]; 
	Float32 *pDRBuf = new Float32[nBlockSize]; 
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
			/////////////////////////////////////////////////////
			//-------------------------------------------------->
			//无效值如何处理
			//-------------------------------------------------->
			/////////////////////////////////////////////////////

			//double a=pSrcBandR->GetNoDataValue(pbSuccess=NULL);
			//读取失败

			if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pRBuf,w,h,GDT_Float64,0,0) != CE_None||
				pSrcBandG->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pGBuf,w,h,GDT_Float64,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				delete [] pGBuf;
				pGBuf = 0;
				return false;
			}
			for (int i=0;i<nBlockSize;i++)
			{
				///////////////////////////////////////////////////////////
				//-------------------------------------------------------->
				//计算细节图Id
				//-------------------------------------------------------->
				///////////////////////////////////////////////////////////
				pDRBuf[i]=abs(pRBuf[i]-pGBuf[i]);


			}

			//写入失败
			if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDRBuf,w,h,GDT_Float64,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				delete [] pGBuf;
				pGBuf = 0;
				delete [] pDRBuf;
				pDRBuf = 0;
				return false;
			}
		}
	}
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pGBuf;
	pGBuf = 0;
	delete [] pDRBuf;
	pDRBuf = 0;
	return true;
}