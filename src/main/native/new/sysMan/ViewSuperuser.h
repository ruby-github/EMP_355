#ifndef VIEW_SUPER_USER_H
#define VIEW_SUPER_USER_H

#include <vector>
#include <string>
#include "utils/FakeXEvent.h"

class ViewSuperuser:public FakeXEvent {
public:
    ~ViewSuperuser();
    static ViewSuperuser* GetInstance();
    void Authenticate(void);
    void CreateWindow(void);
    void CreateDemoWindow(void);
    void DestroyWindow(void);
    bool GetDemoStatus() {
        return !m_statusDemo;
    }
    void ExitDemo();

#ifdef TRANSDUCER
    int GetPressValue() {
        return m_tranPressCorrect;
    }
#endif

private:
    ViewSuperuser();

    static ViewSuperuser* m_ptrInstance;
    static const unsigned int AUTHEN_NUM = 11;
    static const unsigned int DEMO_NUM = 5;

#if defined(EMP_360)
    //static const int WPORBE_NUM = 6;
    static const int WPORBE_NUM = 8;
#elif defined(EMP_161)
    static const int WPORBE_NUM = 5; //4;
#elif (defined(EMP_322) || defined(EMP_313))
    static const int WPORBE_NUM = 9; //7;
#elif defined(EMP_430)
    static const int WPORBE_NUM = 5;
#elif defined(EMP_440)
    static const int WPORBE_NUM = 4;
#elif (defined(EMP_355))
#ifdef VET
    static const int WPORBE_NUM = 13;
#else
    static const int WPORBE_NUM = 12; //10;//11;
#endif
#else
#ifdef VET
    static const int WPORBE_NUM = 11;
#else
    static const int WPORBE_NUM = 11;
#endif
#endif

    static std::string m_probeName[WPORBE_NUM];

    GtkWidget *m_window;
    GtkWidget *m_labelHint;
    GtkWidget *m_treeviewProbe;

    // anthen
    std::vector<unsigned char> m_vecAuthenInfo;
    bool m_statusAuthen;
    bool m_statusDemo;
    int m_timer;

    //probe
    int m_probeType;
    //transducer press value correct
    int m_tranPressCorrect;
    GtkWidget* create_probe_treeview(void);
    GtkTreeModel* create_probe_model(void);
    virtual void KeyEvent(unsigned char keyValue);

    // probe
    gboolean UpdateProbeStatus(const char* socket, const char* status);
    gboolean IsAuthenValid(void);
    gboolean WriteProbe(void);
    bool ConfirmAuthen(void);
    bool ConfirmDemo(void);
    void Exit(void);

// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void MachineChanged(GtkComboBox *combobox);
    void ProbeChanged(GtkComboBox *combobox);
    void BtnWriteClicked(GtkButton *button);
    void BtnExitClicked(GtkButton *button);
    void ChgAperture(GtkComboBox *combobox);

#ifdef TRANSDUCER
    void SpinbuttonPressAdjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
#endif

    //demo
    void BtnEnterDemoClicked(GtkButton *button);
    void BtnChgMonitorClicked(GtkButton *button);
    void BtnChgMonitorClicked2(GtkButton *button);

    // signal connect
    static gboolean HandleWindowDeleteEvent(GtkWidget *widget, GdkEvent *event, ViewSuperuser *data) {
        return data->WindowDeleteEvent(widget, event);
    }
    static void HandleMachineChanged(GtkComboBox *widget, ViewSuperuser *data) {
        return data->MachineChanged(widget);
    }
    static void HandleProbeChanged(GtkComboBox *widget, ViewSuperuser *data) {
        return data->ProbeChanged(widget);
    }
    static void HandleBtnWriteClicked(GtkButton *widget, ViewSuperuser *data) {
        return data->BtnWriteClicked(widget);
    }
    static void HandleBtnExitClicked(GtkButton *widget, ViewSuperuser *data) {
        return data->BtnExitClicked(widget);
    }
    static void HandleChgAperture(GtkComboBox *widget, ViewSuperuser *data) {
        return data->ChgAperture(widget);
    }
    static void HandleBtnChgMonitorClicked(GtkButton *widget, ViewSuperuser *data) {
        return data->BtnChgMonitorClicked(widget);
    }
    static void HandleBtnChgMonitorClicked2(GtkButton *widget, ViewSuperuser *data) {
        return data->BtnChgMonitorClicked2(widget);
    }

#ifdef TRANSDUCER
    static void on_spinbutton_press_adjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSuperuser *data) {
        data->SpinbuttonPressAdjust(editable, new_text, new_text_length, position);
    }
#endif

    //demo
    static void HandleBtnEnterDemoClicked(GtkButton *widget, ViewSuperuser *data) {
        return data->BtnEnterDemoClicked(widget);
    }

    static gboolean HandleAuthen(gpointer data) {
        return m_ptrInstance->IsAuthenValid();
    }
    static gboolean HandleWriteProbe(gpointer data) {
        return m_ptrInstance->WriteProbe();
    }
};

#endif
