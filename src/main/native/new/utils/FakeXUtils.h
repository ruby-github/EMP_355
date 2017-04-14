#ifndef __FAKE_X_UTILS_H__
#define __FAKE_X_UTILS_H__

extern unsigned char g_key_note;

enum LANG_TYPE {EN, ZH, RU, PL, FR, ES, DE, LANG_MAX};

void FakeXMotionEvent(int offsetx, int offsety);
void FakeXMotionEventFullRange(int offsetx, int offsety);
void FakeXMotionEventImage(int &x, int &y, int offsetx, int offsety);
void FakeXMotionEventMenu(int &x, int &y, int offsetx, int offsety);
void FakeXMotionEventMenuBiopsy(int offsetx, int offsety);
void FakeXMotionEventScreen(int &x, int &y, int offsetx, int offsety);

void FakeEscKey();

bool FakeAlphabet(unsigned char key);
bool FakeMouseButton(unsigned char key);
bool FakeNumKey(unsigned char key);
bool FakePunctuation(unsigned char key);

void doBtnEvent(const unsigned int button, int is_press);
void doKeyEvent(const unsigned int keycode, int is_press);

void SetMenuBiopsyCursorYRange(int y0, int y1);
void SetSystemCursor(int x, int y);
void SetSystemCursorToCenter();

void ChangeKeymap();
void ResetIfOutOfRange();

#endif