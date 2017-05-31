#include "comment/MenuNote.h"

#include "comment/NoteArea.h"
#include "probe/ExamItem.h"
#include "probe/ProbeSelect.h"
#include "sysMan/ViewSystem.h"

enum {
  NAME_COLUMN,
  INDEX_COLUMN,
  N_COLUMNS
};

MenuNote g_menuNote;

// ---------------------------------------------------------

MenuNote::MenuNote() {
  m_table = NULL;

  m_comboDept = NULL;
  m_modelDept = NULL;

  m_scrolledwindow_item_current = NULL;
  m_scrolledwindow_item_abdo = NULL;
  m_scrolledwindow_item_uro = NULL;
  m_scrolledwindow_item_car = NULL;
  m_scrolledwindow_item_ob = NULL;
  m_scrolledwindow_item_gyn = NULL;
  m_scrolledwindow_item_sp = NULL;
  m_scrolledwindow_item_vs = NULL;
  m_scrolledwindow_item_ortho = NULL;

  m_treeview_item_current = NULL;
  m_treeview_item_abdo = NULL;
  m_treeview_item_uro = NULL;
  m_treeview_item_car = NULL;
  m_treeview_item_ob = NULL;
  m_treeview_item_gyn = NULL;
  m_treeview_item_sp = NULL;
  m_treeview_item_vs = NULL;
  m_treeview_item_ortho = NULL;
}

MenuNote::~MenuNote() {
}

GtkWidget* MenuNote::Create() {
  m_table = Utils::create_table(10, 3);

  gtk_container_set_border_width(GTK_CONTAINER(m_table), 0);

  // ComboBox
  m_comboDept = Utils::create_combobox_text();

  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_comboDept), 1, 2, 0, 1);

  m_modelDept = CreateDeptModel();
  gtk_combo_box_set_model(GTK_COMBO_BOX(m_comboDept), m_modelDept);

  g_signal_connect(m_comboDept, "changed", G_CALLBACK(signal_combobox_changed_dept), this);

  // ScrolledWindow
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateCurrentItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateAbdoItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateUroItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateCarItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateObItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateGynItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateSpItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateVsItem()), 0, 3, 1, 10);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(CreateOrthoItem()), 0, 3, 1, 10);

  return GTK_WIDGET(m_table);
}

void MenuNote::Show() {
  UpdateLabel();
  gtk_widget_show_all(GTK_WIDGET(m_table));

  // 根据不同的探头部位设置不同科别
  ExamItem exam;

  IniFile ini_default(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string current_item = exam.ReadDefaultProbeDefaultItemName(&ini_default);
  string current_probe = exam.ReadDefaultProbe(&ini_default);
  string current_exam = exam.TransItemNameEng(current_item);
  string username = exam.ReadDefaultUserSelect(&ini_default);

  string path;

  if (username == "System Default") {
    path = string(CFG_RES_PATH) + string(COMMENT_FILE);
  } else {
    stringstream ss;
    ss << CFG_RES_PATH << COMMENT_PATH << username << ".ini";

    path = ss.str();
  }

  int index = 0;
  ExamItem::EItem item = ProbeSelect::GetItemIndex();

  IniFile ini(path);
  int number = ini.ReadInt(current_probe + "-" + current_exam, "Number");

  if (number == 0) {
    switch(item) {
    case ExamItem::ABDO_ADULT:
    case ExamItem::ABDO_LIVER:
    case ExamItem::ABDO_KID:
    case ExamItem::CAR_KID:
    case ExamItem::USER1:
    case ExamItem::USER2:
    case ExamItem::USER3:
    case ExamItem::USER4:
      index = 1;
      break;
    case ExamItem::KIDNEY:
    case ExamItem::BLADDER:
      index = 2;
      break;
    case ExamItem::CAR_ADULT:
      index = 3;
      break;
    case ExamItem::EARLY_PREG:
    case ExamItem::LATER_PREG:
      index = 4;
      break;
    case ExamItem::GYN:
      index = 5;
      break;
    case ExamItem::SMALL_PART:
    case ExamItem::GLANDS:
    case ExamItem::THYROID:
    case ExamItem::EYE:
      index = 6;
      break;
    case ExamItem::CAROTID:
    case ExamItem::JUGULAR:
    case ExamItem::PERI_ARTERY:
    case ExamItem::PERI_VEIN:
      index = 7;
      break;
    case ExamItem::HIP_JOINT:
    case ExamItem::JOINT_CAVITY:
    case ExamItem::MENISCUS:
    case ExamItem::SPINE:
      index = 8;
      break;
    default:
      index = 1;
      break;
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboDept), index);

  string department;

  if (index == 0) {
    department = current_probe + "-" + current_exam;
  } else {
    department = DepartmentName(index);
  }

  HideMenu();
  ShowMenu(index, department);
}

void MenuNote::Hide() {
  gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboDept));
  gtk_widget_hide_all(GTK_WIDGET(m_table));
}

void MenuNote::Focus() {
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_current), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_abdo), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_uro), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_car), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_ob), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_gyn), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_sp), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_vs), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_treeview_item_ortho), TRUE);

  int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboDept));

  switch (index) {
  case 0:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_current));
      break;
    }
  case 1:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_abdo));
      break;
    }
  case 2:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_uro));
      break;
    }
  case 3:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_car));
      break;
    }
  case 4:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_ob));
      break;
    }
  case 5:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_gyn));
      break;
    }
  case 6:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_sp));
      break;
    }
  case 7:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_vs));
      break;
    }
  default:
    {
      gtk_widget_grab_focus(GTK_WIDGET(m_treeview_item_ortho));
      break;
    }
  }
}

void MenuNote::UpdateLabel() {
  UpdateScaleModel();
}

// ---------------------------------------------------------

void MenuNote::ComboBoxChangedDept(GtkComboBox* combobox) {
  int index = gtk_combo_box_get_active(combobox);

  if (index >= 0) {
    string department;

    if (index == 0) {
      ExamItem exam;
      IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

      string current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
      string current_probe = exam.ReadDefaultProbe(&ini);
      string current_exam = exam.TransItemNameEng(current_item);

      department = current_probe + "-" + current_exam;
    } else {
      department = DepartmentName(index);
    }

    HideMenu();
    ShowMenu(index, department);
  }
}

void MenuNote::TreeButtonPress(GtkWidget* widget, GdkEventButton* event) {
  GtkTreeView* tree = GTK_TREE_VIEW(widget);

  if (event->window == gtk_tree_view_get_bin_window(tree) && event->type == GDK_BUTTON_PRESS && event->button == 1) {
    int x =0;
    int y = 0;

    gtk_tree_view_convert_widget_to_bin_window_coords(tree, event->x, event->y, &x, &y);

    GtkTreeIter iter;
    GtkTreePath* path = NULL;
    GtkTreeModel* model = gtk_tree_view_get_model(tree);

    if (gtk_tree_view_get_path_at_pos(tree, x, y, &path, NULL, NULL, NULL)) {
      if (gtk_tree_model_get_iter(model, &iter, path)) {
        gchar* text = NULL;
        gtk_tree_model_get (model, &iter, NAME_COLUMN, &text, -1);
        NoteArea::GetInstance()->Focus();
        NoteArea::GetInstance()->SetNewText(text);
        g_free (text);
      }

      gtk_tree_path_free(path);
    }
  }
}

GtkTreeModel* MenuNote::CreateDeptModel() {
  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string current_item = exam.ReadDefaultProbeDefaultItemName(&ini);

  GtkTreeIter iter;
  GtkListStore* liststore = gtk_list_store_new(1, G_TYPE_STRING);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _(current_item.c_str()), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Abdomen"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Urology"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Cardiac"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Obstetrics"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Gynecology"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Small Part"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Vascular"), -1);

  gtk_list_store_append(liststore, &iter);
  gtk_list_store_set(liststore, &iter, 0, _("Orthopedic"), -1);

  return GTK_TREE_MODEL(liststore);
}

GtkScrolledWindow* MenuNote::CreateCurrentItem() {
  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  string current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
  string current_probe = exam.ReadDefaultProbe(&ini);
  string current_exam = exam.TransItemNameEng(current_item);
  string department = current_probe + "-" + current_exam;

  m_scrolledwindow_item_current = Utils::create_scrolled_window();
  m_treeview_item_current = CreateItemList(0, department);

  gtk_tree_view_set_headers_visible(m_treeview_item_current, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_current, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_current, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_current), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_current), GTK_WIDGET(m_treeview_item_current));

  gtk_tree_view_expand_all(m_treeview_item_current);
  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_current);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_current;
}

GtkScrolledWindow* MenuNote::CreateAbdoItem() {
  m_scrolledwindow_item_abdo = Utils::create_scrolled_window();
  m_treeview_item_abdo = CreateItemList(1, "Abdomen");

  gtk_tree_view_set_headers_visible(m_treeview_item_abdo, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_abdo, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_abdo, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_abdo), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_abdo), GTK_WIDGET(m_treeview_item_abdo));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_abdo);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_abdo;
}

GtkScrolledWindow* MenuNote::CreateUroItem() {
  m_scrolledwindow_item_uro = Utils::create_scrolled_window();
  m_treeview_item_uro = CreateItemList(2, "Urology");

  gtk_tree_view_set_headers_visible(m_treeview_item_uro, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_uro, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_uro, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_uro), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_uro), GTK_WIDGET(m_treeview_item_uro));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_uro);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_uro;
}

GtkScrolledWindow* MenuNote::CreateCarItem() {
  m_scrolledwindow_item_car = Utils::create_scrolled_window();
  m_treeview_item_car = CreateItemList(3, "Cardiac");

  gtk_tree_view_set_headers_visible(m_treeview_item_car, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_car, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_car, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_car), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_car), GTK_WIDGET(m_treeview_item_car));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_car);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_car;
}

GtkScrolledWindow* MenuNote::CreateObItem() {
  m_scrolledwindow_item_ob = Utils::create_scrolled_window();
  m_treeview_item_ob = CreateItemList(4, "Obstetrics");

  gtk_tree_view_set_headers_visible(m_treeview_item_ob, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_ob, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_ob, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_ob), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_ob), GTK_WIDGET(m_treeview_item_ob));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_ob);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_ob;
}

GtkScrolledWindow* MenuNote::CreateGynItem() {
  m_scrolledwindow_item_gyn = Utils::create_scrolled_window();
  m_treeview_item_gyn = CreateItemList(5, "Gynecology");

  gtk_tree_view_set_headers_visible(m_treeview_item_gyn, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_gyn, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_gyn, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_gyn), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_gyn), GTK_WIDGET(m_treeview_item_gyn));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_gyn);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_gyn;
}

GtkScrolledWindow* MenuNote::CreateSpItem() {
  m_scrolledwindow_item_sp = Utils::create_scrolled_window();
  m_treeview_item_sp = CreateItemList(6, "Small Part");

  gtk_tree_view_set_headers_visible(m_treeview_item_sp, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_sp, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_sp, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_sp), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_sp), GTK_WIDGET(m_treeview_item_sp));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_sp);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_sp;
}

GtkScrolledWindow* MenuNote::CreateVsItem() {
  m_scrolledwindow_item_vs = Utils::create_scrolled_window();
  m_treeview_item_vs = CreateItemList(7, "Vascular");

  gtk_tree_view_set_headers_visible(m_treeview_item_vs, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_vs, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_vs, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_vs), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_vs), GTK_WIDGET(m_treeview_item_vs));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_vs);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_vs;
}

GtkScrolledWindow* MenuNote::CreateOrthoItem() {
  m_scrolledwindow_item_ortho = Utils::create_scrolled_window();
  m_treeview_item_ortho = CreateItemList(8, "Orthopedic");

  gtk_tree_view_set_headers_visible(m_treeview_item_ortho, FALSE);
  gtk_tree_view_set_rules_hint(m_treeview_item_ortho, TRUE);
  gtk_tree_view_set_enable_search(m_treeview_item_ortho, FALSE);

  g_signal_connect(G_OBJECT(m_treeview_item_ortho), "button-press-event", G_CALLBACK (signal_tree_button_press), this);

  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_item_ortho), GTK_WIDGET(m_treeview_item_ortho));

  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_item_ortho);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  return m_scrolledwindow_item_ortho;
}

string MenuNote::DepartmentName(int index) {
  switch (index) {
  case 1:
    return "Abdomen";
  case 2:
    return "Urology";
  case 3:
    return "Cardiac";
  case 4:
    return "Obstetrics";
  case 5:
    return "Gynecology";
  case 6:
    return "Small Part";
  case 7:
    return "Vascular";
  default:
    return "Orthopedic";
  }
}

void MenuNote::HideMenu() {
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_current));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_abdo));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_uro));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_car));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_ob));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_gyn));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_sp));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_vs));
  gtk_widget_hide(GTK_WIDGET(m_scrolledwindow_item_ortho));
}

void MenuNote::ShowMenu(int index, string department) {
  GtkScrolledWindow* scrolledwindow = NULL;
  GtkTreeView* treeview = NULL;

  switch (index) {
  case 0:
    {
      scrolledwindow = m_scrolledwindow_item_current;
      treeview = m_treeview_item_current;

      break;
    }
  case 1:
    {
      scrolledwindow = m_scrolledwindow_item_abdo;
      treeview = m_treeview_item_abdo;

      break;
    }
  case 2:
    {
      scrolledwindow = m_scrolledwindow_item_uro;
      treeview = m_treeview_item_uro;

      break;
    }
  case 3:
    {
      scrolledwindow = m_scrolledwindow_item_car;
      treeview = m_treeview_item_car;

      break;
    }
  case 4:
    {
      scrolledwindow = m_scrolledwindow_item_ob;
      treeview = m_treeview_item_ob;

      break;
    }
  case 5:
    {
      scrolledwindow = m_scrolledwindow_item_gyn;
      treeview = m_treeview_item_gyn;

      break;
    }
  case 6:
    {
      scrolledwindow = m_scrolledwindow_item_sp;
      treeview = m_treeview_item_sp;

      break;
    }
  case 7:
    {
      scrolledwindow = m_scrolledwindow_item_vs;
      treeview = m_treeview_item_vs;

      break;
    }
  case 8:
    {
      scrolledwindow = m_scrolledwindow_item_ortho;
      treeview = m_treeview_item_ortho;

      break;
    }
  default:
    {
      scrolledwindow = NULL;
      treeview = NULL;

      break;
    }
  }

  if (scrolledwindow != NULL) {
    GtkTreeModel* model = CreateItemNoteModel(index, department);

    gtk_tree_view_set_model(treeview, NULL);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);

    gtk_widget_queue_draw(GTK_WIDGET(treeview));
    gtk_widget_show(GTK_WIDGET(treeview));
    gtk_widget_show(GTK_WIDGET(scrolledwindow));
  }
}

void MenuNote::UpdateScaleModel() {
  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string current_item = exam.ReadDefaultProbeDefaultItemName(&ini);

  GtkTreeIter iter;
  GtkListStore* store = GTK_LIST_STORE(m_modelDept);

  gtk_tree_model_get_iter_first(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _(current_item.c_str()), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Abdomen"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Urology"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Cardiac"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Obstetrics"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Gynecology"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Small Part"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Vascular"), -1);

  gtk_tree_model_iter_next(m_modelDept, &iter);
  gtk_list_store_set(store, &iter, 0, _("Orthopedic"), -1);
}

GtkTreeModel* MenuNote::CreateItemNoteModel(int index, string department) {
  string path;

  if (index == 0) {
    ExamItem exam;
    IniFile ini_default(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
    string username = exam.ReadDefaultUserSelect(&ini_default);

    if (username == "System Default") {
      path = string(CFG_RES_PATH) + string(COMMENT_FILE);
    } else {
      stringstream ss;
      ss << CFG_RES_PATH << COMMENT_PATH << username << ".ini";

      path = ss.str();
    }
  } else {
    path = string(CFG_RES_PATH) + string(NOTE_FILE);
  }

  IniFile ini(path);

  int number = ini.ReadInt(department, "Number");

  vector<ExamPara> vecItemComment;

  for(int i = 1; i<= number; i++) {
    stringstream ss;
    ss << "Note" << i;

    ExamPara exampara;
    exampara.dept_name = "";
    exampara.name = ini.ReadString(department, ss.str());
    exampara.index=ExamItem::USERNAME;
    vecItemComment.push_back(exampara);
  }

  if(vecItemComment.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeStore* store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  vector<ExamPara>::iterator iterItem;
  for (iterItem = vecItemComment.begin(); iterItem != vecItemComment.end(); iterItem++) {
    gtk_tree_store_append(store, &iter, NULL);

    if(index == 0) {
      gtk_tree_store_set(store, &iter, NAME_COLUMN, iterItem->name.c_str(),
        INDEX_COLUMN, iterItem->index, -1);
    } else {
      gtk_tree_store_set(store, &iter, NAME_COLUMN, iterItem->name.c_str(),
        INDEX_COLUMN, iterItem->index, -1);
    }
  }

  return GTK_TREE_MODEL(store);
}

GtkTreeView* MenuNote::CreateItemList(int index, string department) {
  GtkTreeModel* model = CreateItemNoteModel(index, department);

  GtkTreeView* treeView = Utils::create_tree_view(model);
  AddColumnsComment(treeView);

  /* set view property */
  gtk_tree_view_set_enable_search(treeView, FALSE);
  gtk_tree_view_set_enable_tree_lines(treeView, TRUE);
  gtk_tree_view_set_headers_visible(treeView, FALSE);
  gtk_tree_view_set_rules_hint(treeView, TRUE);

  /* selection handling */
  GtkTreeSelection* select = gtk_tree_view_get_selection(treeView);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
  gtk_widget_add_events(GTK_WIDGET(treeView), (gtk_widget_get_events(GTK_WIDGET(treeView)) | GDK_BUTTON_RELEASE_MASK));
  gtk_tree_view_collapse_all(treeView);

  return treeView;
}

void MenuNote::AddColumnsComment(GtkTreeView* treeview) {
  GtkCellRenderer* cellrenderer = gtk_cell_renderer_text_new();
  gint col_offset = gtk_tree_view_insert_column_with_attributes(
    treeview, -1, "", cellrenderer, "text", 0, NULL);
  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);

  gtk_tree_view_column_set_clickable(column, TRUE);
}
