#include "sysMan/MeasureSetting.h"

#include "utils/IniFile.h"
#include "utils/MessageDialog.h"

#include "Def.h"
#include "calcPeople/MeaCalcFun.h"
#include "display/TopArea.h"
#include "measure/MenuMeasure.h"
#include "patient/FileMan.h"
#include "probe/ExamItem.h"
#include "sysMan/CalcSetting.h"

#include "display/gui_global.h"

MeasureSetting* MeasureSetting::m_instance = NULL;

// ---------------------------------------------------------

MeasureSetting* MeasureSetting::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new MeasureSetting();
  }

  return m_instance;
}

MeasureSetting::MeasureSetting() {
  m_parent = NULL;

  m_combobox_exam_type = NULL;
  m_combobox_sequence = NULL;

  m_scrolled_window_selected_item = NULL;

  m_treeview_item = NULL;
  m_treeview_selected_item = NULL;
}

MeasureSetting::~MeasureSetting() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* MeasureSetting::CreateMeasureWindow(GtkWidget* parent) {
  m_parent = parent;

  GtkTable* table = Utils::create_table(10, 8);

  // Exam Type
  GtkLabel* label_exam_type = Utils::create_label(_("Exam Type:"));
  m_combobox_exam_type = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_exam_type), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_exam_type), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  vector<string> vec_exam_type = CalcSetting::GetInstance()->GetExamItemsCalc();
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
  gtk_table_attach(table, GTK_WIDGET(label_department), 0, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  // scrolled_window
  GtkScrolledWindow* scrolled_window_item = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window_item), 0, 3, 2, 9);

  m_treeview_item = Utils::create_tree_view(CreateItemCalcModel2());
  gtk_container_add(GTK_CONTAINER(scrolled_window_item), GTK_WIDGET(m_treeview_item));

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

  string exam_type = exam.TransItemNameEng(examType.c_str());
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

  gtk_table_attach(table, GTK_WIDGET(button_select_one), 3, 4, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_select_all), 3, 4, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_back_one), 3, 4, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_back_all), 3, 4, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkButton* button_up = Utils::create_button(_("Up"));
  g_signal_connect(button_up, "clicked", G_CALLBACK(signal_button_clicked_up), this);

  GtkButton* button_down = Utils::create_button(_("Down"));
  g_signal_connect(button_down, "clicked", G_CALLBACK(signal_button_clicked_down), this);

  gtk_table_attach(table, GTK_WIDGET(button_up), 7, 8, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_down), 7, 8, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_default), this);

  gtk_table_attach(table, GTK_WIDGET(button_default), 5, 7, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  return GTK_WIDGET(table);
}

// 获得exam_type测量的排序方法 0:none   1:repeat   2:next
int MeasureSetting::GetMeasureSequence(string exam_type) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);

  return  ini.ReadInt(exam_type, "Sequence");
}

// 获得exam_type检查部位下所有测量项的item，push到vector中
void MeasureSetting::GetMeasureListEtype(string exam_type, vector<int> & vecItemCalc) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile* ptrIni= &ini;

  int number;
  number = ptrIni->ReadInt(exam_type, "Number");

  if(number == 0) {
    return;
  }

  for(int i=1; i<=number; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = ptrIni->ReadInt(exam_type, CalcNumber);
    vecItemCalc.push_back(item_num);
  }
}

void MeasureSetting::ChangeExamBox(string check_part) {
  gtk_combo_box_text_append_text(m_combobox_exam_type, check_part.c_str());
}

void MeasureSetting::ChangeExamBoxDelete() {
  vector<string> vecExamItem_calc = CalcSetting::GetInstance()->GetExamItemsCalc();

  // 用户自定义的检查部位
  CreateDefineItemCalc(vecExamItem_calc);
  int exam_size(0);

  exam_size = vecExamItem_calc.size();

  for(int i = exam_size; i >= 0; i--) {
    gtk_combo_box_text_remove(m_combobox_exam_type, i);
  }

  for (int i = 0; i <exam_size; i++) {
    gtk_combo_box_text_append_text(m_combobox_exam_type, _(vecExamItem_calc[i].c_str()));
  }
}

// ---------------------------------------------------------

void MeasureSetting::ComboBoxChangedExamType(GtkComboBox* combobox) {
  ChangeModel2();

  // update measur sequence
  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if (exam_type_name.empty()) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name.c_str());
  int sequence = GetSequence(exam_type);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence), sequence);
}

void MeasureSetting::ComboBoxChangedMeasureSequence(GtkComboBox* combobox) {
  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if(exam_type_name.empty()) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);

  int number = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sequence));

  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  ini.WriteInt(exam_type, "Sequence", number);
  ini.SyncConfigFile();
}

void MeasureSetting::ButtonClickedSelectOne(GtkButton* button) {
  GtkTreeIter iter;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_item);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_item);

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please select a item before move!"), NULL);

    return;
  }

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);

  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if(exam_type_name.empty()) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  vector<string> vecItem_Calc;
  vecItem_Calc.clear();
  CreateItemListForMeasureed(exam_type, vecItem_Calc);

  int item_length(0);
  item_length = vecItem_Calc.size();

  if(!vecItem_Calc.empty()) {
    for(int i=0; i<item_length; i++) {
      if(strcmp(select_name, _(vecItem_Calc[i].c_str())) == 0) {
        GtkTreeIter iter_tmp;

        GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_selected_item);
        GtkTreeModel* model_tmp= gtk_tree_view_get_model(m_treeview_selected_item);
        //高亮要插入的词条，并更新滚动条
        iter_tmp= InsertUniqueCalc(model_tmp, select_name);
        gtk_tree_selection_select_iter(selection, &iter_tmp);

        GtkTreePath* path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
        gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path_scroll, NULL, FALSE, 1.0, 1.0);
        gtk_tree_path_free (path_scroll);

        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
          MessageDialog::DLG_INFO, _("Item has existed. Please select item again!"), NULL);

        return;
      }
    }
  }

  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile* ptrIni = &ini;
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

  GtkTreeModel* new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(m_treeview_selected_item, new_model1);

  // 高亮插入的词条，并更新滚动条
  GtkTreeIter iter_new;
  GtkTreeSelection* new_selection = gtk_tree_view_get_selection(m_treeview_selected_item);
  iter_new= InsertUniqueCalc(new_model1, select_name);
  gtk_tree_selection_select_iter(new_selection, &iter_new);

  GtkTreePath* path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
  gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path_scroll, NULL, TRUE, 1.0, 1.0);
  gtk_tree_path_free (path_scroll);

  // 更新departent列表
  GtkTreeStore* calculate_store = GTK_TREE_STORE(gtk_tree_view_get_model(m_treeview_item));
  gtk_tree_store_remove(calculate_store, &iter);

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedSelectAll(GtkButton* button) {
  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if(exam_type_name.empty()) {
    return;
  }

  // if m_treeview_item is empty
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_item);
  if (gtk_tree_model_iter_n_children(model, NULL) == 0) {
    PRINTF("=========== tree view department items is empty!\n");
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini11(path11);

  string username;
  username = exam.ReadDefaultUserSelect(&ini11);

  char path[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile* ptrIni = &ini;

  int Num = ptrIni->ReadInt(exam_type, "Number");

  vector<string> vecSelect_Calc;
  vecSelect_Calc.clear();
  CreateItemListForMeasureed(exam_type, vecSelect_Calc);

  int select_length(0);
  select_length = vecSelect_Calc.size();

  char path12[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path12, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);
  } else {
    sprintf(path12, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile new_ini(path12);
  IniFile* new_ptrIni= &new_ini;
  int number = new_ptrIni->ReadInt("Measure", "Number");

  for(int i=1; i<=number; i++) {
    int writeCalc=1;
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = new_ptrIni->ReadInt("Measure", CalcNumber);
    string item_name;
    item_name= ItemMenuTransEnglish(item_num);

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

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedBackOne(GtkButton* button) {
  GtkTreeIter iter;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_selected_item);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_selected_item);

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please select a item before move!"), NULL);

    return;
  }

  GtkTreePath* path = gtk_tree_model_get_path(model, &iter);

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);

  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if (exam_type_name.empty()) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile* ptrIni_calc = &ini;

  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemListForDeleteMeasureed(select_name, exam_type, vecDelete_Calc);

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

  GtkTreeModel* new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(m_treeview_selected_item, new_model1);
  //设置光标，更新滚动条的值
  gtk_tree_view_set_cursor(m_treeview_selected_item, path, NULL, TRUE);
  gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolled_window_selected_item));
  gtk_tree_path_free (path);

  ChangeModel();

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedBackAll(GtkButton* button) {
  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if (exam_type_name.empty()) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile* ptrIni = &ini;

  int squence_copy;
  squence_copy= ptrIni->ReadInt(exam_type, "Sequence");

  ptrIni->RemoveGroup(exam_type);

  ptrIni->WriteInt(exam_type, "Number", 0);
  ptrIni->WriteInt(exam_type, "Sequence", squence_copy);
  ptrIni->SyncConfigFile();

  //更新列表
  ChangeModel2();

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedUp(GtkButton* button) {
  GtkTreeIter iter;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_selected_item);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_selected_item);

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please select a item before up!"), NULL); //请先选择待插入结点的父结点
    return;
  }

  GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
  char* path_string = gtk_tree_path_to_string(path);
  int path_num = atoi(path_string);
  PRINTF("---path_string:%s %d\n",path_string,path_num);

  if(path_num == 0) {
    return;
  }

  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);

  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemListCalc2(exam_type, vecDelete_Calc);

  int item_length(0);
  item_length = vecDelete_Calc.size();

  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile* ptrIni_calc = &ini;

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

  GtkTreeModel* new_model1 = CreateItemCalcModel1();
  gtk_tree_view_set_model(m_treeview_selected_item, new_model1);

  g_free(path_string);

  if(gtk_tree_path_prev(path)) {
    gtk_tree_view_set_cursor(m_treeview_selected_item, path, NULL, TRUE);

    if(item_length-path_num >12) {
      gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolled_window_selected_item));//保证在最上端的词条移动时能够正确显示
    }
  }

  gtk_tree_path_free (path);

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedDown(GtkButton* button) {
  GtkTreeIter iter;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_selected_item);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_selected_item);

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please select a item before down!"), NULL);

    return;
  }

  GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
  char* path_string = gtk_tree_path_to_string(path);
  int path_num = atoi(path_string);

  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  vector<int> vecDelete_Calc;
  vecDelete_Calc.clear();
  CreateItemListCalc2(exam_type, vecDelete_Calc);

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
      sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    } else {
      sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }

    IniFile ini(path1);
    IniFile* ptrIni_calc = &ini;

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

    //更新树
    GtkTreeModel* new_model1 = CreateItemCalcModel1();
    gtk_tree_view_set_model(m_treeview_selected_item, new_model1);

    g_free(path_string);
    gtk_tree_path_next(path);
    gtk_tree_view_set_cursor(m_treeview_selected_item, path, NULL, TRUE);

    if(item_length-path_num >2) {
      gtk_tree_view_scroll_to_cell(m_treeview_selected_item, path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolled_window_selected_item));//保证在最下端的词条移动后能够正常显示
    }

    gtk_tree_path_free (path);
  }

  if (g_menuMeasure.GetExamItem() == exam_type) {
    g_menuMeasure.ChangeExamItem(exam_type);
  }
}

void MeasureSetting::ButtonClickedDefault(GtkButton* button) {
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
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    sprintf(path3, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    sprintf(path3, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");
  }

  FileMan f;

  // 计算预设已选所用
  sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_FILE);
  f.CopyFile(userselectname2, path);

  //计算预设词库所用
  sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_ITEM_FILE );
  f.CopyFile(userselectname3, path3);

  //更新Exam Item 和Departmen 列表
  ChangeModel2();

  g_menuMeasure.ChangeAllMeasureItems();
}

void MeasureSetting::CreateDefineItemCalc(vector<string>& vec) {
  ExamItem examitem;
  vector<string> useritemgroup = examitem.GetDefaultUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    if (!useritemgroup[i].empty()) {
      string userselect;
      string probelist;
      string username;
      string department;
      string firstgenitem;

      examitem.GetUserItem(useritemgroup[i], userselect, probelist, username, department, firstgenitem);
      vec.push_back(username);
    }
  }
}

GtkTreeModel* MeasureSetting::CreateItemCalcModel1() {
  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if (exam_type_name.empty()) {
    return NULL;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  int sequence = GetSequence(exam_type);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence), sequence);

  vector<string> vecItemCalc1;
  vecItemCalc1.clear();
  CreateItemListForMeasureed(exam_type, vecItemCalc1);
  int item_size(0);
  item_size = vecItemCalc1.size();

  if(vecItemCalc1.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeStore* store = gtk_tree_store_new(1, G_TYPE_STRING);

  for (int i = 0; i<item_size; i++) {
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0,  _(vecItemCalc1[i].c_str()), -1);
  }

  return GTK_TREE_MODEL(store);
}

GtkTreeModel* MeasureSetting::CreateItemCalcModel2() {
  vector<string> vecItemCalc;
  vecItemCalc.clear();
  CreateItemListForMeasure(vecItemCalc);
  int item_size(0);
  item_size = vecItemCalc.size();

  string exam_type_name;

  if (m_combobox_exam_type != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_exam_type)) >= 0) {
    exam_type_name = gtk_combo_box_text_get_active_text(m_combobox_exam_type);
  }

  if (exam_type_name.empty()) {
    return NULL;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);

  vector<string> vecItemCalc1;
  vecItemCalc1.clear();
  CreateItemListForMeasureed(exam_type, vecItemCalc1);
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
  GtkTreeStore* store = gtk_tree_store_new(1, G_TYPE_STRING);

  for (int i = 0; i<(item_size-pos_num); i++) {
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0,  _(vecItemCalc[i].c_str()), -1);
  }

  return GTK_TREE_MODEL(store);
}

void MeasureSetting::AddColumnsCalc(GtkTreeView* treeview) {
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  gint col_offset = gtk_tree_view_insert_column_with_attributes(
    treeview, -1, "", renderer, "text", 0, NULL);
  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}

void MeasureSetting::AddColumnsCalc1(GtkTreeView* treeview) {
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  gint col_offset = gtk_tree_view_insert_column_with_attributes(
    treeview, -1, "", renderer, "text", 0, NULL);
  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}

void MeasureSetting::ChangeModel() {
  gtk_tree_view_set_model(m_treeview_item, CreateItemCalcModel2());
}

void MeasureSetting::ChangeModel2() {
  gtk_tree_view_set_model(m_treeview_selected_item, CreateItemCalcModel1());
  gtk_tree_view_set_model(m_treeview_item, CreateItemCalcModel2());
}

int MeasureSetting::GetSequence(const string exam_type) {
  ExamItem exam;
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  int squence = ini.ReadInt(exam_type, "Sequence");
  return squence;
}

string MeasureSetting::ItemMenuTransEnglish(int item_num) {
  string item_name;
  if((item_num <ABD_MEA_START)&&(item_num >=0)) {
    for(int i=0; i < ABD_MEA_START; i++) {
      if(item_num == BasicInfo[i].item) {
        item_name = BasicInfo[i].title;
      }
    }
  }

  return item_name;
}

int MeasureSetting::ItemNameTransEtype(string select_name) {
  int select_num = -1;
  for(int i=0; i < ABD_MEA_START; i++) {
    if(strcmp(select_name.c_str(), _(BasicInfo[i].title))==0 ) {
      select_num = BasicInfo[i].item;

      return select_num;
    }
  }

  return select_num;
}

GtkTreeIter MeasureSetting::InsertUniqueCalc(GtkTreeModel* model, const string str) {
  GtkTreeIter tmp_iter;
  char* strtmp = NULL;
  gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);

  while (has_node) {
    gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);

    if (strcmp(strtmp, str.c_str()) == 0) {
      return tmp_iter;
    } else {
      has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
  }

  return tmp_iter;
}

void MeasureSetting::CreateItemListCalc2(string probe_exam, vector<int>& vec) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile* ptrIni= &ini;

  int number = ptrIni->ReadInt(probe_exam, "Number");

  if(number ==0) {
    return;
  }

  for(int i=1; i<=number; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
    vec.push_back(item_num);
  }
}

void MeasureSetting::CreateItemListForMeasure(vector<string>& vec) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile* ptrIni= &ini;
  int number = ptrIni->ReadInt("Measure", "Number");

  for(int i=1; i<=number; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = ptrIni->ReadInt("Measure", CalcNumber);
    string item_name;
    item_name= ItemMenuTransEnglish(item_num);
    vec.push_back(item_name.c_str());
  }
}

void MeasureSetting::CreateItemListForMeasureed(string probe_exam, vector<string>& vec) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile* ptrIni= &ini;

  int number = ptrIni->ReadInt(probe_exam, "Number");

  if(number ==0) {
    return;
  }

  for(int i=1; i<=number; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
    string item_name;
    item_name= ItemMenuTransEnglish(item_num);
    vec.push_back(item_name);
  }
}

void MeasureSetting::CreateItemListForDeleteMeasureed(string select_name, string probe_exam, vector<int>& vec) {
  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path1);
  ExamItem exam;
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path[256];

  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);

  int number = ini.ReadInt(probe_exam, "Number");

  if(number ==0) {
    return;
  }

  for(int i=1; i<=number; i++) {
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);

    int item_num = ini.ReadInt(probe_exam, CalcNumber);
    string item_name = ItemMenuTransEnglish(item_num);

    if(strcmp(select_name.c_str(), _(item_name.c_str()))!=0) {
      vec.push_back(item_num);
    }
  }
}
