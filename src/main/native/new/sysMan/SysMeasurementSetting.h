#ifndef __SYS_MEASUREMENT_SETTING_H__
#define __SYS_MEASUREMENT_SETTING_H__

#include "utils/IniFile.h"

struct TraceItem {
  unsigned char psSet;
  unsigned char edSet;
  unsigned char riSet;
  unsigned char sdSet;
  unsigned char tamaxSet;
  unsigned char piSet;
  unsigned char timeSet;
  unsigned char hrSet;
  unsigned char pgmaxSet;
  unsigned char pgmeanSet;
};

class SysMeasurementSetting {
public:
  SysMeasurementSetting();
  ~SysMeasurementSetting();

public:
  bool GetAutoCalcPS();
  bool GetAutoCalcED();
  bool GetAutoCalcRI();
  bool GetAutoCalcSD();
  bool GetAutoCalcTAmax();
  bool GetAutoCalcPI();
  bool GetAutoCalcTime();
  bool GetAutoCalcHR();
  bool GetAutoCalcPGmax();
  bool GetAutoCalcPGmean();

  int GetMeasureResult();
  int GetMeasureLineDisplay();
  int GetMeasureCursorSize();
  int GetMeasureSequence();
  int GetMeasureLineDensity();
  int GetMeasureColorCur();
  int GetMeasureColorConfirm();
  int GetTraceMethod();
  int GetReportResult();
  int GetHeartBeatCycle();
  int GetUnitDist();
  int GetUnitArea();
  int GetUnitVol();
  int GetUnitTime();
  int GetUnitVel();
  int GetUnitSlope();
  int GetUnitAccel();
  int GetUnitEfw();

  TraceItem GetTraceItem();

  void SetAutoCalcPS(bool ps);
  void SetAutoCalcED(bool ed);
  void SetAutoCalcRI(bool ri);
  void SetAutoCalcSD(bool sd);
  void SetAutoCalcTAmax(bool tamax);
  void SetAutoCalcPI(bool pi);
  void SetAutoCalcTime(bool time);
  void SetAutoCalcHR(bool hr);
  void SetAutoCalcPGmax(bool pgmax);
  void SetAutoCalcPGmean(bool pgmean);

  void SetMeasureResult(int mearesult);
  void SetMeasureLineDisplay(int measureLineDisplay);
  void SetMeasureCursorSize(int measureCursorSize);
  void SetMeasureSequence(int measureSequence);
  void SetMeasureLineDensity(int measureLineDensity);
  void SetMeasureColorCur(int measureColor);
  void SetMeasureColorConfirm(int measureColor);
  void SetTraceMethod(int manualTrace);
  void SetReportResult(int reportResult);
  void SetHeartBeatCycle(int cycle);

  void SetUnitDist(int unit);
  void SetUnitArea(int unit);
  void SetUnitVol(int unit);
  void SetUnitTime(int unit);
  void SetUnitVel(int unit);
  void SetUnitSlope(int unit);
  void SetUnitAccel(int unit);
  void SetUnitEfw(int unit);

  void DefaultFactory();
  void UpdateTraceItemSetting();
  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
