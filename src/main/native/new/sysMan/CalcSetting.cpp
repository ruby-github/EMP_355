#include "sysMan/CalcSetting.h"

#include "utils/Const.h"
#include "utils/FakeXUtils.h"
#include "utils/Logger.h"
#include "utils/MainWindowConfig.h"
#include "utils/MessageDialog.h"
#include "utils/StringUtils.h"

#include "Def.h"
#include "display/TopArea.h"
#include "calcPeople/MeasureDef.h"
#include "probe/ExamItem.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "patient/FileMan.h"
#include "periDevice/PeripheralMan.h"
#include "calcPeople/MenuCalcNew.h"
#include "sysMan/UserSelect.h"
#include "measure/MeasureMan.h"

#ifdef VET
  static const int EXAM_NUM = 9;
#else
  #ifdef EMP_322
    static const int EXAM_NUM = 18;
  #else
    static const int EXAM_NUM = 22;
  #endif
#endif

#ifdef EMP_322
  const string SECTIONS[] = {
    N_("Abdomen-General") ,
    N_("Gynecology") ,
    N_("Obstetrics-General") ,
    N_("Obstetrics-FetalHeart") ,
    N_("Urology") ,
    N_("Orthopedic") ,
    N_("SmallPart-Glands") ,
    N_("SmallPart-Thyroid") ,
    N_("SmallPart-Eye") ,
    N_("SmallPart-Testicle") ,
    N_("Cardiac") ,
    N_("Fetal Cardio")
  };

  const string EXAM_TYPES_CALC[] = {
    N_("Adult Abdomen"),
    N_("Adult Liver"),
    N_("Kid Abdomen"),
    N_("Adult Cardio"),
    N_("Kid Cardio"),
    N_("Mammary Glands"),
    N_("Thyroid"),
    N_("Eye Ball"),
    N_("Testicle"),
    N_("Gynecology"),
    N_("Early Pregnancy"),
    N_("Middle-late Pregnancy"),
    N_("Fetal Cardio"),
    N_("Kidney Ureter"),
    N_("Bladder Prostate"),
    N_("Hip Joint"),
    N_("Meniscus"),
    N_("Joint Cavity"),
    N_("Spine")
  };
#else
  #ifdef VET
    const string SECTIONS[] = {
      N_("Abdomen-General") ,
      N_("Abdomen-Arterial") ,
      N_("Abdomen-Venous") ,
      N_("Obstetrics-Dog") ,
      N_("Obstetrics-Cat") ,
      N_("Obstetrics-Sheep") ,
      N_("Obstetrics-Swine") ,
      N_("Obstetrics-Bovine") ,
      N_("Obstetrics-Equine") ,
      N_("Urology") ,
      N_("Orthopedic") ,
      N_("SmallPart-Glands") ,
      N_("SmallPart-Thyroid") ,
      N_("SmallPart-Eye") ,
      N_("SmallPart-Testicle") ,
      N_("Vascular-LE") ,
      N_("Vascular-UE") ,
      N_("Vascular-Carotid") ,
      N_("Cardiac") ,
      N_("Fetal Cardio") ,
      N_("TCD"),
      N_("Tendon")
    };
  #else
    const string SECTIONS[] = {
      N_("Abdomen-General") ,
      N_("Abdomen-Arterial") ,
      N_("Abdomen-Venous") ,
      N_("Gynecology") ,
      N_("Obstetrics-General") ,
      N_("Obstetrics-FetalHeart") ,
      N_("Urology") ,
      N_("Orthopedic") ,
      N_("SmallPart-Glands") ,
      N_("SmallPart-Thyroid") ,
      N_("SmallPart-Eye") ,
      N_("SmallPart-Testicle") ,
      N_("Vascular-LE") ,
      N_("Vascular-UE") ,
      N_("Vascular-Carotid") ,
      N_("Cardiac") ,
      N_("Fetal Cardio") ,
      N_("TCD")
    };
  #endif
#endif

struct CustomEtype {
  string name;
  int etype;
};

struct CustomTypeAndMethod {
  int type;
  string name;
  int etype;
};

const string METHOD_NAMES[] = {
  N_("Distance"),
  N_("Length"),
  N_("Peri"),
  N_("Area"),
  N_("Volume"),
  N_("Angle"),
  N_("Ratio"),
  N_("Depth"),
  N_("EF"),
  N_("Profile"),
  N_("Histogram"),
  N_("Time"),
  N_("Slope"),
  N_("Vel"),
  N_("HR"),
  N_("Acceleration"),
  N_("PG Mean")
};

const CustomTypeAndMethod CUSTOM_TYPES[] = {
  {0, N_("Dist-Dot"), DIST_DOT},
  {0, N_("Dist-Line"), DIST_LINE},

  #ifndef EMP_322
    {1, N_("Length-Track"), LENGTH_TRACK},
    {1, N_("Length-Dot"), LENGTH_DOT},
  #endif

  {1, N_("Integral-Track"), INTEGRAL_TRACK},
  {2, N_("Peri-Track"), PERI_TRACK},
  {2, N_("Peri-Ellipse"), PERI_ELLIPSE},
  {3, N_("Area-Track"), AREA_TRACK},

  #ifndef EMP_322
    {3, N_("Area-Dot"), AREA_TRACK},
  #endif

  {3, N_("Area-Rectangle"), AREA_REC},
  {3, N_("Area-Ellipse"), AREA_ELLIPSE},
  {4, N_("Vol-3Axis"), VOL_3AXIS},
  {4, N_("Vol-Ellipse1"), VOL_ELLIPSE1},
  {4, N_("Vol-Ellipse2"), VOL_ELLIPSE2},
  {5, N_("Angle-3Dot"), ANGLE_3DOT},
  {5, N_("Angle-2Line"), ANGLE_2LINE},
  {6, N_("Angusty-Dist-Dot"), ANGUSTY_DIST_DOT},
  {6, N_("Ratio-Dist-Dot"), RATIO_DIST_DOT},
  {6, N_("Angusty-Area"), ANGUSTY_AREA},
  {6, N_("Ratio-Area"), RATIO_AREA},
  {6, N_("Ratio-D-P"), RATIO_D_P},
  {6, N_("Ratio-Vol"), RATIO_VOL},
  {7, N_("Depth-Dist"), DEPTH_DIST},
  {7, N_("Depth-Dist-M"), DEPTH_DIST_M},
  {8, N_("EF"), EF},
  {9, N_("Profile"), PROFILE},
  {10, N_("Histogram"), HISTOGRAM},
  {11, N_("Time-M"), TIME_M},

  #ifndef EMP_322
    {11, N_("Time-D"), TIME_D},
  #endif

  {12, N_("Slope"), SLOPE},
  {13, N_("Vel-M"), VEL_M},

  #ifdef EMP_322
    {14, N_("HR-M"), HR_M}
  #else
    {13, N_("Vel-D"), VEL_D},
    {14, N_("HR-M"), HR_M},
  #endif

  #ifndef EMP_322
    {15, N_("Acceleration"), ACCEL},
    {16, N_("PG Mean"), PGMEAN}
  #endif
};

// ---------------------------------------------------------
//
//  CalcSetting
//
// ---------------------------------------------------------

CalcSetting* CalcSetting::m_instance = NULL;

// ---------------------------------------------------------

CalcSetting* CalcSetting::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new CalcSetting();
  }

  return m_instance;
}

CalcSetting::CalcSetting() {
}

CalcSetting::~CalcSetting() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* CalcSetting::GetWindow() {
  return m_parent;
}

GtkWidget* CalcSetting::CreateCalcWindow(GtkWidget* parent) {
  m_parent = parent;

  GtkTable* table = GTK_TABLE(gtk_table_new(10, 8, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table), 30);

  gtk_table_set_row_spacings(table, 10);
  gtk_table_set_col_spacings(table, 10);

  // Exam Type
  GtkLabel* label_exam_type = Utils::create_label(_("Exam Type:"));
  m_combobox_exam_type = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_exam_type), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_exam_type), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  vector<string> vec_exam_type = GetExamItemsCalc();
  CreateDefineItemCalc(vec_exam_type);

  for (size_t i = 0; i < vec_exam_type.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_exam_type, vec_exam_type[i].c_str());
  }

  for (size_t i = 0; i < vec_exam_type.size(); i++) {
    string cur_exam_type;
    TopArea::GetInstance()->GetCheckPart(cur_exam_type);

    if (cur_exam_type == vec_exam_type[i]) {
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_type), i);

      break;
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_type), 0);
  g_signal_connect(GTK_COMBO_BOX(m_combobox_exam_type), "changed", G_CALLBACK(signal_combobox_changed_exam_type), this);

  // Available Items
  GtkLabel* label_department = Utils::create_label(_("Available Items:"));
  m_combobox_department = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_department), 0, 3, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_department), 0, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  size_t size = sizeof(SECTIONS) / sizeof(SECTIONS[0]);

  for(int i = 0; i < size; i++ ) {
    gtk_combo_box_text_append_text(m_combobox_department, SECTIONS[i].c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department), 0);
  g_signal_connect(GTK_COMBO_BOX(m_combobox_department), "changed", G_CALLBACK(signal_combobox_changed_department), this);

  // scrolled_window
  m_scrolled_window_item = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(m_scrolled_window_item), 0, 3, 3, 9);

  m_treeview_item = Utils::create_tree_view(CreateItemCalcModel2());
  gtk_container_add(GTK_CONTAINER(m_scrolled_window_item), GTK_WIDGET(m_treeview_item));

  AddColumnsCalc(m_treeview_item);

  // Measure Sequence
  GtkLabel* label_sequence = Utils::create_label(_("Measure Sequence:"));
  m_combobox_sequence = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_sequence), 4, 6, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_sequence), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_sequence, _("None"));
  gtk_combo_box_text_append_text(m_combobox_sequence, _("Repeat"));
  gtk_combo_box_text_append_text(m_combobox_sequence, _("Next"));

  stringstream ss;
  ss << CFG_RES_PATH << STORE_DEFAULT_ITEM_PATH;

  IniFile ini(ss.str());

  ExamItem exam;
  string examType = exam.ReadDefaultProbeDefaultItemName(&ini);

  char exam_type[256] = {0};
  exam.TransItemNameEng(examType.c_str(), exam_type);
  int sequence = GetSequence(exam_type);

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence), sequence);
  g_signal_connect(m_combobox_sequence, "changed", G_CALLBACK(signal_combobox_changed_sequence), this);

  // Selected Items
  GtkLabel* label_department_selected = Utils::create_label(_("Selected Items:"));
  m_scrolled_window_selected_item = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_department_selected), 4, 7, 1, 2);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_scrolled_window_selected_item), 4, 7, 2, 9);

  m_treeview_selected_item = Utils::create_tree_view(CreateItemCalcModel1());
  gtk_container_add(GTK_CONTAINER(m_scrolled_window_selected_item), GTK_WIDGET(m_treeview_selected_item));

  AddColumnsCalc1(m_treeview_selected_item);

  // button

  GtkButton* button_select_one = Utils::create_button(">");
  g_signal_connect(button_select_one, "clicked", G_CALLBACK(signal_button_clicked_selectone), this);

  GtkButton* button_select_all = Utils::create_button(">>");
  g_signal_connect(button_select_all, "clicked", G_CALLBACK(signal_button_clicked_selectall), this);

  GtkButton* button_back_one = Utils::create_button("<");
  g_signal_connect(button_back_one, "clicked", G_CALLBACK(signal_button_clicked_backone), this);

  GtkButton* button_back_all = Utils::create_button("<<");
  g_signal_connect(button_back_all, "clicked", G_CALLBACK(signal_button_clicked_backall), this);

  GtkButton* button_add = Utils::create_button(_("Add"));
  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_add), this);

  GtkButton* button_delete = Utils::create_button(_("Delete"));
  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_delete), this);

  gtk_widget_set_size_request(GTK_WIDGET(button_select_one), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_select_all), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_back_one), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_back_all), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_add), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_delete), -1, 30);

  gtk_table_attach(table, GTK_WIDGET(button_select_one), 3, 4, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_select_all), 3, 4, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_back_one), 3, 4, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_back_all), 3, 4, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_add), 3, 4, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_delete), 3, 4, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkButton* button_up = Utils::create_button(_("Up"));
  g_signal_connect(button_up, "clicked", G_CALLBACK(signal_button_clicked_up), this);

  GtkButton* button_down = Utils::create_button(_("Down"));
  g_signal_connect(button_down, "clicked", G_CALLBACK(signal_button_clicked_down), this);

  GtkButton* button_export = Utils::create_button(_("Export"));
  g_signal_connect(button_export, "clicked", G_CALLBACK(signal_button_clicked_export), this);

  GtkButton* button_import = Utils::create_button(_("Import"));
  g_signal_connect(button_import, "clicked", G_CALLBACK(signal_button_clicked_import), this);

  gtk_widget_set_size_request(GTK_WIDGET(button_up), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_down), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_export), -1, 30);
  gtk_widget_set_size_request(GTK_WIDGET(button_import), -1, 30);

  gtk_table_attach(table, GTK_WIDGET(button_up), 7, 8, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_down), 7, 8, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_export), 7, 8, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_import), 7, 8, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_default), this);

  gtk_widget_set_size_request(GTK_WIDGET(button_default), -1, 30);

  gtk_table_attach(table, GTK_WIDGET(button_default), 5, 7, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  return GTK_WIDGET(table);
}

vector<string> CalcSetting::GetExamItemsCalc() {
  // 系统默认的检查部位

  vector<string> vec;

  #ifdef EMP_322
    size_t size = sizeof(EXAM_TYPES_CALC) / sizeof(EXAM_TYPES_CALC[0]);

    for (size_t i = 0; i < size; i++) {
      vec.push_back(EXAM_TYPES_CALC[i]);
    }
  #else
    size_t size = sizeof(EXAM_TYPES) / sizeof(EXAM_TYPES[0]);

    for (size_t i = 0; i < size; i++) {
      vec.push_back(EXAM_TYPES[i]);
    }
  #endif

  return vec;
}

void CalcSetting::ClearAll() {
  MeasureMan::GetInstance()->ClearAllValue();
  g_menuCalc.ClearAllData();

  //clear screen
  KeyClearScreen kcs;
  kcs.Execute();
  g_menuCalc.ChangeAllCalcItems();
  g_menuCalc.ClearAllFlag();
}

// ---------------------------------------------------------

void CalcSetting::ComboBoxChangedExamType(GtkComboBox* combobox) {
  ChangeModel2();

  // update measur sequence
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));
  if(!exam_type_name) {
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  int sequence = GetSequence(exam_type);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence), sequence);
}

void CalcSetting::ComboBoxChangedDepartment(GtkComboBox* combobox) {
  printf("--%s--\n", __FUNCTION__);
  ChangeModel();
}

void CalcSetting::ComboBoxChangedSequence(GtkComboBox* combobox) {
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);

  int number = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sequence));

  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni = &ini;
  ptrIni->WriteInt(exam_type, "Sequence", number);
  ptrIni->SyncConfigFile();
}

void CalcSetting::ButtonClickedSelectOne(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(m_treeview_item);
  model = gtk_tree_view_get_model(m_treeview_item);

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("Please select a item before move!"), NULL);

    return;
  }

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  vector<string> vecItem_Calc;
  vecItem_Calc.clear();
  CreateItemList_Calc1(exam_type, vecItem_Calc);

  int item_length(0);
  item_length = vecItem_Calc.size();

  if(!vecItem_Calc.empty()) {
    for(int i=0; i<item_length; i++) {
      if( strcmp(select_name, _(vecItem_Calc[i].c_str())) == 0) {
        GtkTreeModel *model_tmp;
        GtkTreeSelection *selection;
        GtkTreeIter iter_tmp;
        selection = gtk_tree_view_get_selection(m_treeview_selected_item);
        model_tmp= gtk_tree_view_get_model(m_treeview_selected_item);

        //高亮要插入的词条，并更新滚动条
        iter_tmp= InsertUniqueCalc(model_tmp, select_name);
        gtk_tree_selection_select_iter(selection, &iter_tmp);

        GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
        gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path_scroll, NULL, FALSE, 1.0, 1.0);
        gtk_tree_path_free (path_scroll);

        MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
          MessageDialog::DLG_INFO, _(" Item has existed. Please select item again!"), NULL);

        return;
      }
    }
  }

  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni = &ini;
  int number;
  char SelectNum[256];
  number = ptrIni->ReadInt(exam_type, "Number");

  sprintf(SelectNum, "Calc%d",number+1);
  int select_num = ItemNameTransEtype(select_name);

  if (select_num == -1) {
    PRINTF("Fail to ItemNameTransEtype, not exist the etype!\n");
    return;
  }

  ptrIni->WriteInt(exam_type, SelectNum, select_num);
  ptrIni->WriteInt(exam_type, "Number", number+1);
  ptrIni->SyncConfigFile();

  GtkTreeModel *new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(m_treeview_selected_item, new_model1);

  // 高亮插入的词条，并更新滚动条
  GtkTreeSelection *new_selection;
  GtkTreeIter iter_new;
  new_selection = gtk_tree_view_get_selection(m_treeview_selected_item);
  iter_new= InsertUniqueCalc(new_model1, select_name);
  gtk_tree_selection_select_iter(new_selection, &iter_new);

  GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
  gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path_scroll, NULL, TRUE, 1.0, 1.0);
  gtk_tree_path_free (path_scroll);

  // 更新departent列表
  GtkTreeStore *calculate_store = GTK_TREE_STORE(gtk_tree_view_get_model(m_treeview_item));
  gtk_tree_store_remove(calculate_store, &iter);

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedSelectAll(GtkButton* button) {
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return;
  }

  // if m_treeview_item is empty
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item));
  if (gtk_tree_model_iter_n_children(model, NULL) == 0) {
    PRINTF("=========== tree view department items is empty!\n");
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini11(path11);

  string username;
  username = exam.ReadDefaultUserSelect(&ini11);
  char path[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
      sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
      sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile *ptrIni = &ini;

  int Num = ptrIni->ReadInt(exam_type, "Number");

  vector<string> vecSelect_Calc;
  vecSelect_Calc.clear();
  CreateItemList_Calc1(exam_type, vecSelect_Calc);

  int select_length(0);
  select_length = vecSelect_Calc.size();

  gchar *department= gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department));
  char department_name[50];

  size_t size = sizeof(SECTIONS) / sizeof(SECTIONS[0]);

  for(int i=0; i<size; i++) {
    if(strcmp(department, _(SECTIONS[i].c_str()))==0) {
      strcpy(department_name, SECTIONS[i].c_str());
      break;
    }
  }

  char path12[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path12, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(path12, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile new_ini(path12);
  IniFile *new_ptrIni= &new_ini;
  int number = new_ptrIni->ReadInt(department_name, "Number");

  for(int i=1; i<=number; i++) {
    int writeCalc=1;
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = new_ptrIni->ReadInt(department_name, CalcNumber);
    string item_name;

    if(item_num < USER_START - BASIC_MEA_END)
      item_name= ItemMenuTransEnglish(item_num);
    else {
      item_name = CustomItemTransName(item_num);
    }

    if(!vecSelect_Calc.empty()) {
      for(int j=0; j<select_length; j++) {
        if(strcmp(_(item_name.c_str()), _(vecSelect_Calc[j].c_str())) ==0) {
          writeCalc=0;
        }
      }
    }

    if(writeCalc) {
      char CalcNum[256];
      sprintf(CalcNum, "Calc%d", ++Num);
      ptrIni->WriteInt(exam_type, CalcNum, item_num);
    }
  }

  ptrIni->WriteInt(exam_type, "Number", Num);
  ptrIni->SyncConfigFile();

  //更新Exam Item和 Department 列表
  ChangeModel2();

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedBackOne(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_selected_item));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_selected_item));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("Please select a item before move!"), NULL);

    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni_calc = &ini;

  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemList_Delete_Calc1(select_name, exam_type, vecDelete_Calc);

  int item_length(0);
  item_length = vecDelete_Calc.size();
  int squence_copy;
  squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
  ptrIni_calc->RemoveGroup(exam_type);
  ptrIni_calc->SyncConfigFile();

  for(int i=0; i<item_length; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i+1);
    ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
  }

  ptrIni_calc->WriteInt(exam_type, "Number", item_length);
  ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
  ptrIni_calc->SyncConfigFile();

  GtkTreeModel *new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_selected_item), new_model1);

  //设置光标，更新滚动条的值
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_selected_item), path, NULL, TRUE);
  gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_scrolled_window_selected_item)));
  gtk_tree_path_free (path);

  ChangeModel();

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedBackAll(GtkButton* button) {
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni = &ini;

  int squence_copy;
  squence_copy= ptrIni->ReadInt(exam_type, "Sequence");

  ptrIni->RemoveGroup(exam_type);

  ptrIni->WriteInt(exam_type, "Number", 0);
  ptrIni->WriteInt(exam_type, "Sequence", squence_copy);
  ptrIni->SyncConfigFile();

  //更新列表
  ChangeModel2();

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedAdd(GtkButton* button) {
  if(g_menuCalc.IsFlagExist()) {
    const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_QUESTION, _(info), signal_callback_add);
  } else {
    AddItem();
  }
}

void CalcSetting::ButtonClickedDelete(GtkButton* button) {
  if(g_menuCalc.IsFlagExist()) {
    const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_QUESTION, _(info), signal_callback_delete);
  } else {
      DeleteItem();
  }
}

void CalcSetting::ButtonClickedUp(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_selected_item));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_selected_item));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("Please select a item before up!"), NULL); //请先选择待插入结点的父结点

    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  char *path_string = gtk_tree_path_to_string(path);
  int path_num = atoi(path_string);
  PRINTF("---path_string:%s %d\n",path_string,path_num);

  if(path_num == 0) {
    return;
  }

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));
  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);

  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemList_Calc2(exam_type, vecDelete_Calc);

  int item_length(0);
  item_length = vecDelete_Calc.size();

  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni_calc = &ini;

  int squence_copy;
  squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
  ptrIni_calc->RemoveGroup(exam_type);
  ptrIni_calc->SyncConfigFile();

  swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num-1]);

  for(int i=0; i<item_length; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i+1);
    ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
  }

  ptrIni_calc->WriteInt(exam_type, "Number", item_length);
  ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
  ptrIni_calc->SyncConfigFile();

  GtkTreeModel *new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_selected_item), new_model1);

  g_free(path_string);

  if(gtk_tree_path_prev(path)) {
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_selected_item), path, NULL, TRUE);

    if(item_length-path_num >12) {
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_selected_item), path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_scrolled_window_selected_item)));//保证在最上端的词条移动时能够正确显示
    }
  }

  gtk_tree_path_free (path);

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedDown(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_selected_item));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_selected_item));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("Please select a item before down!"), NULL);

    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  char *path_string = gtk_tree_path_to_string(path);
  int path_num = atoi(path_string);

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));
  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemList_Calc2(exam_type, vecDelete_Calc);

  int item_length(0);
  item_length = vecDelete_Calc.size();

  int path_total = item_length-1;

  if(path_num != path_total) {
    char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
      sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
      sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }

    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini;

    int squence_copy;
    squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
    ptrIni_calc->RemoveGroup(exam_type);
    ptrIni_calc->SyncConfigFile();

    swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num+1]);

    for(int i=0; i<item_length; i++) {
      char CalcNumber[256];
      sprintf(CalcNumber, "Calc%d", i+1);
      ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
    }

    ptrIni_calc->WriteInt(exam_type, "Number", item_length);
    ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc->SyncConfigFile();

    GtkTreeModel *new_model1 = CreateItemCalcModel1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_selected_item), new_model1);

    g_free(path_string);
    gtk_tree_path_next(path);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_selected_item), path, NULL, TRUE);

    if(item_length-path_num >2) {
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_selected_item), path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_scrolled_window_selected_item)));//保证在最下端的词条移动后能够正常显示
    }

    gtk_tree_path_free (path);
  }

  if (g_menuCalc.GetExamItem() == exam_type) {
    g_menuCalc.ChangeExamItem(exam_type);
  }
}

void CalcSetting::ButtonClickedExport(GtkButton* button) {
  CustomCalc::GetInstance()->CreateExportCalcSettingWin(GetWindow());
}

void CalcSetting::ButtonClickedImport(GtkButton* button) {
  PeripheralMan *ptr = PeripheralMan::GetInstance();

  if(!ptr->CheckUsbStorageState()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("No USB storage found!"), NULL);

    return ;
  } else {
    if(!ptr->MountUsbStorage()) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
        MessageDialog::DLG_ERROR, _("Failed to mount USB storage!"), NULL);

      return ;
    }
  }

  ConfigToHost::GetInstance()->CreateCalcImportWindow(GTK_WINDOW(GetWindow()));
}

void CalcSetting::ButtonClickedDefault(GtkButton* button) {
  if(g_menuCalc.IsFlagExist()) {
    const char* info = N_("Clicking OK will clear calculated value, whether to cotunue?");

    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_QUESTION, _(info), signal_callback_default);
  } else {
    UpdateAll();
  }
}

void CalcSetting::AddItem() {
  CustomCalc::GetInstance()->CreateCalcSettingWin(GetWindow());
}

void CalcSetting::DeleteItem() {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_ERROR, _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点

    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);
  gtk_tree_path_free (path);
  int select_num = ItemNameTransEtype(select_name);

  if (select_num == -1) {
    PRINTF("Fail to ItemNameTransEtype, not exist the etype!\n");
    return;
  }

  if(select_num < USER_START - BASIC_MEA_END) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(GetWindow()),
      MessageDialog::DLG_INFO,  _(" Only Userdefined items can be deleted!"), NULL);

    return;
  }

  char path3[256];
  sprintf(path3, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path3);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  const gchar *department_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department));
  char department[50];
  size_t size = sizeof(SECTIONS) / sizeof(SECTIONS[0]);

  for(int i=0; i<size; i++) {
    if(strcmp(department_name, _(SECTIONS[i].c_str()))==0) {
      strcpy(department, SECTIONS[i].c_str());
      break;
    }
  }

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile new_ini(path1);
  IniFile *ptrIni_calc= &new_ini;

  vector<int> vecDeleteCalc;
  vecDeleteCalc.clear();
  CreateItemList_Delete_Calc(select_name, department,vecDeleteCalc, ptrIni_calc);

  int item_length(0);
  item_length = vecDeleteCalc.size();
  ptrIni_calc->RemoveGroup(department);
  ptrIni_calc->SyncConfigFile();

  for(int i=0; i<item_length; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i+1);
    ptrIni_calc->WriteInt(department, CalcNumber, vecDeleteCalc[i]);
  }

  ptrIni_calc->WriteInt(department, "Number", item_length);
  ptrIni_calc->SyncConfigFile();

  //同时删除各个检查部位下的词条
  for (int i=0; i<= EXAM_NUM; i++) {
    char exam_type[50];
    strcpy(exam_type, ExamItem::ITEM_LIB[i].c_str());

    char path_exam[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
      sprintf(path_exam, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
      sprintf(path_exam, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }

    IniFile ini(path_exam);
    IniFile *ptrIni_calc1 = &ini;

    vector<int> vecDelete_Select;
    vecDelete_Select.clear();
    CreateItemList_Delete_Calc1(select_name, exam_type, vecDelete_Select);

    int item_length(0);
    item_length = vecDelete_Select.size();
    int squence_copy;
    squence_copy= ptrIni_calc1->ReadInt(exam_type, "Sequence");
    ptrIni_calc1->RemoveGroup(exam_type);
    ptrIni_calc1->SyncConfigFile();

    for(int i=0; i<item_length; i++) {
      char CalcNumber[256];
      sprintf(CalcNumber, "Calc%d", i+1);
      ptrIni_calc1->WriteInt(exam_type, CalcNumber, vecDelete_Select[i]);
    }

    ptrIni_calc1->WriteInt(exam_type, "Number", item_length);
    ptrIni_calc1->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc1->SyncConfigFile();
  }

  //同时删除自定义的配置字段
  char CustomEtype[256];
  sprintf(CustomEtype, "CustomEtype-%d", select_num);
  ptrIni_calc->RemoveGroup(CustomEtype);
  ptrIni_calc->SyncConfigFile();

  //更新Exam Item 和Departmen 列表
  ChangeModel2();
  ClearAll();
}

void CalcSetting::UpdateAll() {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path[256];
  char userselectname2[256];
  char path3[256];
  char userselectname3[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    sprintf(path3, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  FileMan f;

  // 计算预设已选所用
  sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_CALC_FILE);
  f.CopyFile(userselectname2, path);

  //计算预设词库所用
  sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE );
  f.CopyFile(userselectname3, path3);

  //更新Exam Item 和Departmen 列表
  ChangeModel2();
  ClearAll();
}

void CalcSetting::CreateDefineItemCalc(vector<string>& vecExamItem) {
  char probelist[256];
  char useritem[256];
  char department[256];
  char firstgenitem[256];
  char src_group[256];
  char userselect[256];
  char path_userselect[256];
  sprintf(path_userselect, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini_userselect(path_userselect);
  ExamItem examitem;
  vector<string> useritemgroup;
  useritemgroup = examitem.GetDefaultUserGroup();

  int group_length(0);
  group_length = useritemgroup.size();
  for (int i= 0 ; i <  group_length; i++) {
    if (useritemgroup[i].length() != 0) {
      sprintf(src_group ,"%s", useritemgroup[i].c_str());
      examitem.GetUserItem(src_group, userselect, probelist, useritem, department, firstgenitem);
      string username=useritem;
      vecExamItem.push_back(username);
    }
  }
}

void CalcSetting::AddColumnsCalc(GtkTreeView* treeview) {
  GtkCellRenderer* cellrenderer_text = gtk_cell_renderer_text_new();

  gint col_offset = gtk_tree_view_insert_column_with_attributes(
    treeview, -1, "", cellrenderer_text, "text", 0, NULL);

  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable(column, TRUE);
}

void CalcSetting::AddColumnsCalc1(GtkTreeView* treeview) {
  GtkCellRenderer* cellrenderer_text = gtk_cell_renderer_text_new();

  gint col_offset = gtk_tree_view_insert_column_with_attributes(
    treeview, -1, "", cellrenderer_text, "text", 0, NULL);

  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable(column, TRUE);
}

GtkTreeModel* CalcSetting::CreateItemCalcModel1() {
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return NULL;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);
  int sequence = GetSequence(exam_type);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence), sequence);

  vector<string> vecItemCalc1;
  vecItemCalc1.clear();
  CreateItemList_Calc1(exam_type, vecItemCalc1);
  int item_size(0);
  item_size = vecItemCalc1.size();

  if(vecItemCalc1.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

  for (int i = 0; i<item_size; i++) {
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0,  _(vecItemCalc1[i].c_str()), -1);
  }

  return GTK_TREE_MODEL(store);
}

GtkTreeModel* CalcSetting::CreateItemCalcModel2() {
  gchar *department_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department));

  if(department_name== NULL) {
    return NULL;
  }

  char department[50];

  size_t size = sizeof(SECTIONS) / sizeof(SECTIONS[0]);

  for(int i=0; i<size; i++) {
    if(strcmp(department_name, _(SECTIONS[i].c_str()))==0) {
      strcpy(department, SECTIONS[i].c_str());
      break;
    }
  }

  vector<string> vecItemCalc;
  vecItemCalc.clear();
  CreateItemList_Calc(department, vecItemCalc);
  int item_size(0);
  item_size = vecItemCalc.size();

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));

  if(!exam_type_name) {
    return NULL;
  }

  char exam_type[256];
  ExamItem exam;
  exam.TransItemName(exam_type_name, exam_type);

  vector<string> vecItemCalc1;
  vecItemCalc1.clear();
  CreateItemList_Calc1(exam_type, vecItemCalc1);
  int item_size1(0);
  item_size1 = vecItemCalc1.size();
  int pos_num=0;

  if(!vecItemCalc1.empty()) {
    for (int i = 0; i<item_size1; i++) {
      vector<string>::iterator pos;
      pos = find(vecItemCalc.begin(), vecItemCalc.end(), vecItemCalc1[i]);

      if(pos != vecItemCalc.end()) {
        vecItemCalc.erase(pos);
        pos_num++;
      } else if(pos == vecItemCalc.end()) {
        // vecItemCalc.pop_back();
        // pos_num++;
      }
    }
  }

  if(vecItemCalc.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

  for (int i = 0; i<(item_size-pos_num); i++) {
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0,  _(vecItemCalc[i].c_str()), -1);
  }

  return GTK_TREE_MODEL(store);
}


// ---------------------------------------------------------
//
//  CustomCalc
//
// ---------------------------------------------------------

CustomCalc* CustomCalc::m_instance = NULL;
GCancellable* CustomCalc::m_cancellable = NULL;

extern vector<int> vecAdd;
extern int item_num_exist;

// ---------------------------------------------------------

CustomCalc* CustomCalc::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new CustomCalc();
  }

  return m_instance;
}

CustomCalc::CustomCalc() {
  m_dialog = NULL;
  m_entry_name = NULL;
  m_combobox_type = NULL;
  m_combobox_method = NULL;

  m_entry_export_name = NULL;
  m_progress_bar = NULL;
  m_image = NULL;
  m_label_notice = NULL;

  m_export_name = "";
}

CustomCalc::~CustomCalc() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void CustomCalc::CreateCalcSettingWin(GtkWidget* parent) {
  m_dialog = Utils::create_dialog(GTK_WINDOW(parent), _("CalcSetting"), 400, 300);

  GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK, GTK_STOCK_APPLY);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  GtkTable* table = GTK_TABLE(gtk_table_new(3, 3, TRUE));
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  gtk_container_set_border_width(GTK_CONTAINER(table), 30);

  gtk_table_set_row_spacings(table, 10);
  gtk_table_set_col_spacings(table, 10);

  // Item
  GtkLabel* label_name = Utils::create_label(_("Item:"));
  m_entry_name = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_name), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_name), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_name, 10);
  gtk_widget_grab_focus(GTK_WIDGET(m_entry_name));
  g_signal_connect_after(G_OBJECT(m_entry_name), "focus-in-event", G_CALLBACK(signal_entry_focus_in), this);

  // Type
  GtkLabel* label_type = Utils::create_label(_("Type:"));
  m_combobox_type = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_type), 0, 1, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_type), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  size_t size = sizeof(METHOD_NAMES) / sizeof(string);

  for(size_t i = 0; i < size; i++) {
    gtk_combo_box_text_append_text(m_combobox_type, METHOD_NAMES[i].c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_type), 0);
  g_signal_connect(m_combobox_type, "changed", G_CALLBACK(signal_combobox_changed_type), this);

  // Method
  GtkLabel* label_method = Utils::create_label(_("Method:"));
  m_combobox_method = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_method), 0, 1, 2, 3);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_method), 1, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  size = sizeof(CUSTOM_TYPES) / sizeof(CustomTypeAndMethod);

  for(size_t i = 0; i < size; i++) {
    if (CUSTOM_TYPES[i].type == 0) {
      gtk_combo_box_text_append_text(m_combobox_method, CUSTOM_TYPES[i].name.c_str());
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_method), 0);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void CustomCalc::CreateExportCalcSettingWin(GtkWidget* parent) {
  m_dialog = Utils::create_dialog(GTK_WINDOW(parent), _("Export"), 450, 300);

  GtkButton* button_export = Utils::add_dialog_button(m_dialog, _("Export"), GTK_RESPONSE_OK, GTK_STOCK_OPEN);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_export, "clicked", G_CALLBACK(signal_button_clicked_export), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  GtkTable* table = GTK_TABLE(gtk_table_new(6, 6, TRUE));
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  gtk_container_set_border_width(GTK_CONTAINER(table), 10);

  gtk_table_set_row_spacings(table, 10);
  gtk_table_set_col_spacings(table, 10);

  // input notice
  GtkLabel* label_input_notice = Utils::create_label(_("Please input name for exporting data:"));
  gtk_table_attach_defaults(table, GTK_WIDGET(label_input_notice), 0, 6, 0, 1);

  // data name
  GtkLabel* label_name = Utils::create_label(_("Data Name:"));
  m_entry_export_name = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_name), 0, 2, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_export_name), 2, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_export_name, 30);
  gtk_entry_set_text(m_entry_export_name, CalcSetting::GetInstance()->GetExamName().c_str());
  gtk_widget_grab_focus(GTK_WIDGET(m_entry_export_name));
  g_signal_connect_after(G_OBJECT(m_entry_export_name), "focus-in-event", G_CALLBACK(signal_entry_focus_in), this);

  // notice
  GtkLabel* label_notice = Utils::create_label(_("Please insert USB storage before export! \nClick Export will export current exam data to USB storage!"));
  gtk_table_attach_defaults(table, GTK_WIDGET(label_notice), 0, 6, 2, 4);

  // progress_bar
  m_progress_bar = Utils::create_progress_bar();
  gtk_table_attach_defaults(table, GTK_WIDGET(m_progress_bar), 0, 6, 4, 5);

  // image, notice

  // GTK_STOCK_APPLY, GTK_STOCK_STOP, GTK_STOCK_YES
  m_image = GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));
  m_label_notice = Utils::create_label("");

  gtk_table_attach_defaults(table, GTK_WIDGET(m_image), 0, 1, 5, 6);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_notice), 1, 6, 5, 6);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  gtk_widget_hide(GTK_WIDGET(m_progress_bar));
  gtk_widget_hide(GTK_WIDGET(m_image));
  gtk_widget_hide(GTK_WIDGET(m_label_notice));

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void CustomCalc::SetProgressBar(double fraction) {
  if (m_progress_bar != NULL) {
    gtk_progress_bar_set_fraction(m_progress_bar, fraction);

    stringstream ss;
    ss << fraction * 100 << "%";

    gtk_progress_bar_set_text(m_progress_bar, ss.str().c_str());
    gtk_widget_show(GTK_WIDGET(m_progress_bar));
  }
}

void CustomCalc::ExportLoadInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

void CustomCalc::ExportRightInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

void CustomCalc::ExportErrorInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_STOP, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

void CustomCalc::ImportSuccess() {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char userselectname[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
  } else {
    sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
  }

  char exam_name[256];
  char exam_type[256];
  strcpy(exam_name, CalcSetting::GetInstance()->GetExamName().c_str());
  exam.TransItemName(exam_name, exam_type);
  IniFile ini_add3(userselectname);
  IniFile *ptrIni_add3 = &ini_add3;
  ptrIni_add3->RemoveGroup(exam_type);
  ptrIni_add3->SyncConfigFile();

  int item_length(0);
  item_length = vecAdd.size();

  for(int i=1; i<=item_length; i++) {
    char CalcNumber1[256];
    sprintf(CalcNumber1, "Calc%d", i);
    ptrIni_add3->WriteInt(exam_type, CalcNumber1, vecAdd[(i-1)]);
  }

  ptrIni_add3->WriteInt(exam_type, "Number", item_length);
  ptrIni_add3->SyncConfigFile();

  char userselectname2[256];
  sprintf(userselectname2, "%s", CALC_TMP_DATA_PATH);
  FileMan f;

  f.DelDirectory(userselectname2);

  usleep(100000);

  ConfigToHost::GetInstance()->ExportRightInfoNotice(_("Success to import from USB storage."));

  ConfigToHost::GetInstance()->OKAndCancelClicked();

  g_timeout_add(1000, Destroy, NULL);

  usleep(400000);
  CalcSetting::GetInstance()->ChangeModel2();

  PeripheralMan *ptr = PeripheralMan::GetInstance();
  ptr->UmountUsbStorage();
  g_menuCalc.ClearAllFlag();
  g_menuCalc.ChangeAllCalcItems();
}

void CustomCalc::ImportWrite(string item_name, int& item_num) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path2[256];
  char path3[256];
  char userselectname[256];
  char userselectname1[256];

  #ifdef VET
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcSetting.ini");
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcItemSetting.ini");
  #else
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/CalcSetting.ini");
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/CalcItemSetting.ini");
  #endif

  FileMan f;
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
    sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile ini_add2(userselectname1);
  IniFile *ptrIni_add2 = &ini_add2;
  int ItemAllNum;
  ItemAllNum=ptrIni_add2->ReadInt("MaxNumber", "Number");
  int ItemDeleteNum=ptrIni_add2->ReadInt("Delete", "Number");

  IniFile ini_add1(path3);
  IniFile *ptrIni_add1 = &ini_add1;
  char CustomEtype[256];
  sprintf(CustomEtype, "CustomEtype-%d",item_num);
  int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
  int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
  string department_tmp = ptrIni_add1->ReadString(CustomEtype, "Department");
  char department_in[256];
  strcpy(department_in, department_tmp.c_str());

  if(ItemDeleteNum) {
    char CalcNumber1[256];
    sprintf(CalcNumber1, "Calc%d", ItemDeleteNum);
    int item_num1=ptrIni_add2->ReadInt("Delete", CalcNumber1);
    ptrIni_add2->RemoveString("Delete", CalcNumber1);
    ptrIni_add2->WriteInt("Delete", "Number", ItemDeleteNum-1);
    ptrIni_add2->SyncConfigFile();

    char CustomEtype2[256];
    sprintf(CustomEtype2, "CustomEtype-%d",item_num1);
    ptrIni_add2->WriteInt(CustomEtype2, "Etype", item_num1);
    ptrIni_add2->WriteString(CustomEtype2, "Name", item_name.c_str());
    ptrIni_add2->WriteInt(CustomEtype2, "Method", method_index);
    ptrIni_add2->WriteInt(CustomEtype2, "MeasureType", type_index);
    ptrIni_add2->WriteString(CustomEtype2, "Department", department_in);
    ptrIni_add2->SyncConfigFile();
    ItemAllNum = item_num1;
    item_num =item_num1;
  } else {
    char CustomEtype1[256];
    sprintf(CustomEtype1, "CustomEtype-%d",++ItemAllNum);
    ptrIni_add2->WriteInt(CustomEtype1, "Etype", ItemAllNum);
    ptrIni_add2->WriteString(CustomEtype1, "Name", item_name.c_str());
    ptrIni_add2->WriteInt(CustomEtype1, "Method", method_index);
    ptrIni_add2->WriteInt(CustomEtype1, "MeasureType", type_index);
    ptrIni_add2->WriteInt("MaxNumber", "Number", ItemAllNum);
    ptrIni_add2->WriteString(CustomEtype1, "Department", department_in);
    ptrIni_add2->SyncConfigFile();
    item_num = ItemAllNum;
  }

  int number;
  char SelectNum[256];
  number = ptrIni_add2->ReadInt(department_in, "Number");
  sprintf(SelectNum, "Calc%d",number+1);
  ptrIni_add2->WriteInt(department_in, SelectNum, ItemAllNum);
  ptrIni_add2->WriteInt(department_in, "Number", number+1);
  ptrIni_add2->SyncConfigFile();

  vecAdd.push_back(item_num);
}

void CustomCalc::ButtonImportNameOK() {
  string item_name = CalcSetting::GetInstance()->CustomItemTransName(item_num_exist);

  char name_copy[256];
  for(int i=1; i<100; i++) {
    sprintf(name_copy, "%s(%d)", item_name.c_str(), i);

    if(RenameCompare(name_copy)) {
      break;
    }

    if(i==99) {
      sprintf(name_copy, "%s(1)", item_name.c_str());
    }
  }

  item_name = name_copy;

  ImportRenameCopy(item_name);
}

// ---------------------------------------------------------

void CustomCalc::ButtonClickedOK(GtkButton* button) {
  string name = strip(gtk_entry_get_text(m_entry_name));
  const char *tmp_name = name.c_str();

  if (tmp_name == NULL || strlen(tmp_name) == 0) {
    gtk_entry_set_text(m_entry_name, "");
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("Please Input Name!"), NULL);
    return;
  }

  int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_method));
  if (index == -1) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
                                      _("Please select measure method!"), NULL);
    return;
  }

  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile *ptrIni = &ini;

  vector<string> useritemgroup;
  useritemgroup = ptrIni->GetGroupName();

  char src_group[256];
  int group_length(0);
  group_length = useritemgroup.size();

  for (int i= 0 ; i <  group_length; i++) {
    sprintf(src_group ,"%s", useritemgroup[i].c_str());
    int number1;
    number1 = ptrIni->ReadInt(src_group, "Number");

    for(int i=1; i<=number1; i++) {
      char CalcNumber[256];
      sprintf(CalcNumber, "Calc%d", i);
      int item_num = ptrIni->ReadInt(src_group, CalcNumber);

      string item_name;
      if(item_num < (USER_START - BASIC_MEA_END)) {
        item_name= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num);
      } else {
        item_name = CalcSetting::GetInstance()->CustomItemTransName(item_num);
      }

      PRINTF("item_name= %s\n", item_name.c_str());
      PRINTF("item_tmp_name =%s\n", tmp_name);

      if((strcmp(tmp_name, item_name.c_str()) == 0) ||(strcmp(tmp_name, _(item_name.c_str())) == 0)) {
        gtk_entry_set_text(GTK_ENTRY(m_entry_name), "");
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("The name has existed, please rename!"), NULL);
        return;
      }
    }
  }

  int ItemDeleteNum=ptrIni->ReadInt("Delete", "Number");

  int ItemAllNum;
  ItemAllNum=ptrIni->ReadInt("MaxNumber", "Number");
  if((ItemDeleteNum==0)&&(ItemAllNum >= (USER_START -BASIC_MEA_END + MAX_USER_CALC_NUM))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
                                      _("The defined items have reached the maximum!"), NULL);
    return;
  }

  int type_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_type));
  if (type_index == -1) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
                                      _("Please select measure type!"), NULL);
    return;
  }

  int type_method_index = 0;
  gchar *method_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_method));
  printf("method_name = %s\n", method_name);

  size_t size = sizeof(CUSTOM_TYPES) / sizeof(CUSTOM_TYPES[0]);

  for(int i=0; i<size; i++) {
    if(strcmp(_(method_name), CUSTOM_TYPES[i].name.c_str()) == 0) {
      type_method_index = CUSTOM_TYPES[i].etype;
      break;
    }
  }

  const gchar *department_name = CalcSetting::GetInstance()->GetDepartmentName().c_str();
  char department[50];

  size = sizeof(SECTIONS) / sizeof(SECTIONS[0]);

  for(int i=0; i<size; i++) {
    if(strcmp(department_name, _(SECTIONS[i].c_str()))==0) {
      strcpy(department, SECTIONS[i].c_str());
      break;
    }
  }

  bool NewNumber = false;
  if(ItemDeleteNum) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", ItemDeleteNum);
    int item_num=ptrIni->ReadInt("Delete", CalcNumber);
    ptrIni->RemoveString("Delete", CalcNumber);
    ptrIni->WriteInt("Delete", "Number", ItemDeleteNum-1);
    ptrIni->SyncConfigFile();

    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",item_num);
    ptrIni->WriteInt(CustomEtype, "Etype", item_num);
    ptrIni->WriteString(CustomEtype, "Name", tmp_name);
    ptrIni->WriteInt(CustomEtype, "Method", type_method_index);
    ptrIni->WriteInt(CustomEtype, "MeasureType", index);
    ptrIni->WriteString(CustomEtype, "Department", department);
    ptrIni->SyncConfigFile();
    ItemAllNum = item_num;
  } else {
    NewNumber = true;
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",++ItemAllNum);
    ptrIni->WriteInt(CustomEtype, "Etype", ItemAllNum);
    ptrIni->WriteString(CustomEtype, "Name", tmp_name);
    ptrIni->WriteInt(CustomEtype, "Method", type_method_index);
    ptrIni->WriteInt(CustomEtype, "MeasureType", index);
    ptrIni->WriteInt("MaxNumber", "Number", ItemAllNum);
    ptrIni->WriteString(CustomEtype, "Department", department);
    ptrIni->SyncConfigFile();
  }

  int number;
  char SelectNum[256];
  number = ptrIni->ReadInt(department, "Number");
  sprintf(SelectNum, "Calc%d",number+1);
  ptrIni->WriteInt(department, SelectNum, ItemAllNum);
  ptrIni->WriteInt(department, "Number", number+1);
  ptrIni->SyncConfigFile();

  DestroyWin();
  CalcSetting::GetInstance()->ChangeModelAndLight(tmp_name);
  CalcSetting::GetInstance()->ClearAll();
}

void CustomCalc::ButtonClickedExport(GtkButton* button) {
  m_export_name = gtk_entry_get_text(m_entry_export_name);

  if (m_export_name.empty()) {
    CustomCalc::GetInstance()->ExportErrorInfoNotice(_("Please Input Name!"));

    return;
  }

  PeripheralMan *ptr = PeripheralMan::GetInstance();
  vector<string> vec = CalcSetting::GetInstance()->GetSelectedVec();

  if(!ptr->CheckUsbStorageState()) {
    CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("No USB storage found!"));
    return ;
  } else {
    if(!ptr->MountUsbStorage()) {
      CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("Failed to mount USB storage!"));
      return ;
    }
  }

  UserSelect::GetInstance()->create_udisk_data_dir();//addec by LL

  int tmp=0;
  struct dirent *ent;
  char udisk_path[256];
  sprintf(udisk_path, "%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH);
  DIR *dir = opendir(udisk_path);

  while( (ent = readdir(dir)) != NULL) {
    PRINTF("ent->name=%s\n", ent->d_name);

    if(ent->d_name[0]=='.') {
      continue;
    }

    if(strcmp(ent->d_name, m_export_name.c_str())==0) {
      CustomCalc::GetInstance()-> ExportErrorInfoNotice(_("The name has existed, please rename!"));
      tmp=1;
      break;
    }
  }

  closedir(dir);
  if(tmp) {
    return;
  }

  if(CalcSetting::GetInstance()->GetSelectPath()) {
    HideClickedOKAndCancel();
    CustomCalc::GetInstance()-> ExportLoadInfoNotice(_("Loading......"));
    g_timeout_add(1000, signal_load_data, this);
  }
}

void CustomCalc::ButtonClickedCancel(GtkButton* button) {
  DestroyWin();
}

void CustomCalc::EntryItemFocusIn(GtkEditable* editable, GdkEventFocus* event) {
  if (KeySwitchIM::m_imOn) {
    KeySwitchIM ksim;
    ksim.ExcuteChange(TRUE);
  }
}

void CustomCalc::ComboboxChangedType(GtkComboBox* combobox) {
  int active = gtk_combo_box_get_active(combobox);

  if (active == -1) {
    return;
  }

  Utils::combobox_clear(GTK_COMBO_BOX(m_combobox_method));

  size_t size = sizeof(CUSTOM_TYPES) / sizeof(CUSTOM_TYPES[0]);

  for(size_t i = 0; i < size; i++) {
    if(active == CUSTOM_TYPES[i].type) {
      gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_method), CUSTOM_TYPES[i].name.c_str());
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_method), 0);
}

void CustomCalc::LoadData() {
  int cond = 0;
  int total=1;
  int count =0;
  char str_info[256], result[256];
  FileMan fm;
  PeripheralMan *ptr = PeripheralMan::GetInstance();
  vector<string> vec = CalcSetting::GetInstance()->GetSelectedVec();

  m_cancellable = g_cancellable_new();

  vector<string>::iterator ite = vec.begin();
  total = vec.size();

  while(ite < vec.end() && !cond) {
    GFile *fAbs = g_file_new_for_path((*ite).c_str());
    GFile *fParent = g_file_get_parent(fAbs);
    g_object_unref(fAbs);
    gchar *strDestParent = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, g_file_get_basename(fParent), NULL);
    g_object_unref(fParent);
    GFile *fDest = g_file_new_for_path(strDestParent);

    //create the parent directory
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(fDest, NULL, &err_mkdir)) {
      if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
        PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
        sprintf(result, _("Failed to send data to USB storage!\nError: Failed to create directory."));
        cond = -1;
        g_error_free(err_mkdir);
        g_object_unref(fDest);
        g_free(strDestParent);
        break;
      }
    }
    g_object_unref(fDest);

    gchar *basename = g_path_get_basename((*ite).c_str());
    if(fm.CompareSuffix(basename, "ini") != 0) {
      CustomCalc::GetInstance()->SetProgressBar(0);
      count++;
    }

    //Perform copy operation

    gchar *destPath = g_build_path(G_DIR_SEPARATOR_S, strDestParent, basename, NULL);
    g_free(strDestParent);
    g_free(basename);
    PRINTF("Dest Path: %s\n", destPath);
    GFile *src = g_file_new_for_path((*ite).c_str());
    GFile *dest = g_file_new_for_path(destPath);
    g_free(destPath);

    GError *err = NULL;
    int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, m_cancellable, signal_callback_progress, NULL, &err);
    g_object_unref(src);
    g_object_unref(dest);

    if(!ret) {
      PRINTF("g_file_copy error: %s\n", err->message);
      switch(err->code) {
      case G_IO_ERROR_NO_SPACE:
        sprintf(result, _("Failed to send data to USB storage!\nError: No space left on storage."));
        break;
      case G_IO_ERROR_CANCELLED:
        sprintf(result, _("Failed to send data to USB storage!\nError: Operation was cancelled."));
        break;
      default:
        sprintf(result, _("Failed to send data to USB storage!"));
        break;
      }
      cond = -1;
      g_error_free(err);
      break;
    }

    ite++;
  }

  //Handle result
  if(!cond) {
    UserSelect::GetInstance()->create_exportUSB_dir(m_export_name.c_str());

    char path4[256];
    char path5[256];

    #ifdef VET
      sprintf(path4, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_export_name.c_str(),"/", "VetCalcSetting.ini");
      sprintf(path5, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_export_name.c_str(),"/", "VetCalcItemSetting.ini");
    #else
      sprintf(path4, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_export_name.c_str(),"/", "CalcSetting.ini");
      sprintf(path5, "%s%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_export_name.c_str(),"/", "CalcItemSetting.ini");
    #endif

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char userselectname[256];
    char userselectname1[256];
    char userselectname2[256];
    char userselectname3[256];

    if(strcmp(username.c_str(), "System Default") == 0) {
      #ifdef VET
        sprintf(userselectname, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/", "VetDSCalcSetting.ini");
        sprintf(userselectname1, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/","VetDSCalcItemSetting.ini");
      #else
        sprintf(userselectname, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/", "DSCalcSetting.ini");
        sprintf(userselectname1, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/","DSCalcItemSetting.ini");
      #endif
    } else {
      sprintf(userselectname, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting/",username.c_str(),".ini");
      sprintf(userselectname1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting/",username.c_str(),".ini");
    }

    char exam_type[256];
    char exam_name[256];
    strcpy(exam_name, CalcSetting::GetInstance()->GetExamName().c_str());
    exam.TransItemName(exam_name, exam_type);
    PRINTF("exam_type=%s\n", exam_type);
    IniFile ini_tmp(userselectname);
    IniFile *ptrIni_tmp = &ini_tmp;

    char src_group[256];
    vector<string> itemgroup;
    itemgroup = ptrIni_tmp->GetGroupName();
    int group_size(0);
    group_size =itemgroup.size();

    for(int i=0; i<group_size; i++) {
      sprintf(src_group ,"%s", itemgroup[i].c_str());
      PRINTF("src_group=%s\n", src_group);

      if(strcmp(src_group,exam_type) != 0) {
        PRINTF("src_group_delete=%s\n", src_group);
        ptrIni_tmp->RemoveGroup(src_group);
        ptrIni_tmp->SyncConfigFile();
      }
    }

    FileMan f;
    f.CopyFile(userselectname, path4);
    f.CopyFile(userselectname1, path5);

    sprintf(userselectname2, "%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcsetting");
    sprintf(userselectname3, "%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/calcitemsetting");
    f.DelDirectory(userselectname2);
    f.DelDirectory(userselectname3);

    ptr->UmountUsbStorage();
    gdk_threads_enter();
    CustomCalc::GetInstance()->SetProgressBar(0.2);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();
    usleep(100000);
    gdk_threads_enter();
    CustomCalc::GetInstance()->SetProgressBar(0.4);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();
    usleep(100000);
    gdk_threads_enter();
    CustomCalc::GetInstance()->SetProgressBar(0.6);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();

    usleep(100000);
    gdk_threads_enter();
    CustomCalc::GetInstance()->SetProgressBar(0.8);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();

    usleep(100000);
    CustomCalc::GetInstance()-> ExportRightInfoNotice(_("Success to export to USB storage."));

    gdk_threads_enter();
    CustomCalc::GetInstance()->SetProgressBar(1.0);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();

    usleep(1000000);
    CustomCalc::GetInstance()->ClickedOKAndCancel();
    CustomCalc::GetInstance()-> DelayDestroyWin();
  } else {
    CustomCalc::GetInstance()->ClickedOKAndCancel();
    CustomCalc::GetInstance()-> ExportErrorInfoNotice(result);
  }
}

void CustomCalc::KeyEvent(unsigned char keyValue) {
  // FakeXEvent::KeyEvent(keyValue);

  #if defined(EMP_322)
    if (keyValue == KEY_CTRL_SHIFT_SPACE) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(FALSE);

      return;
    }
  #elif defined(EMP_313)
    if (keyValue == KEY_ONE) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(FALSE);

      return;
    }
  #else
    if (keyValue == KEY_SHIFT_CTRL) {
      KeySwitchIM ksim;
      ksim.ExcuteChange(FALSE);

      return;
    }
  #endif

  if (FakeMouseButton(keyValue)) {
    return;
  }

  // 数字键
  if (FakeNumKey(keyValue)) {
    return;
  }

  // 字母键
  if (FakeAlphabet(keyValue)) {
    return;
  }

  // 符号键
  if (FakePunctuation(keyValue)) {
    return;
  }

  if (keyValue == KEY_ESC) {
    g_timeout_add(100, signal_exit_customecalc, this);

    FakeEscKey();
  }
}

void CustomCalc::ClickedOKAndCancel() {
  if (m_dialog != NULL) {
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_OK), TRUE);
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_CANCEL), TRUE);
  }
}

void CustomCalc::HideClickedOKAndCancel() {
  if (m_dialog != NULL) {
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_OK), FALSE);
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_CANCEL), FALSE);
  }
}

void CustomCalc::DestroyWin() {
  if (GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();
    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }

  m_dialog = NULL;
  m_entry_name = NULL;
  m_combobox_type = NULL;
  m_combobox_method = NULL;

  m_entry_export_name = NULL;
  m_progress_bar = NULL;
  m_image = NULL;
  m_label_notice = NULL;

  m_export_name = "";
}

void CustomCalc::DelayDestroyWin() {
  DestroyWin();
}

void CustomCalc::ImportRenameCopy(string item_name) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path2[256];
  char path3[256];
  char userselectname[256];
  char userselectname1[256];

  #ifdef VET
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcSetting.ini");
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcItemSetting.ini");
  #else
    sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/CalcSetting.ini");
    sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/CalcItemSetting.ini");
  #endif

  FileMan f;
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
    sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile ini_add2(userselectname1);
  IniFile *ptrIni_add2 = &ini_add2;
  int ItemAllNum;
  ItemAllNum=ptrIni_add2->ReadInt("MaxNumber", "Number");
  int ItemDeleteNum=ptrIni_add2->ReadInt("Delete", "Number");

  IniFile ini_add1(path3);
  IniFile *ptrIni_add1 = &ini_add1;
  char CustomEtype[256];
  sprintf(CustomEtype, "CustomEtype-%d",item_num_exist);
  int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
  int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
  string department_tmp = ptrIni_add1->ReadString(CustomEtype, "Department");
  char department_in[256];
  strcpy(department_in, department_tmp.c_str());

  if(ItemDeleteNum) {
    char CalcNumber1[256];
    sprintf(CalcNumber1, "Calc%d", ItemDeleteNum);
    int item_num1=ptrIni_add2->ReadInt("Delete", CalcNumber1);
    ptrIni_add2->RemoveString("Delete", CalcNumber1);
    ptrIni_add2->WriteInt("Delete", "Number", ItemDeleteNum-1);
    ptrIni_add2->SyncConfigFile();

    char CustomEtype2[256];
    sprintf(CustomEtype2, "CustomEtype-%d",item_num1);
    ptrIni_add2->WriteInt(CustomEtype2, "Etype", item_num1);
    ptrIni_add2->WriteString(CustomEtype2, "Name", item_name.c_str());
    ptrIni_add2->WriteInt(CustomEtype2, "Method", method_index);
    ptrIni_add2->WriteInt(CustomEtype2, "MeasureType", type_index);
    ptrIni_add2->WriteString(CustomEtype2, "Department", department_in);
    ptrIni_add2->SyncConfigFile();
    ItemAllNum = item_num1;
    item_num_exist =item_num1;
  } else {
    char CustomEtype1[256];
    sprintf(CustomEtype1, "CustomEtype-%d",++ItemAllNum);
    ptrIni_add2->WriteInt(CustomEtype1, "Etype", ItemAllNum);
    ptrIni_add2->WriteString(CustomEtype1, "Name", item_name.c_str());
    ptrIni_add2->WriteInt(CustomEtype1, "Method", method_index);
    ptrIni_add2->WriteInt(CustomEtype1, "MeasureType", type_index);
    ptrIni_add2->WriteInt("MaxNumber", "Number", ItemAllNum);
    ptrIni_add2->WriteString(CustomEtype1, "Department", department_in);
    ptrIni_add2->SyncConfigFile();
    item_num_exist = ItemAllNum;
  }

  int number;
  char SelectNum[256];
  number = ptrIni_add2->ReadInt(department_in, "Number");
  sprintf(SelectNum, "Calc%d",number+1);
  ptrIni_add2->WriteInt(department_in, SelectNum, ItemAllNum);
  ptrIni_add2->WriteInt(department_in, "Number", number+1);
  ptrIni_add2->SyncConfigFile();

  vecAdd.push_back(item_num_exist);
}

bool CustomCalc::RenameCompare(string name_copy) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char userselectname1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
  } else {
    sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile ini_add4(userselectname1);
  IniFile *ptrIni_add4 = &ini_add4;
  vector<string> useritemgroup;
  useritemgroup = ptrIni_add4->GetGroupName();

  char src_group[256];
  int group_length(0);
  group_length = useritemgroup.size();
  for (int i= 0 ; i <  group_length; i++) {
    sprintf(src_group ,"%s", useritemgroup[i].c_str());
    int number1;
    number1 = ptrIni_add4->ReadInt(src_group, "Number");

    for(int i=1; i<=number1; i++) {
      char CalcNumber[256];
      sprintf(CalcNumber, "Calc%d", i);
      int item_num2 = ptrIni_add4->ReadInt(src_group, CalcNumber);
      string item_name1;

      if(item_num2 < (USER_START - BASIC_MEA_END)) {
        item_name1= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num2);
      } else {
        item_name1 = CalcSetting::GetInstance()->CustomItemTransName(item_num2);
      }

      if(strcmp(item_name1.c_str(),name_copy.c_str()) == 0) {
        return false;
      }
    }
  }

  return true;
}

/////////////////////////////////////


#include <dirent.h>

#include "display/gui_global.h"
#include "display/gui_func.h"

#include "keyboard/KeyDef.h"

#include "calcPeople/MeaCalcFun.h"
#include "ViewMain.h"







CalcSetting g_menuCalcExamType;







void CalcSetting::CreateItemList_Calc(char *department,vector<string>& vecItemCalc) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");
    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(department, CalcNumber);
        string item_name("");
        if(item_num < (USER_START - BASIC_MEA_END))
            item_name= ItemMenuTransEnglish(item_num);
        else {
            item_name = CustomItemTransName(item_num);
        }
        if(strlen(item_name.c_str()) != 0)
            vecItemCalc.push_back(item_name.c_str());
    }
}

void CalcSetting::CreateDefaultItemList_Calc(char *department,vector<string>& vecItemCalc) {
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE);

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(department, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
            item_name= ItemMenuTransEnglish(item_num);
        else {
            item_name = CustomItemTransName(item_num);
        }
        vecItemCalc.push_back(item_name);
    }
}

int CalcSetting::ItemNameTransEtype(char *select_name) {
    int select_num = -1;
    for(int i=0; i < (ABD_MEA_END - ABD_MEA_START); i++) {
        if(strcmp(select_name, _(AbdoInfo[i].title))==0 ) {
            select_num = AbdoInfo[i].item - BASIC_MEA_END;
            return select_num;
        }
    }
    for(int i=0; i < (ADULT_MEA_END - ADULT_MEA_START); i++) {
        if(strcmp(select_name, _(AdultInfo[i].title))==0 ) {
            select_num = AdultInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (UR_MEA_END - UR_MEA_START); i++) {
        if(strcmp(select_name, _(URInfo[i].title))==0 ) {
            select_num = URInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (OB_MEA_END - OB_MEA_START); i++) {
        if(strcmp(select_name, _(OBInfo[i].title))==0 ) {
            select_num = OBInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (GYN_MEA_END - GYN_MEA_START); i++) {
        if(strcmp(select_name, _(GYNInfo[i].title))==0 ) {
            select_num = GYNInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (SP_MEA_END - SP_MEA_START); i++) {
        if(strcmp(select_name, _(SPInfo[i].title))==0 ) {
            select_num = SPInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (VS_MEA_END - VS_MEA_START); i++) {
        if(strcmp(select_name, _(VSInfo[i].title))==0 ) {
            select_num = VSInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (FETAL_MEA_END - FETAL_MEA_START); i++) {
        if(strcmp(select_name, _(FetalInfo[i].title))==0 ) {
            select_num = FetalInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (TCD_MEA_END - TCD_MEA_START); i++) {
        if(strcmp(select_name, _(TCDInfo[i].title))==0 ) {
            select_num = TCDInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (ORTHO_MEA_END - ORTHO_MEA_START); i++) {
        if(strcmp(select_name, _(OrthoInfo[i].title))==0 ) {
            select_num = OrthoInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (EFW_MEA_END - EFW_MEA_START); i++) {
        if(strcmp(select_name, _(EFWInfo[i].title))==0 ) {
            select_num = EFWInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }

#ifdef VET
    for(int i=0; i < (TD_MEA_END - TD_MEA_START); i++) {
        if(strcmp(select_name, _(TDInfo[i].title))==0 ) {
            select_num = TDInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
    for(int i=0; i < (ANOB_MEA_END - ANOB_MEA_START); i++) {
        if(strcmp(select_name, _(AnOBInfo[i].title))==0 ) {
            select_num = AnOBInfo[i].item - BASIC_MEA_END;
            return select_num;
        }

    }
#endif
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    int number = ptrIni->ReadInt("MaxNumber", "Number");
    for(int i=(USER_START - BASIC_MEA_END); i<=number; i++) {
        char CustomEtype[256];
        sprintf(CustomEtype, "CustomEtype-%d",i);
        string item_name = ptrIni->ReadString(CustomEtype, "Name");
        if(strcmp(select_name, _(item_name.c_str()))==0 ) {
            select_num = ptrIni->ReadInt(CustomEtype, "Etype");

            return select_num;
        }

    }

    return select_num;
}

string CalcSetting::ItemMenuTransEnglish(int item_num) {
    item_num += BASIC_MEA_END;
    string item_name("");
    if((item_num >= ABD_MEA_START)&&(item_num < ABD_MEA_END)) {
        for(int i=0; i < (ABD_MEA_END - ABD_MEA_START); i++) {
            if(item_num == AbdoInfo[i].item) {
                item_name = AbdoInfo[i].title;
            }
        }
    } else if((item_num >= ADULT_MEA_START)&&(item_num < ADULT_MEA_END)) {
        for(int i=0; i < (ADULT_MEA_END - ADULT_MEA_START); i++) {
            if(item_num == AdultInfo[i].item) {
                item_name = AdultInfo[i].title;
            }
        }
    } else if((item_num >= UR_MEA_START)&&(item_num < UR_MEA_END)) {
        for(int i=0; i < (UR_MEA_END - UR_MEA_START); i++) {
            if(item_num == URInfo[i].item) {
                item_name = URInfo[i].title;
            }
        }
    }

    else if((item_num >= OB_MEA_START)&&(item_num < OB_MEA_END)) {
        for(int i=0; i < (OB_MEA_END - OB_MEA_START); i++) {
            if(item_num == OBInfo[i].item) {
                item_name = OBInfo[i].title;
            }
        }
    }

    else if((item_num >= GYN_MEA_START)&&(item_num < GYN_MEA_END)) {
        for(int i=0; i < (GYN_MEA_END - GYN_MEA_START); i++) {
            if(item_num == GYNInfo[i].item) {
                item_name = GYNInfo[i].title;
            }
        }
    }

    else if((item_num >= SP_MEA_START)&&(item_num < SP_MEA_END)) {
        for(int i=0; i < (SP_MEA_END - SP_MEA_START); i++) {
            if(item_num == SPInfo[i].item) {
                item_name = SPInfo[i].title;
            }
        }
    } else if((item_num >= VS_MEA_START)&&(item_num < VS_MEA_END)) {
        for(int i=0; i < (VS_MEA_END - VS_MEA_START); i++) {
            if(item_num == VSInfo[i].item) {
                item_name = VSInfo[i].title;
            }
        }
    } else  if((item_num >= FETAL_MEA_START)&&(item_num < FETAL_MEA_END)) {
        for(int i=0; i < (FETAL_MEA_END - FETAL_MEA_START); i++) {
            if(item_num == FetalInfo[i].item) {
                item_name = FetalInfo[i].title;
            }
        }
    } else if((item_num >= TCD_MEA_START)&&(item_num < TCD_MEA_END)) {
        for(int i=0; i < (TCD_MEA_END - TCD_MEA_START); i++) {
            if(item_num == TCDInfo[i].item) {
                item_name = TCDInfo[i].title;
            }
        }
    } else if((item_num >= ORTHO_MEA_START)&&(item_num < ORTHO_MEA_END)) {
        for(int i=0; i < (ORTHO_MEA_END - ORTHO_MEA_START); i++) {
            if(item_num == OrthoInfo[i].item) {
                item_name = OrthoInfo[i].title;
            }
        }
    } else if((item_num >= EFW_MEA_START)&&(item_num < EFW_MEA_END)) {
        for(int i=0; i < (EFW_MEA_END - EFW_MEA_START); i++) {
            if(item_num == EFWInfo[i].item) {
                item_name = EFWInfo[i].title;
            }
        }
    }
#ifdef VET
    else if((item_num >= TD_MEA_START)&&(item_num < TD_MEA_END)) {
        for(int i=0; i < (TD_MEA_END - TD_MEA_START); i++) {
            if(item_num == TDInfo[i].item) {
                item_name = TDInfo[i].title;
            }
        }
    }

    else if((item_num >= ANOB_MEA_START)&&(item_num < ANOB_MEA_END)) {
        for(int i=0; i < (ANOB_MEA_END - ANOB_MEA_START); i++) {
            if(item_num == AnOBInfo[i].item) {
                item_name = AnOBInfo[i].title;
            }
        }
    }

#endif
    return item_name;
}

string CalcSetting::CustomItemTransName(int item_num) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",item_num);
    string item_name = ptrIni->ReadString(CustomEtype, "Name");
    return item_name;
}

void CalcSetting::CreateItemList_Calc1(char *probe_exam,vector<string>& vecItemCalc1) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END) {
            item_name= ItemMenuTransEnglish(item_num);
        } else {
            item_name = CustomItemTransName(item_num);
        }
        vecItemCalc1.push_back(item_name);
    }
}

void CalcSetting::CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        vecItemCalc1.push_back(item_num);
    }
}



void CalcSetting::CreateItemList_Delete_Calc(char *select_name, char *department, vector<int>& vecDeleteCalc, IniFile *ini) {
    IniFile *ptrIni= ini;
    ExamItem examitem;

    int number;
    number = ptrIni->ReadInt(department, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num= ptrIni->ReadInt(department, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
            item_name= ItemMenuTransEnglish(item_num);
        else {
            item_name = CustomItemTransName(item_num);
        }
        if(strcmp(select_name, _(item_name.c_str()))!=0) {
            vecDeleteCalc.push_back(item_num);
        } else {
            int deletenum;
            char CalcNum[256];
            deletenum = ptrIni->ReadInt("Delete", "Number");
            sprintf(CalcNum, "Calc%d", (deletenum+1));
            ptrIni->WriteInt("Delete", CalcNum, item_num);
            ptrIni->WriteInt("Delete", "Number", (deletenum+1));
            ptrIni->SyncConfigFile();
        }
    }
}

void CalcSetting::CreateItemList_Delete_Calc1(char *select_name, string probe_exam,vector<int>& vecItemCalc1) {

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);

        int item_num = ptrIni->ReadInt(probeExam, CalcNumber);
        string item_name;
        if(item_num < USER_START - BASIC_MEA_END)
            item_name= ItemMenuTransEnglish(item_num);
        else {
            item_name = CustomItemTransName(item_num);
        }

        if(strcmp(select_name, _(item_name.c_str()))!=0) {
            vecItemCalc1.push_back(item_num);
        }
    }
}





void CalcSetting::ChangeModel2(void) {
    GtkTreeModel *model1 = CreateItemCalcModel1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_selected_item), model1);

    GtkTreeModel *model = CreateItemCalcModel2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item), model);

}

void CalcSetting::ChangeModel(void) {
    GtkTreeModel *model = CreateItemCalcModel2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item), model);

}

void CalcSetting::ChangeExamBox(char *check_part) {
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_type), check_part);
}

void CalcSetting::ChangeExamBoxDelete(void) {
    vector<string> vecExamItem_calc = GetExamItemsCalc();

    //用户自定义的检查部位
    CreateDefineItemCalc(vecExamItem_calc);
    int exam_size(0);
    exam_size = vecExamItem_calc.size();
    for(int i = exam_size; i >= 0; i--) {
        gtk_combo_box_remove_text(GTK_COMBO_BOX (m_combobox_exam_type), i);
    }

    for (int i = 0; i <exam_size; i++) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_type), _(vecExamItem_calc[i].c_str()));
    }
}

void CalcSetting::ChangeExamBoxToDefault(void) {
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_type), 0);
}

void CalcSetting::ChangeModelAndLight(const char *name) {
    GtkTreeModel *model = CreateItemCalcModel2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item), model);

    GtkTreeSelection *selection;
    GtkTreeIter iter_tmp;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item));
    iter_tmp= InsertUniqueCalc(model, name);
    gtk_tree_selection_select_iter(selection, &iter_tmp);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(model, &iter_tmp);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item), path_scroll, NULL, FALSE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);
}

const string CalcSetting::GetExamName() {
  if (m_combobox_exam_type != NULL) {
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_type));
  } else {
    return "";
  }
}

const string CalcSetting::GetDepartmentName() {
  if (m_combobox_department != NULL) {
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_department));
  } else {
    return "";
  }
}

int CalcSetting::GetSequence(const char *exam_type) {
    ExamItem exam;
    char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    string examType = exam.ReadDefaultProbeDefaultItemName(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path1, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini;
    int squence = ptrIni_calc->ReadInt(exam_type, "Sequence");
    //printf("===================squence=%d exam_type=%s\n", squence, exam_type);
    return squence;
}

GtkTreeIter CalcSetting::InsertUniqueCalc(GtkTreeModel *model, const char *str) {
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
    while (has_node == TRUE) {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strcmp(strtmp, str) == 0)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}











gboolean CalcSetting::GetSelectPath(void) {
    m_vecPath.clear();

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char path[256];
    char userselectname2[256];
    char path3[256];
    char userselectname3[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
        sprintf(path3, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);

    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");

    }

    m_vecPath.push_back(path);
    m_vecPath.push_back(path3);

    if(!m_vecPath.empty())
        return TRUE;
    else
        return FALSE;

}







//获得exam_type检查部位下所有测量项的item，push到vector中
void CalcSetting::GetCalcListEtype(char *exam_type, vector<int> & vecItemCalc) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(exam_type, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(exam_type, CalcNumber);
        vecItemCalc.push_back(item_num);
    }
}

//获得exam_type检查部位下的测量项总数
int CalcSetting::GetCalcListNum(char *exam_type) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Number");

}

//获得exam_type测量的排序方法 0:none   1:repeat   2:next
int CalcSetting::GetMeasureSequence(const char *exam_type) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Sequence");

}

/*
 *int Etype: 自定义的测量Etype
 *int measure_type: 通过Etype，需要得到的测量方式
 *string calc_name: 通过Etype，需要得到的自定义测量的名称
 */
void CalcSetting::GetCustomCalcMeasure(int Etype, int &measure_type, string &calc_name) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",Etype);
    measure_type=ptrIni->ReadInt(CustomEtype, "Method");
    calc_name=ptrIni->ReadString(CustomEtype, "Name");
}

// 返回自定义测量项的最大值
int CalcSetting::GetCalcMaxEtype() {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    return ptrIni->ReadInt("MaxNumber", "Number");
}

//get department for custom measure
void CalcSetting::GetDepartmentForCustomMeasure(int Etype, string &department) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;
    char CustomEtype[256];
    sprintf(CustomEtype, "CustomEtype-%d",Etype-BASIC_MEA_END);
    department=ptrIni->ReadString(CustomEtype, "Department");
}
