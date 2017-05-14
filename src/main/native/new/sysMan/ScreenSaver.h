#ifndef __SCREEN_SAVER_h__
#define __SCREEN_SAVER_h__

#include "utils/Utils.h"

#include <X11/Xlib.h>

class ScreenSaver {
public:
  static ScreenSaver* GetInstance();

public:
  ~ScreenSaver();

  void Reset();
  void SetPeriod(guint time);

  void EnterScreenSaver2();
  void EnterScreenSaver3();

private:
  // signal

  static gboolean signal_callback_timer(gpointer data) {
    ScreenSaver* screensaver = (ScreenSaver*)data;

    if (screensaver != NULL) {
      screensaver->Timer();
    }

    return TRUE;
  }

  // signal

  void Timer();

private:
  ScreenSaver();

  void Init();

  void EnterScreenSaver();
  void ExitScreenSaver();

  void BlackScreen();
  void BlackScreen2();
  void BlackScreen3();

  void DrawHintMessage();

private:
  static ScreenSaver* m_instance;
  static const guint m_interval = 5;  // second

private:
  guint m_count;
  guint m_period;
  guint m_timer;

  gboolean m_state;

  Display* m_dpy;
  Window m_win;
};

#endif
