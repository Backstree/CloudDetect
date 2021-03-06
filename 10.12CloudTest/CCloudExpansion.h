#ifdef CLOUDEXPANSION_H_INCLUDE
#endif CLOUDEXPANSION_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class CloudExpansion
{
public:
	CloudExpansion();
	~CloudExpansion();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(const string & strOutputRFileName,Float32 k);	
	void TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth);
	void OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth);
	bool cloudExpansion(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR,Float32 k);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strOutputHFileName;//输出H文件名
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};