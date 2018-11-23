#ifdef COMP_ID_H_INCLUDE
#endif COMP_ID_H_INCLUDE
#include <string>
using std::string;
typedef unsigned short UInt16;
typedef double Float32;
class GDALDataset;
class Comp_Id
{
public:
	Comp_Id();
	~Comp_Id();
	bool Initilization(const string & strInputRFileName,const string & strInputGFileName);
	bool Execute(const string & strOutputRFileName);	
	bool comp_Id(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,GDALDataset * pDrcDatasetR);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strInputGFileName;//输入G文件名
	string m_strOutputRFileName;//输出R文件名
	bool   m_bInit;

};
