#ifndef __KEY_VALUE_OPR_H__
#define __KEY_VALUE_OPR_H__

#include <vector>
#include <gtk/gtk.h>
#include "AbsKeyboardEvent.h"

using std::vector;

// KeyValueOpr:  键盘接口类
//   Push          : 在每个窗口建立时调用此函数将本窗口对键盘的响应函数指针压入vector
//   Pop           : 在每个窗口退出是调用此函数将本窗口对键盘的响应函数指针从vector撤销
//   GetElement    : 获得vector的第n个元素，此函数暂时没用
//   SendKeyValue  : 将接收到的键值发送给各个窗口
//
//   SendLightValue: 接收键盘灯值, 并将灯值发送给键盘

class KeyValueOpr {
public:
  KeyValueOpr();

public:
  void Push(AbsKeyboardEvent* win_opr );
  void Pop();
  int Size();
  bool Empty();

  void SendKeyValue(unsigned char* keyValue );
  void CtrlLight(bool on, unsigned char lightValue );
  AbsKeyboardEvent* GetElement(int n);

  int GetMouseSpeed();
  void SetMouseSpeed(int s);

  bool IsLighten(unsigned char lightValue);
  void ListLighten();

private:
  void SendLightValue(unsigned char light_value);
  void AddLighten(unsigned char lightValue);
  void RemoveLighten(unsigned char lightValue);

private:
  int m_mouseSpeed;

  vector<AbsKeyboardEvent*> m_winOprStack;
  vector<unsigned char> m_vecLighten;
};

// 获取键值并发送给各个窗口
extern gboolean GetKeyValue(GIOChannel* source, GIOCondition condition, gpointer data);

// 键盘监视
extern void* KeyboardOversee(void* pKeyInterface, bool isHandShake = true);

// 请求获得键盘TGC值
void KeyboardRequestTGC();

// 键盘声音控制
void KeyboardSound(bool value);

extern KeyValueOpr g_keyInterface;

#endif
