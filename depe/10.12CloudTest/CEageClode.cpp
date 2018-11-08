#include "CEageCloude.h"
#include <iostream>
#include "gdal_priv.h"
EageCloude::EageCloude()
{
	string m_strInputRFileName=string();
	m_bInit = false;
}
EageCloude::~EageCloude()
{
}
//��ʼ��
bool EageCloude::Initilization(const string & strInputRFileName)
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
bool EageCloude::Execute(Float32 *pRBuf,const string & strOutputHFileName)
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
	GDALRasterBand * pDSrcBandR = pSrcDatasetR->GetRasterBand(1);
	//�������Ӱ������飬��ȡʹ��
	Float32 *pDsdBuf=new Float32 [nWidth*nHeight];
	//��ȡ����Ӱ��->pDstBuf
	////�Ѷ�ȡӰ��תΪ��ά����
	//Float32 **pDDsdBuf=new Float32 *[nHeight];
	//for (int i=0;i<nHeight;i++)
	//{
	//	pDDsdBuf[i]=new Float32 [nWidth];
	//}
	//int jj=0;
	////������Ӱ��ת��Ϊ��ά����
	//for (int ii =0; ii <nHeight; ii++)
	//{
	//	for (int cc =0; cc <nWidth; cc++)
	//	{
	//		pDDsdBuf[ii][cc]=pDsdBuf[jj++];
	//		if (jj>nWidth*nHeight)
	//		{
	//			break;
	//		}
	//	}
	//}
	//jj=0;
	if (false==eageCloude(pSrcDatasetR,pDrcDatasetH,pRBuf))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"�������ͳ���"<<std::endl;
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

//˫���˲�
bool EageCloude::eageCloude(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR, Float32 *pRBuf)
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
	int iXBlock=0,iYBlock=0;
	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	
	//���ÿ�Ĵ�СΪ3
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
		std::cout<<"��ȡ�ļ�����"<<std::endl;
		return false;
	}
	//�Ѷ�ȡӰ��תΪ��ά����
	Float32 **pDDsdBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		pDDsdBuf[i]=new Float32 [nWidth];
	}
	Float32 **pdDsdBuf=new Float32 *[nHeight];
	for (int i=0;i<nHeight;i++)
	{
		pdDsdBuf[i]=new Float32 [nWidth];
	}
	int jj=0;
	//������Ӱ��ת��Ϊ��ά����
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
	for (iYBlock=0; iYBlock<nHeight; iYBlock++)
	{
		for (iXBlock=0; iXBlock<nWidth; iXBlock++)
		{
			//�����ش���
			if (pDDsdBuf[iYBlock][iXBlock]!=0)
			{
				//��������Ϊ���ĵ�3*3���ڿ�
				if (iXBlock+1<nWidth&&iYBlock+1<nHeight&&iXBlock-1>=0&&iYBlock-1>=0)
				{
					if (pSrcBandR->RasterIO(GF_Read,iXBlock-1,iYBlock-1,3,3,pDRBuf,3,3,GDT_Float64,0,0) != CE_None)
					{
						delete [] pDRBuf;
						pDRBuf = 0;
						std::cout<<"��ȡ�ļ�����"<<std::endl;
						return false;
					}
					//��3��Ӱ���ת���ɶ�ά����->psdBuf
					int j=0;
					for (int i = iYBlock-1; i <=iYBlock+1; i++)
					{
						for (int c = iXBlock-1; c <=iXBlock+1; c++)
						{
							psdBuf[i][c]=pDRBuf[j++];
						}
					}
					//�ж��Ƿ�3*3����,���������������ش�Ϊ1
					for (int i = iYBlock-1; i <=iYBlock+1; i++)
					{
						for (int c = iXBlock-1; c <=iXBlock+1; c++)
						{

							if(psdBuf[i][c]==0&&i!=iYBlock&&c!=iXBlock)
							{
								pdDsdBuf[i][c]=1;
							}
							else
							{
								pdDsdBuf[iYBlock][iXBlock]=pDDsdBuf[iYBlock][iXBlock];
							}
						}
					}
				}
			}
			else
			{
				pdDsdBuf[iYBlock][iXBlock]=pDDsdBuf[iYBlock][iXBlock];
			}
		}
	}
	int hhh=0;
	for (int i=0;i<nHeight;i++)
	{
		for (int j=0;j<nWidth;j++)
		{
			pDDDRBuf[hhh++]=pdDsdBuf[i][j];
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
		std::cout<<"д���ļ�����"<<std::endl;
		return false;
	}
	//�ͷ��ڴ�
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