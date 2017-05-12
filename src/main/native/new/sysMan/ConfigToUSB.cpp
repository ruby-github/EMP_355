#include "sysMan/ConfigToUSB.h"

#include "utils/FakeXUtils.h"
#include "utils/MessageDialog.h"

#include "keyboard/KeyDef.h"
#include "keyboard/KeyValueOpr.h"
#include "patient/FileMan.h"
#include "periDevice/PeripheralMan.h"

enum {
  COL_CHECKED,
  COL_NAME,
  NUM_COLS
};

ConfigToUSB* ConfigToUSB::m_instance = NULL;
GCancellable* ConfigToUSB::m_cancellable = NULL;

// ---------------------------------------------------------

ConfigToUSB* ConfigToUSB::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ConfigToUSB();
  }

  return m_instance;
}

ConfigToUSB::ConfigToUSB() {
  m_parent = NULL;
  m_dialog = NULL;
  m_treeview_root = NULL;
  m_treeview_branch = NULL;

  m_listBranch = NULL;
}

ConfigToUSB::~ConfigToUSB() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ConfigToUSB::CreateWindow(GtkWidget* parent) {
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

// ---------------------------------------------------------

void ConfigToUSB::ButtonClickedOK(GtkButton* button) {
  // Copy the selected file to SLIDE_PATH
  if(GetAllSelectPath()) {
    g_timeout_add(1000, signal_callback_load_selected_data, this);

    // PRINTF("Load From U disk!\n");
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_PROGRESS_CANCEL,
      _("Please wait, sending data to USB storage..."),
      signal_callback_cancelloadusb);
  }

  DestroyWindow();
}

void ConfigToUSB::ButtonClickedCancel(GtkButton* button) {
  DestroyWindow();
}

void ConfigToUSB::RootSelectionChanged(GtkTreeSelection *selection) {
  GtkTreeIter iter;
  GtkTreeModel* model;
  gchar* text;

  if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, COL_NAME, &text, -1);
    GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
    //  PRINTF("Selection path: %s\n", gtk_tree_path_to_string(path));
    UpdateBranchModel(atoi(gtk_tree_path_to_string(path)));
    gtk_tree_path_free (path);
    g_free(text);
  }
}

void ConfigToUSB::LoadSelectedData() {
  int cond = 0;
  int count = 1;
  int total = 0;
  char str_info[256], result[256];
  FileMan fm;
  PeripheralMan *ptr = PeripheralMan::GetInstance();

  vector<string> vec = GetSelectedVec();

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

  // list all string for test
  vector<string>::iterator ite = vec.begin();
  total = vec.size() / 2; // 1 image = 1 pic + 1 ini

  while(ite < vec.end() && !cond) {
    PRINTF("Send file: %s\n", (*ite).c_str());
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

    //Update info
    gchar *basename = g_path_get_basename((*ite).c_str());
    if(fm.CompareSuffix(basename, "ini") != 0) {
      sprintf(str_info, "%s %s   %d/%d\n%s", _("Loading..."), basename, count, total, _("Please wait..."));
      MessageDialog::GetInstance()->SetText(str_info);
      MessageDialog::GetInstance()->SetProgressBar(0);
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

  ptr->UmountUsbStorage();
  MessageDialog::GetInstance()->Destroy();

  // Handle result
  if(!cond) {
    sprintf(result, _("Success to export to USB storage."));
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, result, NULL);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, result, NULL);
  }
}

void ConfigToUSB::CallbackProgress(goffset current, goffset total) {
  if(g_cancellable_is_cancelled(m_cancellable)) {
    return;
  }

  double prac = (double)current/total;

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

void ConfigToUSB::ToggleData(GtkCellRendererToggle* cell, gchar* path_str) {
  GtkTreeView *treeview;
  gint type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "type"));

  if(type == 0) {
    treeview = m_treeview_root;
  } else if(type == 1) {
    treeview = m_treeview_branch;
  } else {
    return;
  }

  // PRINTF("Toggle path: %s\n", path_str);
  GtkTreeModel* model = gtk_tree_view_get_model(treeview);
  GtkTreeIter iter;
  GtkTreePath* path = gtk_tree_path_new_from_string (path_str);
  gboolean checked;

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, COL_CHECKED, &checked, -1);

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
      GtkTreeSelection *s = gtk_tree_view_get_selection(m_treeview_root);

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
        GtkTreeSelection *s = gtk_tree_view_get_selection(m_treeview_root);

        if(gtk_tree_selection_get_selected(s, &m, &i)) {
          gtk_list_store_set(GTK_LIST_STORE(m), &i, COL_CHECKED, TRUE, -1);
        }
      }
    }
  }

  /* clean up */
  gtk_tree_path_free (path);
}

void ConfigToUSB::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  if (keyValue == KEY_ESC) {
    ButtonClickedCancel(NULL);
  }
}

void ConfigToUSB::DestroyWindow() {
  if(m_listBranch != NULL && g_list_length(m_listBranch) > 0) {
    // clean the data in list
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
}

// type: 0=root, 1=branch
GtkTreeView* ConfigToUSB::CreateTreeview(gint type) {
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

GtkTreeModel* ConfigToUSB::CreateRootModel() {
  GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);

  return GTK_TREE_MODEL (store);
}

void ConfigToUSB::UpdateRootModel() {
  DIR *dir;
  struct dirent *ent;

  GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(m_treeview_root));
  GtkTreeIter iter;

  dir = opendir(USERCONFIG_PARENT_PATH);
  if(!dir) {
    perror("open path error");
    return;
  }

  while((ent = readdir(dir)) != NULL) {
    PRINTF("DIR: %s\n", ent->d_name);
    if(ent->d_name[0]=='.')
        continue;
    // if(ent->d_type==DT_DIR)
    gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, ent->d_name, NULL);
    //export file of default dir or  userconfig dir

    if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR) && ((strcmp(ent->d_name, "userconfig") == 0) || (strcmp(ent->d_name, "default") == 0))) {
      // PRINTF("DIR: %s\n", ent->d_name);
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
         COL_CHECKED, FALSE,
         COL_NAME, ent->d_name,
         -1);
      GtkTreeModel *model = GTK_TREE_MODEL(LoadBranchModel(ent->d_name));
      m_listBranch = g_list_append(m_listBranch, model);
    }

    g_free(tmpPath);
  }

  closedir(dir);
}

// rows: the row's number in the Root model
void ConfigToUSB::UpdateBranchModel(gint rows) {
  if (m_listBranch != NULL) {
    GList *list = g_list_first(m_listBranch);
    gtk_tree_view_set_model (m_treeview_branch, GTK_TREE_MODEL(g_list_nth_data(list, rows)));
  }
}

GtkTreeModel* ConfigToUSB::LoadBranchModel(gchar* branch) {
  DIR *dir;
  struct dirent* ent;
  gchar *path = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, branch, NULL);

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
      if (fm.CompareSuffix(ent->d_name, "ini")==0) {
        // PRINTF("FILE: %s\n", ent->d_name);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
           COL_CHECKED, FALSE,
           COL_NAME, ent->d_name,
           -1);
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

bool ConfigToUSB::GetAllSelectPath() {
  m_vecPath.clear();

  gchar *nameRoot, *nameBranch;
  gboolean checkedRoot, checkedBranch;
  gboolean validRoot, validBranch;
  GtkTreeModel *modelRoot, *modelBranch;
  GtkTreeIter iterRoot, iterBranch;
  FileMan fm;

  modelRoot = gtk_tree_view_get_model(m_treeview_root);
  validRoot = gtk_tree_model_get_iter_first(modelRoot, &iterRoot);

  if(m_listBranch != NULL && g_list_length(m_listBranch) > 0) {
    GList *list = g_list_first(m_listBranch);

    while(list) {
      // PRINTF("New Branch\n");
      if(!validRoot || !list->data) {
        validRoot = gtk_tree_model_iter_next(modelRoot, &iterRoot);
        list = list->next;
        continue;
      }

      gtk_tree_model_get(modelRoot, &iterRoot, COL_CHECKED, &checkedRoot, COL_NAME, &nameRoot, -1);
      // PRINTF("Root Name=%s\n", nameRoot);

      modelBranch = GTK_TREE_MODEL(list->data);
      validBranch = gtk_tree_model_get_iter_first(modelBranch, &iterBranch);

      //check all iter in branch list
      while(validBranch) {
        gtk_tree_model_get(modelBranch, &iterBranch, COL_CHECKED, &checkedBranch, COL_NAME, &nameBranch, -1);
        //PRINTF("Branch Name=%s, Checked=%d\n", nameBranch, checkedBranch);

        if(checkedBranch) {
          gchar *path = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, nameRoot, nameBranch, NULL);
          // PRINTF("Push to vector: %s\n", path);
          m_vecPath.push_back(path);
          gchar pathIni[255];
          fm.GetIniFilePath(path, pathIni);
          // PRINTF("Ini path is: %s\n", pathIni);
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

void ConfigToUSB::SetAllToggleValue(GtkTreeModel* model, gboolean value) {
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
