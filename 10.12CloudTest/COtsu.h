#ifdef COTSU_H_INCLUDE
#endif COTSU_H_INCLUDE
#include <string>
using std::string;
typedef unsigned char GByte;
typedef double Float32;
class GDALDataset;
class Otsu
{
public:
	Otsu();
	~Otsu();
	bool Initilization(const string & strInputRFileName);
	Float32 Execute(double T);	
	Float32 otsu(GDALDataset * pSrcDatasetR);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;
	bool   m_bDouble;
};


