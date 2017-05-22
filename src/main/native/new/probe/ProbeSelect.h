#ifndef __PROBE_SELECT_H__
#define __PROBE_SELECT_H__

#include "probe/ExamItem.h"
#include "probe/ProbeSocket.h"
#include "probe/ProbeMan.h"

#include <vector>

using std::vector;

class ProbeSelect {
public:
  static ExamItem::EItem GetItemIndex();

public:
  ProbeSelect();
  ~ProbeSelect();

public:
  bool Execute();
  bool ProbeRead();
  void UserItemOfProbeInit(int indexSocket, ExamItem::EItem indexItem, const string item);
  bool OneProbeRead(int socket);
  void ProbeInit(int indexSocket, ExamItem::EItem indexItem);
  void GetDefaultValue(int& socket, ExamItem::EItem& item, bool& flag);
  void GetDefaultItemNameandFlag(string& itemName, bool& flag);
  void GetPara(ProbeSocket::ProbePara*& para, vector<ExamItem::EItem>*& item, int& maxSocket);
  void ActiveHV(bool on);

private:
  static int m_socketIndex;
  static ExamItem::EItem m_itemIndex;

private:
  ProbeMan* m_ptrProbe;
  ExamItem m_e;

  ProbeSocket::ProbePara m_para[MAX_SOCKET];
  vector<ExamItem::EItem> m_itemList[MAX_SOCKET];
  vector<ExamItem::EItem> probeItemName[MAX_SOCKET];
};

#endif
