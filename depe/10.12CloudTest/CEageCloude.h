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
	bool Execute(Float32 *pRBuf,const string & strOutputHFileName);	
	bool eageCloude(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR,Float32 *pRBuf);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strOutputHFileName;//输出H文件名
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};