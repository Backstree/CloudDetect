#ifdef COMJ_H_INCLUDE
#endif COMJ_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class ComJ
{
public:
	ComJ();
	~ComJ();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(const string & strOutputRFileName);	
	bool comJ(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strOutputRFileName;//输出R文件名
	bool   m_bInit;

};