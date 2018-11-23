#ifdef COMPARE_IR_H_INCLUDE
#endif COMPARE_IR_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Compare_IR
{
public:
	Compare_IR();
	~Compare_IR();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(const string & strOutputBFileName);	
	bool compare_IR(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strOutputRFileName;//输出R文件名
	bool   m_bInit;
	int m_nBandCouts;
};


