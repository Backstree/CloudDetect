#include <iostream>
#include "Include.h"
extern const int BufSize=512*512;
using namespace std;
int main()
{
	Float32 *pBufR=new Float32[BufSize];
	Float32 *pBufG=new Float32[BufSize];
	Float32 *pBufB=new Float32[BufSize];
	GByte *pRBuf=new GByte[];
	cout<<"���㿪ʼ��"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//��������RGB
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszRInputName="E:\\LC8\\4.TIF";
	const char *pszGInputName="E:\\LC8\\3.TIF";
	const char *pszBInputName="E:\\LC8\\2.TIF";
	const char *pszROutputName="E:\\third\\ReR.TIF";
	const char *pszGOutputName="E:\\third\\ReG.TIF";
	const char *pszBOutputName="E:\\third\\ReB.TIF";
	//ReRGB reRGB;
	//reRGB.Initilization(pszRInputName,pszGInputName,pszBInputName);
	//reRGB.Execute(pBufR,pszROutputName,pBufG,pszGOutputName,pBufB,pszBOutputName);
	cout<<"ת��RGB�����ˣ�"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//�������쵽01
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszRRInputName="E:\\third\\ReR.TIF";
	const char *pszRGInputName="E:\\third\\ReG.TIF";
	const char *pszRBInputName="E:\\third\\ReB.TIF";
	const char *pszLSROutputName="E:\\third\\LSR.TIF";
	const char *pszLSGOutputName="E:\\third\\LSG.TIF";
	const char *pszLSBOutputName="E:\\third\\LSB.TIF";
	//CLineStreach LineStreach;
	//LineStreach.Initilization(pszRRInputName,pszRGInputName,pszRBInputName);
	//LineStreach.Execute(pBufR,pszLSROutputName,pBufG,pszLSGOutputName,pBufB,pszLSBOutputName,0,1);
	cout<<"��������RGB�����ˣ�"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//����HSI
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComRInputName="E:\\third\\LSR.TIF";
	const char *pszComGInputName="E:\\third\\LSG.TIF";
	const char *pszComBInputName="E:\\third\\LSB.TIF";
	const char *pszHOutputName="E:\\third\\ComH.TIF";
	const char *pszSOutputName="E:\\third\\ComS.TIF";
	const char *pszIOutputName="E:\\third\\ComI.TIF";
	//CComputeHSI ComputeHSI;
	//ComputeHSI.Initilization(pszComRInputName,pszComGInputName,pszComBInputName);
	//ComputeHSI.Execute(pBufR,pszHOutputName,pBufG,pszSOutputName,pBufB,pszIOutputName);
	cout<<"����HSI�����ˣ�"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//��SI�������쵽01
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComSInputName="E:\\third\\ComS.TIF";
	const char *pszComIInputName="E:\\third\\ComI.TIF";
	const char *pszSingleLSSOutputName="E:\\third\\SingleLSS.TIF";
	const char *pszSingleLSIOutputName="E:\\third\\SingleLSI.TIF";
	CSingleLineStreach SinglelineStreach;
	//SinglelineStreach.Initilization(pszComSInputName);
	//SinglelineStreach.Execute(pBufR,pszSingleLSSOutputName,0,1);
	cout<<"��������S�����ˣ�"<<endl;
	//SinglelineStreach.Initilization(pszComIInputName);
	//SinglelineStreach.Execute(pBufR,pszSingleLSIOutputName,0,1);
	cout<<"��������I�����ˣ�"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//�������ֵJ���������쵽0-255
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszSingleLSSInputName="E:\\third\\SingleLSS.TIF";
	const char *pszSingleLSIInputName="E:\\third\\SingleLSI.TIF";
	const char *pszComJOutputName="E:\\third\\ComJ.TIF";
	const char *pszComLSJOutputName="E:\\third\\ComLSJ.TIF";
	//ComJ comJ;
	//comJ.Initilization(pszSingleLSSInputName,pszSingleLSIInputName);
	//comJ.Execute(pBufR,pBufG,pszComJOutputName);
		cout<<"����ֵJ��������ˣ�"<<endl;
	//SinglelineStreach.Initilization(pszComJOutputName);
	//SinglelineStreach.Execute(pBufR,pszComLSJOutputName,0,255);
	cout<<"����ֵJ���������������ˣ�"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//����Otsu�㷨���õ�T
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComLSJInputName="E:\\third\\ComLSJ.TIF";
	const char *pszJLSoutputName="E:\\third\\ComLSJ_GByte.TIF";
	CSingleLineStreach_GByte LsGByte;
	//LsGByte.Initilization(pszComLSJInputName);
	//LsGByte.Execute(pRBuf,pszJLSoutputName,0,255);
	cout<<"�������쵽0-266_GByte��ɣ�"<<endl;
	//Float32 T=0;
	Otsu otsu;
	//otsu.Initilization(pszJLSoutputName);
	//T=otsu.Execute(pRBuf,T);
	//cout<<"Otsu::T="<<T<<endl;
	cout<<"����Otsu������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//����ϸ��ͼ
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszInInputName="E:\\LC8\\5.TIF";
	const char *pszCompareOutputName="E:\\third\\Compare.TIF";
	//Compare compare;
	//compare.Initilization(pszJLSoutputName,pszInInputName,pszHOutputName);
	//compare.Execute(pBufR,pBufG,pBufB,pszCompareOutputName,T);
	cout<<"����Compare������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//Iֱ��ͼ���⻯
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszILSOutputName="E:\\third\\ILS_255.TIF";

	const char *pszILSOGByteutputName="E:\\third\\ILSGByte_255.TIF";
	//SinglelineStreach.Initilization(pszIOutputName);
	//SinglelineStreach.Execute(pBufR,pszILSOutputName,0,255);

	//LsGByte.Initilization(pszILSOutputName);
	//LsGByte.Execute(pRBuf,pszILSOGByteutputName,0,255);
	cout<<"I�������쵽0-256_GByte��ɣ�"<<endl;
	const char *pszHHisOutputName="E:\\third\\IHistogramEqualization.TIF";
	//CHistogramEqualization HistogramEqualization;
	//HistogramEqualization.Initilization(pszILSOGByteutputName);
	//HistogramEqualization.Execute(pRBuf,pszHHisOutputName);
	cout<<"Iֱ��ͼ���⻯������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//˫���˲�
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszBFOutputName="E:\\third\\BilaterlFiltering.TIF";
	//CBilateralFiltering BF;
	//int r=0;
	//cout<<"����R"<<endl;
	//cin>>r;
	//BF.Initilization(pszHHisOutputName);
	//BF.Execute(pBufR,pszBFOutputName,r);
	cout<<"˫���˲�������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//ϸ��ͼId
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszIdOutputName="E:\\third\\Comp_Id.TIF";
	//Comp_Id Id;
	//Id.Initilization(pszHHisOutputName,pszBFOutputName);
	//Id.Execute(pBufR,pBufG,pszIdOutputName);
	const char *pszId255OutputName="E:\\third\\Comp_Id_255.TIF";

	//LsGByte.Initilization(pszIdOutputName);
	//LsGByte.Execute(pRBuf,pszId255OutputName,0,255);
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//���޶�������Otsu��������
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	//Float32 T=0;
	//Otsu_Float otsu_float;
	//otsu_float.Initilization(pszHHisOutputName);
	//T=otsu_float.Execute(pBufR,T);
	//cout<<"Otsu_1::T="<<T<<endl;
	cout<<"����Otsu����!"<<endl;
	const char *pszOtsu1OutputName="E:\\third\\Otsu_1.TIF";
	//Otsu_1 otsu_1;
	//otsu_1.Initilization(pszHHisOutputName);
	//otsu_1.Execute(pBufR,pszOtsu1OutputName,T);
	//����ڶ���Otsu
	//otsu_float.Initilization(pszOtsu1OutputName);
	//T=otsu_float.Execute(pBufR,T);
	//cout<<"Otsu_2::T:"<<T<<endl;
	const char *pszOtsu2OutputName="E:\\third\\Otsu_2.TIF";
	/*Otsu_2 otsu_2;
	otsu_2.Initilization(pszOtsu1OutputName);
	otsu_2.Execute(pBufR,pszOtsu2OutputName,T);*/
	cout<<"����Otsu������"<<endl;
	const char *pszIRutputName="E:\\third\\IR.TIF";
	/*Compare_IR compare_ir;
	compare_ir.Initilization(pszCompareOutputName,pszOtsu2OutputName);
	compare_ir.Execute(pBufR,pBufG,pszIRutputName);*/
	cout<<"����������ͼ������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//����������ͼ��Ե
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszEdgeutputName="E:\\third\\Edge.TIF";
	/*EageCloude edgecloud;
	edgecloud.Initilization(pszIRutputName);
	edgecloud.Execute(pBufR,pszEdgeutputName);*/
	cout<<"�����Ʊ�Ե������"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//���ͼ���
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComI255putName="E:\\third\\ComI_255.TIF";
	const char *pszCloudExputName="E:\\third\\CloudExpansion.TIF";
	//SinglelineStreach.Initilization(pszComIInputName);
	//SinglelineStreach.Execute(pBufR,pszComI255putName,0,255);
	CloudExpansion cloudExpansion;
	cloudExpansion.Initilization(pszComI255putName,pszEdgeutputName);
	cloudExpansion.Execute(pBufR,pBufG,pszCloudExputName,0.008);
	cout<<"�������ͣ�"<<endl;
	const char *pszThinkCloudExputName="E:\\third\\ThinkCloud.TIF";
	ThickCloud thinkCloud;
	thinkCloud.Initilization(pszCloudExputName);
	thinkCloud.Execute(pBufR,pszThinkCloudExputName);



	cout<<"���ƽ�������ʼ���ƣ�"<<endl;
	const char *pszCloudexputName="E:\\third\\Expansion.TIF";
	Expansion expansion;
	expansion.Initilization(pszComI255putName,pszThinkCloudExputName);
	expansion.Execute(pBufR,pBufG,pszCloudexputName,0.3);




	cout<<"�������һ����"<<endl;
	const char *pszCloudexpansionputName="E:\\third\\ThinExpansion.TIF";
	expansion.Initilization(pszComI255putName,pszCloudexputName);
	expansion.Execute(pBufR,pBufG,pszCloudexpansionputName,0.012);


	cout<<"ȫ�����������"<<endl;
	system("pause");
	return 0;
}