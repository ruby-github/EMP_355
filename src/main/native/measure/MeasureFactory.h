#ifndef MEASURE_FACTORY_H
#define MEASURE_FACTORY_H

#include "calcPeople/MeasureDef.h"
#include "AbsMeasure.h"
#include "measure/Measure2D.h"
#include "imageProc/ModeStatus.h"

extern AbsMeasure* g_ptrAbsMeasure;

class MeasureFactory {
public:
    ~MeasureFactory();
    enum EMeasureMode {MEA_2D, MEA_M, MEA_PW, MEA_2D_M, MEA_2D_PW};
    EMeasureMode JudgeMode(void);
    FreezeMode::EFreezeMode JudgeFreeze(void);
    static MeasureFactory* GetInstance();

    AbsMeasure* Create(EMeasureFactory type);

    int GetType();
    void UndoLastMeasure();

private:
    MeasureFactory();

    static MeasureFactory* m_ptrInstance;
    //EMeasureFactory m_type;
    int m_type;
};

//杞欢鍖呮祴閲忎腑鍙祴閲忚窛绂昏€屾病鏈夎绠楃殑閫氱敤鍑芥暟
//extern void DistanceTitle(Results results);

#endif
