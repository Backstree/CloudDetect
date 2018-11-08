#ifdef CBILATERAIFILTERING_H_INCLUDE
#endif CBILATERAIFILTERING_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class CBilateralFiltering
{
public:
	CBilateralFiltering();
	~CBilateralFiltering();
	bool Initilization(const string & strInputRFileName);
	bool Execute(Float32 *pRBuf,const string & strOutputHFileName,int r);	
	bool BilateralFiltering(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR,Float32 *pRBuf,Float32 **pDsdBuf,int r);
	double SpatialWeight(int nwidth,int nheight,int row,int col);
	double ColorWeight(double nwidth,double nheight);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};