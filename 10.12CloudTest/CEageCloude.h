#ifdef EAGECLOUDE_H_INCLUDE
#endif EAGECLOUDE_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class EageCloude
{
public:
	EageCloude();
	~EageCloude();
	bool Initilization(const string & strInputRFileName);
	bool Execute(const string & strOutputHFileName);	
	bool eageCloude(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR);
	void TwoArr2OneArr(Float32 **pBuf,Float32 *pRBuf,int ii,int nHeight,int cc,int nWidth);
	void OneArr2TwoArr(Float32 *pBuf,Float32 **pRBuf,int ii,int nHeight,int cc,int nWidth);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputHFileName;//���H�ļ���
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};