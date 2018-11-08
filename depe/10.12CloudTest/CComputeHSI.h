#ifdef COMPUTEHSI_H_INCLUDE
#endif COMPUTEHSI_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class CComputeHSI
{
public:
	CComputeHSI();
	~CComputeHSI();
	bool Initilization(const string & strInputRFileName,
		const string & strInputGFileName,const string & strInputBFileName);
	bool Execute(Float32 *pRBuf,const string & strOutputHFileName,
		Float32 *pGBuf,const string & strOutputSFileName,
		Float32 *pBBuf,const string & strOutputIFileName);	
	bool ComputeHSI(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		GDALDataset * pDrcDatasetR,GDALDataset * pDrcDatasetG,GDALDataset * pDrcDatasetB,
		Float32 *pRBuf,Float32 *pGBuf,Float32 *pBBuf);
	Float32 Max(Float32 a,Float32 b);
	Float32 Min(Float32 a,Float32 b);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strInputBFileName;//输入B文件名
	string m_strOutputHFileName;//输出H文件名
	string m_strOutputSFileName;//输出S文件名
	string m_strOutputIFileName;//输出I文件名
	Float32 *max;
	Float32 *min;
	bool   m_bInit;
};


