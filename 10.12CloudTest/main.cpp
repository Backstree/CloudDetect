#include <iostream>
#include "Include.h"
extern const int BufSize=512*512;
using namespace std;
int main()
{
	int count=0;
	double time;
loop:cout<<"计算开始！"<<endl;
	clock_t a,b;
	a=clock();
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//重新排序RGB
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	//system("pause");
	const char *pszRInputName="E:\\LC8\\4.TIF";
	const char *pszGInputName="E:\\LC8\\3.TIF";
	const char *pszBInputName="E:\\LC8\\2.TIF";
	const char *pszROutputName="E:\\third\\1.1ReR.TIF";
	const char *pszGOutputName="E:\\third\\1.2ReG.TIF";
	const char *pszBOutputName="E:\\third\\1.3ReB.TIF";
	ReRGB reRGB;
	reRGB.Initilization(pszRInputName,pszGInputName,pszBInputName);
	reRGB.Execute(pszROutputName,pszGOutputName,pszBOutputName);
	cout<<"转化RGB结束了！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//线性拉伸到01
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszLSROutputName="E:\\third\\2.1LSR.TIF";
	const char *pszLSGOutputName="E:\\third\\2.2LSG.TIF";
	const char *pszLSBOutputName="E:\\third\\2.3LSB.TIF";
	CLineStreach LineStreach;
	LineStreach.Initilization(pszROutputName,pszGOutputName,pszBOutputName);
	LineStreach.Execute(pszLSROutputName,pszLSGOutputName,pszLSBOutputName,0,1);
	cout<<"线性拉伸RGB结束了！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//计算HSI
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszHOutputName="E:\\third\\3.1ComH.TIF";
	const char *pszSOutputName="E:\\third\\3.2ComS.TIF";
	const char *pszIOutputName="E:\\third\\3.3ComI.TIF";
	CComputeHSI ComputeHSI;
	ComputeHSI.Initilization(pszLSROutputName,pszLSGOutputName,pszLSBOutputName);
	ComputeHSI.Execute(pszHOutputName,pszSOutputName,pszIOutputName);
	cout<<"计算HSI结束了！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//把SI线性拉伸到01
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszSingleLSSOutputName="E:\\third\\4.1SingleLSS.TIF";
	const char *pszSingleLSIOutputName="E:\\third\\4.2SingleLSI.TIF";
	CSingleLineStreach SinglelineStreach;
	SinglelineStreach.Initilization(pszSOutputName);
	SinglelineStreach.Execute(pszSingleLSSOutputName,0,1);
	cout<<"线性拉伸S结束了！"<<endl;
	SinglelineStreach.Initilization(pszIOutputName);
	SinglelineStreach.Execute(pszSingleLSIOutputName,0,1);
	cout<<"线性拉伸I结束了！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//计算基底值J并线性拉伸到0-255
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComJOutputName="E:\\third\\5.1ComJ.TIF";
	const char *pszComLSJOutputName="E:\\third\\5.2ComLSJ.TIF";
	ComJ comJ;
	comJ.Initilization(pszSingleLSSOutputName,pszSingleLSIOutputName);
	comJ.Execute(pszComJOutputName);
	cout<<"基底值J计算结束了！"<<endl;
	SinglelineStreach.Initilization(pszComJOutputName);
	SinglelineStreach.Execute(pszComLSJOutputName,0,255);
	cout<<"基底值J线性拉伸计算结束了！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//计算Otsu算法，得到T
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszJLSoutputName="E:\\third\\6.1ComLSJ_GByte.TIF";
	CSingleLineStreach_GByte LsGByte;
	LsGByte.Initilization(pszComLSJOutputName);
	LsGByte.Execute(pszJLSoutputName,0,255);
	cout<<"线性拉伸到0-266_GByte完成！"<<endl;
	Float32 T=0;
	Otsu otsu;
	otsu.Initilization(pszJLSoutputName);
	T=otsu.Execute(T);
	cout<<"Otsu::T="<<T<<endl;
	cout<<"计算Otsu结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//计算细节图
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszInInputName="E:\\LC8\\5.TIF";
	const char *pszCompareOutputName="E:\\third\\7.1Compare.TIF";
	Compare compare;
	compare.Initilization(pszJLSoutputName,pszInInputName,pszHOutputName);
	compare.Execute(pszCompareOutputName,T);
	cout<<"计算Compare结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//I直方图均衡化
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszILSOutputName="E:\\third\\8.1ILS_255.TIF";
	const char *pszILSOGByteutputName="E:\\third\\8.2ILSGByte_255.TIF";
	SinglelineStreach.Initilization(pszIOutputName);
	SinglelineStreach.Execute(pszILSOutputName,0,255);
	LsGByte.Initilization(pszILSOutputName);
	LsGByte.Execute(pszILSOGByteutputName,0,255);
	cout<<"I线性拉伸到0-256_GByte完成！"<<endl;
	const char *pszHHisOutputName="E:\\third\\9.1IHistogramEqualization.TIF";
	CHistogramEqualization HistogramEqualization;
	HistogramEqualization.Initilization(pszILSOGByteutputName);
	HistogramEqualization.Execute(pszHHisOutputName);
	cout<<"I直方图均衡化结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//双边滤波
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszBFOutputName="E:\\third\\10.1BilaterlFiltering.TIF";
	CBilateralFiltering BF;
	int r=0;
	r=1;
	cout<<"R=1"<<endl;
	//cin>>r;
	BF.Initilization(pszHHisOutputName);
	BF.Execute(pszBFOutputName,r);
	cout<<"双边滤波结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//细节图Id
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszIdOutputName="E:\\third\\11.1Comp_Id.TIF";
	Comp_Id Id;
	Id.Initilization(pszHHisOutputName,pszBFOutputName);
	Id.Execute(pszIdOutputName);
	const char *pszId255OutputName="E:\\third\\12.1Comp_Id_255.TIF";
	LsGByte.Initilization(pszIdOutputName);
	LsGByte.Execute(pszId255OutputName,0,255);
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//带限定条件的Otsu计算两次
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	T=0;
	Otsu_Float otsu_float;
	otsu_float.Initilization(pszHHisOutputName);
	T=otsu_float.Execute(T);
	cout<<"Otsu_1::T="<<T<<endl;
	cout<<"计算Otsu结束!"<<endl;
	const char *pszOtsu1OutputName="E:\\third\\13.1Otsu_1.TIF";
	Otsu_1 otsu_1;
	otsu_1.Initilization(pszHHisOutputName);
	otsu_1.Execute(pszOtsu1OutputName,T);
	//计算第二次Otsu
	otsu_float.Initilization(pszOtsu1OutputName);
	T=otsu_float.Execute(T);
	cout<<"Otsu_2::T:"<<T<<endl;
	const char *pszOtsu2OutputName="E:\\third\\14.1Otsu_2.TIF";
	Otsu_2 otsu_2;
	otsu_2.Initilization(pszHHisOutputName);
	otsu_2.Execute(pszOtsu2OutputName,T);
	cout<<"计算Otsu结束！"<<endl;
	const char *pszIRutputName="E:\\third\\15.1IR.TIF";
	Compare_IR compare_ir;
	compare_ir.Initilization(pszCompareOutputName,pszOtsu2OutputName);
	compare_ir.Execute(pszIRutputName);
	cout<<"计算云种子图结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//计算云种子图边缘
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszEdgeutputName="E:\\third\\16.1Edge.TIF";
	EageCloude edgecloud;
	edgecloud.Initilization(pszIRutputName);
	edgecloud.Execute(pszEdgeutputName);
	cout<<"计算云边缘结束！"<<endl;
	/////////////////////////////////////////////////////
	//-------------------------------------------------->
	//膨胀计算
	//-------------------------------------------------->
	/////////////////////////////////////////////////////
	const char *pszComI255putName="E:\\third\\17.1ComI_255.TIF";
	const char *pszCloudExputName="E:\\third\\18.1CloudExpansion.TIF";
	SinglelineStreach.Initilization(pszIOutputName);
	SinglelineStreach.Execute(pszComI255putName,0,255);
	CloudExpansion cloudExpansion;
	cloudExpansion.Initilization(pszComI255putName,pszEdgeutputName);
	cloudExpansion.Execute(pszCloudExputName,0.008);
	cout<<"厚云膨胀！"<<endl;
	const char *pszThinkCloudExputName="E:\\third\\19.1ThinkCloud.TIF";
	ThickCloud thinkCloud;
	thinkCloud.Initilization(pszCloudExputName);
	thinkCloud.Execute(pszThinkCloudExputName);
	cout<<"厚云结束，开始薄云，厚云向薄云过渡！"<<endl;
	const char *pszCloudexputName="E:\\third\\20.1Expansion.TIF";
	Expansion expansion;
	expansion.Initilization(pszComI255putName,pszThinkCloudExputName);
	expansion.Execute(pszCloudexputName,0.3);
	cout<<"还有最后一步，薄云膨胀！"<<endl;
	const char *pszCloudexpansionputName="E:\\third\\21.1ThinExpansion.TIF";
	expansion.Initilization(pszComI255putName,pszCloudexputName);
	expansion.Execute(pszCloudexpansionputName,0.012);
	cout<<"全部计算结束！"<<endl;
	count++;
	cout<<"计算了"<<count<<"次"<<endl;
	b=clock();
	cout<<"Time="<<((double)(b-a)/1000)/60<<"分钟"<<endl;
	//goto loop;
	system("pause");
	return 0;
}