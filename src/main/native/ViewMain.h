// -*- c++ -*-
#ifndef VIEWMAIN_H
#define VIEWMAIN_H

#include "keyboard/KeyValueOpr.h"
#include "keyboard/KnobEvent.h"
#include "display/MenuArea.h"


//       |------------------------------------- 1024 --------------------------------------|
//           130       130                                          150          180
// ---   |---------|---------|-----------------------------------|---------|---------------|
//  |    |         |         |                                   |         |               |
//  | 60 |  LOGO   |Hospital |---------- Top --------------------|  Time   |   MenuLabel   |
//  |    |         |         |                                   |         |               |
//  |    |---------|---------|-----------------------------------|---------|---------------|
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
// 768   |                                Image                            | MenuNoteBook  |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |-----------------------------------------------------------------|---------------|
//  |    |                                                                 |               |
//  | 105|                                ShortcutMenu                     |   Reserved    |
//  |    |                                                                 |               |
//  |    |-----------------------------------------------------------------|---------------|
//  | 35 |                                   Hint                          | SystemStatus  |
// ---   |-----------------------------------------------------------------|---------------|

#define TOP_AREA_X 0 //ModLayout 180 PreLayout 220
#define TOP_AREA_Y 0

#define GAP_AREA_H 2

#define IMG_AREA_X 0 //ModLayout 180 PreLayout 220
#define IMG_AREA_Y 60

// #define STATUS_X 220
// #define STATUS_Y 644
// #define STATUS_WIDTH 150
// #define STATUS_HEIGHT (45-2)

//#define HINT_X (370+2)
#define HINT_X 0 //ModLayout 180 PreLayout 220
#define HINT_Y 733 //ModLayout 664 PreLayout 642

#define KNOB_X 0 //ModLayout 180 PreLayout 220
#define KNOB_Y 628 //ModLayout 705 PreLayout 685

#define SYSCURSOR_X             (SCREEN_WIDTH - MENU_AREA_W + MENU_AREA_W / 2)
#define SYSCUROSR_Y             (0 + MENU_AREA_H / 2)

gboolean UpdateTopArea(gpointer data);
void InvisibleCursor(bool invisible, bool resetCursor=TRUE);
void ChangeSystemCursor(GdkCursorType type);

class ImageArea;
class HintArea;
class KnobMenu;
class MenuArea;
class TopArea;
class NoteArea;

class ViewMain:public CKnobEvent {
public:
    ~ViewMain();
    static ViewMain* GetInstance();

    void Create(void);
    void Show(void);
    void Hide(void);
    void ShowMenu(void);
    void HideMenu(void);

    void MenuReviewCallBack(void);

    GtkWidget* GetMainWindow(void);
    void KeyEvent(unsigned char keyValue);
    void KnobEvent(unsigned char keyValue, unsigned char offset);
    void SliderEvent(unsigned char keyValue, unsigned char offset);
    void MouseEvent(char offsetX, char offsetY);
    void KnobKeyEvent(unsigned char keyValue);
    bool GetCursorVisible(void) {
        return m_cursorVisible;
    }
    void SetCursorVisible(bool visible) {
        m_cursorVisible = visible;
    }

private:
    ViewMain();

    unsigned char m_keyValue;
    static ViewMain* m_ptrInstance;
    static bool m_cursorVisible;
    int countN;
    GtkWidget* m_mainWindow;
    GtkWidget* m_fixedWindow;
    GtkWidget* m_daMenu;
    KnobMenu* m_ptrKnob;
    ImageArea* m_ptrImgArea;
    TopArea* m_ptrTopArea;
    HintArea* m_ptrHintArea;
    MenuArea* m_ptrMenuArea;
    NoteArea* m_ptrNoteArea;
    //signal handle
    //signal connect

    void MySleep(int msecond);
    int keyTSIN;

    static const unsigned int SUPER_AUTHEN_NUM = 11;
    static const unsigned int AUTHEN_NUM = 6;
    // anthen
    std::vector<unsigned char> m_vecAuthenInfo;
    std::vector<unsigned char> m_vecSuperAuthenInfo;
    bool m_statusAuthen;
    bool m_statusSuperAuthen;
    int m_timer;
    int m_super_timer;
    gboolean IsAuthenValid(void);
    gboolean IsSuperAuthenValid(void);
    static gboolean HandleAuthen(gpointer data) {
        return m_ptrInstance->IsAuthenValid();
    }
    static gboolean HandleSuperAuthen(gpointer data) {
        return m_ptrInstance->IsSuperAuthenValid();
    }

};

#endif
