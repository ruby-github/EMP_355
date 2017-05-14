#include "sysMan/SysDicomSetting.h"

#include "Def.h"

SysDicomSetting::SysDicomSetting() {
  m_inifile = new IniFile(string(CFG_RES_PATH) + string(SYS_SETTING_FILE));
}

SysDicomSetting::~SysDicomSetting() {
  if (m_inifile != NULL) {
    delete m_inifile;
  }

  m_inifile = NULL;
}

string SysDicomSetting::GetRemoteAE() {
  return m_inifile->ReadString("DicomSetting", "RemoteAE");
}

string SysDicomSetting::GetRemoteIP() {
  return m_inifile->ReadString("DicomSetting", "RemoteIP");
}

string SysDicomSetting::GetRemotePort() {
  return m_inifile->ReadString("DicomSetting", "RemotePort");
}

string SysDicomSetting::GetLocalAE() {
  return m_inifile->ReadString("DicomSetting", "LocalAE");
}

string SysDicomSetting::GetLocalIP() {
  return m_inifile->ReadString("DicomSetting", "LocalIP");
}

string SysDicomSetting::GetLocalGateWay() {
  return m_inifile->ReadString("DicomSetting", "LocalGateWay");
}

string SysDicomSetting::GetLocalNetMask() {
  return m_inifile->ReadString("DicomSetting", "LocalNetMask");
}

string SysDicomSetting::GetLocalPort() {
  return m_inifile->ReadString("DicomSetting", "LocalPort");
}

int SysDicomSetting::GetVideoFrames() {
  return m_inifile->ReadInt("DicomSetting", "VideoFrames");
}

bool SysDicomSetting::GetAutoQuery() {
  return m_inifile->ReadBool("DicomSetting", "AutoQuery");
}

bool SysDicomSetting::GetSendReport() {
  return m_inifile->ReadBool("DicomSetting", "SendReport");
}

bool SysDicomSetting::GetSendVideo() {
  return m_inifile->ReadBool("DicomSetting", "SendVideo");
}

bool SysDicomSetting::GetMPPS() {
  return m_inifile->ReadBool("DicomSetting", "MPPS");
}

bool SysDicomSetting::GetStorageCommitment() {
  return m_inifile->ReadBool("DicomSetting", "StorageCommitment");
}

void SysDicomSetting::SetRemoteAE(string remoteAE) {
  m_inifile->WriteString("DicomSetting", "RemoteAE", remoteAE);
}

void SysDicomSetting::SetRemoteIP(string remoteIP) {
  m_inifile->WriteString("DicomSetting", "RemoteIP", remoteIP);
}

void SysDicomSetting::SetRemotePort(string remotePort) {
  m_inifile->WriteString("DicomSetting", "RemotePort", remotePort);
}

void SysDicomSetting::SetLocalAE(string localAE) {
  m_inifile->WriteString("DicomSetting", "LocalAE", localAE);
}

void SysDicomSetting::SetLocalIP(string localIP) {
  m_inifile->WriteString("DicomSetting", "LocalIP", localIP);
}

void SysDicomSetting::SetLocalGateWay(string localGateway) {
  m_inifile->WriteString("DicomSetting", "LocalGateWay", localGateway);
}

void SysDicomSetting::SetLocalNetMask(string localNetmask) {
  m_inifile->WriteString("DicomSetting", "LocalNetMask", localNetmask);
}

void SysDicomSetting::SetLocalPort(string localPort) {
  m_inifile->WriteString("DicomSetting", "LocalPort", localPort);
}

void SysDicomSetting::SetVideoFrames(int frames) {
  m_inifile->WriteInt("DicomSetting", "VideoFrames", frames);
}

void SysDicomSetting::SetAutoQuery(bool autoQuery) {
  m_inifile->WriteBool("DicomSetting", "AutoQuery", autoQuery);
}

void SysDicomSetting::SetSendReport(bool report) {
  m_inifile->WriteBool("DicomSetting", "SendReport", report);
}

void SysDicomSetting::SetSendVideo(bool video) {
  m_inifile->WriteBool("DicomSetting", "SendVideo", video);
}

void SysDicomSetting::SetMPPS(bool mpps) {
  m_inifile->WriteBool("DicomSetting", "MPPS", mpps);
}

void SysDicomSetting::SetStorageCommitment(bool storageCommit) {
  m_inifile->WriteBool("DicomSetting", "StorageCommitment", storageCommit);
}

void SysDicomSetting::SyncFile() {
  m_inifile->SyncConfigFile();
}
