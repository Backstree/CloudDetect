#ifdef EXPANSION_H_INCLUDE
#endif EXPANSION_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class Expansion
{
public:
	Expansion();
	~Expansion();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(const string & strOutputRFileName,Float32 k);	
	bool expansion(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR,Float32 k);
	void TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth);
	void OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};