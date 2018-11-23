#ifdef RERGB_H_INCLUDE
#endif RERGB_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class ReRGB
{
public:
	ReRGB();
	~ReRGB();
	bool Initilization(const string & strInputRFileName,
		const string & strInputGFileName,const string & strInputBFileName);
	bool Execute(const string & strOutputRFileName,const string & strOutputGFileName,const string & strOutputBFileName);	
	bool reRGB(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pSrcDatasetB,
		 GDALDataset * pDrcDatasetR,GDALDataset * pDrcDatasetG,GDALDataset * pDrcDatasetB);
	Float32 Max(Float32 a,Float32 b);
	Float32 Min(Float32 a,Float32 b);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strInputBFileName;//����B�ļ���
	string m_strOutputRFileName;//���R�ļ���
	string m_strOutputGFileName;//���G�ļ���
	string m_strOutputBFileName;//���B�ļ���
	bool   m_bInit;
	Float32 *mid;
	Float32 *max;
	Float32 *min;
	int m_nBandCouts;
};


