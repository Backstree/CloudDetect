#ifdef LINESTREATCH_H_INCLUDE
#endif LINESTREATCH_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class CLineStreach
{
public:
	CLineStreach();
	~CLineStreach();
	bool Initilization(const string & strInputRFileName,
		const string & strInputGFileName,const string & strInputBFileName);
	bool Execute(Float32 *pRBuf,const string & strOutputHFileName,
		Float32 *pGBuf,const string & strOutputSFileName,
		Float32 *pBBuf,const string & strOutputIFileName,
		int AimsMin,int AimsMax);	
	bool LineStrecth(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		GDALDataset * pDrcDatasetR,GDALDataset * pDrcDatasetG,GDALDataset * pDrcDatasetB,
		Float32 *pRBuf,Float32 *pGBuf,Float32 *pBBuf,int AimsMin,int AimsMax);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strInputBFileName;//输入B文件名
	string m_strOutputHFileName;//输出H文件名
	string m_strOutputSFileName;//输出S文件名
	string m_strOutputIFileName;//输出I文件名
	Float32 adfRmaxmin[2];
	Float32 adfGmaxmin[2];
	Float32 adfBmaxmin[2];
	bool   m_bInit;
};


