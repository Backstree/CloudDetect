#ifdef OUST_1_H_INCLUDE
#endif OUST_1_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Otsu_1
{
public:
	Otsu_1();
	~Otsu_1();
	bool Initilization(const string & strInputRFileName);
	bool Execute(Float32 *pRBuf,const string & strOutputBFileName,int T);	
	bool otsu_1(GDALDataset * pSrcDatasetR,	GDALDataset * pDrcDatasetR,	Float32 *pRBuf,int T);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	int m_nBandCouts;
};


