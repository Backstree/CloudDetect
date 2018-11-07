#include "COtsu_Float.h"
#include <iostream>
#include "gdal_priv.h"
Otsu_Float::Otsu_Float()
{
	string m_strInputRFileName=string();
	m_bInit = false;
	m_bDouble=false;
}
Otsu_Float::~Otsu_Float()
{
}
//��ʼ��
bool Otsu_Float::Initilization(const string & strInputRFileName)
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
Float32 Otsu_Float::Execute(Float32 *pRBuf,double T)
{
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

	T=otsu_float(pSrcDatasetR,pRBuf);
	if (false==m_bDouble)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;

		std::cout<<"����Otsu����"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	return T;
}

//����Otsu
Float32 Otsu_Float::otsu_float(GDALDataset * pSrcDatasetR, Float32 *pRBuf)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"��ʽ����"<<std::endl;
		return false;
	}
	pRBuf = new Float32[nWidth*nHeight]; 
	///////////////////////////////////////////////////////////
	//-------------------------------------------------------->
	//����Otsu��ֵ
	//-------------------------------------------------------->
	///////////////////////////////////////////////////////////
	int anHistogram[256];
	pSrcBandR->GetHistogram(-0.5,255.5,256,anHistogram,FALSE,FALSE,GDALDummyProgress,NULL);
	int t=0;
	double u1=0,u2=0,w1=0,w2=0;
	double G=0,T=0;
	for (int Tem=0;Tem<256;Tem++)
	{
		double Gray1=0,Gray2=0;
		double  N1=0,N2=0;
		for (int j = 0; j <= Tem; j++)
		{

			N1+=anHistogram[j];
			Gray1+=j*anHistogram[j];
		}
		u1=(N1/(nWidth*nHeight));
		w1=Gray1/N1;
		for (int k = Tem+1; k <256; k++)
		{
			N2+=anHistogram[k];
			Gray2+=k*anHistogram[k];
		}

		w2=Gray2/N2;
		u2=(N2/(nWidth*nHeight));
		if ((N1+N2)!=(nWidth*nHeight))
		{
			std::cout<<"���ݲ���ȣ�"<<std::endl;
			system("pause");
			return false;
		}
		if ((u1+u2)!=1)
		{
			std::cout<<"������Ӳ���һ��"<<std::endl;
			system("pause");
			return false;
		}

		G=u1*u2*((w1-w2)*(w1-w2));
		if (G<0)
		{
			std::cout<<"GΪ��ֵ��"<<std::endl;
			system("pause");
		}
		if (G>t)
		{
			t=G;
			T=Tem;
		}	
	}
	delete [] pRBuf;
	pRBuf = 0;
	m_bDouble=true;
	return T;
}