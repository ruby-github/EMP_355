#include <gtk/gtk.h>

#include "sysMan/ViewSuperuser.h"
#include "ViewMain.h"
#include "display/gui_func.h"
#include "keyboard/KeyDef.h"
#include "probe/ProbeMan.h"
#include "keyboard/MultiFuncFactory.h"
#include "imageControl/Img2D.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "sysMan/ScreenSaver.h"
#include <iostream>
#include "sysMan/SysGeneralSetting.h"
#include "display/ImageAreaPara.h"
#include "periDevice/MonitorControl.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

using namespace std;

#if defined(EMP_360)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "3.5CV", "6.5VMC", "7.5LVS", "3.5MC", "6.5MC", "30P16A", "35D40J(SNR)", "35D40J(NDK)"
};
#elif defined(EMP_161)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "3.5CV", "6.5VMC", "7.5LVS", "3.5MC", "30P16A"
};
#elif (defined(EMP_322) || defined(EMP_313))
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C50J", "75L40J", "65C10J", "35C20G", "65C20G", "90L40J", "35D40J(SNR)", "35D40J(NDK)", "30P16A"
};
#elif defined(EMP_430)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C60E", "65L40E", "65C10E", "65C15D", "35D50D"
};
#elif defined(EMP_440)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C60E", "65C10E", "65L40E",  "65C15D"
};
#elif (defined(EMP_355))
#ifdef VET
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C50L", "35C20I", "70L40J", "75L40J","70L60J", "90L40J", "65C10L", "65C15E(APX)", "65C15E(JR)", "30P16B", "10L25K", "65C10I", "55L60G"
};
#else
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C50L", "35C20I", "70L40J", "75L40J", "70L60J", "90L40J", "65C10L", "65C15E(APX)", "65C15E(JR)", "30P16B", "10L25K", "65C10I"
};
#endif
#else
#ifdef VET
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C50K", "65C10K", "75L40K", "35C20H", "65C15D", "30P16A", "90L25K", "10L25J", "35D40J(SNR)", "35D40J(NDK)", "55L60H"
};
#else
std::string ViewSuperuser::m_probeName[WPORBE_NUM] = {
    "35C50K", "65C10K", "75L40K", "35C20H", "65C15D", "30P16A", "90L25K", "10L25J", "35D40J(SNR)", "35D40J(NDK)", "65C10H"
};
#endif
#endif

ViewSuperuser* ViewSuperuser::m_ptrInstance = NULL;
ViewSuperuser::ViewSuperuser() {
    m_vecAuthenInfo.clear();
    m_statusAuthen = TRUE;
    m_timer = 0;
    m_statusDemo = TRUE;
    m_probeType = 0;
    m_window = NULL;
#ifdef TRANSDUCER
    m_tranPressCorrect = 0;
#endif
}

ViewSuperuser::~ViewSuperuser() {
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewSuperuser* ViewSuperuser::GetInstance() {
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewSuperuser;

    return m_ptrInstance;
}

void ViewSuperuser::DestroyWindow(void) {
    if(GTK_IS_WIDGET(m_window))	{
        Exit();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1) {
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        }
        m_window = NULL;
    }
}

gboolean ViewSuperuser::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event) {
    DestroyWindow();
    return FALSE;
}

void ViewSuperuser::Authenticate(void) {
    PRINTF("--------------enter authenticate\n");
    if (m_timer > 0) {
        g_source_remove(m_timer);
        m_timer = 0;
        g_keyInterface.Pop();
    }

    g_keyInterface.Push(this);
    m_vecAuthenInfo.clear();
    m_statusAuthen = TRUE;
    m_statusDemo = TRUE;
    m_timer = g_timeout_add(1000, HandleAuthen, NULL);
}

void ViewSuperuser::CreateWindow(void) {
    GtkWidget *fixed;
    GtkWidget *frameProbe;
    GtkWidget *labelProbe;
    GtkWidget *tableProbe;
    GtkWidget *labelWrite;
    GtkWidget *imageWrite;
    GtkWidget *btnWrite;
    GtkWidget *labelExit;
    GtkWidget *imageExit;
    GtkWidget *btnExit;
    GtkWidget *frameAperture;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 480, 640);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), "超级用户工具");
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    frameProbe = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (fixed), frameProbe, 20, 10);
    gtk_widget_set_size_request (frameProbe, 440, 140);
    gtk_frame_set_shadow_type (GTK_FRAME (frameProbe), GTK_SHADOW_OUT);

    GtkWidget* tableHint = gtk_table_new(1, 1, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableHint), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableHint), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableHint), 10);
    gtk_widget_set_size_request(tableHint, 380, 30);
    gtk_fixed_put(GTK_FIXED(fixed), tableHint, 20, 600);

    m_labelHint = gtk_label_new("");
    gtk_table_attach (GTK_TABLE (tableHint), m_labelHint, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);

    labelProbe = gtk_label_new ("探头工具");
    gtk_frame_set_label_widget (GTK_FRAME (frameProbe), labelProbe);
    gtk_label_set_use_markup (GTK_LABEL (labelProbe), TRUE);

    tableProbe = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableProbe), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableProbe), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableProbe), 10);
    gtk_container_add (GTK_CONTAINER (frameProbe), tableProbe);

    GtkWidget *comboboxMachine = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), comboboxMachine, 0, 1, 0, 1);

#if defined(EMP_360)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G60");
#elif defined(EMP_161)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-3000");
#elif defined(EMP_322)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-2900Plus");
#elif defined(EMP_313)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-2800");
#elif (defined(EMP_430)|| defined(EMP_440))
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "A60");
#elif defined(EMP_355)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G30Plus");
#else
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G70");
#endif

    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxMachine), 0);
    g_signal_connect (G_OBJECT(comboboxMachine), "changed", G_CALLBACK(HandleMachineChanged), this);

    GtkWidget *comboboxProbe = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), comboboxProbe, 0, 1, 1, 2);
    int i;
    for (i = 0; i < WPORBE_NUM; i ++) {
        char newProbeName[256];
        ProbeMan::GetInstance()->VerifyProbeName(m_probeName[i].c_str(), newProbeName);
        gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxProbe), newProbeName);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxProbe), 0);
    m_probeType = 0;
    g_signal_connect (G_OBJECT(comboboxProbe), "changed", G_CALLBACK(HandleProbeChanged), this);

    labelWrite = gtk_label_new_with_mnemonic("写探头");
    imageWrite = gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_BUTTON);
    btnWrite = create_button_icon(labelWrite, imageWrite);
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), btnWrite, 1, 2, 0, 2);
    g_signal_connect (G_OBJECT(btnWrite), "clicked", G_CALLBACK(HandleBtnWriteClicked), this);

    m_treeviewProbe = create_probe_treeview();
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(m_treeviewProbe), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), m_treeviewProbe, 2, 3, 0, 2);

    // single aperture
    frameAperture = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (fixed), frameAperture, 20, 180);
    gtk_widget_set_size_request (frameAperture, 200, 80);
    gtk_frame_set_shadow_type (GTK_FRAME (frameAperture), GTK_SHADOW_OUT);

    GtkWidget *labelAperutre = gtk_label_new ("暗道测试工具");
    gtk_frame_set_label_widget (GTK_FRAME (frameAperture), labelAperutre);
    gtk_label_set_use_markup (GTK_LABEL (labelAperutre), TRUE);

    GtkWidget *tableAperture = gtk_table_new(1, 1, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableAperture), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableAperture), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableAperture), 10);
    gtk_container_add (GTK_CONTAINER (frameAperture), tableAperture);

    GtkWidget *comboboxAperture = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableAperture), comboboxAperture, 0, 1, 0, 1);
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxAperture), "OFF");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxAperture), "ON");
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxAperture), -1);
    g_signal_connect (G_OBJECT(comboboxAperture), "changed", G_CALLBACK(HandleChgAperture), this);

    //test the parameter of monitor
    GtkWidget *btnMonitor = gtk_button_new_with_mnemonic("显示器测试");
    g_signal_connect(G_OBJECT(btnMonitor), "clicked", G_CALLBACK(HandleBtnChgMonitorClicked), this);
    gtk_fixed_put (GTK_FIXED (fixed), btnMonitor, 230, 190);
    gtk_widget_set_size_request (btnMonitor, 180, 68);

    GtkWidget *btnMonitor2 = gtk_button_new_with_mnemonic("显示器测试 2");
    g_signal_connect(G_OBJECT(btnMonitor2), "clicked", G_CALLBACK(HandleBtnChgMonitorClicked2), this);
    gtk_fixed_put (GTK_FIXED (fixed), btnMonitor2, 230, 290);
    gtk_widget_set_size_request (btnMonitor2, 180, 68);

#ifdef TRANSDUCER
    // Transducer
    SysGeneralSetting *sysGel = new SysGeneralSetting;
    int index_press = sysGel->GetPressAjust();

    GtkWidget *framePress = gtk_frame_new (NULL);
    gtk_widget_show (framePress);
    gtk_fixed_put (GTK_FIXED (fixed), framePress, 20, 290+78);//90
    gtk_widget_set_size_request (framePress, 440, 70); //550
    gtk_frame_set_shadow_type (GTK_FRAME (framePress), GTK_SHADOW_OUT);

    GtkWidget *labelPress = gtk_label_new ("压力校准");
    gtk_widget_show (labelPress);
    gtk_frame_set_label_widget (GTK_FRAME (framePress), labelPress);
    gtk_label_set_use_markup (GTK_LABEL (labelPress), TRUE);

    GtkWidget *fixedPress = gtk_fixed_new ();
    gtk_widget_show (fixedPress);
    gtk_container_add (GTK_CONTAINER (framePress), fixedPress);

    GtkWidget *tablePress = gtk_table_new (1, 2, FALSE); //14
    gtk_widget_show (tablePress);
    gtk_fixed_put (GTK_FIXED (fixedPress), tablePress, 5, 10);
    gtk_widget_set_size_request (tablePress, 200, 20);//520
    gtk_table_set_row_spacings (GTK_TABLE(tablePress), 10);

    // press adjust
    GtkWidget *labelPressAdjust = gtk_label_new ("校准值");
    gtk_widget_show (labelPressAdjust);
    gtk_widget_set_size_request (labelPressAdjust, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (tablePress), labelPressAdjust, 0, 1, 0, 1);

    GtkObject *spinButtonPressAdjust = gtk_adjustment_new (0, -0.02, 0.02, 0.001, 0.001, 0);
    GtkWidget *m_spinbuttonPressAdjust= gtk_spin_button_new (GTK_ADJUSTMENT (spinButtonPressAdjust), 1, 3);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbuttonPressAdjust), (double)index_press/1000.0);
    gtk_widget_show (m_spinbuttonPressAdjust);
    gtk_widget_set_size_request (m_spinbuttonPressAdjust, -1, 27);
    gtk_table_attach_defaults (GTK_TABLE (tablePress), m_spinbuttonPressAdjust, 1, 2, 0, 1);
    g_signal_connect(G_OBJECT(m_spinbuttonPressAdjust), "insert_text", G_CALLBACK(on_spinbutton_press_adjust), this);

    GtkWidget *label_press = gtk_label_new ("（默认为0，范围正负0.02）");
    gtk_widget_show (label_press);
    gtk_fixed_put (GTK_FIXED (fixedPress), label_press, 220, 10);
    gtk_widget_set_size_request (label_press, 200, 30);
    gtk_misc_set_alignment (GTK_MISC (label_press), 0, 0.5);

    labelExit = gtk_label_new_with_mnemonic("确定");
#else
    labelExit = gtk_label_new_with_mnemonic("退出");
#endif
    imageExit = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_BUTTON);
    btnExit = create_button_icon(labelExit, imageExit);
    gtk_fixed_put (GTK_FIXED (fixed), btnExit, 340, 570);
    g_signal_connect (G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

    gtk_widget_show_all(m_window);
    g_keyInterface.Pop();

    //clear
    if (m_vecAuthenInfo.size())
        m_vecAuthenInfo.clear();

    m_statusAuthen = false;

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

void ViewSuperuser::CreateDemoWindow(void) {
    GtkWidget *fixed;
    GtkWidget *labelDemo;
    GtkWidget *imageDemo;
    GtkWidget *btnEnterDemo;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 480, 320);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), "进入演示");
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    labelDemo = gtk_label_new_with_mnemonic("进入演示功能");
    imageDemo = gtk_image_new_from_stock(GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
    btnEnterDemo = create_button_icon(labelDemo, imageDemo);
    g_signal_connect (G_OBJECT(btnEnterDemo), "clicked", G_CALLBACK(HandleBtnEnterDemoClicked), this);
    gtk_fixed_put(GTK_FIXED(fixed),btnEnterDemo, 40, 20);

    gtk_widget_show_all(m_window);

    m_statusAuthen = false;

    g_keyInterface.Pop();

    //clear
    if (m_vecAuthenInfo.size())
        m_vecAuthenInfo.clear();

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

GtkWidget *ViewSuperuser::create_probe_treeview(void) {
    GtkWidget *treeview;
    GtkTreeModel *model;
    GtkCellRenderer *render;
    GtkTreeViewColumn *column;

    treeview = gtk_tree_view_new();
    g_object_set(G_OBJECT(treeview),
                 "enable-search", FALSE,
                 "headers-visible", FALSE,
                 "sensitive", FALSE,
                 NULL);

    render = gtk_cell_renderer_text_new();
#ifdef EMP_355
    gtk_cell_renderer_set_fixed_size(render, -1, 33);
#else
    gtk_cell_renderer_set_fixed_size(render, -1, 25);
#endif
    column = gtk_tree_view_column_new_with_attributes("Porbe", render, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    model = create_probe_model();
    if(model != NULL)
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);

    return treeview;
}

GtkTreeModel* ViewSuperuser::create_probe_model(void) {
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);
    for (unsigned int i = 0; i < ProbeMan::MAX_SOCKET; i++) {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter, 0, "No Probe", -1);
    }
    return GTK_TREE_MODEL (store);
}

void ViewSuperuser::MachineChanged(GtkComboBox *combobox) {
}

void ViewSuperuser::ProbeChanged(GtkComboBox *combobox) {
    m_probeType = gtk_combo_box_get_active(combobox);

    int i;
    char index[5];
    char name[20];
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++) {
        sprintf(index, "%d", i);
        sprintf(name, "%d: 无探头", i+1);
        UpdateProbeStatus(index, name);
    }
}

void ViewSuperuser::BtnWriteClicked(GtkButton *button) {
    ASSERT(m_probeType < WPORBE_NUM);

    gtk_label_set_text(GTK_LABEL(m_labelHint), "探头参数写入中...");
    g_timeout_add(5, HandleWriteProbe, NULL);
}

void ViewSuperuser::BtnExitClicked(GtkButton *button) {
    DestroyWindow();
}

#ifdef TRANSDUCER
void ViewSuperuser::SpinbuttonPressAdjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    char str[256];
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    double pressad = atof(old_str.c_str());
    int press1 = (int) (pressad * 1000); //atoi(old_str.c_str());

    SysGeneralSetting sysGel;
    sysGel.SetPressAdjust(press1);
    sysGel.SyncFile();

    printf("--press1=%d\n", press1);

    if (press1 < -20 || press1 > 20) {
        PRINTF("++++press is beyond value\n");
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }

    return ;
}
#endif

void ViewSuperuser::ChgAperture(GtkComboBox *combobox) {
    bool on;
    if (gtk_combo_box_get_active(combobox) == 0)
        on = FALSE;
    else
        on = TRUE;

    Img2D::GetInstance()->EnterSingleAperture(on);
}

gboolean ViewSuperuser::IsAuthenValid(void) {
    m_timer = 0;
    if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'e')) {
        PRINTF("not authen\n");
        Exit();
    } else {
        PRINTF("authen\n");
    }

    return FALSE;
}

gboolean ViewSuperuser::WriteProbe(void) {
    ProbeMan::GetInstance()->WriteProbe(m_probeType);

    sleep(1);
    ProbeSocket::ProbePara para[ProbeMan::MAX_SOCKET];
    ProbeMan::GetInstance()->GetAllProbe(para);
    int i;
    char index[5];
    char name[20];
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++) {
        sprintf(index, "%d", i);
        if (para[i].exist)
            sprintf(name, "%d: %s", i+1, para[i].model);
        else
            sprintf(name, "%d: 无探头", i+1);
        UpdateProbeStatus(index, name);
    }
    gtk_label_set_text(GTK_LABEL(m_labelHint), "");

    return FALSE;
}

bool ViewSuperuser::ConfirmAuthen(void) {
    bool ret = false;
    unsigned char info[AUTHEN_NUM+1];
    int len = m_vecAuthenInfo.size();
    int size = (len > AUTHEN_NUM)?AUTHEN_NUM:len;
    memset(info, 0, sizeof(info));
    for (int i = 0; i < size; i++) {
        info[i] = m_vecAuthenInfo[len-size+i];
    }

    // compare
    if (strcmp((const char*)info, "emperor1997") == 0) {
        if (m_statusAuthen) {
            m_vecAuthenInfo.clear();
            CreateWindow();
            ret = true;
        }
    }
    return ret;
}

bool ViewSuperuser::ConfirmDemo(void) {
    bool ret = false;
    unsigned char info[DEMO_NUM+1];
    int len = m_vecAuthenInfo.size();
    int size = (len > DEMO_NUM)?DEMO_NUM:len;
    memset(info, 0, sizeof(info));
    for (int i = 0; i < size; i++) {
        info[i] = m_vecAuthenInfo[len-size+i];
    }

    // compare
    if(strcmp((const char*)info, "e1997") == 0) {
        if (m_statusDemo) {
            m_vecAuthenInfo.clear();
            CreateDemoWindow();
            ret = true;
        }
    }
    return ret;
}

void ViewSuperuser::Exit(void) {
    g_keyInterface.Pop();
}
/*
 * socket: a string to the path, "0" means the fisrt socket
 * status : a string to show
 */
gboolean ViewSuperuser::UpdateProbeStatus(const char* socket, const char* status) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeviewProbe));

    GtkTreeIter iter;
    if(gtk_tree_model_get_iter_from_string(model, &iter, socket)) {
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, status, -1);
        return TRUE;
    } else
        return FALSE;
}

static gboolean ExitWindow(gpointer data) {
    ViewSuperuser *tmp;
    tmp = (ViewSuperuser*)data;
    tmp->DestroyWindow();
    return FALSE;
}
void ViewSuperuser::KeyEvent(unsigned char keyValue) {
    switch(keyValue) {
    case KEY_ESC:
        g_timeout_add(100, ExitWindow, this);
        FakeEscKey();
        break;

    default:
        FakeXEvent::KeyEvent(keyValue);
        break;

#if 0

        if (m_statusAuthen) {
            Exit();
        } else if(m_statusDemo) {
            g_timeout_add(100, ExitWindow, this);
        } else {
            //DestroyWindow();
            g_timeout_add(100, ExitWindow, this);
            FakeEscKey();
        }
        break;

    default:
        m_vecAuthenInfo.push_back(keyValue);
        if(!ConfirmAuthen()&& !ConfirmDemo()) {
            FakeXEvent::KeyEvent(keyValue);
        }
        break;
#endif
    }
}
//test
void ViewSuperuser::BtnEnterDemoClicked(GtkButton *button) {
    m_statusAuthen = false;
    m_statusDemo = false;
    ViewMain::GetInstance()->MenuReviewCallBack();
    DestroyWindow();
}

void ViewSuperuser::ExitDemo() {
    ViewMain::GetInstance()->MenuReviewCallBack();
    m_statusDemo = TRUE;
}

/**
 * test parameter of monitor
 */
void ViewSuperuser::BtnChgMonitorClicked(GtkButton *button) {
    MonitorControl mc;
    mc.AutoAdjust();
    ScreenSaver::GetInstance()->EnterScreenSaver2();
}

void ViewSuperuser::BtnChgMonitorClicked2(GtkButton *button) {
    MonitorControl mc;
    mc.AutoBalance();
    ScreenSaver::GetInstance()->EnterScreenSaver3();
}
