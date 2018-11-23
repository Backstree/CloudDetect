#include "COtsu.h"
#include <iostream>
#include "gdal_priv.h"
Otsu::Otsu()
{
	string m_strInputRFileName=string();
	m_bInit = false;
	m_bDouble=false;
}
Otsu::~Otsu()
{
}
//初始化
bool Otsu::Initilization(const string & strInputRFileName)
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
Float32 Otsu::Execute(double T)
{
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

	 T=otsu(pSrcDatasetR);
	if (false==m_bDouble)
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
	
		std::cout<<"计算Otsu出错！"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	return T;
}

//计算Otsu
Float32 Otsu::otsu(GDALDataset * pSrcDatasetR)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"格式错误"<<std::endl;
		return false;
	}
	GByte* pRBuf = new GByte[nWidth*nHeight]; 
	///////////////////////////////////////////////////////////
	//-------------------------------------------------------->
	//计算Otsu阈值
	//-------------------------------------------------------->
	///////////////////////////////////////////////////////////
	int anHistogram[256];
	pSrcBandR->GetHistogram(-0.5,255.5,256,anHistogram,FALSE,FALSE,GDALDummyProgress,NULL);
	double t=0;
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
			std::cout<<"数据不相等！"<<std::endl;
			system("pause");
			return false;
		}
		if ((u1+u2)!=1)
		{
			std::cout<<"数据相加不等一！"<<std::endl;
			system("pause");
			return false;
		}
		
		G=u1*u2*((w1-w2)*(w1-w2));
		//G=u1*((w1*u1+w2*u2)-w1)*((w1*u1+w2*u2)-w1)+u2*((w1*u1+w2*u2)-w2)*((w1*u1+w2*u2)-w2);
		if (G<0)
		{
			std::cout<<"G为负值！"<<std::endl;
			system("pause");
		}
		if (G>t)
		{
			t=G;
			T=Tem;
		}	
	}
	if (T>130)
	{
		T=130;
	}
	if (T<80)
	{
		T=80;
	}
	delete [] pRBuf;
	pRBuf = 0;
	m_bDouble=true;
	return T;
}