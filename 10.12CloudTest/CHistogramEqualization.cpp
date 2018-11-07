#include "CHistogramEqualization.h"
#include <iostream>
#include "gdal_priv.h"
CHistogramEqualization::CHistogramEqualization()
{
	string m_strInputRFileName=string();
	Float32 adfRmaxmin[2]={0.0,0.0};
	m_bInit = false;
}
CHistogramEqualization::~CHistogramEqualization()
{
}
//��ʼ��
bool CHistogramEqualization::Initilization(const string & strInputRFileName)
{
	if (strInputRFileName.empty())
	{
		std::cout<<"�ļ���Ϊ�գ�";
		system("pause");
		return false;
	}
	m_strInputRFileName=strInputRFileName;
	//ע����������
	GDALAllRegister();
	GDALDataset *pDatasetR=(GDALDataset*)GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	if (pDatasetR==0)
	{
		m_strInputRFileName.erase(m_strInputRFileName.begin(),m_strInputRFileName.end());
		std::cout<<"�ļ��򿪴���"<<std::endl;
		system("pause");
		return false;
	}
	GDALClose(pDatasetR);
	m_bInit=true;
	return true;
}
//ִ��
bool CHistogramEqualization::Execute(Gbyte *pRBuf,const string & strOutputHFileName)
{
	if (strOutputHFileName.empty())
	{
		std::cout<<"����ļ���Ϊ�գ�";
		system("pause");
		return false;
	}
	m_strOutputHFileName=strOutputHFileName;
	if (m_bInit==false)
	{
		std::cout<<"δִ�г�ʼ����"<<std::endl;
		system("pause");
		return false;
	}
	GDALAllRegister();
	GDALDataset * pSrcDatasetR = (GDALDataset *) GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	//��ȡӰ����
	int nWidth, nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();

	double HRGeoTransForm[6];
	pSrcDatasetR->GetGeoTransform(HRGeoTransForm);

	//����Ŀ���ļ�
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

	if (false==HistogramEqualization(pSrcDatasetR,pDrcDatasetH,pRBuf))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"���쵽01ʱ����"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}

//��������
bool CHistogramEqualization::HistogramEqualization(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR, Gbyte *pRBuf)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"��ʽ����GDT_Float32"<<std::endl;
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
		nXBlockSize =512;
	}
	if (nYBlockSize<64||nYBlockSize>512)
	{
		nYBlockSize =512;
	}
	int nXBlocks = (nWidth + nXBlockSize - 1)/nXBlockSize;
	int nYBlocks = (nHeight + nYBlockSize - 1)/nYBlockSize;
	long int nBlockSize = nXBlockSize * nYBlockSize;
	pRBuf = new Gbyte[nBlockSize]; 
	Float32 *pDRBuf=new Float32[nBlockSize];
	int bRGoMin=0,bRGoMax=0;
	adfRmaxmin[0]=pSrcBandR->GetMaximum(&bRGoMax);
	adfRmaxmin[1]=pSrcBandR->GetMinimum(&bRGoMax);
	if (!(bRGoMax&&bRGoMin))
	{
		GDALComputeRasterMinMax(pSrcBandR,FALSE,adfRmaxmin);
	}
	int anHistogram[256];
	pSrcBandR->GetHistogram(adfRmaxmin[0],adfRmaxmin[1],256,anHistogram,FALSE,FALSE,GDALDummyProgress,NULL);
	double  C[256]={0};
	for (int i=0;i<256;i++)
	{
		C[i]=((double)anHistogram[i])/(nWidth*nHeight);
	}
	double c[256]={0};
	for (int i=0;i<256;i++)
	{
		for (int j=0;j<=i;j++)
		{
			c[i]+=C[j];
		}
	}
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
			if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pRBuf,w,h,GDT_Byte,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				std::cout<<"��ȡ�ļ�����"<<std::endl;
				return false;
			}
			for (int i=0;i<nBlockSize;i++)
			{
				///////////////////////////////////////////////////////////
				//-------------------------------------------------------->
				//ֱ��ͼ���⻯
				//-------------------------------------------------------->
				///////////////////////////////////////////////////////////
				pDRBuf[i]=(double)c[pRBuf[i]]*pRBuf[i];
					
			}
			//д��ʧ��
			if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,w,h,pDRBuf,w,h,GDT_Float64,0,0) != CE_None)
			{
				delete [] pRBuf;
				pRBuf = 0;
				delete [] pDRBuf;
				pDRBuf = 0;
				std::cout<<"д���ļ�����"<<std::endl;
				return false;
			}
		}
	}
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pDRBuf;
	pDRBuf = 0;
	return true;
}