#include "sysMan/ConfigToHost.h"

#include "utils/FakeXUtils.h"
#include "utils/MessageDialog.h"

#include "calcPeople/MeasureDef.h"
#include "keyboard/KeyDef.h"
#include "keyboard/KeyValueOpr.h"
#include "patient/FileMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/UserSelect.h"
#include "sysMan/ViewSystem.h"

enum {
  COL_CHECKED,
  COL_NAME,
  NUM_COLS
};

vector<int> vecAdd;
int item_num_exist = 0;

ConfigToHost* ConfigToHost::m_instance = NULL;
GCancellable* ConfigToHost::m_cancellable = NULL;

// ---------------------------------------------------------

ConfigToHost* ConfigToHost::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ConfigToHost();
  }

  return m_instance;
}

ConfigToHost::ConfigToHost() {
  m_parent = NULL;
  m_dialog = NULL;
  m_treeview_root = NULL;
  m_treeview_branch = NULL;

  m_check_button_del = NULL;
  m_progress_bar = NULL;
  m_image = NULL;
  m_label_notice = NULL;

  m_listBranch = NULL;
}

ConfigToHost::~ConfigToHost() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ConfigToHost::CreateWindow(GtkWidget* parent) {
  m_parent = parent;
  m_dialog = Utils::create_dialog(GTK_WINDOW(m_parent), _("Data Selection"), 640, 480);

  GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK, GTK_STOCK_OPEN);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  GtkTable* table = Utils::create_table(1, 3);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  gtk_table_set_col_spacings(table, 0);

  // scrolled_window
  GtkScrolledWindow* scrolled_window_root = Utils::create_scrolled_window();
  GtkScrolledWindow* scrolled_window_branch = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window_root), 0, 1, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window_branch), 1, 3, 0, 1);

  // Root
  m_treeview_root = CreateTreeview(0);
  gtk_container_add(GTK_CONTAINER(scrolled_window_root), GTK_WIDGET(m_treeview_root));

  GtkTreeSelection* selectionRoot = gtk_tree_view_get_selection(m_treeview_root);
  gtk_tree_selection_set_mode(selectionRoot, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(selectionRoot), "changed", G_CALLBACK(signal_treeselection_changed_root), this);

  GtkTreeModel* modelRoot = CreateRootModel();

  if (modelRoot != NULL) {
    gtk_tree_view_set_model(m_treeview_root, modelRoot);
  }

  g_object_unref(modelRoot);

  // Branch
  m_treeview_branch = CreateTreeview(1);
  gtk_container_add(GTK_CONTAINER(scrolled_window_branch), GTK_WIDGET(m_treeview_branch));

  UpdateRootModel();

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ConfigToHost::CreateCalcImportWindow(GtkWidget* parent) {
  m_parent = parent;
  m_dialog = Utils::create_dialog(GTK_WINDOW(parent), _("Import"), 400, 450);

  GtkButton* button_import = Utils::add_dialog_button(m_dialog, _("Import"), GTK_RESPONSE_OK, GTK_STOCK_OPEN);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_import, "clicked", G_CALLBACK(signal_button_clicked_import), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  GtkTable* table = Utils::create_table(9, 6);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  // item
  GtkLabel* label_item = Utils::create_label(_("Plsease select one item:"));
  GtkScrolledWindow* scrolled_window_root = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_item), 0, 6, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window_root), 0, 6, 1, 5);

  m_treeview_root = CreateTreeviewCalc();
  gtk_container_add(GTK_CONTAINER(scrolled_window_root), GTK_WIDGET(m_treeview_root));

  GtkTreeSelection* selectionRoot = gtk_tree_view_get_selection(m_treeview_root);
  gtk_tree_selection_set_mode(selectionRoot, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(selectionRoot), "changed", G_CALLBACK(signal_treeselection_changed_root_calc), this);

  GtkTreeModel* modelRoot = CreateRootModelCalc();

  if (modelRoot != NULL) {
    gtk_tree_view_set_model(m_treeview_root, modelRoot);
  }

  g_object_unref(modelRoot);

  // check_button
  m_check_button_del = Utils::create_check_button(_("Delete data from USB storage after import."));
  gtk_table_attach_defaults(table, GTK_WIDGET(m_check_button_del), 0, 6, 5, 6);

  // notice
  GtkLabel* label_notice = Utils::create_label(_("Import data to override the current exam data!"));
  gtk_table_attach_defaults(table, GTK_WIDGET(label_notice), 0, 6, 6, 7);

  // progress_bar
  m_progress_bar = Utils::create_progress_bar();
  gtk_table_attach(table, GTK_WIDGET(m_progress_bar), 0, 6, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  // image, notice

  // GTK_STOCK_APPLY, GTK_STOCK_STOP, GTK_STOCK_YES
  m_image = GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));
  m_label_notice = Utils::create_label("");

  gtk_table_attach_defaults(table, GTK_WIDGET(m_image), 0, 1, 8, 9);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_notice), 1, 6, 8, 9);

  UpdateRootModelCalc();

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  gtk_widget_hide(GTK_WIDGET(m_progress_bar));
  gtk_widget_hide(GTK_WIDGET(m_image));
  gtk_widget_hide(GTK_WIDGET(m_label_notice));

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ConfigToHost::DestroyWindow() {
  if(m_listBranch != NULL && g_list_length(m_listBranch) > 0) {
    //clean the data in list
    m_listBranch = g_list_first(m_listBranch);

    while(m_listBranch) {
      GtkTreeModel* model = NULL;

      if(m_listBranch->data) {
        model = GTK_TREE_MODEL(m_listBranch->data);
      }

      m_listBranch = g_list_remove(m_listBranch, GTK_TREE_MODEL(m_listBranch->data));

      if(model) {
        g_object_unref(model);
      }
    }

    // PRINTF("%s: List length is %d\n", __FUNCTION__, g_list_length(m_listBranch));
    g_list_free(m_listBranch);
    m_listBranch = NULL;
  }

  if(GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();
    gtk_widget_destroy(GTK_WIDGET(m_dialog));
  }

  m_dialog = NULL;
  m_treeview_root = NULL;
  m_treeview_branch = NULL;

  m_check_button_del = NULL;
  m_progress_bar = NULL;
  m_image = NULL;
  m_label_notice = NULL;
}

void ConfigToHost::ClickedOKAndCancel() {
  if (m_dialog != NULL) {
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_OK), TRUE);
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_CANCEL), TRUE);
  }
}

void ConfigToHost::ExportRightInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

// ---------------------------------------------------------

void ConfigToHost::ButtonClickedOK(GtkButton* button) {
  // Copy the selected file to host
  if(GetAllSelectPath()) {
    g_timeout_add(1000, signal_callback_load_selected_data, this);

    PRINTF("Load From U disk!\n");

    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_PROGRESS_CANCEL,
      _("Please wait, loading data from USB storage..."),
      signal_callback_cancelloadhost);
  }

  DestroyWindow();
}

void ConfigToHost::ButtonClickedImport(GtkButton* button) {
  // Copy the selected file to host
  if(GetAllSelectPathCalc()) {
    HideClickedOKAndCancel();
    ExportLoadInfoNotice(_("Loading......"));

    // g_timeout_add(1000, signal_callback_load_selected_data_calc, this);
    LoadSelectedDataCalc();
  }
}

void ConfigToHost::ButtonClickedCancel(GtkButton* button) {
  DestroyWindow();
}

void ConfigToHost::RootSelectionChanged(GtkTreeSelection* selection) {
  GtkTreeIter iter;
  GtkTreeModel* model;
  gchar* text;

  if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, COL_NAME, &text, -1);
    GtkTreePath* path = gtk_tree_model_get_path(model, &iter);

    UpdateBranchModel(atoi(gtk_tree_path_to_string(path)));
    gtk_tree_path_free (path);
    g_free(text);
  }
}

void ConfigToHost::RootSelectionChangedCalc(GtkTreeSelection* selection) {
  GtkTreeIter iter;
  GtkTreeModel* model;
  gchar* text;

  if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, 0, &text, -1);

    PRINTF("Selection path: %s\n", text);
    m_rootName = text;
    g_free(text);
  }
}

void ConfigToHost::ToggleData(GtkCellRendererToggle* cell, gchar* path_str) {
  GtkTreeView *treeview;
  gint type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "type"));

  if(type == 0) {
    treeview = m_treeview_root;
  } else if(type == 1) {
    treeview = m_treeview_branch;
  } else {
    return;
  }

  //  PRINTF("Toggle path: %s\n", path_str);
  GtkTreeModel* model = gtk_tree_view_get_model(treeview);
  GtkTreeIter iter;
  GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
  gboolean checked;

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get(model, &iter, COL_CHECKED, &checked, -1);

  /* do something with the value */
  checked ^= 1; //按位异或

  /* set new value */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_CHECKED, checked, -1);

  if(type == 0 ) { //Root
    gint rows = atoi(gtk_tree_path_to_string(path));
    UpdateBranchModel(rows);
    GList *list = g_list_first(m_listBranch);
    SetAllToggleValue(GTK_TREE_MODEL(g_list_nth_data(list, rows)), checked);
  } else if(type == 1) { //Branch
    GtkTreeIter i;
    GtkTreeModel *m;

    if(!checked) {
      GtkTreeSelection* s = gtk_tree_view_get_selection(m_treeview_root);

      if(gtk_tree_selection_get_selected(s, &m, &i)) {
        gtk_list_store_set(GTK_LIST_STORE(m), &i, COL_CHECKED, checked, -1);
      }
    } else {
      //check all status
      gboolean checkall = TRUE;
      m = gtk_tree_view_get_model(m_treeview_branch);
      gboolean valid = gtk_tree_model_get_iter_first(m, &i);

      while(valid) {
        gboolean val;
        gtk_tree_model_get(m, &i, COL_CHECKED, &val, -1);

        if(!val) {
          checkall = FALSE;
        }

        valid = gtk_tree_model_iter_next(m, &i);
      }

      if(checkall) {
        GtkTreeSelection* s = gtk_tree_view_get_selection(m_treeview_root);

        if(gtk_tree_selection_get_selected(s, &m, &i)) {
          gtk_list_store_set(GTK_LIST_STORE(m), &i, COL_CHECKED, TRUE, -1);
        }
      }
    }
  }

  /* clean up */
  gtk_tree_path_free (path);
}

void ConfigToHost::LoadSelectedData() {
  int cond = 0;
  int count = 1;
  int total = 0;
  char str_info[256], result[256];
  FileMan fm;
  PeripheralMan *ptr = PeripheralMan::GetInstance();
  vector<string> vec = ConfigToHost::GetInstance()->GetSelectedVec();

  if(!ptr->CheckUsbStorageState()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("No USB storage found!"), NULL);

    return;
  } else {
    if(!ptr->MountUsbStorage()) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_ERROR, _("Failed to mount USB storage!"), NULL);

      return;
    }
  }

  m_cancellable = g_cancellable_new();

  //list all string for test
  vector<string>::iterator ite = vec.begin();
  total = vec.size() / 2;
  char usb_file_path[256];

  while(ite < vec.end() && !cond) {
    //Update info
    gchar *basename = g_path_get_basename((*ite).c_str());
    PRINTF("basename:%s\n", basename);

    // only display .ini file
    if(fm.CompareSuffix(basename, "ini") != 0) {
      sprintf(str_info, "%s %s   %d/%d\n%s", _("Loading..."), basename, count, total, _("Please wait..."));
      MessageDialog::GetInstance()->SetText(str_info);
      MessageDialog::GetInstance()->SetProgressBar(0);
      count++;
    }

    //lhm20140925
    gchar *destPath;
    gchar *destPath_other = NULL;

    #ifdef VET
      sprintf(usb_file_path, "%s/default/VetItemPara.ini",UDISK_DATA_PATH);
    #else
      sprintf(usb_file_path, "%s/default/ItemPara.ini",UDISK_DATA_PATH);
    #endif

    char name[50]= {0};
    if(strcmp(usb_file_path, (*ite).c_str()) == 0) { //default目录下文件路径
      destPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, basename, NULL);
      destPath_other = g_build_path(G_DIR_SEPARATOR_S, EXAM_FILE_OTHER, NULL);
      // delete the '.ini'
      strcpy(name, "System Default");
    } else {
      destPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PATH, basename, NULL);

      // delete the '.ini'
      strcpy(name, basename);
      int i;
      for (i =0; i < 50; i++) {
        if ((name[i] == '.') && (name[i+1] == 'i') && (name[i+2] =='n') && (name[i+3] == 'i')) {
          name[i] = name[i+1] = name[i+2] = name[i+3] = '\0';
          break;
        }
      }
      if (UserSelect::GetInstance()->username_unique_db(USERNAME_DB, name)) {
        UserSelect::GetInstance()->insert_username_db(USERNAME_DB, name);
        ViewSystem::GetInstance()->ShowList(name);
        int last =  UserSelect::GetInstance()->get_active_user();
        ViewSystem::GetInstance()->set_active_user(last);
      }
    }

    g_free(basename);

    //  PRINTF("Dest Path: %s\n", destPath);
    GFile *src = g_file_new_for_path((*ite).c_str());
    GFile *dest = g_file_new_for_path(destPath);
    g_free(destPath);
    GFile *dest_other = NULL;

    if(destPath_other != NULL) {
      dest_other = g_file_new_for_path(destPath_other);
      g_free(destPath_other);
    }

    GError *err = NULL;
    int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, m_cancellable, signal_callback_progress, NULL, &err);

    if(dest_other != NULL) {
      int ret_other = g_file_copy(src, dest_other, G_FILE_COPY_OVERWRITE, m_cancellable, signal_callback_progress, NULL, &err);
      g_object_unref(dest_other);

      if(!ret_other) {
        PRINTF("g_file_copy error: %s\n", err->message);

        switch(err->code) {
        case G_IO_ERROR_NO_SPACE:
          sprintf(result, _("Failed to load data from USB storage!\nError: No space left on storage."));
          break;
        case G_IO_ERROR_CANCELLED:
          sprintf(result, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
          break;
        default:
          sprintf(result, _("Failed to load data from USB storage!"));
          break;
        }

        cond = -1;
        g_error_free(err);
        break;
      }
    }

    g_object_unref(src);
    g_object_unref(dest);

    if(!ret) {
      PRINTF("g_file_copy error: %s\n", err->message);

      switch(err->code) {
      case G_IO_ERROR_NO_SPACE:
        sprintf(result, _("Failed to load data from USB storage!\nError: No space left on storage."));
        break;
      case G_IO_ERROR_CANCELLED:
        sprintf(result, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
        break;
      default:
        sprintf(result, _("Failed to load data from USB storage!"));
        break;
      }

      cond = -1;
      g_error_free(err);

      break;
    }

    ViewSystem::GetInstance()->CreateDefineItemFormUsbToHost(name);

    ite++;
  }

  ptr->UmountUsbStorage();
  MessageDialog::GetInstance()->Destroy();

  // Handle result
  if(!cond) {
    sprintf(result, _("Success to load data from USB storage."));

    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, result, NULL);

    ViewSystem::GetInstance()->UpdateUserItem();
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, result, NULL);
  }
}

void ConfigToHost::LoadSelectedDataCalc() {
  UserSelect::GetInstance()->create_import_tmp_dir();

  int cond = 0;
  int count = 1;
  int total = 0;
  char str_info[256],result1[256];
  FileMan fm;
  PeripheralMan *ptr = PeripheralMan::GetInstance();

  vector<string> vec = ConfigToHost::GetInstance()->GetSelectedVec();

  m_cancellable = g_cancellable_new();

  //list all string for test
  vector<string>::iterator ite = vec.begin();
  total = vec.size() / 2;

  while(ite < vec.end() && !cond) {
    //Update info
    gchar *basename = g_path_get_basename((*ite).c_str());

    gchar *destPath;
    gchar *destPath_other = NULL;
    char name[50]= {0};

    destPath = g_build_path(G_DIR_SEPARATOR_S, CALC_TMP_DATA_PATH, basename, NULL);

    // delete the '.ini'
    strcpy(name, basename);
    int i;
    for (i =0; i < 50; i++) {
      if ((name[i] == '.') && (name[i+1] == 'i') && (name[i+2] =='n') && (name[i+3] == 'i')) {
        name[i] = name[i+1] = name[i+2] = name[i+3] = '\0';
        break;
      }
    }

    g_free(basename);

    //  PRINTF("Dest Path: %s\n", destPath);
    GFile *src = g_file_new_for_path((*ite).c_str());
    GFile *dest = g_file_new_for_path(destPath);
    g_free(destPath);
    GFile *dest_other = NULL;
    if(destPath_other != NULL) {
      dest_other = g_file_new_for_path(destPath_other);
      g_free(destPath_other);
    }

    GError *err = NULL;
    int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, m_cancellable, NULL, NULL, &err);

    if(dest_other != NULL) {
      int ret_other = g_file_copy(src, dest_other, G_FILE_COPY_OVERWRITE, m_cancellable, signal_callback_progress, NULL, &err);
      g_object_unref(dest_other);

      if(!ret_other) {
        PRINTF("g_file_copy error: %s\n", err->message);

        switch(err->code) {
        case G_IO_ERROR_NO_SPACE:
          sprintf(result1, _("Failed to load data from USB storage!\nError: No space left on storage."));
          break;
        case G_IO_ERROR_CANCELLED:
          sprintf(result1, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
          break;
        default:
          sprintf(result1, _("Failed to load data from USB storage!"));
          break;
        }

        cond = -1;
        g_error_free(err);

        break;
      }
    }

    g_object_unref(src);
    g_object_unref(dest);

    if(!ret) {
      PRINTF("g_file_copy error: %s\n", err->message);

      switch(err->code) {
      case G_IO_ERROR_NO_SPACE:
        sprintf(result1, _("Failed to load data from USB storage!\nError: No space left on storage."));
        break;
      case G_IO_ERROR_CANCELLED:
        sprintf(result1, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
        break;
      default:
        sprintf(result1, _("Failed to load data from USB storage!"));
        break;
      }

      cond = -1;
      g_error_free(err);

      break;
    }

    ite++;
  }

  if(!cond) {
    DeleteUdiskFile();
    vecAdd.clear();

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

    IniFile ini_add(path2);
    IniFile *ptrIni_add = &ini_add;

    vector<string> itemgroup_name;
    itemgroup_name = ptrIni_add->GetGroupName();
    string itemgroup = itemgroup_name[0];
    int num_add;
    num_add = ptrIni_add->ReadInt(itemgroup.c_str(), "Number");

    if(num_add != 0) {
      for(int i=1; i<=num_add; i++) {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni_add->ReadInt(itemgroup.c_str(), CalcNumber);

        if(item_num < (USER_START - BASIC_MEA_END)) {
          vecAdd.push_back(item_num);
        } else {
          bool rename_no = true;
          IniFile ini_add1(path3);
          IniFile *ptrIni_add1 = &ini_add1;
          char CustomEtype[256];
          sprintf(CustomEtype, "CustomEtype-%d",item_num);
          string item_name = ptrIni_add1->ReadString(CustomEtype, "Name");
          int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
          int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
          string department_in = ptrIni_add1->ReadString(CustomEtype, "Department");

          IniFile ini_add4(userselectname1);
          IniFile *ptrIni_add4 = &ini_add4;
          vector<string> useritemgroup;
          useritemgroup = ptrIni_add4->GetGroupName();
          char src_group[256];
          int group_length(0);
          group_length = useritemgroup.size();

          for (int i = 0 ; i <  group_length; i++) {
            sprintf(src_group ,"%s", useritemgroup[i].c_str());
            int number1;
            number1 = ptrIni_add4->ReadInt(src_group, "Number");

            for(int i=1; i<=number1; i++) {
              char CalcNumber[256];
              sprintf(CalcNumber, "Calc%d", i);
              item_num_exist = ptrIni_add4->ReadInt(src_group, CalcNumber);
              string item_name1;

              if(item_num_exist < (USER_START - BASIC_MEA_END))
                item_name1= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num_exist);
              else {
                item_name1 = CalcSetting::GetInstance()->CustomItemTransName(item_num_exist);
              }

              if(strcmp(item_name1.c_str(), item_name.c_str()) == 0) {
                rename_no = false;
                char rename[256];
                char renametrans[256];
                strcpy(renametrans,  _("Item has existed, \nClicked Import will rename the item and continue, \nClicked Cancel will quit and continue!"));
                sprintf(rename , "%s %s", item_name.c_str(), renametrans);

                GtkDialog* dialog = Utils::create_dialog(GTK_WINDOW(m_dialog), _("Rename"));

                Utils::add_dialog_button(dialog, _("OK"), GTK_RESPONSE_OK, GTK_STOCK_OK);
                Utils::add_dialog_button(dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

                gtk_dialog_set_default_response(dialog, GTK_RESPONSE_OK);

                GtkLabel* notice = Utils::create_label(rename);
                gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(notice));
                gtk_widget_show_all(GTK_WIDGET(dialog));

                gint result = gtk_dialog_run(dialog);

                switch (result) {
                case GTK_RESPONSE_OK:
                  CustomCalc::GetInstance()->ButtonImportNameOK();
                  break;
                case GTK_RESPONSE_CANCEL:
                default:
                  break;
                }

                gtk_widget_destroy(GTK_WIDGET(dialog));
              }
            }
          }

          if(rename_no) {
            CustomCalc::GetInstance()->ImportWrite(item_name, item_num);
          }
        }
      }
    }

    CustomCalc::GetInstance()->ImportSuccess();
  } else {
    ExportErrorInfoNotice(result1);
    ClickedOKAndCancel();
  }
}

void ConfigToHost::CallbackProgress(goffset current, goffset total) {
  if(g_cancellable_is_cancelled(m_cancellable)) {
    return;
  }

  double prac = (double)current/total;

  //  PRINTF("prac = %f\n", prac);
  if(prac >= 0 && prac <= 1.0) {
    gdk_threads_enter();

    MessageDialog::GetInstance()->SetProgressBar(prac);

    while(gtk_events_pending()) {
      gtk_main_iteration();
    }

    gdk_threads_leave();
  } else {
    PRINTF("fraction out of range!\n");
  }
}

void ConfigToHost::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  if (keyValue == KEY_ESC) {
    ButtonClickedCancel(NULL);
  }
}

// type: 0=root, 1=branch
GtkTreeView* ConfigToHost::CreateTreeview(gint type) {
  GtkTreeView* treeview = Utils::create_tree_view();

  g_object_set(G_OBJECT(treeview),
    "enable-search", FALSE,
    "headers-visible", FALSE,
    "rules-hint", TRUE,
    NULL);

  GtkCellRenderer* render = gtk_cell_renderer_toggle_new();
  g_signal_connect(G_OBJECT(render), "toggled", G_CALLBACK(signal_callback_toggled), this);

  GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes("", render, "active", COL_CHECKED, NULL);
  gtk_tree_view_append_column(treeview, col);
  g_object_set_data(G_OBJECT(render), "type", GINT_TO_POINTER(type));

  render = gtk_cell_renderer_text_new();
  col = gtk_tree_view_column_new_with_attributes(_("Name"), render, "text", COL_NAME, NULL);
  gtk_tree_view_append_column(treeview, col);

  return treeview;
}

GtkTreeModel* ConfigToHost::CreateRootModel() {
  GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);

  return GTK_TREE_MODEL(store);
}

GtkTreeView* ConfigToHost::CreateTreeviewCalc() {
  GtkTreeView* treeview = Utils::create_tree_view();

  g_object_set(G_OBJECT(treeview),
    "enable-search", FALSE,
    "headers-visible", FALSE,
    "rules-hint", TRUE,
    NULL);

  GtkCellRenderer* render = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes(_("Name"), render, "text", 0, NULL);
  gtk_tree_view_append_column(treeview, col);

  return treeview;
}

GtkTreeModel* ConfigToHost::CreateRootModelCalc() {
  GtkListStore* store = gtk_list_store_new(1, G_TYPE_STRING);

  return GTK_TREE_MODEL(store);
}

void ConfigToHost::HideClickedOKAndCancel() {
  if (m_dialog != NULL) {
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_OK), FALSE);
    gtk_widget_set_sensitive(gtk_dialog_get_widget_for_response(m_dialog, GTK_RESPONSE_CANCEL), FALSE);
  }
}

void ConfigToHost::ExportErrorInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_STOP, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

void ConfigToHost::ExportLoadInfoNotice(string result) {
  if (m_label_notice != NULL) {
    gtk_label_set_text(m_label_notice, result.c_str());
    gtk_widget_show(GTK_WIDGET(m_label_notice));
  }

  if (m_image != NULL) {
    gtk_image_set_from_stock(m_image, GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(m_image));
  }
}

void ConfigToHost::DeleteUdiskFile() {
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_check_button_del))) {
    FileMan f;
    char path4[256];
    sprintf(path4, "%s%s%s%s", UDISK_DATA_PATH,"/", m_rootName.c_str(), "/");
    PRINTF("PATH4=%s\n", path4);
    f.DelDirectory(path4);
  }
}

void ConfigToHost::UpdateRootModel() {
  DIR *dir;
  struct dirent *ent;

  GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(m_treeview_root));
  GtkTreeIter iter;

  dir = opendir(UDISK_DATA_PATH);

  if(!dir) {
    perror("open udisk path error");
    return;
  }

  while((ent = readdir(dir)) != NULL) {
    if(ent->d_name[0]=='.') {
      continue;
    }

    // if(ent->d_type==DT_DIR)
    gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, ent->d_name, NULL);
    PRINTF("DIR: %s\n", tmpPath);

    if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR)) {
      PRINTF("DIR: %s\n", ent->d_name);
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, COL_CHECKED, FALSE, COL_NAME, ent->d_name, -1);
      GtkTreeModel *model = GTK_TREE_MODEL(LoadBranchModel(ent->d_name));
      m_listBranch = g_list_append(m_listBranch, model);
    }

    g_free(tmpPath);
  }

  closedir(dir);
}

void ConfigToHost::UpdateRootModelCalc() {
  DIR *dir;
  struct dirent *ent;

  GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(m_treeview_root));
  GtkTreeIter iter;

  dir = opendir(UDISK_DATA_PATH);

  if(!dir) {
    perror("open udisk path error");
    return;
  }

  while((ent = readdir(dir)) != NULL) {
    if(ent->d_name[0]=='.') {
      continue;
    }

    // if(ent->d_type==DT_DIR)
    gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, ent->d_name, NULL);
    PRINTF("DIR: %s\n", tmpPath);

    if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR)) {
      PRINTF("DIR: %s\n", ent->d_name);
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, ent->d_name, -1);

      // GtkTreeModel *model = GTK_TREE_MODEL(LoadBranchModel(ent->d_name));
      // m_listBranch = g_list_append(m_listBranch, model);
    }

    g_free(tmpPath);
  }

  closedir(dir);
}

// rows: the row's number in the Root model
void ConfigToHost::UpdateBranchModel(gint rows) {
  GList *list = g_list_first(m_listBranch);
  gtk_tree_view_set_model (m_treeview_branch, GTK_TREE_MODEL(g_list_nth_data(list, rows)));
}

GtkTreeModel* ConfigToHost::LoadBranchModel(gchar* branch) {
  DIR *dir;
  struct dirent *ent;
  gchar* path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, branch, NULL);

  GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);
  gtk_list_store_clear(store);
  GtkTreeIter iter;

  dir = opendir(path);
  if(!dir) {
    perror("open path error");
    return NULL;
  }

  FileMan fm;
  gboolean flag = 0;

  while((ent = readdir(dir)) != NULL) {
    if(ent->d_name[0]=='.') {
      continue;
    }

    // if(ent->d_type==DT_REG)
    gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, path, ent->d_name, NULL);

    if(g_file_test(tmpPath, G_FILE_TEST_IS_REGULAR)) {
      if(fm.CompareSuffix(ent->d_name, "ini")==0) {
        // PRINTF(" FILE: %s\n", ent->d_name);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, COL_CHECKED, FALSE, COL_NAME, ent->d_name, -1);
        flag = 1;
      }
    }

    g_free(tmpPath);
  }

  closedir(dir);
  g_free(path);

  if(flag) {
    return GTK_TREE_MODEL(store);
  } else {
    return NULL;
  }
}

bool ConfigToHost::GetAllSelectPath() {
  m_vecPath.clear();

  gchar *nameRoot, *nameBranch;
  gboolean checkedRoot, checkedBranch;
  gboolean validRoot, validBranch;
  GtkTreeModel *modelRoot, *modelBranch;
  GtkTreeIter iterRoot, iterBranch;
  FileMan fm;

  modelRoot = gtk_tree_view_get_model(m_treeview_root);
  validRoot = gtk_tree_model_get_iter_first(modelRoot, &iterRoot);

  if(g_list_length(m_listBranch) > 0) {
    GList *list = g_list_first(m_listBranch);

    while(list) {
      // PRINTF("New Branch\n");
      if(!validRoot || !list->data) {
        validRoot = gtk_tree_model_iter_next(modelRoot, &iterRoot);
        list = list->next;
        continue;
      }

      gtk_tree_model_get(modelRoot, &iterRoot, COL_CHECKED, &checkedRoot, COL_NAME, &nameRoot, -1);
      //  PRINTF("Root Name=%s\n", nameRoot);

      modelBranch = GTK_TREE_MODEL(list->data);
      validBranch = gtk_tree_model_get_iter_first(modelBranch, &iterBranch);

      //check all iter in branch list
      while(validBranch) {
        gtk_tree_model_get(modelBranch, &iterBranch, COL_CHECKED, &checkedBranch, COL_NAME, &nameBranch, -1);
        // PRINTF("Branch Name=%s, Checked=%d\n", nameBranch, checkedBranch);

        if(checkedBranch) {
          gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, nameRoot, nameBranch, NULL);
          PRINTF("Push to vector: %s\n", path);
          m_vecPath.push_back(path);
          gchar pathIni[255];
          fm.GetIniFilePath(path, pathIni);
          PRINTF("Ini path is: %s\n", pathIni);
          m_vecPath.push_back(pathIni);

          g_free(path);
        }

        validBranch = gtk_tree_model_iter_next(modelBranch, &iterBranch);
        g_free(nameBranch);
      }

      validRoot = gtk_tree_model_iter_next(modelRoot, &iterRoot);
      g_free(nameRoot);
      list = list->next;
    }
  }

  if(!m_vecPath.empty()) {
    return true;
  } else {
    return false;
  }
}

bool ConfigToHost::GetAllSelectPathCalc() {
  m_vecPath.clear();

  PRINTF("Root Name=%s\n", m_rootName.c_str());
  char path[256];
  char path1[256];

  #ifdef VET
    sprintf(path, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_rootName.c_str(), "/VetCalcSetting.ini");
    sprintf(path1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_rootName.c_str(), "/VetCalcItemSetting.ini");
  #else
    sprintf(path, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_rootName.c_str(), "/CalcSetting.ini");
    sprintf(path1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", m_rootName.c_str(), "/CalcItemSetting.ini");
  #endif

  m_vecPath.push_back(path);
  m_vecPath.push_back(path1);

  if(!m_vecPath.empty()) {
    return true;
  } else {
    return false;
  }
}

void ConfigToHost::SetAllToggleValue(GtkTreeModel* model, gboolean value) {
  gboolean valid;
  GtkTreeIter iter;

  if(!model) {
    return;
  }

  valid = gtk_tree_model_get_iter_first(model, &iter);

  while(valid) {
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED, value, -1);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}

void ConfigToHost::SetProgressBar(double fraction) {
   if (m_progress_bar != NULL) {
    gtk_progress_bar_set_fraction(m_progress_bar, fraction);

    stringstream ss;
    ss << fraction * 100 << "%";

    gtk_progress_bar_set_text(m_progress_bar, ss.str().c_str());
    gtk_widget_show(GTK_WIDGET(m_progress_bar));
  }
}
