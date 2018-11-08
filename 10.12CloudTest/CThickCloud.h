#ifdef THICKCLOUD_H_INCLUDE
#endif THICKCLOUD_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class ThickCloud
{
public:
	ThickCloud();
	~ThickCloud();
	bool Initilization(const string & strInputRFileName);
	bool Execute(Float32 *pRBuf,const string & strOutputBFileName);	
	bool thickCloud(GDALDataset * pSrcDatasetR,	GDALDataset * pDrcDatasetR,	Float32 *pRBuf);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	int m_nBandCouts;
};