#ifdef SINGLELINESTREACH_H_INCLUDE
#endif SINGLELINESTREACH_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class CSingleLineStreach
{
public:
	CSingleLineStreach();
	~CSingleLineStreach();
	bool Initilization(const string & strInputRFileName);
	bool Execute(const string & strOutputHFileName,int AimsMin,int AimsMax);	
	bool LineStrecth(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR,int AimsMin,int AimsMax);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};


