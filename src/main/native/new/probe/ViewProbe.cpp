#include "probe/ViewProbe.h"

#include "utils/Const.h"
#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include "display/TopArea.h"
#include "display/HintArea.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuPW.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/ScanMode.h"
#include "keyboard/KeyDef.h"
#include "keyboard/LightDef.h"
#include "probe/BiopsyMan.h"
#include "sysMan/UserSelect.h"

#include "ViewMain.h"
#include "sysMan/ViewSystem.h"

enum {
  PROBE_INDEX,
  EXAM_COLUMN,
  N_COLUMNS
};

ViewProbe* ViewProbe::m_instance = NULL;

// ---------------------------------------------------------

ViewProbe* ViewProbe::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewProbe();
  }

  return m_instance;
}

ViewProbe::ViewProbe() {
  m_dialog = NULL;
  m_comboboxtext_user = NULL;

  m_treeview0 = NULL;
  m_treeview1 = NULL;
  m_treeview2 = NULL;
  m_treeview3 = NULL;

  int m_preSocketIndex = -1;
  int m_examItemIndex = -1;
  int m_treeViewIndex = -1;
  int m_probeIndex = -1;
  int m_itemIndex = -1;
  int m_probe_index = -1;

  m_itemList = NULL;
}

ViewProbe::~ViewProbe() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

bool ViewProbe::Create() {
  HintArea::GetInstance()->UpdateHint(_("[Probe Select]: Reading Probe Para..."));

  if (ImgProcPw::GetInstance()->GetTraceStatus() && ModeStatus::IsFreezeMode()) {
    if (g_menuPW.GetAutoTraceStatus()) {
      ImgProcPw::GetInstance()->SetAutoCalc(false);
    }
  }

  gdk_threads_enter();

  while (gtk_events_pending()) {
    gtk_main_iteration(); // 解决pw冻结，屏幕上有pw trace计算时，不能读探头
  }

  gdk_threads_leave();

  ReadProbe();

  return true;
}

bool ViewProbe::Create(int socket) {
  HintArea::GetInstance()->UpdateHint(_("[Probe Select]: Reading Probe Para..."));

  gdk_threads_enter();

  while (gtk_events_pending()) {
    gtk_main_iteration();
  }

  gdk_threads_leave();

  ReadOneProbe(socket);

  return true;
}

void ViewProbe::Destroy() {
  gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboboxtext_user));

  m_pKps.ActiveHV(true);

  if (GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();

    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
        SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }

  ScanMode::GetInstance()->DarkAllModeLight();
  g_keyInterface.CtrlLight(TRUE, LIGHT_D2);

  if (ImgProcPw::GetInstance()->GetTraceStatus() && ModeStatus::IsFreezeMode()) {
    if (g_menuPW.GetAutoTraceStatus()) {
      ImgProcPw::GetInstance()->SetAutoCalc(true);
    }
  }

  m_dialog = NULL;
  m_comboboxtext_user = NULL;

  m_treeview0 = NULL;
  m_treeview1 = NULL;
  m_treeview2 = NULL;
  m_treeview3 = NULL;

  m_itemList = NULL;
}

void ViewProbe::ReadProbe() {
  if (!m_pKps.ProbeRead()) {  // no probe is found
    HintArea::GetInstance()->UpdateHint(_("[Probe Select]: No Probe was found."), 2);

    return ;
  }

  HintArea::GetInstance()->ClearHint();

  // display probe dialog and wait user's operation
  ProbeSocket::ProbePara* para = NULL;
  int maxSocket = 0;

  m_pKps.GetPara(para, m_itemList, maxSocket);

  if (para == NULL) {
    return;
  }

  CreateWindow(para, m_itemList, maxSocket);
}

void ViewProbe::ReadOneProbe(int socket) {
  if (!m_pKps.OneProbeRead(socket)) { // no probe is found
    HintArea::GetInstance()->UpdateHint(_("[Probe Select]: No Probe was found."), 2);

    return ;
  }

  HintArea::GetInstance()->ClearHint();

  // display probe dialog and wait user's operation
  ProbeSocket::ProbePara* para = NULL;
  int maxSocket = 0;

  m_pKps.GetPara(para, m_itemList, maxSocket);

  if (para == NULL) {
    return;
  }

  CreateWindow(para, m_itemList, maxSocket);
}

string ViewProbe::GetUserName() {
  return ""; //Utils::combobox_active_text(m_combobox_user_select);
}

string ViewProbe::GetItemNameUserDef() {
  return m_itemNameUserDef;
}

int ViewProbe::GetProbeIndex() {
  return m_probeIndex;
}

int ViewProbe::GetCurExamItemIndex() {
  return m_examItemIndex;
}

// ---------------------------------------------------------

void ViewProbe::BtnClickedExit(GtkButton* button) {
  m_probe_index = ProbeMan::GetInstance()->GetCurProbeSocket();
  ProbeMan::GetInstance()->SetProbeSocket(m_probe_index);

  Destroy();
}

void ViewProbe::ComboboxChangedProbeUser(GtkComboBox* combobox) {
  string name;
  int select = gtk_combo_box_get_active(combobox);
  if (select >= 0) {
    name = gtk_combo_box_get_active_text (combobox);
  }

  UserSelect::GetInstance()->save_cur_username(name);

  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  ExamItem exam;
  string username = exam.TransUserSelectForEng(name);
  exam.WriteDefaultUserSelect(&ini, username);

  ProbeSocket::ProbePara* para = NULL;
  int maxSocket = 0;

  m_pKps.GetPara(para, m_itemList, maxSocket);

  if (para == NULL) {
    return;
  } else {
    for (int i = 0; i < maxSocket; i++) {
      if (para[i].exist) {
        vector<string> exam_type = CreateAllExamType(para[i].model, m_itemList[i]);
        GetCurExamItemIndex(exam_type, i);
        GtkTreeModel* model = CreateModel(exam_type, i);

        if (i==0) {
          gtk_tree_view_set_model(m_treeview0, model);
          gtk_tree_view_expand_all(m_treeview0);
        }

        if (i==1) {
          gtk_tree_view_set_model(m_treeview1, model);
          gtk_tree_view_expand_all(m_treeview1);
        }

        if (i==2) {
          gtk_tree_view_set_model(m_treeview2, model);
          gtk_tree_view_expand_all(m_treeview2);
        }

        if (i==3) {
          gtk_tree_view_set_model(m_treeview3, model);
          gtk_tree_view_expand_all(m_treeview3);
        }
      }
    }
  }
}

void ViewProbe::TreeViewFocusOut(GtkTreeView* treeview, GdkEventFocus* event) {
  GtkTreeSelection* select = gtk_tree_view_get_selection(treeview);
  gtk_tree_selection_unselect_all(select);
}

void ViewProbe::TreeViewButtonClicked(GtkTreeView* treeview, GdkEventButton* event) {
  if (event->window == gtk_tree_view_get_bin_window(treeview) &&
    event->type == GDK_BUTTON_PRESS && event->button == 1) {

    string name = Utils::combobox_active_text(m_comboboxtext_user);

    if (!name.empty()) {
      int num = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxtext_user));
      UserSelect::GetInstance()->save_cur_username(name);

      ExamItem exam;
      string username = exam.TransUserSelectForEng(name);

      IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

      exam.WriteDefaultUserSelect(&ini, username);
      exam.WriteDefaultUserIndex(&ini, num);

      stringstream ss;

      if (num > 0) {
        ss << "userconfig/" << name << ".ini";
      } else {
        ss << "ItemPara.ini";
      }

      sprintf(user_configure, "%s", ss.str().c_str());
    }

    int x = 0;
    int y = 0;

    gtk_tree_view_convert_widget_to_bin_window_coords(treeview, event->x, event->y, &x, &y);
    GtkTreeModel* model = gtk_tree_view_get_model(treeview);

    GtkTreePath* path = NULL;

    if (gtk_tree_view_get_path_at_pos(treeview, x, y, &path, NULL, NULL, NULL)) {
      GtkTreeIter iter;

      if (gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_model_get(model, &iter, PROBE_INDEX, &m_probe_index, -1);
        m_probeIndex = m_probe_index;

        string path_string = gtk_tree_path_to_string(path);
        int i = atoi(path_string.c_str());
        int itemNumOfProbe = m_itemList[m_probeIndex].size();

        ExamItem exam;
        IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

        if (i >= itemNumOfProbe) {
          gtk_tree_model_get(model, &iter, EXAM_COLUMN, &m_userItemName, -1);
          m_itemNameUserDef = m_userItemName;
          m_itemIndex = 0;
          m_pKps.UserItemOfProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex, m_userItemName);
          exam.WriteUserItemFlag(&ini, true);
          exam.WriteDefaultProbeItemName(&ini, m_userItemName);
        } else {
          m_itemIndex = m_itemList[m_probeIndex][i];
          m_pKps.ProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex);
          exam.WriteUserItemFlag(&ini, false);
          exam.WriteDefaultProbeItemName(&ini, EXAM_TYPES[m_itemIndex]);
        }

        gtk_tree_path_free(path);

        int counts = 0;
        while (gtk_events_pending()) {
          gtk_main_iteration();
          counts++;

          if(counts > 15) {
            break;
          }
        }

        g_timeout_add(500, signal_callback_destroy_window, this);

        ProbeSocket::ProbePara para;

        ProbeMan::GetInstance()->GetCurProbe(para);
        ProbeMan::GetInstance()->WriteDefaultProbe((char*)para.model, &ini);
        exam.WriteDefaultProbeItem(&ini, m_itemIndex);
        BiopsyMan::GetInstance()->SetCurBioBracketAndAngleTypeOfProbeChanged();
      }
    }
  }
}

void ViewProbe::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  if (keyValue == KEY_ESC) {
    BtnClickedExit(NULL);
  }
}

void ViewProbe::CreateWindow(ProbeSocket::ProbePara* para, vector<ExamItem::EItem>* itemList, int maxSocket) {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  m_dialog = Utils::create_dialog(NULL, _("Probe Select"), 550, 600);

  GtkButton* button_exit = Utils::add_dialog_button(m_dialog, _("Exit"), GTK_RESPONSE_CLOSE, GTK_STOCK_QUIT);
  g_signal_connect(button_exit, "clicked", G_CALLBACK(signal_button_clicked_exit), this);

  GtkTable* table = Utils::create_table(8, 1);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  if (maxSocket > 0) {
    GtkTable* table_probe = Utils::create_table(1, maxSocket);

    gtk_container_set_border_width(GTK_CONTAINER(table_probe), 10);
    gtk_table_set_row_spacings(table_probe, 0);

    for (int i = 0; i < maxSocket; i++) {
      GtkWidget* probe = NULL;

      if (para != NULL && para[i].exist) {
        ProbeSocket::ProbePara probe_para;
        ProbeMan::GetInstance()->GetCurProbe(probe_para);

        if (probe_para.model == para[i].model) {
          m_preSocketIndex = i;
        } else {
          m_preSocketIndex = -1;
        }

        vector<string> exam_type = CreateAllExamType(para[i].model, itemList[i]);

        probe = CreateProbe(para[i].model, para[i].type, exam_type, i);
      } else {
        m_examItemIndex = -1;

        vector<string> exam_type;
        probe = CreateProbe("", 'C', exam_type, i);
      }

      gtk_table_attach_defaults(table_probe, probe, i, i + 1, 0, 1);
    }

    gtk_table_attach_defaults(table, GTK_WIDGET(table_probe), 0, 1, 0, 7);
  }

  GtkTable* table_user = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_user), 10);
  gtk_table_attach_defaults(table, GTK_WIDGET(table_user), 0, 1, 7, 8);

  GtkLabel* label = Utils::create_label(_("Current User:"));
  m_comboboxtext_user = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_user, GTK_WIDGET(label), 0, 1, 0, 1);
  gtk_table_attach(table_user, GTK_WIDGET(m_comboboxtext_user), 1, 2, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  UserSelect::GetInstance()->read_default_username(GTK_COMBO_BOX(m_comboboxtext_user));
  UserSelect::GetInstance()->read_username_db(USERNAME_DB, GTK_COMBO_BOX(m_comboboxtext_user));
  UserSelect::GetInstance()->set_active_user(GTK_COMBO_BOX(m_comboboxtext_user), UserSelect::GetInstance()->get_active_user());

  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  ExamItem exam;
  int index_username = exam.ReadDefaultUserIndex(&ini);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxtext_user), index_username);

  g_signal_connect(GTK_COMBO_BOX(m_comboboxtext_user), "changed", G_CALLBACK(signal_combobox_changed_probe_user), this);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

GtkWidget* ViewProbe::CreateProbe(const string probe_name, const char probeType,
  const vector<string> exam_type, int probe_index) {
  GtkBox* vbox = Utils::create_vbox();

  GtkLabel* label_name = NULL;

  if (!probe_name.empty()) {
    label_name = Utils::create_label(ProbeMan::GetInstance()->VerifyProbeName(probe_name));
  } else {
    label_name = Utils::create_label(_("No Probe"));
  }

  GtkFrame* frame_image = Utils::create_frame();
  GtkFrame* frame_exam = Utils::create_frame();

  gtk_box_pack_start(vbox, GTK_WIDGET(label_name), FALSE, TRUE, 0);
  gtk_box_pack_start(vbox, GTK_WIDGET(frame_image), FALSE, TRUE, 0);
  gtk_box_pack_start(vbox, GTK_WIDGET(frame_exam), TRUE, TRUE, 0);

  gtk_frame_set_shadow_type(frame_image, GTK_SHADOW_OUT);
  gtk_frame_set_shadow_type(frame_exam, GTK_SHADOW_OUT);

  string probe_path;

  switch ((int)probeType) {
  case 'C':
  case 'c':
    {
      probe_path = "res/probe/35C50K.png";
      break;
    }
  case 'L':
  case 'l':
    {
      if (probe_name == "55L60H") {
        probe_path = "res/probe/55L60H.png";
      } else {
        probe_path = "res/probe/75L40K.png";
      }

      break;
    }
  case 'T':
  case 't':
    {
      if (probe_name == "65C10H" || probe_name == "65C10E") {
        probe_path = "res/probe/65C10H.png";
      } else {
        probe_path = "res/probe/65C10K.png";
      }
      break;
    }
  case 'N':
  case 'n':
    probe_path = "res/probe/35C20H.png";
    break;
  case 'P':
  case 'p':
    probe_path = "res/probe/30P16A.png";
    break;
  case 'V':
  case 'v':
    probe_path = "res/probe/35D40J.png";
    break;
  case 0:
    probe_path = "res/probe/NoProbe.png";
    break;
  default:
    probe_path = "res/probe/35C50K.png";
    break;
  }

  GtkImage* image_probe = Utils::create_image(string(CFG_RES_PATH) + probe_path);
  gtk_container_add(GTK_CONTAINER(frame_image), GTK_WIDGET(image_probe));

  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_container_add(GTK_CONTAINER(frame_exam), GTK_WIDGET(scrolled_window));

  if (probe_index >= 0 && probe_index <= 3) {
    GtkTreeView* treeview = CreateTreeView(exam_type, probe_index);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(treeview));

    if (probe_index == 0) {
      m_treeview0 = treeview;
    }

    if (probe_index == 1) {
      m_treeview1 = treeview;
    }

    if (probe_index == 2) {
      m_treeview2 = treeview;
    }

    if (probe_index == 3) {
      m_treeview3 = treeview;
    }
  }

  return GTK_WIDGET(vbox);
}

vector<string> ViewProbe::CreateAllExamType(const string model, vector<ExamItem::EItem> item) {
  vector<string> vec;

  for (int i = 0; i < item.size(); i++) {
    vec.push_back(EXAM_TYPES[item[i]]);
  }

  GetUserItemNameOfProbe(model);

  for (int i = 0; i < m_vecUserItemName.size(); i++) {
    vec.push_back(m_vecUserItemName[i]);
  }
}

void ViewProbe::GetUserItemNameOfProbe(const string model) {
  m_vecUserItemName.clear();

  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  ExamItem exam;
  string username = exam.ReadDefaultUserSelect(&ini);

  ExamItem examitem;
  vector<string> useritemgroup = examitem.GetDefaultUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string firstGenItem;

    examitem.GetDefaultUserItem(useritemgroup[i], userselect, probelist, useritem, department, firstGenItem);

    ExamPara exampara;
    exampara.name = useritem;
    exampara.index = ExamItem::USERNAME;

    if(username == userselect) {
      if (probelist == model) {
        m_vecUserItemName.push_back(exampara.name);
      }
    }
  }
}

GtkTreeView* ViewProbe::CreateTreeView(const vector<string> exam_type, int probe_index) {
  GtkTreeModel* model = CreateModel(exam_type, probe_index);
  GetCurExamItemIndex(exam_type, probe_index);

  if (probe_index > 0 && probe_index <= 3) {
    GtkTreeView* treeview = Utils::create_tree_view(model);
    AddColumn(treeview);

    gtk_tree_view_set_enable_search(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    GtkTreeSelection* select = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
    GTK_WIDGET_UNSET_FLAGS (treeview, GTK_CAN_FOCUS);

    if (m_preSocketIndex == probe_index && m_examItemIndex >= 0) {
      stringstream ss;
      ss << m_examItemIndex;

      m_treeViewPath = ss.str();
      GtkTreePath* path = gtk_tree_path_new_from_string(m_treeViewPath.c_str());

      if (path != NULL) {
        gtk_tree_view_set_cursor(treeview, path, NULL, TRUE);
        gtk_tree_path_free(path);
        m_treeViewIndex = m_preSocketIndex;
      }
    }

    g_signal_connect(treeview, "focus-out-event", G_CALLBACK(signal_treeview_focusout), this);
    g_signal_connect(treeview, "button-press-event", G_CALLBACK(signal_treeview_button_clicked), this);

    g_object_unref(model);

    if (probe_index == 0) {
      m_treeview0 = treeview;
    }

    if (probe_index == 1) {
      m_treeview1 = treeview;
    }

    if (probe_index == 2) {
      m_treeview2 = treeview;
    }

    if (probe_index == 3) {
      m_treeview3 = treeview;
    }

    return treeview;
  }

  return NULL;
}

GtkTreeModel* ViewProbe::CreateModel(const vector<string> exam_type, int probe_index) {
  GtkTreeIter iter;
  GtkListStore* store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);

  for (int i = 0; i < exam_type.size(); i++) {
    if (!exam_type[i].empty()) {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
        PROBE_INDEX, probe_index, EXAM_COLUMN, exam_type[i].c_str(), -1);
    }
  }

  return GTK_TREE_MODEL(store);
}

void ViewProbe::AddColumn(GtkTreeView* treeview) {
  GtkCellRenderer* renderer_text = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column_text = gtk_tree_view_column_new_with_attributes("EXAM_NAME",
    renderer_text, "text", EXAM_COLUMN, NULL);

  gtk_tree_view_append_column(treeview, column_text);
}

void ViewProbe::GetCurExamItemIndex(const vector<string> exam_type, int probe_index) {
  string str_check_park = TopArea::GetInstance()->GetCheckPart();
  m_examItemIndex = 0;

  for (int  i = 0; i < exam_type.size(); i++) {
    if (!exam_type[i].empty()) {
      if (m_preSocketIndex == probe_index) {
        if (str_check_park == exam_type[i]) {
          m_examItemIndex++;
        } else {
          break;
        }
      }
    }
  }
}
