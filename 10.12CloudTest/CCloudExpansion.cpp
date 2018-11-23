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
//��ʼ��
bool CloudExpansion::Initilization(const string & strInputRFileName,const string & strInputGFileName)
{
	if (strInputRFileName.empty()||strInputGFileName.empty())
	{
		std::cout<<"�ļ���Ϊ�գ�";
		system("pause");
		return false;
	}
	m_strInputGFileName=strInputGFileName;
	m_strInputRFileName=strInputRFileName;
	//ע����������
	GDALAllRegister();
	GDALDataset *pDatasetR=(GDALDataset*)GDALOpen(m_strInputRFileName.c_str(),GA_Update);
	GDALDataset *pDatasetG=(GDALDataset*)GDALOpen(m_strInputGFileName.c_str(),GA_Update);
	if (pDatasetG==0||pDatasetR==0)
	{
		m_strInputGFileName.erase(m_strInputGFileName.begin(),m_strInputGFileName.end());
		m_strInputRFileName.erase(m_strInputRFileName.begin(),m_strInputRFileName.end());
		std::cout<<"�ļ��򿪴���"<<std::endl;
		system("pause");
		return false;
	}

	GDALClose(pDatasetR);
	GDALClose(pDatasetG);
	m_bInit=true;
	return true;
}
//ִ��
bool CloudExpansion::Execute(const string & strOutputHFileName,Float32 k)
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
	GDALDataset * pSrcDatasetG = (GDALDataset *) GDALOpen(m_strInputGFileName.c_str(),GA_Update);
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
	if (false==cloudExpansion(pSrcDatasetR,pSrcDatasetG,pDrcDatasetH,k))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"�������ͳ���"<<std::endl;
		system("pause");
		return false;
	}
	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}
bool CloudExpansion::cloudExpansion(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR,Float32 k)
{
	int nWidth,nHeight;
	nWidth = pSrcDatasetR->GetRasterXSize();
	nHeight = pSrcDatasetR->GetRasterYSize();
	GDALRasterBand * pSrcBandR = pSrcDatasetR->GetRasterBand(1);
	GDALRasterBand * pSrcBandG = pSrcDatasetG->GetRasterBand(1);
	GDALRasterBand * pDstBandR = pDrcDatasetR->GetRasterBand(1);
	if (pSrcBandR->GetRasterDataType() != GDT_Float32||pSrcBandG->GetRasterDataType() != GDT_Float32)
	{
		std::cout<<"��ʽ����GDT_Float32"<<std::endl;
		system("pause");
		return false;
	}
	int nXBlockSize,nYBlockSize,BX,BY;
	int iXBlock=0,iYBlock=0;
	int iXOffset,iYOffset;
	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	
	//���ÿ�Ĵ�СΪ3
	if (nXBlockSize<64||nXBlockSize>512)		
	{
		nXBlockSize = 3;
		BX=513;
	}
	if (nYBlockSize<64||nYBlockSize>512)
	{
		nYBlockSize = 3;
		BY=513;
	}
	int nXBlocks = (nWidth + nXBlockSize - 1)/nXBlockSize;
	int nYBlocks = (nHeight + nYBlockSize - 1)/nYBlockSize;
	int XBL = (nWidth + 512 - 1)/512;
	int YBL = (nHeight + 512 - 1)/512;
	long int BS=BX*BY;
	long int nBlockSize = nXBlockSize * nYBlockSize;
	Float32* pRBuf = new Float32[BS]; 
	//�Ѷ�ȡӰ��תΪ��ά����	
	Float32 **pDDsdBuf=new Float32 *[BY];
	for (int i=0;i<BY;i++)
	{
		pDDsdBuf[i]=new Float32 [BX];
	}
	//������Ӱ��ת��Ϊ��ά����
	Float32* pGBuf = new Float32[BY*BX]; 
	//�Ѷ�ȡӰ��תΪ��ά����
	Float32 **pDDsdGBuf=new Float32 *[BY];
	for (int i=0;i<BY;i++)
	{
		pDDsdGBuf[i]=new Float32 [BX];
	}
	Float32 *pDDDRBuf = new Float32[(BX-1)*(BY-1)];
	Float32 **psdBuf=new Float32 *[BY];
	for (int i=0;i<BY;i++)
	{
		psdBuf[i]=new Float32 [BX];
	}
	Float32 **pdDsdBuf=new Float32 *[BY];
	for (int i=0;i<BY;i++)
	{
		pdDsdBuf[i]=new Float32 [BX];
	}
	Float32 *pDRBuf = new Float32[9];
	int D=0,w=0,h=0;
	for (int T = 0; T < 3; T++)
	{
		//Ӱ��ֿ鴦��
		for (int iYBL=0;iYBL<YBL;iYBL++)
		{
			iYOffset=iYBL*(BY-1);
			for (int iXBL=0;iXBL<XBL;iXBL++)
			{
				iXOffset=iXBL*(BX-1);
				w = BX;
				h = BY;
				//Ӱ���Ե����һ����Ŀ�ͳ�ʱ�Ĳ���
				if (iXBL == XBL - 1)
				{
					w = nWidth - iXBL*(BX-1);				
				}
				if (iYBL == YBL - 1)
				{
					h = nHeight - iYBL*(BY-1);				
				}
				BS = w*h;
				//��ȡӰ��
				if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pRBuf,w,h,GDT_Float64,0,0) != CE_None)
				{
					delete [] pRBuf;
					pRBuf = 0;
					std::cout<<"��ȡ�ļ�����"<<std::endl;
					return false;
				}				
				OneArr2TwoArr(pRBuf,pDDsdBuf,0,h,0,w);
				//��ȡӰ��
				if (pSrcBandG->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pGBuf,w,h,GDT_Float64,0,0) != CE_None)
				{
					delete [] pGBuf;
					pGBuf = 0;
					std::cout<<"��ȡ�ļ�����"<<std::endl;
					return false;
				}
				OneArr2TwoArr(pGBuf,pDDsdGBuf,0,h,0,w);
				//��ÿ����д���
				for (iYBlock=0; iYBlock<h; iYBlock++)
				{
					for (iXBlock=0; iXBlock<w; iXBlock++)
					{
						//�����ش���
						if (pDDsdGBuf[iYBlock][iXBlock]==1)
						{
							//��������Ϊ���ĵ�3*3���ڿ�
							if (iXBlock+1<w&&iYBlock+1<h&&iXBlock-1>=0&&iYBlock-1>=0)
							{

								if (pSrcBandR->RasterIO(GF_Read,iXBlock-1,iYBlock-1,3,3,pDRBuf,3,3,GDT_Float64,0,0) != CE_None)
								{
									delete [] pDRBuf;
									pDRBuf = 0;
									std::cout<<"��ȡ�ļ�����"<<std::endl;
									return false;
								}
								//��3��Ӱ���ת���ɶ�ά����->psdBuf
								int jj = 0;
								for (int i = 0; i < 3; i++)
								{
									for (int c = 0; c < 3; c++)
									{

										psdBuf[iYBlock-1+i][iXBlock-1+c] = pDRBuf[jj++];

									}
									if (jj > 9)
									{
										break;
									}
								}
								for (int i = iYBlock-1; i <=iYBlock+1; i++)
								{
									for (int c = iXBlock-1; c <=iXBlock+1; c++)
									{
										if(pDDsdGBuf[i][c]==0&&i!=iYBlock&&c!=iXBlock)
										{
											if(abs(pDDsdBuf[iYBlock][iXBlock]-psdBuf[i][c])<k*pDDsdBuf[iYBlock][iXBlock])
											{
												pDDsdGBuf[i][c]=1;
												D++;
											}
											else
											{
												pdDsdBuf[i][c]=pDDsdGBuf[i][c];
											}
										}
										else
										{
											pdDsdBuf[i][c]=pDDsdGBuf[i][c];
										}
									}
								}
							}
							else
							{
								pdDsdBuf[iYBlock][iXBlock]=pDDsdGBuf[iYBlock][iXBlock];
							}
						}
						else
						{
							pdDsdBuf[iYBlock][iXBlock]=pDDsdGBuf[iYBlock][iXBlock];
						}
					}
				}
				TwoArr2OneArr(pdDsdBuf,pDDDRBuf,0,h-1,0,w-1);
				//д��Ӱ��
				if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,(w-1),(h-1),pDDDRBuf,(w-1),(h-1),GDT_Float64,0,0) != CE_None)
				{
					delete [] pDDDRBuf;
					pDDDRBuf = 0;
					std::cout<<"д���ļ�����"<<std::endl;
					return false;
				}
			}
		}
		if (D<200)
		{
			break;
		}
		D=0;
	}
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pGBuf;
	pGBuf = 0;	
	delete [] pDRBuf;
	pDRBuf = 0;
	for(int i = 0; i <BY; i++)
	{
		delete [] pDDsdGBuf[i];
	}
	delete [] pDDsdGBuf;
	for(int i = 0; i <BY; i++)
	{
		delete [] pDDsdBuf[i];
	}
	delete [] pDDsdBuf;	
	for(int i = 0; i <BY; i++)
	{
		delete [] psdBuf[i];
	}
	delete [] psdBuf;
	
	//TwoArr2OneArr(pdDsdBuf,pDDDRBuf,0,nHeight,0,nWidth);
	for(int i = 0; i <BY; i++)
	{
		delete [] pdDsdBuf[i];
	}
	delete [] pdDsdBuf;
	//if (pDstBandR->RasterIO(GF_Write,0,0,nWidth,nHeight,pDDDRBuf,nWidth,nHeight,GDT_Float64,0,0) != CE_None)
	//{
	//	delete [] pDDDRBuf;
	//	pDDDRBuf = 0;
	//	std::cout<<"д���ļ�����"<<std::endl;
	//	return false;
	//}
	//�ͷ��ڴ�
	delete [] pDDDRBuf;
	pDDDRBuf = 0;
	return true;
}
void CloudExpansion::TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth)
{
	int hhh=0;
	for (int i=ii;i<nHeight;i++)
	{
		for (int j=cc;j<nWidth;j++)
		{
			pRBuf[hhh++]=pBuf[i][j];
		}
		if (hhh>(nWidth-cc)*(nHeight-ii))
		{
			break;
		}
	}
}
void CloudExpansion::OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth)
{
	int jj=0;
	for (int i =ii; i <nHeight; i++)
	{
		for (int c =cc; c <nWidth; c++)
		{

			pRBuf[i][c]=pBuf[jj++];

		}
		if (jj>(nWidth-cc)*(nHeight-ii))
		{
			break;
		}
	}	
}