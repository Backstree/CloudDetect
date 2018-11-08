#ifdef CLOUDEXPANSION_H_INCLUDE
#endif CLOUDEXPANSION_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class CloudExpansion
{
public:
	CloudExpansion();
	~CloudExpansion();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(Float32 *pRBuf,Float32 *pGBuf,const string & strOutputRFileName,Float32 k);	
	bool cloudExpansion(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR,Float32 *pRBuf,Float32 *pGBuf,Float32 k);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};