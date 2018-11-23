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
	bool Execute(const string & strOutputHFileName,int r);	
	bool BilateralFiltering(GDALDataset * pSrcDatasetR, GDALDataset * pDrcDatasetR,int r);
	double SpatialWeight(int nwidth,int nheight,int row,int col);
	double ColorWeight(double nwidth,double nheight);
	void TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth);
	void OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strOutputHFileName;//输出H文件名
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};