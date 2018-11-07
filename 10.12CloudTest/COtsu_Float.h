#ifdef COTSU_FLOAT_H_INCLUDE
#endif COTSU_FLOAT_H_INCLUDE
#include <string>
using std::string;
typedef double Float32;
class GDALDataset;
class Otsu_Float
{
public:
	Otsu_Float();
	~Otsu_Float();
	bool Initilization(const string & strInputRFileName);
	Float32 Execute(Float32 *pRBuf,double T);	
	Float32 otsu_float(GDALDataset * pSrcDatasetR,Float32 *pRBuf);
protected:
	string m_strInputRFileName;//输入R文件名
	string m_strOutputRFileName;//输出R文件名
	bool   m_bInit;
	bool   m_bDouble;
};


