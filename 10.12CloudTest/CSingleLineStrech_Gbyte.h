#ifdef SINGLELINESTREACH(GBYTE)_H_INCLUDE
#endif SINGLELINESTREACH(GBYTE)_H_INCLUDE
#include <string>
using std::string;
typedef unsigned char GByte;
typedef double Float32;
class GDALDataset;
class CSingleLineStreach_GByte
{
public:
	CSingleLineStreach_GByte();
	~CSingleLineStreach_GByte();
	bool Initilization(const string & strInputRFileName);
	bool Execute(GByte *pRBuf,const string & strOutputHFileName,int AimsMin,int AimsMax);	
	bool LineStrecth(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR,GByte *pRBuf,int AimsMin,int AimsMax);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};


