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
	bool Execute(const string & strOutputHFileName,const string & strOutputSFileName,const string & strOutputIFileName,
		int AimsMin,int AimsMax);	
	bool LineStrecth(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		GDALDataset * pDrcDatasetR,GDALDataset * pDrcDatasetG,GDALDataset * pDrcDatasetB,int AimsMin,int AimsMax);
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


