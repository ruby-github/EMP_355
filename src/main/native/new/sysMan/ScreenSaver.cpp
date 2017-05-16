#include "sysMan/ScreenSaver.h"

#include <X11/Xft/Xft.h>
#include <X11/Xutil.h>
#include "utils/FakeXUtils.h"

#include "Def.h"
#include "display/MenuArea.h"
#include "imageControl/D4FuncMan.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/ModeStatus.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/ViewSuperuser.h"

ScreenSaver* ScreenSaver::m_instance = NULL;

// ---------------------------------------------------------

ScreenSaver* ScreenSaver::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ScreenSaver();
  }

  return m_instance;
}

ScreenSaver::ScreenSaver() {
  m_count = 0;
  m_period = 600;;
  m_timer = 0;

  m_state = FALSE;

  m_dpy = NULL;
}

ScreenSaver::~ScreenSaver() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ScreenSaver::Reset() {
  m_count = 0;

  if(m_state) {
    ExitScreenSaver();
  }

  Init();
}

void ScreenSaver::SetPeriod(guint time) {
  m_period = time;

  Reset();
}

void ScreenSaver::EnterScreenSaver2() {
  #ifdef EMP3D
    if (D4FuncMan::GetInstance()->Get4DMode()) { // 如果当前3D/4D进程在运行，则不进入屏保
      return;
    }
  #endif

  if (ModeStatus::IsAutoReplayMode()) {
    Reset();
  } else if (ModeStatus::IsUnFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
    BlackScreen2();
  } else {
    BlackScreen2();
  }
}

void ScreenSaver::EnterScreenSaver3() {
  #ifdef EMP3D
    if (D4FuncMan::GetInstance()->Get4DMode()) { // 如果当前3D/4D进程在运行，则不进入屏保
      return;
    }
  #endif

  if (ModeStatus::IsAutoReplayMode()) {
    Reset();
  } else if (ModeStatus::IsUnFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
    BlackScreen3();
  } else {
    BlackScreen3();
  }
}

// ---------------------------------------------------------

void ScreenSaver::Timer() {
  m_count ++;
  // PRINTF("ScreenSaver: After %d seconds into the Screensaver state !\n", m_period-(m_count*m_interval));

  if(m_state) {
    m_count = 0;
    DrawHintMessage();
  } else {
    // if(m_count * m_interval == 20) //for test
    if(m_count * m_interval == m_period) {
      // m_timer = 0;
      m_count = 0;

      //screen saver
      EnterScreenSaver();
    }
  }
}

void ScreenSaver::Init() {
  if(m_timer > 0) {
    g_source_remove(m_timer);
  }

  if(m_period > 0) {
    m_timer = g_timeout_add_seconds(m_interval, signal_callback_timer, this);
  }
}

void ScreenSaver::EnterScreenSaver() {
  #ifdef EMP3D
    if (D4FuncMan::GetInstance()->Get4DMode()) { // 如果当前3D/4D进程在运行，则不进入屏保
      return;
    }
  #endif

  if(ViewSuperuser::GetInstance()->GetDemoStatus()) {
    return;
  }

  if(ViewSuperuser::GetInstance()->GetDemoStatus()) {
    return;
  }

  if (ModeStatus::IsAutoReplayMode()) {
    Reset();
  } else if (ModeStatus::IsUnFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
    MenuShowUndo();//solve the segment of exiting screensaver in bodymark func.
  }

  BlackScreen();
}

void ScreenSaver::ExitScreenSaver() {
  XDestroyWindow(m_dpy, m_win);
  XCloseDisplay(m_dpy);

  m_state = FALSE;
}

void ScreenSaver::BlackScreen() {
  char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int screen;
  XSetWindowAttributes wa;
  Pixmap pmap;
  Cursor invisible;
  Window root;
  XColor black, dummy;

  if(!(m_dpy = XOpenDisplay(0))) {
    PRINTF("ScreenSaver: cannot open display\n");
    return;
  }

  screen = DefaultScreen(m_dpy);
  root = RootWindow(m_dpy, screen);

  wa.override_redirect = 1;
  wa.background_pixel = BlackPixel(m_dpy, screen);

  m_win = XCreateWindow(m_dpy, root, 0, 0, DisplayWidth(m_dpy, screen), DisplayHeight(m_dpy, screen),
    0, DefaultDepth(m_dpy, screen), CopyFromParent, DefaultVisual(m_dpy, screen),
    CWOverrideRedirect | CWBackPixel, &wa);
  XAllocNamedColor(m_dpy, DefaultColormap(m_dpy, screen), "black", &black, &dummy);
  pmap = XCreateBitmapFromData(m_dpy, m_win, curs, 8, 8);
  invisible = XCreatePixmapCursor(m_dpy, pmap, pmap, &black, &black, 0, 0);
  XDefineCursor(m_dpy, m_win, invisible);
  XMapRaised(m_dpy, m_win);
  XSync(m_dpy, FALSE);
  XFreePixmap(m_dpy, pmap);

  m_state = TRUE;

  DrawHintMessage();
}

void ScreenSaver::BlackScreen2() {
  char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int screen;
  XSetWindowAttributes wa;
  Pixmap pmap;
  Cursor invisible;
  Window root;
  XColor black, dummy;

  if(!(m_dpy = XOpenDisplay(0))) {
    PRINTF("ScreenSaver: cannot open display\n");
    return;
  }

  screen = DefaultScreen(m_dpy);
  root = RootWindow(m_dpy, screen);

  wa.override_redirect = 1;
  wa.background_pixel = BlackPixel(m_dpy, screen);

  m_win = XCreateWindow(m_dpy, root, 0, 0, DisplayWidth(m_dpy, screen), DisplayHeight(m_dpy, screen),
    0, DefaultDepth(m_dpy, screen), CopyFromParent, DefaultVisual(m_dpy, screen),
    CWOverrideRedirect | CWBackPixel, &wa);
  XAllocNamedColor(m_dpy, DefaultColormap(m_dpy, screen), "black", &black, &dummy);
  pmap = XCreateBitmapFromData(m_dpy, m_win, curs, 8, 8);
  invisible = XCreatePixmapCursor(m_dpy, pmap, pmap, &black, &black, 0, 0);
  XDefineCursor(m_dpy, m_win, invisible);
  XMapRaised(m_dpy, m_win);
  XSync(m_dpy, FALSE);
  XFreePixmap(m_dpy, pmap);

  XGCValues gcvalue;
  Colormap colormap;
  XColor white;
  GC gc;

  XAllocNamedColor(m_dpy, DefaultColormap(m_dpy, screen), "white", &white, &dummy);
  gc = XCreateGC(m_dpy, m_win, 0, &gcvalue);
  colormap = DefaultColormap(m_dpy, screen);
  XAllocColor(m_dpy, colormap, &white);
  XSetForeground(m_dpy, gc, white.pixel);

  //XDrawRectangle(m_dpy, m_win, gc, 0, 0, DisplayWidth(m_dpy, screen) - 1, DisplayHeight(m_dpy, screen) - 1);
  XDrawRectangle(m_dpy, m_win, gc, 0, 0, 1023, 767);

  int  x = 0, y = 0;
  for(x=0,y=0; x<1023;) {
    XDrawLine(m_dpy, m_win, gc, x, 0, x, 767);
    x += 50;
  }

  for(x=0,y=0; y<767;) {
    XDrawLine(m_dpy, m_win, gc, 0, y, 1023, y);
    y += 50;
  }

  //printf("display(%d, %d)\n", DisplayWidth(m_dpy, screen), DisplayHeight(m_dpy, screen));
  XFlush(m_dpy);

  m_state = TRUE;
}

void ScreenSaver::BlackScreen3() {
  char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  int screen;
  XSetWindowAttributes wa;
  Pixmap pmap;
  Cursor invisible;
  Window root;
  XColor black, dummy;

  if(!(m_dpy = XOpenDisplay(0))) {
    PRINTF("ScreenSaver: cannot open display\n");
    return;
  }

  screen = DefaultScreen(m_dpy);
  root = RootWindow(m_dpy, screen);

  wa.override_redirect = 1;
  wa.background_pixel = BlackPixel(m_dpy, screen);

  m_win = XCreateWindow(m_dpy, root, 0, 0, DisplayWidth(m_dpy, screen), DisplayHeight(m_dpy, screen),
    0, DefaultDepth(m_dpy, screen), CopyFromParent, DefaultVisual(m_dpy, screen),
    CWOverrideRedirect | CWBackPixel, &wa);
  XAllocNamedColor(m_dpy, DefaultColormap(m_dpy, screen), "black", &black, &dummy);
  pmap = XCreateBitmapFromData(m_dpy, m_win, curs, 8, 8);
  invisible = XCreatePixmapCursor(m_dpy, pmap, pmap, &black, &black, 0, 0);
  XDefineCursor(m_dpy, m_win, invisible);
  XMapRaised(m_dpy, m_win);
  XSync(m_dpy, FALSE);
  XFreePixmap(m_dpy, pmap);

  XGCValues gcvalue;
  Colormap colormap;
  XColor white;
  GC gc;

  XAllocNamedColor(m_dpy, DefaultColormap(m_dpy, screen), "white", &white, &dummy);
  gc = XCreateGC(m_dpy, m_win, 0, &gcvalue);
  colormap = DefaultColormap(m_dpy, screen);
  XAllocColor(m_dpy, colormap, &white);
  XSetForeground(m_dpy, gc, white.pixel);

  // XDrawRectangle(m_dpy, m_win, gc, 0, 0, DisplayWidth(m_dpy, screen) - 1, DisplayHeight(m_dpy, screen) - 1);
  XDrawRectangle(m_dpy, m_win, gc, 0, 0, 1023, 767);

  int  x = 0, y = 0, width = 50;
  for(x=10; x+width<1023;) {
    for(y=20; y+width<767;) {
      XFillRectangle(m_dpy, m_win, gc, x, y, width, width);
      y += 100;
    }

    x += 100;
  }

  XFlush(m_dpy);

  m_state = TRUE;
}

void ScreenSaver::DrawHintMessage() {
  string info = N_("Press any key to resume!");
  int screen;
  Colormap colormap;
  Visual* visual;
  XftDraw* xftDraw = NULL;
  XftColor xftColor;
  XRenderColor renderColor;
  XftPattern* pattern = NULL;
  XftResult result;
  XftFont* xftFont = NULL;
  SysGeneralSetting sysGS;

  renderColor.red = 0xFFFF;
  renderColor.green = 0xFFFF;
  renderColor.blue = 0xFFFF;
  renderColor.alpha = 0xFFFF;
  screen = DefaultScreen(m_dpy);
  colormap = DefaultColormap(m_dpy, screen);
  visual = DefaultVisual(m_dpy, screen);

  xftDraw = XftDrawCreate(m_dpy, m_win, visual, colormap);
  if(!xftDraw) {
    PRINTF("%s: Error with XftDrawCreate\n", __FUNCTION__);
    return;
  }

  XftColorAllocValue (m_dpy, visual, colormap, &renderColor, &xftColor);

  pattern = XftPatternCreate();
  if(!pattern) {
    PRINTF("%s: Error with XftPatternCreate\n", __FUNCTION__);
    return;
  }

  if(ZH == sysGS.GetLanguage()) {
    XftPatternAddString(pattern, "family", "WenQuanYi");
    XftPatternAddInteger(pattern, "pixelsize", 24);
  } else {
    XftPatternAddString(pattern, "family", "DejaVu Sans");
    XftPatternAddInteger(pattern, "pixelsize", 20);
  }

  XftPatternAddBool(pattern, "antialias", 1);
  xftFont = XftFontOpenPattern(m_dpy, XftFontMatch(m_dpy, screen, pattern, &result));
  if(!xftFont) {
    PRINTF("%s: Error with XftFontOpenPattern\n", __FUNCTION__);
    return;
  }

  int i = rand() % (DisplayWidth(m_dpy, screen)-300);
  int j = rand() % (DisplayHeight(m_dpy, screen)-50);

  XClearWindow(m_dpy, m_win);
  XftDrawStringUtf8 (xftDraw, &xftColor, xftFont,
    i < 100 ? 100 : i,
    j < 50 ? 50 : j,
    (XftChar8*)info.c_str(), info.size());
  // PRINTF("random num %d %d\n", i, j);

  XFlush(m_dpy);
  XftPatternDestroy(pattern);
  XftDrawDestroy(xftDraw);
}
