#ifdef COMPARE_IR_H_INCLUDE
#endif COMPARE_IR_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Compare_IR
{
public:
	Compare_IR();
	~Compare_IR();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(Float32 *pRBuf,Float32 *pGBuf,const string & strOutputBFileName);	
	bool compare_IR(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,
			GDALDataset * pDrcDatasetR,	Float32 *pRBuf,Float32 *pGBuf);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	int m_nBandCouts;
};


