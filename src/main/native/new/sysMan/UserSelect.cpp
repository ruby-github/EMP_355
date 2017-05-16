#include "sysMan/UserSelect.h"

#include <sqlite3.h>

#include "Def.h"
#include "patient/FileMan.h"
#include "probe/ExamItem.h"

string listname[] = {};

UserSelect* UserSelect::m_instance = NULL;

// ---------------------------------------------------------

UserSelect* UserSelect:: GetInstance() {
  if (m_instance == NULL) {
    m_instance = new UserSelect();
  }

  return m_instance;
}

UserSelect::UserSelect() {
  active_user_id = 0;
  cur_username = "";
}

UserSelect::~UserSelect() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void UserSelect::create_usercalcdefined_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s%s", CFG_RES_PATH, CALC_ITEM_FILE);
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      // printf("g_file_make_directory error: %s\n", err_mkdir->message);
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_commentdefined_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s%s", CFG_RES_PATH, COMMENT_PATH);
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
      // printf("g_file_make_directory error: %s\n", err_mkdir->message);
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_userdefined_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s%s", CFG_RES_PATH, EXAM_ITEM_FILE);
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
      // printf("g_file_make_directory error: %s\n", err_mkdir->message);
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_import_tmp_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s", CALC_TMP_DATA_PATH );
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_udisk_data_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s", UDISK_DATA_PATH );
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_userconfig_dir() {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s", USERCONFIG_PATH );
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

void UserSelect::create_exportUSB_dir(string name) {
  // make directory if not exist
  char dirpath[256];
  sprintf(dirpath, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", name.c_str());
  GFile* dir = g_file_new_for_path(dirpath);
  GError* err_mkdir = NULL;

  if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir)) {
    if(err_mkdir->code!=G_IO_ERROR_EXISTS) {
      g_error_free(err_mkdir);
      g_object_unref(dir);
    }
  }
}

bool UserSelect::creat_username_db(string db_dbname) {
  if(access(db_dbname.c_str(), F_OK)) {
    sqlite3* db = 0;
    int max_count = sizeof(listname)/sizeof(listname[0]);

    if (sqlite3_open(db_dbname.c_str(), &db) != SQLITE_OK) {
      PRINTF("Open Database Error!\n");

      return false;
    }

    char* errmsg = NULL;
    char buffer[1024*10], buf[1024];
    memset(buffer, 0, sizeof(buffer));
    //create table
    strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE userlist(username);\nCREATE TABLE userlist_cp(username);\n");

    for(int i = 0; i < max_count; i++) {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "INSERT INTO \"userlist\" VALUES('%s');\n", listname[i].c_str());
      strcat(buffer, buf);
    }

    strcat(buffer, "COMMIT;\n");
    if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK) {
      PRINTF("Init Database Error: %s\n", sqlite3_errmsg(db));
      sqlite3_free(errmsg);

      return false;
    }

    sqlite3_close(db);
  }

  return true;
}

bool UserSelect::read_username_db(string db_dbname, GtkWidget* widget) {
  char buf[1024];
  sqlite3* db = 0;
  int t = 0;

  if (sqlite3_open(db_dbname.c_str(), &db) != SQLITE_OK) {
    PRINTF("Open Database Error!\n");
    return false;
  }

  sqlite3_stmt* stmt_f1 = NULL;
  sprintf(buf, "SELECT DISTINCT username FROM userlist;");

  if(sqlite3_prepare(db, buf, strlen(buf), &stmt_f1, 0) != SQLITE_OK) {
    PRINTF("DISTINCT :%s\n", sqlite3_errmsg(db));
    return false;
  }

  int count = 0;
  while(sqlite3_step(stmt_f1) != SQLITE_DONE) {
    string name = (char*)sqlite3_column_text(stmt_f1, 0);
    PRINTF("name:%s\n", name.c_str());
    if (name.empty()) {
      continue;
    }

    gtk_combo_box_append_text(GTK_COMBO_BOX(widget), name.c_str());
    count++;

    if (name == cur_username) {
      save_active_user_id(count);
    }
  }

  sqlite3_finalize(stmt_f1);
  sqlite3_close(db);

  return true;
}

bool UserSelect::insert_username_db(string db_dbname, string record) {
  sqlite3* db = 0;
  char* errmsg = NULL;
  char buf[1024];

  if (sqlite3_open(db_dbname.c_str(), &db) != SQLITE_OK) {
    PRINTF("Open Database Error!\n");
    return false;
  }

  memset(buf, 0, sizeof(buf));
  sprintf(buf, "INSERT INTO \"userlist\" VALUES('%s');\n", record.c_str());

  if (sqlite3_exec(db, buf, 0, 0, &errmsg) != SQLITE_OK) {
    PRINTF("insert Database Error: %s\n", sqlite3_errmsg(db));
    sqlite3_free(errmsg);
    return false;
  }

  sqlite3_close(db);

  return true;
}

bool UserSelect::delete_username_db(string db_dbname, string record) {
  sqlite3* db = 0;
  char* errmsg = NULL;
  char buf[1024];

  if (sqlite3_open(db_dbname.c_str(), &db) != SQLITE_OK) {
    PRINTF("Open Database Error!\n");

    return false;
  }

  memset(buf, 0, sizeof(buf));
  sprintf(buf, "DELETE FROM userlist WHERE username = '%s';\n", record.c_str());

  if (sqlite3_exec(db, buf, 0, 0, &errmsg) != SQLITE_OK) {
    PRINTF("delete Database Error: %s\n", sqlite3_errmsg(db));
    sqlite3_free(errmsg);

    return false;
  }

  sqlite3_close(db);

  return true;
}

bool UserSelect::username_unique_db(string db_dbname, string record) {
  bool result = false;
  sqlite3_stmt* stmt = NULL;
  char buf[1024];
  sqlite3* db = 0;

  if (sqlite3_open(db_dbname.c_str(), &db) != SQLITE_OK) {
    PRINTF("Open Database Error!\n");

    return false;
  }

  sprintf(buf, "SELECT COUNT(*) AS num_result FROM userlist WHERE username LIKE '%s';\n", record.c_str());
  if (sqlite3_prepare(db, buf, strlen(buf), &stmt, 0) != SQLITE_OK) {
    PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(db));

    return false;
  }

  while (sqlite3_step(stmt) != SQLITE_DONE) {
    if (strcmp(sqlite3_column_name(stmt, 0), "num_result") == 0) {
      result = sqlite3_column_int(stmt, 0);
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  if (result) {
    return false;
  } else {
    return true;
  }
}

int UserSelect::get_active_user() {
  return active_user_id;
}

int UserSelect::compare_with_current_user(string username) {
  return cur_username == username;
}

void UserSelect::read_default_username(GtkWidget* widget) {
  gtk_combo_box_append_text(GTK_COMBO_BOX (widget), _("System Default"));
}

void UserSelect::set_active_user(GtkWidget* widget, gint num) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(widget), num);
}

void UserSelect::write_username(GtkWidget* widget, string db_dbname, string record) {
  if (username_unique_db(db_dbname, record)) {
    gtk_combo_box_append_text(GTK_COMBO_BOX (widget), record.c_str());
    insert_username_db(db_dbname, record);
    char path[256];
    sprintf(path, "%s%s%s", USERCONFIG_PATH, record.c_str(), ".ini");
    FileMan f;
    char srcpath[50]= {0};

    #ifdef VET
      sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "VetDefaultItemPara.ini");
    #else
      sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "DefaultItemPara.ini");
    #endif

    // f.CopyFile("./res/config/ItemPara.ini", path);
    f.CopyFile(srcpath, path);

    memset(user_configure, 0, USERCONFIG_LEN);
    sprintf(user_configure, "%s%s%s", "userconfig/", record.c_str(), ".ini");
  }
}

void UserSelect::save_active_user_id(int num) {
  active_user_id = num;
}

void UserSelect::save_cur_username(string username) {
  cur_username = username;
}
