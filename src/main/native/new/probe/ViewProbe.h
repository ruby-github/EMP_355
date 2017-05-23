#ifndef __VIEW_PROBE_H__
#define __VIEW_PROBE_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"
#include "probe/ProbeSelect.h"

class ViewProbe: public FakeXEvent {
public:
  static ViewProbe* GetInstance();

public:
  ~ViewProbe();

  bool Create();
  bool Create(int socket);
  void Destroy();

  void ReadProbe();
  void ReadOneProbe(int socket);

  string GetUserName();
  string GetItemNameUserDef();

  int GetProbeIndex();
  int GetCurExamItemIndex();

public:
  void CreateWindow(ProbeSocket::ProbePara* para, vector<ExamItem::EItem>* itemList, int maxSocket);

private:
  // signal

  static void signal_button_clicked_exit(GtkButton* button, ViewProbe* data) {
    if (data != NULL) {
      data->BtnClickedExit(button);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewProbe* data) {
    if (data != NULL) {
      data->Destroy();
    }

    return FALSE;
  }

  static void signal_combobox_changed_probe_user(GtkComboBox* combobox, ViewProbe* data) {
    if (data != NULL) {
      data->ComboboxChangedProbeUser(combobox);
    }
  }

  static gboolean signal_treeview_focusout(GtkTreeView* treeview, GdkEventFocus* event, ViewProbe* data) {
    if (data != NULL) {
      data->TreeViewFocusOut(treeview, event);
    }

    return FALSE;
  }

  static gboolean signal_treeview_button_clicked(GtkTreeView* treeview, GdkEventButton* event, ViewProbe* data) {
    if (data != NULL) {
      data->TreeViewButtonClicked(treeview, event);
    }

    return TRUE;
  }

  static gboolean signal_callback_destroy_window(gpointer data) {
    ViewProbe* tmp = (ViewProbe*)data;

    if (tmp != NULL) {
      tmp->Destroy();
    }

    return FALSE;
  }

  // signal

  void BtnClickedExit(GtkButton* button);
  void ComboboxChangedProbeUser(GtkComboBox* combobox);
  void TreeViewFocusOut(GtkTreeView* treeview, GdkEventFocus* event);
  void TreeViewButtonClicked(GtkTreeView* treeview, GdkEventButton* event);

private:
  ViewProbe();

  void KeyEvent(unsigned char keyValue);

  GtkWidget* CreateProbe(const string probe_name, const char probeType,
    const vector<string> exam_type, int probe_index);

  vector<string> CreateAllExamType(const string model, vector<ExamItem::EItem> item);
  void GetUserItemNameOfProbe(const string model);

  GtkTreeView* CreateTreeView(const vector<string> exam_type, int probe_index);
  GtkTreeModel* CreateModel(const vector<string> exam_type, int probe_index);
  void AddColumn(GtkTreeView* treeView);
  void GetCurExamItemIndex(const vector<string> exam_type, int probe_index);

private:
  static ViewProbe* m_instance;

private:
  GtkDialog* m_dialog;
  GtkComboBoxText* m_comboboxtext_user;

  GtkTreeView* m_treeview0;
  GtkTreeView* m_treeview1;
  GtkTreeView* m_treeview2;
  GtkTreeView* m_treeview3;

  ProbeSelect m_pKps;

  string m_treeViewPath;
  string m_userItemName;
  string m_itemNameUserDef;

  int m_preSocketIndex;
  int m_examItemIndex;
  int m_treeViewIndex;
  int m_probeIndex;
  int m_itemIndex;
  int m_probe_index;

  vector<ExamItem::EItem>* m_itemList;
  vector<string> m_vecUserItemName;
};

#endif
