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
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strInputBFileName;//输入B文件名
	string m_strOutputRFileName;//输出R文件名
	string m_strOutputGFileName;//输出G文件名
	string m_strOutputBFileName;//输出B文件名
	bool   m_bInit;
	Float32 *mid;
	Float32 *max;
	Float32 *min;
	int m_nBandCouts;
};


