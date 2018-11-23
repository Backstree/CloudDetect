#include "CBilateralFiltering.h"
#include <iostream>
#include "gdal_priv.h"
CBilateralFiltering::CBilateralFiltering()
{
	string m_strInputRFileName=string();
	Float32 adfRmaxmin[2]={0.0,0.0};
	m_bInit = false;
}
CBilateralFiltering::~CBilateralFiltering()
{
}
//��ʼ��
bool CBilateralFiltering::Initilization(const string & strInputRFileName)
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
bool CBilateralFiltering::Execute(const string & strOutputHFileName,int r)
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

	if (false==BilateralFiltering(pSrcDatasetR,pDrcDatasetH,r))
	{
		GDALClose(pSrcDatasetR);
		pSrcDatasetR = 0;
		GDALClose(pDrcDatasetH);
		pDrcDatasetH = 0;
		std::cout<<"����˫���˲�����"<<std::endl;
		system("pause");
		return false;
	}	
	GDALClose(pSrcDatasetR);
	pSrcDatasetR = 0;
	GDALClose(pDrcDatasetH);
	pDrcDatasetH = 0;
	return true;
}
//˫���˲�
bool CBilateralFiltering::BilateralFiltering(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR,int r)
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
	int nXBlockSize,nYBlockSize,BX,BY;
	int iXBlock=0,iYBlock=0;
	int iXOffset,iYOffset;
	pSrcBandR->GetBlockSize(&nXBlockSize,&nYBlockSize);	
	//���ÿ�Ĵ�СΪ2*r+1
	if (nXBlockSize<64||nXBlockSize>512)		
	{
		nXBlockSize = 2*r+1;
		BX=513;
	}
	if (nYBlockSize<64||nYBlockSize>512)
	{
		nYBlockSize = 2*r+1;
		BY=513;
	}
	int XBL = (nWidth + 512 - 1)/512;
	int YBL = (nHeight + 512 - 1)/512;
	long int nBlockSize = nXBlockSize * nYBlockSize;
	long int BS=BX*BY;
	Float32 *pRBuf = new Float32[nBlockSize]; 
	Float32 *pDRBuf = new Float32[(BX-1)*(BY-1)]; 
	//�������Ӱ������飬��ȡʹ��
	Float32 *pDstBuf=new Float32 [BX*BY];
	Float32 **pDDsdBuf=new Float32 *[BY];
	for (int i=0;i<BY;i++)
	{
		pDDsdBuf[i]=new Float32 [BX];
	}
	int jj=0;
	//������Ӱ��ת��Ϊ��ά����
	jj=0;
	//��ȡ2*r+1��ͼ������
	Float32 **psdBuf=new Float32*[BY];
	for (int i=0;i<BY;i++)
	{
		psdBuf[i]=new Float32[BX];

	}
	//����Ľ������
	Float32 **pDDDsdBuf=new Float32 *[BY];
	for (int ii=0;ii<BY;ii++)
	{
		pDDDsdBuf[ii]=new Float32 [BX];
	}
	//ͳ��Ӱ�����ֵ����Сֵ
	int bRGoMin=0,bRGoMax=0;
	adfRmaxmin[0]=pSrcBandR->GetMaximum(&bRGoMax);
	adfRmaxmin[1]=pSrcBandR->GetMinimum(&bRGoMax);
	if (!(bRGoMax&&bRGoMin))
	{
		GDALComputeRasterMinMax(pSrcBandR,FALSE,adfRmaxmin);
	}
	std::cout<<adfRmaxmin[0]<<"	"<<adfRmaxmin[1]<<std::endl;
	//ѭ������Ӱ��
	int w=0,h=0;
	for (int iYBL=0;iYBL<YBL;iYBL++)
	{
		iYOffset = iYBL*(BY-1);
		for (int iXBL=0;iXBL<XBL;iXBL++)
		{
			iXOffset = iXBL*(BX-1);
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
			if (pSrcBandR->RasterIO(GF_Read,iXOffset,iYOffset,w,h,pDstBuf,w,h,GDT_Float64,0,0) != CE_None)
			{
				delete [] pDstBuf;
				pDstBuf = 0;
				std::cout<<"��ȡ�ļ�����"<<std::endl;
				return false;
			}
			OneArr2TwoArr(pDstBuf,pDDsdBuf,0,h,0,w);
			for (iYBlock=0; iYBlock<h; iYBlock++)
			{
				for (iXBlock=0; iXBlock<w; iXBlock++)
				{	
					if (pDDsdBuf[iYBlock][iXBlock]!=0)
					{
						double k = 0;
						double f = 0;
						//��ȡһ��2*r+1��Ӱ���
						//�߽��ж�
						if (iXBlock+r<w&&iYBlock+r<h&&iXBlock-r>=0&&iYBlock-r>=0)
						{
							if (pSrcBandR->RasterIO(GF_Read,iXBlock-r,iYBlock-r,2*r+1,2*r+1,pRBuf,2*r+1,2*r+1,GDT_Float64,0,0) != CE_None)
							{
								delete [] pRBuf;
								pRBuf = 0;
								std::cout<<"��ȡ�ļ�����"<<std::endl;
								return false;
							}
							//��2*r+1��Ӱ���ת���ɶ�ά����->psdBuf
							int jj = 0;
							for (int i = 0; i < 2*r+1; i++)
							{
								for (int c = 0; c < 2*r+1; c++)
								{

									psdBuf[iYBlock-1+i][iXBlock-1+c] = pRBuf[jj++];

								}
								if (jj > 9)
								{
									break;
								}
							}
							for (int i = iYBlock-r; i <=iYBlock+r; i++)
							{
								for (int c = iXBlock-r; c <=iXBlock+r; c++)
								{
									if (i!=iYBlock && c!=iXBlock)
									{
										f +=(pDDsdBuf[i][c]) * SpatialWeight(iYBlock,iXBlock, i, c) * ColorWeight((pDDsdBuf[iYBlock][iXBlock]),(psdBuf[i][c]));
										k += SpatialWeight(iYBlock,iXBlock, i, c) *ColorWeight((pDDsdBuf[iYBlock][iXBlock]),(psdBuf[i][c]));
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
								//��ά����
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
					else
					{
						pDDDsdBuf[iYBlock][iXBlock]=pDDsdBuf[iYBlock][iXBlock];
					}
				}
			}
			TwoArr2OneArr(pDDDsdBuf,pDRBuf,0,h-1,0,w-1);
			if (pDstBandR->RasterIO(GF_Write,iXOffset,iYOffset,w-1,h-1,pDRBuf,w-1,h-1,GDT_Float64,0,0) != CE_None)
			{

				delete [] pDRBuf;
				pDRBuf = 0;

				std::cout<<"д���ļ�����"<<std::endl;
				return false;
			}
		}
	}	
	delete [] pRBuf;
	pRBuf = 0;
	delete [] pDstBuf;
	pDstBuf = 0;
	for(int i = 0; i < 2*r+1; i++)
	{
		delete [] psdBuf[i];
	}
	delete [] psdBuf;
	for(int i = 0; i < BY; i++)
	{
		delete [] pDDsdBuf[i];
	}
	delete [] pDDsdBuf;
	//�Ѵ�����ת����һά����
	//�������Ӱ������飬д��ʹ��
	//TwoArr2OneArr(pDDDsdBuf,pDRBuf,0,BY,0,BX);
	for(int i = 0; i < BY; i++)
	{
		delete [] pDDDsdBuf[i];
	}
	delete [] pDDDsdBuf;
	//�ͷ��ڴ�
	delete [] pDRBuf;
	pDRBuf = 0;	
	return true;
}
//�ռ�Ȩ��
double CBilateralFiltering::SpatialWeight(int nwidth,int nheight,int row,int col)
{
	double abX=pow((nwidth-row),2);
	double abY=pow((nheight-col),2);
	return exp(-(abX+abY)/8);
}
//����ֵȨ��
double CBilateralFiltering:: ColorWeight(Float32 nwidth,Float32 nheight)
{
	//ŷ�Ͼ���
	return exp(-pow(nwidth-nheight,2)/(2*pow(adfRmaxmin[1]/10,2)));
}
void CBilateralFiltering::TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth)
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
void CBilateralFiltering::OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth)
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