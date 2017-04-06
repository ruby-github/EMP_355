#ifndef __MEA_RESULT_H_
#define __MEA_RESULT_H_

#include "calcPeople/MeasureDef.h"

typedef const float (*SecSingleResult)[][MEA_TIMES];
typedef const ResultMulti (*SecMultiResult)[];
typedef const float (*SecCalcResult)[][CALC_RESULT_CLASSES];

typedef float (*SingleResult)[][MEA_TIMES];
typedef ResultMulti (*MultiResult)[];
typedef float (*CalcResult)[][CALC_RESULT_CLASSES];

typedef struct{
	ESection secOrder;//科别在ESection中的枚举值
	int singleStart;
	int singleEnd;
	int multiStart;
	int multiEnd;
	int calcStart;
	int calcEnd;
	int genStart;
	int genEnd;
	SingleResult ptrSingleResult;
	MultiResult ptrMultiResult;
	CalcResult ptrCalcResult;
}MeaResultInfo;

/*缁撴灉淇濆瓨鐨勭被
 *数据保存在MeaResult中
 *
 * */
class MeaResult{
    public:
	MeaResult();
	~MeaResult();

	static MeaResult *GetInstance();

/**************************娴嬮噺***************************************/
	//SetLastValue鎶婃渶鏂版祴閲忓緱鍒扮殑缁撴灉淇濆瓨鍦∕eaResult[][2]锛屼互鍓嶇殑鍊间緷娆″線鍓嶆尓
	int SetLastValue(const float value[], const int item, const int valueNums);

	//获取MeaResult[][2]中的值，即最近测量的值
	int GetLastValue(const int item, float value[MEA_MULTI], int obFetal=FETAL_BY_SET);

	//获取MeaResult[][]中所有值
	int GetAllValue(const int item, float value[MEA_MULTI][MEA_TIMES], int obFetal=FETAL_BY_SET);

	//读取MeaResult[][]中所有值，并计算平均值
	int GetMeanValue(const int item, float value[MEA_MULTI], int obFetal=FETAL_BY_SET);

	//获取当前是用平均值，最后值，或所有值，并返回这个值
	int GetValue(const int item, float value[MEA_MULTI], float allValue[MEA_MULTI][MEA_TIMES], const int sysValue, int obFetal=FETAL_BY_SET);

/**************************璁＄畻***************************************/
	int CalcSetValue(const float value[], const int calcItem, int section);
	void CalcSetAgwValue(const float value[][CALC_RESULT_CLASSES]);
	int CalcGetLastValue(float *value, const int item, int section, int obFetal=FETAL_BY_SET);
	int CalcGetMeanValue(float *value, const int item, int section, int obFetal=FETAL_BY_SET);
	int CalcGetValue(float *value, const int calcItem, const int section, int obFetal=FETAL_BY_SET);
	//鑾峰彇骞冲潎鑳庨緞(value[0])鍜屽钩鍧囬浜ф湡(value[1])
	int CalcGetAgwValue(float value[], const int fetalOrder);//鏍规嵁鎶ュ憡涓渶鍚庡€艰繕鏄钩鍧囧€肩殑璁剧疆鑾峰緱骞冲潎鑳庨緞

	void CalcSetSPSA(float spsa);//存储SPSA的值
	void CalcSetPSAD(float psad);//存储PSAD的值
	void CalcGetSPSA(float &spsa, float &psad);//获得SPSA和PSAD的值

#if 0
	int CalcGetTDValue(float *value);
	int CalcGetTDMeanValue(float *value);
	int CalcGetTDLastValue(float *value);
#endif
	int CalcGetTDValue(float *total_area, float *affect_area, float *value);
	int CalcGetTDMeanValue(float *total_area, float *affect_area, float *value);
	int CalcGetTDLastValue(float *total_area, float *affect_area, float *value);

	//将MeaResult中所有的数据清0
	void ClearAllValue(void);

	int FindSingleMeaInfo(int section, int item);
	int FindDoubleMeaInfo(int section, int multiItem);
	int ItemLabelCalc(const int item, int *section);
	int ItemLabel(int item, int *item_tmp, ResultMulti **ptrResultMulti, float (**ptrResultSingle)[MEA_TIMES], int *singleVolume, int *multiVolume, int *section, int obFetal=FETAL_BY_SET);
//	int ItemLabelMulti(const int item, int *item_tmp, ResultMulti **ptrResultMulti, int *volume, int *section, int obFetal=FETAL_BY_SET);
//	int ItemLabelSingle(const int item, int *item_tmp, float (**ptrResultSingle)[MEA_TIMES], int *singleVolume, int *section, int obFetal=FETAL_BY_SET);

	SecSingleResult GetSecSingleValue(int section, int obFetal=FETAL_BY_SET);
	SecMultiResult GetSecMultiValue(int section, int obFetal=FETAL_BY_SET);
	SecCalcResult GetSecCalcValue(int section, int obFetal=FETAL_BY_SET);

    private:
	static MeaResult *m_ptrInstance;

	MeaResultInfo SecResultInfo[SECTION_END - SECTION_START];

	int CalcGetAgwLastValue(float value[], const int fetalOrder);//鑾峰緱鏈€鍚庡€肩殑骞冲潎鑳庨緞
	int CalcGetAgwMeanValue(float value[], const int fetalOrder);//鑾峰緱骞冲潎鍊肩殑骞冲潎鑳庨緞

	int EDCBFormula(int gpLen, int gw, int *EDCB);
/**************************娴嬮噺缁撴灉淇濆瓨***************************************/
	float MeaResultAbd[ABD_MEA_END-ABD_MEA_START][MEA_TIMES];
	float MeaResultAdult[ADULT_MEA_END-ADULT_MEA_START][MEA_TIMES];
	float MeaResultUR[UR_MEA_END-UR_MEA_START][MEA_TIMES];
	float MeaResultOB1[OB_MEA_END-OB_MEA_START][MEA_TIMES];//鑳庡効1
	float MeaResultOB2[OB_MEA_END-OB_MEA_START][MEA_TIMES];//鑳庡効2
	float MeaResultGYN[GYN_MEA_END-GYN_MEA_START][MEA_TIMES];
	float MeaResultSP[SP_MEA_END-SP_MEA_START][MEA_TIMES];
	float MeaResultVS[VS_MEA_END-VS_MEA_START][MEA_TIMES];
	float MeaResultFetal[FETAL_MEA_END-FETAL_MEA_START][MEA_TIMES];
	float MeaResultTCD[TCD_MEA_END-TCD_MEA_START][MEA_TIMES];
	float MeaResultOrtho[ORTHO_MEA_END-ORTHO_MEA_START][MEA_TIMES];
    float MeaResultUser[ABD_MEA_START-1][MEA_TIMES];
#ifdef VET
    //hlx
	float MeaResultTD[TD_MEA_END-TD_MEA_START][MEA_TIMES];
	//添加 动物超 产科 结果保存数组
	//hlx
	float MeaResultAnOB[ANOB_MEA_END-ANOB_MEA_START][MEA_TIMES];
#endif
	ResultMulti AbdResultMulti[ABD_MULTI_END-ABD_MULTI_START];
	ResultMulti AdultResultMulti[ADULT_MULTI_END-ADULT_MULTI_START];
	ResultMulti URResultMulti[UR_MULTI_END-UR_MULTI_START];
	ResultMulti OBResultMulti1[OB_MULTI_END-OB_MULTI_START];//鑳庡効1
	ResultMulti OBResultMulti2[OB_MULTI_END-OB_MULTI_START];//鑳庡効2
	ResultMulti GYNResultMulti[GYN_MULTI_END-GYN_MULTI_START];
	ResultMulti SPResultMulti[SP_MULTI_END-SP_MULTI_START];
	ResultMulti VSResultMulti[VS_MULTI_END-VS_MULTI_START];
	ResultMulti FetalResultMulti[FETAL_MULTI_END-FETAL_MULTI_START];
	ResultMulti TCDResultMulti[TCD_MULTI_END-TCD_MULTI_START];
	ResultMulti OrthoResultMulti[ORTHO_MULTI_END-ORTHO_MULTI_START];
#ifdef  VET
//添加 动物超 肌腱科
	//hlx
	ResultMulti TDResultMulti[TD_MULTI_END-TD_MULTI_START];
	//添加 动物超  产科 结果保存数据
	//hlx
	ResultMulti AnOBResultMulti[ANOB_MULTI_END-ANOB_MULTI_START];
#endif
/**************************璁＄畻缁撴灉淇濆瓨***************************************/
	float CalcResultAbd[ABD_CALC_END - ABD_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultAdult[ADULT_CALC_END - ADULT_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultUR[UR_CALC_END - UR_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultURSPSA[2];
	float CalcResultOB1[OB_CALC_END - OB_CALC_START][CALC_RESULT_CLASSES];//鑳庡効1
	float CalcResultOB2[OB_CALC_END - OB_CALC_START][CALC_RESULT_CLASSES];//鑳庡効2
	float CalcResultOB1Agw[2][CALC_RESULT_CLASSES];	//鑳庡効1骞冲潎瀛曞懆鍜屽钩鍧囬浜ф湡
	float CalcResultOB2Agw[2][CALC_RESULT_CLASSES];	//鑳庡効2骞冲潎瀛曞懆鍜屽钩鍧囬浜ф湡
	float CalcResultGYN[GYN_CALC_END - GYN_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultSP[SP_CALC_END - SP_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultVS[VS_CALC_END - VS_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultFetal[FETAL_CALC_END - FETAL_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultTCD[TCD_CALC_END - TCD_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultOrtho[ORTHO_CALC_END - ORTHO_CALC_START][CALC_RESULT_CLASSES];

#ifdef VET

//添加 动物超 肌腱科
	//hlx 1.11
	float CalcResultTD[TD_CALC_END-TD_CALC_START][CALC_RESULT_CLASSES];
	//添加 动物超 产科 结果保存
	//hlx 1.11
	float CalcResultAnOB[ANOB_CALC_END-ANOB_CALC_START][CALC_RESULT_CLASSES];
	float CalcResultAnOBAgw[2][CALC_RESULT_CLASSES];
#endif
    void InitResultInfo(void);

	void InitSign(float singleResult[MEA_TIMES], int position);

	void AbdInit(void);

	void AdultInit(void);

	void URInit(void);

	void OBInit(void);

	void GYNInit(void);

	void SPInit(void);

	void VSInit(void);

	void FetalInit(void);

	void TCDInit(void);

	void OrthoInit(void);
	#ifdef VET
//添加 动物超 TD测量结果初始化
//hlx
	void TDInit(void);
//添加 动物超 产科 测量结果初始化
//hlx
	void AnOBInit(void);
#endif
    void UserInit(void);

//	int JudgeMultiSec(int section);
};

#endif
