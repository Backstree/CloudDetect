#ifdef COMPARE_H_INCLUDE
#endif COMPARE_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Compare
{
public:
	Compare();
	~Compare();
	bool Initilization(const string & strInputRFileName,
		const string & strInputGFileName,const string & strInputBFileName);
	bool Execute(Float32 *pRBuf,Float32 *pGBuf,Float32 *pBBuf,const string & strOutputBFileName,int T);	
	bool compare(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		GDALDataset * pDrcDatasetR,	Float32 *pRBuf,Float32 *pGBuf,Float32 *pBBuf,int T);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strInputBFileName;//����B�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	int m_nBandCouts;
};


