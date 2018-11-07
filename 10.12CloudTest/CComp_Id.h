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
	bool Execute(Float32 *pRBuf,Float32 *pGBuf,const string & strOutputRFileName);	
	bool comp_Id(GDALDataset * pSrcDatasetR,GDALDataset * pSrcDatasetG,
		GDALDataset * pDrcDatasetR,Float32 *pRBuf,Float32 *pGBuf);
protected:
	string m_strInputRFileName;//����R�ļ���
	string m_strInputGFileName;//����G�ļ���
	string m_strOutputRFileName;//���R�ļ���
	bool   m_bInit;

};
