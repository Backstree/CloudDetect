#include "CReRgb.h"
#include <iostream>
#include "gdal_priv.h"
ReRGB::ReRGB()
{
	string m_strInputRFileName=string();
	string m_strInputGFileName=string();
	string m_strInputBFileName=string();
	m_bInit = false;
	m_nBandCouts = 0;
}
ReRGB::~ReRGB()
{
}
//��ʼ��
bool ReRGB::Initilization(const string & strInputRFileName, const string & strInputGFileName,const string & strInputBFileName)
{
	if (strInputRFileName.empty()||strInputGFileName.empty()||strInputBFileName.empty())
	{
		std::cout<<"�ļ���Ϊ�գ�";
		system("pause");
		return false;
	}
	m_strInputBFileName=strInputBFileName;
	m_strInputGFileName=strInputGFileName;
	m_strInputRFileName=strInputRFileName;
	//ע����������
	GDALAllRegister();
	GDALDataset *pDatasetR=(GDALDataset*)GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	GDALDataset *pDatasetG=(GDALDataset*)GDALOpen(m_strInputGFileName.c_str(),GA_Update);
	GDALDataset *pDatasetB=(GDALDataset*)GDALOpen(m_strInputBFileName.c_str(),GA_Update);
	if (pDatasetB==0||pDatasetG==0||pDatasetR==0)
	{
		m_strInputBFileName.erase(m_strInputBFileName.begin(),m_strInputBFileName.end());
		m_strInputGFileName.erase(m_strInputGFileName.begin(),m_strInputGFileName.end());
		m_strInputRFileName.erase(m_strInputRFileName.begin(),m_strInputRFileName.end());
		std::cout<<"�ļ��򿪴���"<<std::endl;
		system("pause");
		return false;
	}
	m_nBandCouts=pDatasetB->GetRasterCount();
	GDALClose(pDatasetB);
	GDALClose(pDatasetR);
	GDALClose(pDatasetG);
	m_bInit=true;
	return true;
}
//ִ��
bool ReRGB::Execute(Float32 *pRBuf,const string & strOutputRFileName,
					Float32 *pGBuf,const string & strOutputGFileName,
					Float32 *pBBuf,const string & strOutputBFileName)
{
	if (strOutputRFileName.empty()||strOutputGFileName.empty()||strOutputBFileName.empty())
	{
		std::cout<<"����ļ���Ϊ�գ�";
		system("pause");
		return false;
	}
	m_strOutputBFileName=strOutputBFileName;
	m_strOutputRFileName=strOutputRFileName;
	m_strOutputGFileName=strOutputGFileName;

	if (m_bInit==false)
	{
		std::cout<<"δִ�г�ʼ����"<<std::endl;
		system("pause");
		return false;
	}
	GDALAllRegister();
	GDALDataset * pSrcDatasetR = (GDALDataset *) GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	GDALDataset * pSrcDatasetG = (GDALDataset *) GDALOpen(m_strInputGFileName.c_str(),GA_Update);
	GDALDataset * pSrcDatasetB = (GDALDataset *) GDALOpen(m_strInputBFileName.c_str(),GA_Update);
	//��ȡӰ����
	int nWidth, nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	//����Ŀ���ļ�
	const char *pszFormat = "GTiff";
	GDALDriver *poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	if (poDriver == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		GDALClose(pSrcDatasetB);
		pSrcDatasetB= 0;
		return false;
	}	
	int nBandCount = pSrcDatasetR->GetRasterCount();
	GDALDataset *pDstDatsetR = poDriver->Create(m_strOutputRFileName.c_str(), nWidth, nHeight, nBandCount, GDT_Float32, NULL);
	GDALDataset *pDstDatsetG = poDriver->Create(m_strOutputGFileName.c_str(), nWidth, nHeight, nBandCount, GDT_Float32, NULL);
	GDALDataset *pDstDatsetB = poDriver->Create(m_strOutputBFileName.c_str(), nWidth, nHeight, nBandCount, GDT_Float32, NULL);
	if (pDstDatsetR == 0||pDstDatsetG == 0||pDstDatsetB == 0)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		GDALClose(pSrcDatasetB);
		pSrcDatasetB = 0;
		return false;
	}
	double HRGeoTransForm[6];
	pSrcDatasetR->GetGeoTransform(HRGeoTransForm);
	const char *  pszSRS_WKT =  pSrcDatasetR->GetProjectionRef();	
	pDstDatsetR->SetGeoTransform(HRGeoTransForm);
	pDstDatsetR->SetProjection(pszSRS_WKT);
	pDstDatsetR->SetMetadata(pDstDatsetR->GetMetadata());

	pDstDatsetG->SetGeoTransform(HRGeoTransForm);
	pDstDatsetG->SetProjection(pszSRS_WKT);
	pDstDatsetG->SetMetadata(pDstDatsetG->GetMetadata());

	pDstDatsetB->SetGeoTransform(HRGeoTransForm);
	pDstDatsetB->SetProjection(pszSRS_WKT);
	pDstDatsetB->SetMetadata(pDstDatsetB->GetMetadata());

	if (false==reRGB(pSrcDatasetR,pSrcDatasetG,pSrcDatasetB,pDstDatsetR,pDstDatsetG,pDstDatsetB,pRBuf, pGBuf,pBBuf))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDstDatsetR);
		pDstDatsetR = 0;
		GDALClose(pSrcDatasetG);
		pSrcDatasetG = 0;
		GDALClose(pDstDatsetG);
		pDstDatsetG = 0;
		GDALClose(pSrcDatasetB);
		pSrcDatasetB = 0;
		GDALClose(pDstDatsetB);
		pDstDatsetB = 0;
		std::cout<<"ת��RGB����"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDstDatsetR);
	pDstDatsetR = 0;
	GDALClose(pSrcDatasetG);
	pSrcDatasetG = 0;
	GDALClose(pDstDatsetG);
	pDstDatsetG = 0;
	GDALClose(pSrcDatasetB);
	pSrcDatasetB = 0;
	GDALClose(pDstDatsetB);
	pDstDatsetB = 0;

	return true;
}

//��������RGB
bool ReRGB::reRGB(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
				  GDALDataset * pDstDatsetR,GDALDataset * pDstDatsetG,GDALDataset * pDstDatsetB,
				  Float32 *pRBuf,Float32 *pGBuf,Float32 *pBBuf)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDstDatsetR->GetRasterBand(1);
	GDALRasterBand * pSrcBandG = pSrcDatasetG->GetRasterBand(1);
	GDALRasterBand * pDstBandG = pDstDatsetG->GetRasterBand(1);
	GDALRasterBand * pSrcBandB = pSrcDatasetB->GetRasterBand(1);
	GDALRasterBand * pDstBandB = pDstDatsetB->GetRasterBand(1);

	if (pSrcBandR->GetRasterDataType() != GDT_UInt16)
	{
		std::cout<<"��ʽ����"<<std::endl;
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
	pBBuf = new Float32[nBlockSize];
	Float32 *pDRBuf = new Float32[nBlockSize]; 
	Float32 *pDGBuf = new Float32[nBlockSize]; 
	Float32 *pDBBuf = new Float32[nBlockSize];
	max=new Float32[nBlockSize];
	min=new Float32[nBlockSize];
	mid=new Float32[nBlockSize];
	for (iYBlock=0; iYBlock<nYBlocks; iYBlock++)
	{
		iYOffset = iYBlock*nYBlockSize;
		for (iXBlock=0; iXBlock<nXBlocks; iXBlock++)
		{
			 iXOffset = iXBlock*nXBlockSize;
			 w = nXBlockSize;
			 h = nYBlockSize;
			//Ӱ���Ե����һ����Ŀ�ͳ�ʱ�Ĳ���
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
			//��Чֵ��δ���
			//-------------------------------------------------->
			/////////////////////////////////////////////////////

			//double a=pSrcBandR->GetNoDataValue(pbSuccess=NULL);
			//��ȡʧ��
			
				if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pRBuf,w,h,GDT_Float64,0,0) != CE_None||
					pSrcBandG->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pGBuf,w,h,GDT_Float64,0,0) != CE_None||
					pSrcBandB->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pBBuf,w,h,GDT_Float64,0,0) != CE_None)
				{
					delete [] pRBuf;
					pRBuf = 0;
					delete [] pGBuf;
					pGBuf = 0;
					delete [] pBBuf;
					pBBuf = 0;
					return false;
				}
				for (int i=0;i<nBlockSize;i++)
				{
					///////////////////////////////////////////////////////////
					//-------------------------------------------------------->
					//��������RGB�Լ�Ȩֵ����������֮�����ݸ�ʽ�Ƿ���GDT_Uint 16?����GDT_Float64?
					//-------------------------------------------------------->
					///////////////////////////////////////////////////////////
					max[i]=Max(Max(pRBuf[i],pGBuf[i]),pBBuf[i]);
					min[i]=Min(Min(pRBuf[i],pGBuf[i]),pBBuf[i]);
					//�����м�ֵ
					if ((pRBuf[i]<=pGBuf[i]&&pGBuf[i]<=pBBuf[i])||(pBBuf[i]<=pGBuf[i]&&pGBuf[i]<=pRBuf[i]))
					{
						mid[i]=pGBuf[i];
					}else if ((pGBuf[i]<=pRBuf[i]&&pRBuf[i]<=pBBuf[i])||(pBBuf[i]<=pRBuf[i]&&pRBuf[i]<=pGBuf[i]))
					{
						mid[i]=pRBuf[i];
					}else if ((pGBuf[i]<=pBBuf[i]&&pBBuf[i]<=pRBuf[i])||(pRBuf[i]<=pBBuf[i]&&pBBuf[i]<=pGBuf[i]))
					{
						mid[i]=pBBuf[i];
					}
					else
					{
						std::cout<<"������ִ���"<<std::endl;
						system("pause");
				
					}
					//Ȩֵ����
					min[i]*=sqrt(8)/2;//pow(8.0,0.5);
					mid[i]*=sqrt(6)/2;//pow(6.0,0.5);
					pDRBuf[i]=min[i];
					pDGBuf[i]=mid[i];
					pDBBuf[i]=max[i];
					if (pDBBuf[i]<0||pDRBuf[i]<0||pDBBuf[i]<0)
					{
						std::cout<<"�ڼ��飺"<<iXBlock<<"*"<<iYBlock<<"������꣺"<<iXOffset<<"��"<<iYOffset<<std::endl;
						system("pause");
					}
				}
				
				//д��ʧ��
				if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDRBuf,w,h,GDT_Float64,0,0) != CE_None||
					pDstBandG->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDGBuf,w,h,GDT_Float64,0,0) != CE_None||
					pDstBandB->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDBBuf,w,h,GDT_Float64,0,0) != CE_None)
				{
					delete [] pRBuf;
					pRBuf = 0;
					delete [] pGBuf;
					pGBuf = 0;
					delete [] pBBuf;
					pBBuf = 0;
					delete [] pDRBuf;
					pDRBuf = 0;
					delete [] pDGBuf;
					pDGBuf = 0;
					delete [] pDBBuf;
					pDBBuf = 0;
					return false;
				}
			}
	}
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pGBuf;
	pGBuf = 0;
	delete [] pBBuf;
	pBBuf = 0;
	delete [] pDRBuf;
	pDRBuf = 0;
	delete [] pDGBuf;
	pDGBuf = 0;
	delete [] pDBBuf;
	pDBBuf = 0;
	return true;
}
Float32 ReRGB::Max(Float32 a,Float32 b)
{
	if (a>=b)
	{
		return a;
	}else
	{
		return b;
	}
}
Float32 ReRGB::Min(Float32 a,Float32 b)
{
	if (a>=b)
	{
		return b;
	}else
	{
		return a;
	}
}