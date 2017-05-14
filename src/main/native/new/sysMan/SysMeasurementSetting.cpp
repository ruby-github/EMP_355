#include "sysMan/SysMeasurementSetting.h"

#include "Def.h"

TraceItem traceCalcItem;

// ---------------------------------------------------------

SysMeasurementSetting::SysMeasurementSetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysMeasurementSetting::~SysMeasurementSetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

bool SysMeasurementSetting::GetAutoCalcPS() {
  return m_inifile->ReadBool("MeasurementSetting", "PS");
}

bool SysMeasurementSetting::GetAutoCalcED() {
  return m_inifile->ReadBool("MeasurementSetting", "ED");
}

bool SysMeasurementSetting::GetAutoCalcRI() {
  return m_inifile->ReadBool("MeasurementSetting", "RI");
}

bool SysMeasurementSetting::GetAutoCalcSD() {
  return m_inifile->ReadBool("MeasurementSetting", "SD");
}

bool SysMeasurementSetting::GetAutoCalcTAmax() {
  return m_inifile->ReadBool("MeasurementSetting", "TAmax");
}

bool SysMeasurementSetting::GetAutoCalcPI() {
  return m_inifile->ReadBool("MeasurementSetting", "PI");
}

bool SysMeasurementSetting::GetAutoCalcTime() {
  return m_inifile->ReadBool("MeasurementSetting", "Time");
}

bool SysMeasurementSetting::GetAutoCalcHR() {
  return m_inifile->ReadBool("MeasurementSetting", "HR");
}

bool SysMeasurementSetting::GetAutoCalcPGmax() {
  return m_inifile->ReadBool("MeasurementSetting", "PGmax");
}

bool SysMeasurementSetting::GetAutoCalcPGmean() {
  return m_inifile->ReadBool("MeasurementSetting", "PGmean");
}

int SysMeasurementSetting::GetMeasureResult() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureResult");
}

int SysMeasurementSetting::GetMeasureLineDisplay() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureLineDisplay");
}

int SysMeasurementSetting::GetMeasureCursorSize() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureCursorSize");
}

int SysMeasurementSetting::GetMeasureSequence() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureSequence");
}

int SysMeasurementSetting::GetMeasureLineDensity() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureLineDensity");
}

int SysMeasurementSetting::GetMeasureColorCur() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureColorCur");
}

int SysMeasurementSetting::GetMeasureColorConfirm() {
  return m_inifile->ReadInt("MeasurementSetting", "MeasureColorConfirm");
}

int SysMeasurementSetting::GetTraceMethod() {
  return m_inifile->ReadInt("MeasurementSetting", "TraceMethod");
}

int SysMeasurementSetting::GetReportResult() {
  return m_inifile->ReadInt("MeasurementSetting", "ReportResult");
}

int SysMeasurementSetting::GetHeartBeatCycle() {
  return m_inifile->ReadInt("MeasurementSetting", "HeartBeatCycle");
}

int SysMeasurementSetting::GetUnitDist() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitDist");
}

int SysMeasurementSetting::GetUnitArea() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitArea");
}

int SysMeasurementSetting::GetUnitVol() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitVol");
}

int SysMeasurementSetting::GetUnitTime() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitTime");
}

int SysMeasurementSetting::GetUnitVel() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitVel");
}

int SysMeasurementSetting::GetUnitSlope() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitSlope");
}

int SysMeasurementSetting::GetUnitAccel() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitAccel");
}

int SysMeasurementSetting::GetUnitEfw() {
  return m_inifile->ReadInt("MeasurementSetting", "UnitEfw");
}

TraceItem SysMeasurementSetting::GetTraceItem() {
  return traceCalcItem;
}

void SysMeasurementSetting::SetAutoCalcPS(bool ps) {
  m_inifile->WriteBool("MeasurementSetting", "PS", ps);
}

void SysMeasurementSetting::SetAutoCalcED(bool ed) {
  m_inifile->WriteBool("MeasurementSetting", "ED", ed);
}

void SysMeasurementSetting::SetAutoCalcRI(bool ri) {
  m_inifile->WriteBool("MeasurementSetting", "RI", ri);
}

void SysMeasurementSetting::SetAutoCalcSD(bool sd) {
  m_inifile->WriteBool("MeasurementSetting", "SD", sd);
}

void SysMeasurementSetting::SetAutoCalcTAmax(bool tamax) {
  m_inifile->WriteBool("MeasurementSetting", "TAmax", tamax);
}

void SysMeasurementSetting::SetAutoCalcPI(bool pi) {
  m_inifile->WriteBool("MeasurementSetting", "PI", pi);
}

void SysMeasurementSetting::SetAutoCalcTime(bool time) {
  m_inifile->WriteBool("MeasurementSetting", "Time", time);
}

void SysMeasurementSetting::SetAutoCalcHR(bool hr) {
  m_inifile->WriteBool("MeasurementSetting", "HR", hr);
}

void SysMeasurementSetting::SetAutoCalcPGmax(bool pgmax) {
  m_inifile->WriteBool("MeasurementSetting", "PGmax", pgmax);
}

void SysMeasurementSetting::SetAutoCalcPGmean(bool pgmean) {
  m_inifile->WriteBool("MeasurementSetting", "PGmean", pgmean);
}

// measureReulst font size: 0-10 1-15
void SysMeasurementSetting::SetMeasureResult(int mearesult) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureResult", mearesult);
}

// measureLineDisplay: 0-OFF 1-ON
void SysMeasurementSetting::SetMeasureLineDisplay(int measureLineDisplay) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureLineDisplay", measureLineDisplay);
}

// easureCursor: 0-Big 1-Mid 2-Small
void SysMeasurementSetting::SetMeasureCursorSize(int measureCursor) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureCursorSize", measureCursor);
}

// measureSequence: 0-Default 1-Repeat 2-None
void SysMeasurementSetting::SetMeasureSequence(int measureSequence) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureSequence", measureSequence);
}

// measureLineDensity: 0-High 1-Mid 2-Low
void SysMeasurementSetting::SetMeasureLineDensity(int measureLineDensity) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureLineDensity", measureLineDensity);
}

// measureColorCur: 0-Red, 1-Yellow, 2-Green, 3-Blue, 4-White
void SysMeasurementSetting::SetMeasureColorCur(int measureColorCur) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureColorCur", measureColorCur);
}

// measureColorConfirm: 0-Red, 1-Yellow, 2-Green, 3-Blue, 4-White
void SysMeasurementSetting::SetMeasureColorConfirm(int measureColorConfirm) {
  m_inifile->WriteInt("MeasurementSetting", "MeasureColorConfirm", measureColorConfirm);
}

// TraceMethod 0-Point 1-Track 2-Auto
void SysMeasurementSetting::SetTraceMethod(int manualTrace) {
  m_inifile->WriteInt("MeasurementSetting", "TraceMethod", manualTrace);
}

// reportResult 0-last 1-average
void SysMeasurementSetting::SetReportResult(int reportResult) {
  m_inifile->WriteInt("MeasurementSetting", "ReportResult", reportResult);
}

void SysMeasurementSetting::SetHeartBeatCycle(int cycle) {
  m_inifile->WriteInt("MeasurementSetting", "HeartBeatCycle", cycle);
}

void SysMeasurementSetting::SetUnitDist(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitDist", unit);
}

void SysMeasurementSetting::SetUnitArea(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitArea", unit);
}

void SysMeasurementSetting::SetUnitVol(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitVol", unit);
}

void SysMeasurementSetting::SetUnitTime(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitTime", unit);
}

void SysMeasurementSetting::SetUnitVel(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitVel", unit);
}

void SysMeasurementSetting::SetUnitSlope(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitSlope", unit);
}

void SysMeasurementSetting::SetUnitAccel(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitAccel", unit);
}

void SysMeasurementSetting::SetUnitEfw(int unit) {
  m_inifile->WriteInt("MeasurementSetting", "UnitEfw", unit);
}

void SysMeasurementSetting::DefaultFactory() {
  int measureLineDisplay=0;
  int measureSequence = 0;
  int measureCursorSize=2;
  int measureLineDensity=1;
  int measureColorCur=2;
  int measureColorConfirm=4;
  int traceInMeasure=0;
  int manualTrace=0;
  int reportResult = 0;
  bool ps = false;
  bool ed = false;
  bool ri = false;
  bool sd = false;
  bool tamax = false;
  bool pi = false;
  bool time = false;
  bool hr = false;
  bool pgmax = false;
  bool pgmean = false;
  int heartBeatCycle = 2;
  int dist=0;
  int area=0;
  int vol=0;
  int tm=0;
  int vel=0;
  int accel=0;
  int efw=0;

  m_inifile->WriteInt("MeasurementSetting", "MeasureLineDisplay", measureLineDisplay);
  m_inifile->WriteInt("MeasurementSetting", "MeasureSequence", measureSequence);
  m_inifile->WriteInt("MeasurementSetting", "MeasureCursorSize", measureCursorSize);
  m_inifile->WriteInt("MeasurementSetting", "MeasureLineDensity", measureLineDensity);
  m_inifile->WriteInt("MeasurementSetting", "MeasureColorCur", measureColorCur);
  m_inifile->WriteInt("MeasurementSetting", "MeasureColorConfirm", measureColorConfirm);
  m_inifile->WriteInt("MeasurementSetting", "TraceInMeasure", traceInMeasure);
  m_inifile->WriteInt("MeasurementSetting", "TraceMethod", manualTrace);
  m_inifile->WriteInt("MeasurementSetting", "ReportResult", reportResult);

  m_inifile->WriteBool("MeasurementSetting", "PS", ps);
  m_inifile->WriteBool("MeasurementSetting", "ED", ed);
  m_inifile->WriteBool("MeasurementSetting", "RI", ri);
  m_inifile->WriteBool("MeasurementSetting", "SD", sd);
  m_inifile->WriteBool("MeasurementSetting", "TAmax", tamax);
  m_inifile->WriteBool("MeasurementSetting", "PI", pi);
  m_inifile->WriteBool("MeasurementSetting", "Time", time);
  m_inifile->WriteBool("MeasurementSetting", "HR", hr);
  m_inifile->WriteBool("MeasurementSetting", "PGmax", pgmax);
  m_inifile->WriteBool("MeasurementSetting", "PGmean", pgmean);

  m_inifile->WriteInt("MeasurementSetting", "UnitDist", dist);
  m_inifile->WriteInt("MeasurementSetting", "UnitArea", area);
  m_inifile->WriteInt("MeasurementSetting", "UnitVol", vol);
  m_inifile->WriteInt("MeasurementSetting", "UnitTime", tm);
  m_inifile->WriteInt("MeasurementSetting", "UnitVel", vel);
  m_inifile->WriteInt("MeasurementSetting", "UnitAccel", accel);
  m_inifile->WriteInt("MeasurementSetting", "UnitEfw", efw);

  m_inifile->WriteInt("MeasurementSetting", "HeartBeatCycle", heartBeatCycle);
}

void SysMeasurementSetting::UpdateTraceItemSetting() {
  traceCalcItem.psSet = GetAutoCalcPS();
  traceCalcItem.edSet = GetAutoCalcED();
  traceCalcItem.riSet = GetAutoCalcRI();
  traceCalcItem.sdSet = GetAutoCalcSD();
  traceCalcItem.tamaxSet = GetAutoCalcTAmax();
  traceCalcItem.piSet = GetAutoCalcPI();
  traceCalcItem.timeSet = GetAutoCalcTime();
  traceCalcItem.hrSet = GetAutoCalcHR();
  traceCalcItem.pgmaxSet = GetAutoCalcPGmax();
  traceCalcItem.pgmeanSet = GetAutoCalcPGmean();
}

void SysMeasurementSetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
