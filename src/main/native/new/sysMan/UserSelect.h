#ifndef USERSELECT
#define USERSELECT

#define USERNAME_DB "./res/username.db"

typedef struct NameList {
    char name[50];
    struct NameList *next;
} namelist, *nmlst;

typedef struct node {
    int active_user_id;
    char active_username[50];
} Active_user;

class UserSelect {
public:
    ~UserSelect();
    static UserSelect* GetInstance();
    void create_usercalcdefined_dir(void);
    void create_commentdefined_dir(void);
    void create_userdefined_dir(void);

    void create_exportUSB_dir(const char *tmp_name);
    void create_import_tmp_dir(void);
    void create_udisk_data_dir(void);
    void create_userconfig_dir(void);
    void read_default_username(GtkWidget* widget);
    void save_active_user_id(int num);
    void save_cur_username(char *username);
    void set_active_user(GtkWidget * widget, gint num);
    int get_active_user(void);
    int compare_with_current_user(char *username);
    void write_username(GtkWidget* widget, const char *db_dbname, const char *record);
    bool creat_username_db(const char *db_dbname);
    bool read_username_db(const char *db_dbname, GtkWidget* widget);
    bool insert_username_db(const char *db_dbname, const char *record);
    bool delete_username_db(const char *db_dbname, const char *record);
    bool username_unique_db(const char *db_dbname, const char *record);
private:
    UserSelect();
    static UserSelect *m_ptrInstance;
    int active_user_id;
    char cur_username[50];
};

#endif
