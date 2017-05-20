#ifndef __PROBE_MAN_H__
#define __PROBE_MAN_H__

#include "utils/IniFile.h"
#include "probe/ProbeSocket.h"

#define MAX_SOCKET 3

class ProbeMan {
public:
  static ProbeMan* GetInstance();

public:
  ~ProbeMan();

  void GetCurProbe(ProbeSocket::ProbePara& para);
  void GetDefaultProbe(ProbeSocket::ProbePara& para);
  void GetAllProbe(ProbeSocket::ProbePara para[MAX_SOCKET]);
  void GetOneProbe(ProbeSocket::ProbePara para[MAX_SOCKET], int socket);
  int GetCurProbeSocket();
  int GetDefaultProbeSocket();

  void WriteProbeManual();
  void WriteProbe(int type);
  void WriteDefaultProbe(const string probeModel, IniFile* ptrIni);

  void ActiveHV(bool on);
  bool IsProbeExist();
  void SetProbeSocket(int socket);
  string VerifyProbeName(string fromType);

private:
  ProbeMan();

  string ReadDefaultProbe(IniFile* ptrIni);

private:
  static ProbeMan* m_instance;

private:
  int m_curSocket;
  int m_defaultSocket;

  vector<ProbeSocket> m_vecSockets;
};

#endif
