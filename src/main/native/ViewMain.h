// -*- c++ -*-
#ifndef VIEWMAIN_H
#define VIEWMAIN_H

#include "keyboard/KeyValueOpr.h"
#include "keyboard/KnobEvent.h"

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
