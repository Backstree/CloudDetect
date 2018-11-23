#ifdef HISTOGRAMEQUALIZATION_H_INCLUDE
#endif HISTOGRAMEQUALIZATION_H_INCLUDE
#include <string>
using std::string;
typedef unsigned char Gbyte;
typedef double Float32;
class GDALDataset;
class CHistogramEqualization
{
public:
	CHistogramEqualization();
	~CHistogramEqualization();
	bool Initilization(const string & strInputRFileName);
	bool Execute(const string & strOutputHFileName);	
	bool HistogramEqualization(GDALDataset * pSrcDatasetR,GDALDataset * pDrcDatasetR);
protected:
	string m_strInputRFileName;
	string m_strOutputHFileName;
	Float32 adfRmaxmin[2];
	bool   m_bInit;
};


