#ifndef __USER_SELECT_H__
#define __USER_SELECT_H__

#include "utils/Utils.h"

#define USERNAME_DB "./res/username.db"

class UserSelect {
public:
  static UserSelect* GetInstance();

public:
  ~UserSelect();

  void create_usercalcdefined_dir();
  void create_commentdefined_dir();
  void create_userdefined_dir();
  void create_import_tmp_dir();
  void create_udisk_data_dir();
  void create_userconfig_dir();
  void create_exportUSB_dir(string name);

  bool creat_username_db(string db_dbname);
  bool read_username_db(string db_dbname, GtkWidget* widget);
  bool insert_username_db(string db_dbname, string record);
  bool delete_username_db(string db_dbname, string record);
  bool username_unique_db(string db_dbname, string record);

  int get_active_user();
  int compare_with_current_user(string username);

  void read_default_username(GtkWidget* widget);
  void set_active_user(GtkWidget* widget, gint num);
  void write_username(GtkWidget* widget, string db_dbname, string record);
  void save_active_user_id(int num);
  void save_cur_username(string username);

private:
  UserSelect();

private:
  static UserSelect* m_instance;

private:
  int active_user_id;
  string cur_username;
};

#endif
