#ifndef __VIEW_MAIN_H__
#define __VIEW_MAIN_H__

#include "keyboard/KnobEvent.h"
#include "utils/Utils.h"

void InvisibleCursor(bool invisible, bool resetCursor = true);
void ChangeSystemCursor(GdkCursorType type);

class ViewMain: public CKnobEvent {
public:
  static ViewMain* GetInstance();

public:
  ~ViewMain();

  void Create();
  void ShowMenu();
  void HideMenu();

  GtkWidget* GetMainWindow();

  void KeyEvent(unsigned char keyValue);
  void KnobKeyEvent(unsigned char keyValue);
  void MenuReviewCallBack();

  bool GetCursorVisible();
  void SetCursorVisible(bool visible);

private:
  // signal

  static gboolean signal_callback_authen(gpointer data) {
    ViewMain* viewMain = (ViewMain*)data;

    if (viewMain != NULL) {
      viewMain->IsAuthenValid();
    }

    return FALSE;
  }

  static gboolean signal_callback_superauthen(gpointer data) {
    ViewMain* viewMain = (ViewMain*)data;

    if (viewMain != NULL) {
      viewMain->IsSuperAuthenValid();
    }

    return FALSE;
  }

  void IsAuthenValid();
  void IsSuperAuthenValid();

private:
  ViewMain();

  void KnobEvent(unsigned char keyValue, unsigned char offset);

private:
  static ViewMain* m_instance;
  static bool m_cursorVisible;

  static const unsigned int AUTHEN_NUM = 6;
  static const unsigned int SUPER_AUTHEN_NUM = 11;

private:
  GtkWindow* m_window;

  bool m_statusAuthen;
  bool m_statusSuperAuthen;
  vector<unsigned char> m_vecAuthenInfo;
  vector<unsigned char> m_vecSuperAuthenInfo;

private:


    unsigned char m_keyValue;

    int countN;


    //signal handle
    //signal connect


    int keyTSIN;


    // anthen




    int m_timer;
    int m_super_timer;



};

#endif
