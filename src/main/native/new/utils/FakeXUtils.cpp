#include "utils/FakeXUtils.h"

#include "Def.h"
#include "keyboard/KeyDef.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysNoteSetting.h"
#include "utils/Logger.h"
#include "utils/MainWindowConfig.h"

#define XK_PUBLISHING
#include <X11/keysymdef.h>

#include <gdk/gdkkeysyms.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>

#include <stdlib.h>
#include <unistd.h>

static int g_cursorRangeY0 = 128;
static int g_cursorRangeY1 = 213;

unsigned char g_key_note = 0;
static POINT g_sysCursor;

int GetLanguage();

Display* getXDisplay();
void doKeysymEvent(const unsigned int keysym, int is_press);
void XRemapKey(Display* display, int old_keycode, KeySym new_keysym);

// ---------------------------------------------------------

void FakeXMotionEvent(int offsetx, int offsety) {
  FakeXMotionEventMenu(g_sysCursor.x, g_sysCursor.y, offsetx, offsety);
}

void FakeXMotionEventFullRange(int offsetx, int offsety) {
  FakeXMotionEventScreen(g_sysCursor.x, g_sysCursor.y, offsetx, offsety);
}

void FakeXMotionEventImage(int &x, int &y, int offsetx, int offsety) {
  int x_start = IMG_AREA_X + IMAGE_X;
  int y_start = IMG_AREA_Y + IMAGE_Y;

  int adjust = 0;

  SysNoteSetting sysNoteSetting;

  switch(sysNoteSetting.GetFontSize()) {
  case 0:
    {
      adjust = 45;

      break;
    }
  case 1:
    {
      adjust = 30;

      break;
    }
  case 2:
    {
      adjust = 20;

      break;
    }
  default:
    {
      adjust = 30;

      break;
    }
  }

  // Mouse x direction Move
  if (((x + offsetx) > x_start) && (x + offsetx) < (x_start + IMAGE_W)) {
    x += offsetx;
  }

  if (((y - offsety) > y_start) && ((y - offsety) < (y_start + IMAGE_H - adjust))) {
    y -= offsety;
  }

  Display* display = getXDisplay();

  XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
  XFlush(display);
  XCloseDisplay(display);
}

void FakeXMotionEventMenu(int &x, int &y, int offsetx, int offsety) {
  int x_start = 0;
  int y_start = 0;

  // Mouse x direction Move
  if (((x + offsetx) > x_start) && (x + offsetx) < (x_start + MENU_AREA_W)) {
    x += offsetx;
  }

  if (((y - offsety) > y_start) && ((y - offsety) < (y_start + MENU_AREA_H))) {
    y -= offsety;
  }

  Display* display = getXDisplay();

  XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
  XFlush(display);
  XCloseDisplay(display);
}

void FakeXMotionEventMenuBiopsy(int offsetx, int offsety) {
  int x_start = 0;

  int x = g_sysCursor.x;
  int y = g_sysCursor.y;

  // Mouse x direction Move
  if (((x + offsetx) > x_start) && (x + offsetx) < (x_start + MENU_AREA_W)) {
    x += offsetx;
  }

  if (((y - offsety) > g_cursorRangeY0) && ((y - offsety) < g_cursorRangeY1)) {
    y -= offsety;
  }

  g_sysCursor.x = x;
  g_sysCursor.y = y;

  Display* display = getXDisplay();

  XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
  XFlush(display);
  XCloseDisplay(display);
}

void FakeXMotionEventScreen(int &x, int &y, int offsetx, int offsety) {
  int x_start = 0;
  int y_start = 0;
  int w = SCREEN_WIDTH;
  int h = SCREEN_HEIGHT;

  // 将鼠标移动范围锁定在SCREEN_WIDTH, SCREEN_HEIGHT之内
  // Mouse x direction Move
  Display* display = getXDisplay();

  if (((x + offsetx) > x_start) && (x + offsetx) < (x_start + w)) {
    x += offsetx;
  }

  if (((y - offsety) > y_start) && ((y - offsety) < (y_start + h))) {
    y -= offsety;
  }

  XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
  XFlush(display);
  XCloseDisplay(display);
}

void FakeEscKey() {
  doKeyEvent(9, 1);
  doKeyEvent(9, 0);
}

bool FakeAlphabet(unsigned char key) {
  g_key_note = key;

  int language = GetLanguage();

  #if defined(EMP_355) || defined(EMP_322)
    if (language == FR) {
      switch (key) {
      case 0x61:  // a
        {
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);

          return true;
        }
      case 0x41:  // A
        {
          doKeyEvent(50, 1);
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x62:  // b
        {
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);

          return true;
        }
      case 0x42:  // B
        {
          doKeyEvent(50, 1);
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x63:  // c
        {
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);

          return true;
        }
      case 0x43:  // C
        {
          doKeyEvent(50, 1);
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x64:  // d
        {
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);

          return true;
        }
      case 0x44:  // D
        {
          doKeyEvent(50, 1);
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x65:  // e
        {
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);

          return true;
        }
      case 0x45:  // E
        {
          doKeyEvent(50, 1);
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x66:  // f
        {
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);

          return true;
        }
      case 0x46:  // F
        {
          doKeyEvent(50, 1);
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x67:  // g
        {
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);

          return true;
        }
      case 0x47:  // G
        {
          doKeyEvent(50, 1);
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x68:  // h
        {
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);

          return true;
        }
      case 0x48:  // H
        {
          doKeyEvent(50, 1);
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x69:  // i
        {
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);

          return true;
        }
      case 0x49:  // I
        {
          doKeyEvent(50, 1);
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6A:  // j
        {
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);

          return true;
        }
      case 0x4A:  // J
        {
          doKeyEvent(50, 1);
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6B:  // k
        {
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);

          return true;
        }
      case 0x4B:  // K
        {
          doKeyEvent(50, 1);
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6C:  // l
        {
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);

          return true;
        }
      case 0x4C:  // L
        {
          doKeyEvent(50, 1);
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x4D:  // M
      case 0x6D:  // m
        {
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);

          return true;
        }
      case 0x6E:  // n
        {
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);

          return true;
        }
      case 0x4E:  // N
        {
          doKeyEvent(50, 1);
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6F:  // o
        {
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);

          return true;
        }
      case 0x4F:  // O
        {
          doKeyEvent(50, 1);
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x70:  // p
        {
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);

          return true;
        }
      case 0x50:  // P
        {
          doKeyEvent(50, 1);
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x71:  // q
        {
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);

          return true;
        }
      case 0x51:  // Q
        {
          doKeyEvent(50, 1);
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x72:  // r
        {
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);

          return true;
        }
      case 0x52:  // R
        {
          doKeyEvent(50, 1);
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x73:  // s
        {
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);

          return true;
        }
      case 0x53:  // S
        {
          doKeyEvent(50, 1);
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x74:  // t
        {
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);

          return true;
        }
      case 0x54:  // T
        {
          doKeyEvent(50, 1);
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x75:  // u
        {
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);

          return true;
        }
      case 0x55:  // U
        {
          doKeyEvent(50, 1);
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x76:  // v
        {
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);

          return true;
        }
      case 0x56:  // V
        {
          doKeyEvent(50, 1);
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x77:  // w
        {
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);

          return true;
        }
      case 0x57:  // W
        {
          doKeyEvent(50, 1);
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x78:  // x
        {
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);

          return true;
        }
      case 0x58:  // X
        {
          doKeyEvent(50, 1);
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x79:  // y
        {
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);

          return true;
        }
      case 0x59:  // Y
        {
          doKeyEvent(50, 1);
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x7A:  // z
        {
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);

          return true;
        }
      case 0x5A:  // Z
        {
          doKeyEvent(50, 1);
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);
          doKeyEvent(50, 0);

          return true;
        }
      }
    } else {
      switch (key) {
      case 0x61:  // a
        {
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);

          return true;
        }
      case 0x41:  // A
        {
          doKeyEvent(50, 1);
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x62:  // b
        {
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);

          return true;
        }
      case 0x42:  // B
        {
          doKeyEvent(50, 1);
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x63:  // c
        {
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);

          return true;
        }
      case 0x43:  // C
        {
          doKeyEvent(50, 1);
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x64:  // d
        {
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);

          return true;
        }
      case 0x44:  // D
        {
          doKeyEvent(50, 1);
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x65:  // e
        {
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);

          return true;
        }
      case 0x45:  // E
        {
          doKeyEvent(50, 1);
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x66:  // f
        {
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);

          return true;
        }
      case 0x46:  // F
        {
          doKeyEvent(50, 1);
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x67:  // g
        {
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);

          return true;
        }
      case 0x47:  // G
        {
          doKeyEvent(50, 1);
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x68:  // h
        {
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);

          return true;
        }
      case 0x48:  // H
        {
          doKeyEvent(50, 1);
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x69:  // i
        {
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);

          return true;
        }
      case 0x49:  // I
        {
          doKeyEvent(50, 1);
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6A:  // j
        {
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);

          return true;
        }
      case 0x4A:  // J
        {
          doKeyEvent(50, 1);
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6B:  // k
        {
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);

          return true;
        }
      case 0x4B:  // K
        {
          doKeyEvent(50, 1);
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6C:  // l
        {
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);

          return true;
        }
      case 0x4C:  // L
        {
          doKeyEvent(50, 1);
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6D:  // m
        {
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);

          return true;
        }
      case 0x4D:  // M
        {
          doKeyEvent(50, 1);
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6E:  // n
        {
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);

          return true;
        }
      case 0x4E:  // N
        {
          doKeyEvent(50, 1);
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6F:  // o
        {
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);

          return true;
        }
      case 0x4F:  // O
        {
          doKeyEvent(50, 1);
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x70:  // p
        {
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);

          return true;
        }
      case 0x50:  // P
        {
          doKeyEvent(50, 1);
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x71:  // q
        {
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);

          return true;
        }
      case 0x51:  // Q
        {
          doKeyEvent(50, 1);
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x72:  // r
        {
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);

          return true;
        }
      case 0x52:  // R
        {
          doKeyEvent(50, 1);
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x73:  // s
        {
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);

          return true;
        }
      case 0x53:  // S
        {
          doKeyEvent(50, 1);
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x74:  // t
        {
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);

          return true;
        }
      case 0x54:  // T
        {
          doKeyEvent(50, 1);
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x75:  // u
        {
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);

          return true;
        }
      case 0x55:  // U
        {
          doKeyEvent(50, 1);
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x76:  // v
        {
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);

          return true;
        }
      case 0x56:  // V
        {
          doKeyEvent(50, 1);
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x77:  // w
        {
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);

          return true;
        }
      case 0x57:  // W
        {
          doKeyEvent(50, 1);
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x78:  // x
        {
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);

          return true;
        }
      case 0x58:  // X
        {
          doKeyEvent(50, 1);
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x79:  // y
        {
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);

          return true;
        }
      case 0x59:  // Y
        {
          doKeyEvent(50, 1);
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x7A:  // z
        {
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);

          return true;
        }
      case 0x5A:  // Z
        {
          doKeyEvent(50, 1);
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);
          doKeyEvent(50, 0);

          return true;
        }
      }
    }
  #else
    if (language == FR) {
      switch (key) {
      case 0x61:  // a
        {
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);

          return true;
        }
      case 0x41:  // A
      case 0xA1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x62:  // b
        {
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);

          return true;
        }
      case 0x42:  // B
      case 0xA2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x63:  // c
        {
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);

          return true;
        }
      case 0x43:  // C
      case 0xA3:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x64:  // d
        {
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);

          return true;
        }
      case 0x44:  // D
      case 0xA4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x65:  // e
        {
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);

          return true;
        }
      case 0x45:  // E
      case 0xA5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x66:  // f
        {
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);

          return true;
        }
      case 0x46:  // F
      case 0xA6:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x67:  // g
        {
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);

          return true;
        }
      case 0x47:  // G
      case 0xA7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x68:  // h
        {
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);

          return true;
        }
      case 0x48:  // H
      case 0xA8:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x69:  // i
        {
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);

          return true;
        }
      case 0x49:  // I
      case 0xB0:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6A:  // j
        {
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);

          return true;
        }
      case 0x4A:  // J
      case 0xB1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6B:  // k
        {
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);

          return true;
        }
      case 0x4B:  // K
      case 0xB2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6C:  // l
        {
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);

          return true;
        }
      case 0x4C:  // L
      case 0xB4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x4D:  // M
      case 0x6D:  // m
        {
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);

          return true;
        }
      case 0xB5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6E:  // n
        {
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);

          return true;
        }
      case 0x4E:  // N
      case 0xB7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6F:  // o
        {
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);

          return true;
        }
      case 0x4F:  // O
        {
          doKeyEvent(50, 1);
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x70:  // p
        {
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);

          return true;
        }
      case 0x50:  // P
        {
          doKeyEvent(50, 1);
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x71:  // q
        {
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);

          return true;
        }
      case 0x51:  // Q
      case 0xC1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x72:  // r
        {
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);

          return true;
        }
      case 0x52:  // R
      case 0xC2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x73:  // s
        {
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);

          return true;
        }
      case 0x53:  // S
      case 0xC3:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x74:  // t
        {
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);

          return true;
        }
      case 0x54:  // T
      case 0xC4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x75:  // u
        {
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);

          return true;
        }
      case 0x55:  // U
      case 0xC5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x76:  // v
        {
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);

          return true;
        }
      case 0x56:  // V
      case 0xC6:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x77:  // w
        {
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);

          return true;
        }
      case 0x57:  // W
      case 0xC7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x78:  // x
        {
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);

          return true;
        }
      case 0x58:  // X
      case 0xC8:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x79:  // y
        {
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);

          return true;
        }
      case 0x59:  // Y
      case 0xC9:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x7A:  // z
        {
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);

          return true;
        }
      case 0x5A:  // Z
      case 0xCA:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);
          doKeyEvent(50, 0);

          return true;
        }
      }
    } else {
      switch (key) {
      case 0x61:  // a
        {
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);

          return true;
        }
      case 0x41:  // A
      case 0xA1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(38, 1);
          doKeyEvent(38, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x62:  // b
        {
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);

          return true;
        }
      case 0x42:  // B
      case 0xA2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(56, 1);
          doKeyEvent(56, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x63:  // c
        {
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);

          return true;
        }
      case 0x43:  // C
      case 0xA3:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(54, 1);
          doKeyEvent(54, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x64:  // d
        {
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);

          return true;
        }
      case 0x44:  // D
      case 0xA4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(40, 1);
          doKeyEvent(40, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x65:  // e
        {
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);

          return true;
        }
      case 0x45:  // E
      case 0xA5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(26, 1);
          doKeyEvent(26, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x66:  // f
        {
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);

          return true;
        }
      case 0x46:  // F
      case 0xA6:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(41, 1);
          doKeyEvent(41, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x67:  // g
        {
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);

          return true;
        }
      case 0x47:  // G
      case 0xA7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(42, 1);
          doKeyEvent(42, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x68:  // h
        {
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);

          return true;
        }
      case 0x48:  // H
      case 0xA8:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(43, 1);
          doKeyEvent(43, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x69:  // i
        {
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);

          return true;
        }
      case 0x49:  // I
      case 0xB0:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(31, 1);
          doKeyEvent(31, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6A:  // j
        {
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);

          return true;
        }
      case 0x4A:  // J
      case 0xB1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(44, 1);
          doKeyEvent(44, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6B:  // k
        {
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);

          return true;
        }
      case 0x4B:  // K
      case 0xB2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(45, 1);
          doKeyEvent(45, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6C:  // l
        {
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);

          return true;
        }
      case 0x4C:  // L
      case 0xB4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(46, 1);
          doKeyEvent(46, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6D:  // m
        {
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);

          return true;
        }
      case 0x4D:  // M
      case 0xB5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(58, 1);
          doKeyEvent(58, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6E:  // n
        {
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);

          return true;
        }
      case 0x4E:  // N
      case 0xB7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(57, 1);
          doKeyEvent(57, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x6F:  // o
        {
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);

          return true;
        }
      case 0x4F:  // O
        {
          doKeyEvent(50, 1);
          doKeyEvent(32, 1);
          doKeyEvent(32, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x70:  // p
        {
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);

          return true;
        }
      case 0x50:  // P
        {
          doKeyEvent(50, 1);
          doKeyEvent(33, 1);
          doKeyEvent(33, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x71:  // q
        {
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);

          return true;
        }
      case 0x51:  // Q
      case 0xC1:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(24, 1);
          doKeyEvent(24, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x72:  // r
        {
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);

          return true;
        }
      case 0x52:  // R
      case 0xC2:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(27, 1);
          doKeyEvent(27, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x73:  // s
        {
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);

          return true;
        }
      case 0x53:  // S
      case 0xC3:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(39, 1);
          doKeyEvent(39, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x74:  // t
        {
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);

          return true;
        }
      case 0x54:  // T
      case 0xC4:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(28, 1);
          doKeyEvent(28, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x75:  // u
        {
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);

          return true;
        }
      case 0x55:  // U
      case 0xC5:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(30, 1);
          doKeyEvent(30, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x76:  // v
        {
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);

          return true;
        }
      case 0x56:  // V
      case 0xC6:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(55, 1);
          doKeyEvent(55, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x77:  // w
        {
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);

          return true;
        }
      case 0x57:  // W
      case 0xC7:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(25, 1);
          doKeyEvent(25, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x78:  // x
        {
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);

          return true;
        }
      case 0x58:  // X
      case 0xC8:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(53, 1);
          doKeyEvent(53, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x79:  // y
        {
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);

          return true;
        }
      case 0x59:  // Y
      case 0xC9:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(29, 1);
          doKeyEvent(29, 0);
          doKeyEvent(50, 0);

          return true;
        }
      case 0x7A:  // z
        {
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);

          return true;
        }
      case 0x5A:  // Z
      case 0xCA:  // shift+
        {
          doKeyEvent(50, 1);
          doKeyEvent(52, 1);
          doKeyEvent(52, 0);
          doKeyEvent(50, 0);

          return true;
        }
      }
    }
  #endif

  return false;
}

bool FakeMouseButton(unsigned char key) {
  if (key == KEY_SET) {
    // left button

    doBtnEvent(1, 1);
    doBtnEvent(1, 0);

    return true;
  }

  return false;
}

bool FakeNumKey(unsigned char key) {
  g_key_note = key;

  switch (key) {
  case 0x31:  // 1
  case 0x11:  // caps + number
    {
      doKeyEvent(10, 1);
      doKeyEvent(10, 0);

      return true;
    }
  case 0x32:  // 2
  case 0x12:  // caps + number
    {
      doKeyEvent(11, 1);
      doKeyEvent(11, 0);

      return true;
    }
  case 0x33:  // 3
  case 0x13:  // caps + number
    {
      doKeyEvent(12, 1);
      doKeyEvent(12, 0);

      return true;
    }
  case 0x34:  // 4
  case 0x14:  // caps + number
    {
      doKeyEvent(13, 1);
      doKeyEvent(13, 0);

      return true;
    }
  case 0x35:  // 5
  case 0x15:  // caps + number
    {
      doKeyEvent(14, 1);
      doKeyEvent(14, 0);

      return true;
    }
  case 0x36:  // 6
  case 0x16:  // caps + number
    {
      doKeyEvent(15, 1);
      doKeyEvent(15, 0);

      return true;
    }
  case 0x37:  // 7
  case 0x17:  // caps + number
    {
      doKeyEvent(16, 1);
      doKeyEvent(16, 0);

      return true;
    }
  case 0x38:  // 8
  case 0x1C:  // caps + number
    {
      doKeyEvent(17, 1);
      doKeyEvent(17, 0);

      return true;
    }
  case 0x39:  // 9
  case 0x1D:  // caps + number
    {
      doKeyEvent(18, 1);
      doKeyEvent(18, 0);

      return true;
    }
  case 0x30:  // 0
  case 0x1E:  // caps + number
    {
      doKeyEvent(19, 1);
      doKeyEvent(19, 0);

      return true;
    }
  }

  return false;
}

bool FakePunctuation(unsigned char key) {
  g_key_note = key;

  int language = GetLanguage();

  if (language == FR || language == ES || language == PL) {
    switch (key) {
    case 0x21:  // !
      {
        doKeyEvent(50, 1);
        doKeyEvent(10, 1);
        doKeyEvent(10, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x40:  // @
      {
        doKeyEvent(50, 1);
        doKeyEvent(11, 1);
        doKeyEvent(11, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x23:  // #
      {
        doKeyEvent(50, 1);
        doKeyEvent(12, 1);
        doKeyEvent(12, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x24:  // $
      {
        doKeyEvent(50, 1);
        doKeyEvent(13, 1);
        doKeyEvent(13, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x25:  // %
      {
        doKeyEvent(50, 1);
        doKeyEvent(14, 1);
        doKeyEvent(14, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5E:  // ^
      {
        doKeyEvent(50, 1);
        doKeyEvent(15, 1);
        doKeyEvent(15, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x26:  // &
      {
        doKeyEvent(50, 1);
        doKeyEvent(16, 1);
        doKeyEvent(16, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2A:  // *
      {
        doKeyEvent(50, 1);
        doKeyEvent(17, 1);
        doKeyEvent(17, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x28:  // (
      {
        doKeyEvent(50, 1);
        doKeyEvent(18, 1);
        doKeyEvent(18, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x29:  // )
      {
        doKeyEvent(50, 1);
        doKeyEvent(19, 1);
        doKeyEvent(19, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2C:  // ,
    case 0xF7:  // , caps + syb
      {
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);

        return true;
      }
    case 0x2E:  // .
    case 0xF8:  // . caps + syb
      {
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);

        return true;
      }
    case 0x2D:  // -
    case 0xF0:  // - caps + syb
      {
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);

        return true;
      }
    case 0x3D:  // =
    case 0xF1:  // = caps + syb
      {
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);

        return true;
      }
    case 0x5C:  // "\"
    case 0xF4:  // "\" caps + syb
      {
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);

        return true;
      }
    case 0x5B:  // [
    case 0xF2:  // [ caps + syb
      {
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);

        return true;
      }
    case 0x5D:  // ]
    case 0xF3:  // ] caps + syb
      {
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);

        return true;
      }
    case 0x3B:  // ;
      {
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);

        return true;
      }
    case 0x27:  // '单引号
    case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);

        return true;
      }
    case 0x2F:  // /
    case 0xF9:  // / caps + syb
      {
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);

        return true;
      }
    case 0x60:  // `ESC上标点
    case 0xFA:  // `ESC上标点 caps + syb
      {
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);

        return true;
      }
    case 0x3C:  // <
      {
        doKeyEvent(50, 1);
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3E:  // >
      {
        doKeyEvent(50, 1);
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5F:  // _
      {
        doKeyEvent(50, 1);
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2B:  // +
      {
        doKeyEvent(50, 1);
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7C:  // |
      {
        doKeyEvent(50, 1);
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7B:  // {
      {
        doKeyEvent(50, 1);
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7D:  // }
      {
        doKeyEvent(50, 1);
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3A:  // :
    case 0xF5:  // : caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x22:  // "
      {
        doKeyEvent(50, 1);
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3F:  // ?
      {
        doKeyEvent(50, 1);
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7E:  // ~
      {
        doKeyEvent(50, 1);
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x08:  // BackSpace
      {
        doKeyEvent(22, 1);
        doKeyEvent(22, 0);

        return true;
      }
    case 0xA0:  // Delete
      {
        doKeyEvent(119, 1);
        doKeyEvent(119, 0);

        return true;
      }
    case 0x20:  // space
      {
        doKeyEvent(65, 1);
        doKeyEvent(65, 0);

        return true;
      }
    case 0x0D:  // enter
      {
        doKeyEvent(36, 1);
        doKeyEvent(36, 0);

        return true;
      }
    case 0x09:  // tab
      {
        doKeyEvent(23, 1);
        doKeyEvent(23, 0);

        return true;
      }
    case 0x18:  // up
      {
        doKeyEvent(111, 1);
        doKeyEvent(111, 0);

        return true;
      }
    case 0x19:  // down
      {
        doKeyEvent(116, 1);
        doKeyEvent(116, 0);

        return true;
      }
    case 0x1A:  // right
      {
        doKeyEvent(114, 1);
        doKeyEvent(114, 0);

        return true;
      }
    case 0x1B:  // left
      {
        doKeyEvent(113, 1);
        doKeyEvent(113, 0);

        return true;
      }
    case 0xAD:  // malesymbol
      {
        doKeysymEvent(GDK_malesymbol, 1);
        doKeysymEvent(GDK_malesymbol, 0);

        return true;
      }
    case 0xAC:  // femalesymbol
      {
        doKeysymEvent(GDK_femalesymbol, 1);
        doKeysymEvent(GDK_femalesymbol, 0);

        return true;
      }
    #if defined(EMP_313)
      case 0xE7:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE0:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #else
      case 0xE0:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE7:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #endif
    }
  } else if (language == DE) {
    switch (key) {
    case 0x21:  // !
      {
        doKeyEvent(50, 1);
        doKeyEvent(10, 1);
        doKeyEvent(10, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x40:  // @
      {
        doKeyEvent(50, 1);
        doKeyEvent(11, 1);
        doKeyEvent(11, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x23:  // #
      {
        doKeyEvent(50, 1);
        doKeyEvent(12, 1);
        doKeyEvent(12, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x24:  // $
      {
        doKeyEvent(50, 1);
        doKeyEvent(13, 1);
        doKeyEvent(13, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x25:  // %
      {
        doKeyEvent(50, 1);
        doKeyEvent(14, 1);
        doKeyEvent(14, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5E:  // ^
      {
        doKeyEvent(50, 1);
        doKeyEvent(15, 1);
        doKeyEvent(15, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x26:  // &
      {
        doKeyEvent(50, 1);
        doKeyEvent(16, 1);
        doKeyEvent(16, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2A:  // *
      {
        doKeyEvent(50, 1);
        doKeyEvent(17, 1);
        doKeyEvent(17, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x28:  // (
      {
        doKeyEvent(50, 1);
        doKeyEvent(18, 1);
        doKeyEvent(18, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x29:  // )
      {
        doKeyEvent(50, 1);
        doKeyEvent(19, 1);
        doKeyEvent(19, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2C:  // ,
    case 0xF7:  // , caps + syb
      {
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);

        return true;
      }
    case 0x2E:  // .
    case 0xF8:  // . caps + syb
      {
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);

        return true;
      }
    case 0x2D:  // -
    case 0xF0:  // - caps + syb
      {
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);

        return true;
      }
    case 0x3D:  // =
    case 0xF1:  // = caps + syb
      {
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);

        return true;
      }
    case 0x5C:  // "\"
    case 0xF4:  // "\" caps + syb
      {
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);

        return true;
      }
    case 0x5B:  // [
    // case 0xF2:  // [ caps + syb
      {
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);

        return true;
      }
    case 0x5D:  // ]
    case 0xF3:  // ] caps + syb
      {
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);

        return true;
      }
    case 0x3B:  // ;
    // case 0xF5:  // : caps + syb
      {
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);

        return true;
      }
    case 0x27:  // '单引号
    // case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);

        return true;
      }
    case 0x2F:  // /
    case 0xF9:  // / caps + syb
      {
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);

        return true;
      }
    case 0x60:  // `ESC上标点
    case 0xFA:  // `ESC上标点 caps + syb
      {
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);

        return true;
      }
    case 0x3C:  // <
      {
        doKeyEvent(50, 1);
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3E:  // >
      {
        doKeyEvent(50, 1);
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5F:  // _
      {
        doKeyEvent(50, 1);
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2B:  // +
      {
        doKeyEvent(50, 1);
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7C:  // |
      {
        doKeyEvent(50, 1);
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7B:  // {
    case 0xF2:  // [ caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7D:  // }
      {
        doKeyEvent(50, 1);
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3A:  // :
    case 0xF5:
      {
        doKeyEvent(50, 1);
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x22:  // "
    case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3F:  // ?
      {
        doKeyEvent(50, 1);
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7E:  // ~
      {
        doKeyEvent(50, 1);
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x08:  // BackSpace
      {
        doKeyEvent(22, 1);
        doKeyEvent(22, 0);

        return true;
      }
    case 0xA0:  // Delete
      {
        doKeyEvent(119, 1);
        doKeyEvent(119, 0);

        return true;
      }
    case 0x20:  // space
      {
        doKeyEvent(65, 1);
        doKeyEvent(65, 0);

        return true;
      }
    case 0x0D:  // enter
      {
        doKeyEvent(36, 1);
        doKeyEvent(36, 0);

        return true;
      }
    case 0x09:  // tab
      {
        doKeyEvent(23, 1);
        doKeyEvent(23, 0);

        return true;
      }
    case 0x18:  // up
      {
        doKeyEvent(111, 1);
        doKeyEvent(111, 0);

        return true;
      }
    case 0x19:  // down
      {
        doKeyEvent(116, 1);
        doKeyEvent(116, 0);

        return true;
      }
    case 0x1A:  // right
      {
        doKeyEvent(114, 1);
        doKeyEvent(114, 0);

        return true;
      }
    case 0x1B:  // left
      {
        doKeyEvent(113, 1);
        doKeyEvent(113, 0);

        return true;
      }
    case 0xAD:  // malesymbol
      {
        doKeysymEvent(GDK_malesymbol, 1);
        doKeysymEvent(GDK_malesymbol, 0);

        return true;
      }
    case 0xAC:  // femalesymbol
      {
        doKeysymEvent(GDK_femalesymbol, 1);
        doKeysymEvent(GDK_femalesymbol, 0);

        return true;
      }
    #if defined(EMP_313)
      case 0xE7:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE0:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #else
      case 0xE0:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE7:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #endif
    }
  } else if (language == RU) {
    switch (key) {
    case 0x21:  // !
      {
        doKeyEvent(50, 1);
        doKeyEvent(10, 1);
        doKeyEvent(10, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x40:  // @
      {
        doKeyEvent(50, 1);
        doKeyEvent(11, 1);
        doKeyEvent(11, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x23:  // #
      {
        doKeyEvent(50, 1);
        doKeyEvent(12, 1);
        doKeyEvent(12, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x24:  // $
      {
        doKeyEvent(50, 1);
        doKeyEvent(13, 1);
        doKeyEvent(13, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x25:  // %
      {
        doKeyEvent(50, 1);
        doKeyEvent(14, 1);
        doKeyEvent(14, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5E:  // ^
      {
        doKeyEvent(50, 1);
        doKeyEvent(15, 1);
        doKeyEvent(15, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x26:  // &
      {
        doKeyEvent(50, 1);
        doKeyEvent(16, 1);
        doKeyEvent(16, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2A:  // *
      {
        doKeyEvent(50, 1);
        doKeyEvent(17, 1);
        doKeyEvent(17, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x28:  // (
      {
        doKeyEvent(50, 1);
        doKeyEvent(18, 1);
        doKeyEvent(18, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x29:  // )
      {
        doKeyEvent(50, 1);
        doKeyEvent(19, 1);
        doKeyEvent(19, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2C:  // ,
    // case 0xF7:  // , caps + syb
      {
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);

        return true;
      }
    case 0x2E:  // .
    // case 0xF8:  // . caps + syb
      {
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);

        return true;
      }
    case 0x2D:  // -
    case 0xF0:  // - caps + syb
      {
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);

        return true;
      }
    case 0x3D:  // =
    case 0xF1:  // = caps + syb
      {
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);

        return true;
      }
    case 0x5C:  // "\"
    case 0xF4:  // "\" caps + syb
      {
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);

        return true;
      }
    case 0x5B:  // [
    // case 0xF2:  // [ caps + syb
      {
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);

        return true;
      }
    case 0x5D:  // ]
    // case 0xF3:  // ] caps + syb
      {
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);

        return true;
      }
    case 0x3B:  // ;
    // case 0xF5:  // : caps + syb
      {
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);

        return true;
      }
    case 0x27:  // '单引号
    // case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);

        return true;
      }
    case 0x2F:  // /
    case 0xF9:  // / caps + syb
      {
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);

        return true;
      }
    case 0x60:  // `ESC上标点
    case 0xFA:  // `ESC上标点 caps + syb
      {
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);

        return true;
      }
    case 0x3C:  // <
    case 0xF7:  // , caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3E:  // >
    case 0xF8:  // . caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5F:  // _
      {
        doKeyEvent(50, 1);
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2B:  // +
      {
        doKeyEvent(50, 1);
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7C:  // |
      {
        doKeyEvent(50, 1);
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7B:  // {
    case 0xF2:  // [ caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7D:  // }
    case 0xF3:  // ] caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3A:  // :
    case 0xF5:  // : caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x22:  // "
    case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(50, 1);
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3F:  // ?
      {
        doKeyEvent(50, 1);
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7E:  // ~
      {
        doKeyEvent(50, 1);
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x08:  // BackSpace
      {
        doKeyEvent(22, 1);
        doKeyEvent(22, 0);

        return true;
      }
    case 0xA0:  // Delete
      {
        doKeyEvent(119, 1);
        doKeyEvent(119, 0);

        return true;
      }
    case 0x20:  // space
      {
        doKeyEvent(65, 1);
        doKeyEvent(65, 0);

        return true;
      }
    case 0x0D:  // enter
      {
        doKeyEvent(36, 1);
        doKeyEvent(36, 0);

        return true;
      }
    case 0x09:  // tab
      {
        doKeyEvent(23, 1);
        doKeyEvent(23, 0);

        return true;
      }
    case 0x18:  // up
      {
        doKeyEvent(111, 1);
        doKeyEvent(111, 0);

        return true;
      }
    case 0x19:  // down
      {
        doKeyEvent(116, 1);
        doKeyEvent(116, 0);

        return true;
      }
    case 0x1A:  // right
      {
        doKeyEvent(114, 1);
        doKeyEvent(114, 0);

        return true;
      }
    case 0x1B:  // left
      {
        doKeyEvent(113, 1);
        doKeyEvent(113, 0);

        return true;
      }
    case 0xAD:  // malesymbol
      {
        doKeysymEvent(GDK_malesymbol, 1);
        doKeysymEvent(GDK_malesymbol, 0);

        return true;
      }
    case 0xAC:  // femalesymbol
      {
        doKeysymEvent(GDK_femalesymbol, 1);
        doKeysymEvent(GDK_femalesymbol, 0);

        return true;
      }
    #if defined(EMP_313)
      case 0xE7:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE0:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #else
      case 0xE0:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE7:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #endif
    }
  } else {
    switch (key) {
    case 0x21:  // !
      {
        doKeyEvent(50, 1);
        doKeyEvent(10, 1);
        doKeyEvent(10, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x40:  // @
      {
        doKeyEvent(50, 1);
        doKeyEvent(11, 1);
        doKeyEvent(11, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x23:  // #
      {
        doKeyEvent(50, 1);
        doKeyEvent(12, 1);
        doKeyEvent(12, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x24:  // $
      {
        doKeyEvent(50, 1);
        doKeyEvent(13, 1);
        doKeyEvent(13, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x25:  // %
      {
        doKeyEvent(50, 1);
        doKeyEvent(14, 1);
        doKeyEvent(14, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5E:  // ^
      {
        doKeyEvent(50, 1);
        doKeyEvent(15, 1);
        doKeyEvent(15, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x26:  // &
      {
        doKeyEvent(50, 1);
        doKeyEvent(16, 1);
        doKeyEvent(16, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2A:  // *
      {
        doKeyEvent(50, 1);
        doKeyEvent(17, 1);
        doKeyEvent(17, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x28:  // (
      {
        doKeyEvent(50, 1);
        doKeyEvent(18, 1);
        doKeyEvent(18, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x29:  // )
      {
        doKeyEvent(50, 1);
        doKeyEvent(19, 1);
        doKeyEvent(19, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2C:  // ,
    case 0xF7:  // , caps + syb
      {
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);

        return true;
      }
    case 0x2E:  // .
    case 0xF8:  // . caps + syb
      {
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);

        return true;
      }
    case 0x2D:  // -
    case 0xF0:  // - caps + syb
      {
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);

        return true;
      }
    case 0x3D:  // =
    case 0xF1:  // = caps + syb
      {
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);

        return true;
      }
    case 0x5C:  // "\"
    case 0xF4:  // "\" caps + syb
      {
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);

        return true;
      }
    case 0x5B:  // [
    case 0xF2:  // [ caps + syb
      {
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);

        return true;
      }
    case 0x5D:  // ]
    case 0xF3:  // ] caps + syb
      {
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);

        return true;
      }
    case 0x3B:  // ;
    case 0xF5:  // ; caps + syb
      {
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);

        return true;
      }
    case 0x27:  // '单引号
    case 0xF6:  // '单引号 caps + syb
      {
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);

        return true;
      }
    case 0x2F:  // /
    case 0xF9:  // / caps + syb
      {
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);

        return true;
      }
    case 0x60:  // `ESC上标点
    case 0xFA:  // `ESC上标点 caps + syb
      {
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);

        return true;
      }
    case 0x3C:  // <
      {
        doKeyEvent(50, 1);
        doKeyEvent(59, 1);
        doKeyEvent(59, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3E:  // >
      {
        doKeyEvent(50, 1);
        doKeyEvent(60, 1);
        doKeyEvent(60, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x5F:  // _
      {
        doKeyEvent(50, 1);
        doKeyEvent(20, 1);
        doKeyEvent(20, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x2B:  // +
      {
        doKeyEvent(50, 1);
        doKeyEvent(21, 1);
        doKeyEvent(21, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7C:  // |
      {
        doKeyEvent(50, 1);
        doKeyEvent(51, 1);
        doKeyEvent(51, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7B:  // {
      {
        doKeyEvent(50, 1);
        doKeyEvent(34, 1);
        doKeyEvent(34, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7D:  // }
      {
        doKeyEvent(50, 1);
        doKeyEvent(35, 1);
        doKeyEvent(35, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3A:  // :
      {
        doKeyEvent(50, 1);
        doKeyEvent(47, 1);
        doKeyEvent(47, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x22:  // "
      {
        doKeyEvent(50, 1);
        doKeyEvent(48, 1);
        doKeyEvent(48, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x3F:  // ?
      {
        doKeyEvent(50, 1);
        doKeyEvent(61, 1);
        doKeyEvent(61, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x7E:  // ~
      {
        doKeyEvent(50, 1);
        doKeyEvent(49, 1);
        doKeyEvent(49, 0);
        doKeyEvent(50, 0);

        return true;
      }
    case 0x08:  // BackSpace
      {
        doKeyEvent(22, 1);
        doKeyEvent(22, 0);

        return true;
      }
    case 0xA0:  // Delete
      {
        doKeyEvent(119, 1);
        doKeyEvent(119, 0);

        return true;
      }
    case 0x20:  // space
      {
        doKeyEvent(65, 1);
        doKeyEvent(65, 0);

        return true;
      }
    case 0x0D:  // enter
      {
        doKeyEvent(36, 1);
        doKeyEvent(36, 0);

        return true;
      }
    case 0x09:  // tab
      {
        doKeyEvent(23, 1);
        doKeyEvent(23, 0);

        return true;
      }
    case 0x18:  // up
      {
        doKeyEvent(111, 1);
        doKeyEvent(111, 0);

        return true;
      }
    case 0x19:  // down
      {
        doKeyEvent(116, 1);
        doKeyEvent(116, 0);

        return true;
      }
    case 0x1A:  // right
      {
        doKeyEvent(114, 1);
        doKeyEvent(114, 0);

        return true;
      }
    case 0x1B:  // left
      {
        doKeyEvent(113, 1);
        doKeyEvent(113, 0);

        return true;
      }
    case 0xAD:  // malesymbol
      {
        doKeysymEvent(GDK_malesymbol, 1);
        doKeysymEvent(GDK_malesymbol, 0);

        return true;
      }
    case 0xAC:  // femalesymbol
      {
        doKeysymEvent(GDK_femalesymbol, 1);
        doKeysymEvent(GDK_femalesymbol, 0);

        return true;
      }
    #if defined(EMP_313)
      case 0xE7:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE0:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #else
      case 0xE0:  // page up
        {
          doKeyEvent(112, 1);
          doKeyEvent(112, 0);

          return true;
        }
      case 0xE7:  // page down
        {
          doKeyEvent(117, 1);
          doKeyEvent(117, 0);

          return true;
        }
    #endif
    }
  }

  return false;
}

void doBtnEvent(const unsigned int button, int is_press) {
  Display* display = getXDisplay();

  unsigned long delay = 0;

  if (is_press) {
    XTestFakeButtonEvent(display, button, TRUE, delay);
  } else {
    XTestFakeButtonEvent(display, button, FALSE, delay);
  }

  XCloseDisplay(display);
}

void doKeyEvent(const unsigned int keycode, int is_press) {
  Display* display = getXDisplay();

  unsigned long delay = 0;

  if (is_press) {
    XTestFakeKeyEvent(display, keycode, TRUE, delay);
  } else {
    XTestFakeKeyEvent(display, keycode, FALSE, delay);
  }

  usleep(20000);
  XFlush(display);
  XCloseDisplay(display);
}

void SetMenuBiopsyCursorYRange(int y0, int y1) {
  g_cursorRangeY0 = y0;
  g_cursorRangeY1 = y1;
}

void SetSystemCursor(int x, int y) {
  g_sysCursor.x = x;
  g_sysCursor.y = y;

  Display* display = getXDisplay();

  XTestFakeMotionEvent(display, DefaultScreen(display), x, y, 0);
  XFlush(display);
  XCloseDisplay(display);
}

void SetSystemCursorToCenter() {
  int x = SCREEN_WIDTH / 2;
  int y = SCREEN_HEIGHT / 2;

  SetSystemCursor(x, y);
}

void ChangeKeymap() {
  Display* display = getXDisplay();

  int min_keycode = 0;
  int max_keycode = 255;

  XDisplayKeycodes(display, &min_keycode, &max_keycode);

  XRemapKey(display, max_keycode, XK_femalesymbol);
  XRemapKey(display, max_keycode - 1, XK_malesymbol);

  XFlush(display);
  XCloseDisplay(display);
}

void ResetIfOutOfRange() {
  if ((g_sysCursor.x > MENU_AREA_W) || (g_sysCursor.y > MENU_AREA_H)) {
    SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
  }
}

// ---------------------------------------------------------

int GetLanguage() {
  SysGeneralSetting sysGeneralSetting;

  return sysGeneralSetting.GetLanguage();
}

Display* getXDisplay() {
  int ourEvent, ourError, ourMajor, ourMinor;

  Display* display = XOpenDisplay(NULL);

  if (display == NULL) {
    Logger::error("This program is made to be run in X Windows");

    exit(1);
  }

  if (!XTestQueryExtension(display, &ourEvent, &ourError, &ourMajor, &ourMinor)) {
    Logger::error("XServer " + string(DisplayString(display)) + " doesn't support the Xtest extensions we need...");

    XCloseDisplay(display);

    exit(1);
  }

  XSync(display, TRUE);

  return display;
}

void doKeysymEvent(const unsigned int keysym, int is_press) {
  Display* display = getXDisplay();

  unsigned int keycode = XKeysymToKeycode(display, keysym);
  unsigned long delay = 0;

  if (is_press) {
    XTestFakeKeyEvent(display, keycode, TRUE, delay);
  } else {
    XTestFakeKeyEvent(display, keycode, FALSE, delay);
  }

  usleep(20000);

  XFlush(display);
  XCloseDisplay(display);
}

void XRemapKey(Display* display, int old_keycode, KeySym new_keysym) {
  if (old_keycode > 0) {
    XChangeKeyboardMapping(display, old_keycode, 1, &new_keysym, 1);
  }
}
