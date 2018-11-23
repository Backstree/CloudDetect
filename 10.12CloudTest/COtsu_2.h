#ifdef OUST_2_H_INCLUDE
#endif OUST_2_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Otsu_2
{
public:
	Otsu_2();
	~Otsu_2();
	bool Initilization(const string & strInputRFileName);
	bool Execute(const string & strOutputBFileName,int T);	
	bool otsu_2(GDALDataset * pSrcDatasetR,	GDALDataset * pDrcDatasetR,int T);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	int m_nBandCouts;
};


