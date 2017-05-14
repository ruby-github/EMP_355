#ifndef __SYS_DICOM_SETTING_H__
#define __SYS_DICOM_SETTING_H__

#include "utils/IniFile.h"

class SysDicomSetting {
public:
  SysDicomSetting();
  ~SysDicomSetting();

public:
  string GetRemoteAE();
  string GetRemoteIP();
  string GetRemotePort();
  string GetLocalAE();
  string GetLocalIP();
  string GetLocalGateWay();
  string GetLocalNetMask();
  string GetLocalPort();
  int GetVideoFrames();
  bool GetAutoQuery();
  bool GetSendReport();
  bool GetSendVideo();
  bool GetMPPS();
  bool GetStorageCommitment();

  void SetRemoteAE(string remoteAE);
  void SetRemoteIP(string remoteIP);
  void SetRemotePort(string remotePort);
  void SetLocalAE(string localAE);
  void SetLocalIP(string localIP);
  void SetLocalGateWay(string localGateway);
  void SetLocalNetMask(string localNetmask);
  void SetLocalPort(string localPort);
  void SetVideoFrames(int frames);
  void SetAutoQuery(bool autoQuery);
  void SetSendReport(bool report);
  void SetSendVideo(bool video);
  void SetMPPS(bool mpps);
  void SetStorageCommitment(bool storageCommit);

  void SyncFile();

private:
  IniFile* m_inifile;
};

#endif
