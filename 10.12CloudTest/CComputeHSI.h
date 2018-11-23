#ifdef COMPUTEHSI_H_INCLUDE
#endif COMPUTEHSI_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class CComputeHSI
{
public:
	CComputeHSI();
	~CComputeHSI();
	bool Initilization(const string & strInputRFileName,
		const string & strInputGFileName,const string & strInputBFileName);
	bool Execute(const string & strOutputHFileName,const string & strOutputSFileName,const string & strOutputIFileName);	
	bool ComputeHSI(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		GDALDataset * pDrcDatasetR,GDALDataset * pDrcDatasetG,GDALDataset * pDrcDatasetB);
	Float32 Max(Float32 a,Float32 b);
	Float32 Min(Float32 a,Float32 b);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strInputBFileName;//����B�ļ���
	string m_strOutputHFileName;//���H�ļ���
	string m_strOutputSFileName;//���S�ļ���
	string m_strOutputIFileName;//���I�ļ���
	Float32 *max;
	Float32 *min;
	bool   m_bInit;
};


