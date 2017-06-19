#include "sysMan/ViewSystem.h"

#include <cmath>
#include <errno.h>
#include <sys/wait.h>

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"
#include "utils/MessageDialog.h"
#include "utils/StringUtils.h"

#include "calcPeople/MeaCalcFun.h"
#include "calcPeople/MenuCalcNew.h"
#include "calcPeople/ViewReport.h"
#include "display/KnobMenu.h"
#include "display/MenuArea.h"
#include "display/TopArea.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcCfm.h"
#include "imageProc/ImgProcPw.h"
#include "keyboard/KeyDef.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/MultiFuncFactory.h"
#include "measure/MenuMeasure.h"
#include "patient/ImgMan.h"
#include "patient/VideoMan.h"
#include "patient/ViewNewPat.h"
#include "periDevice/DCMRegister.h"
#include "periDevice/PeripheralMan.h"
#include "periDevice/Printer.h"
#include "periDevice/ViewPrinterAdd.h"
#include "sysMan/CalcSetting.h"
#include "sysMan/ConfigToUSB.h"
#include "sysMan/DicomLocalSetting.h"
#include "sysMan/DicomServerSetting.h"
#include "sysMan/DicomServiceSetting.h"
#include "sysMan/MeasureSetting.h"
#include "sysMan/ScreenSaver.h"
#include "sysMan/SysCalculateSetting.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysMeasurementSetting.h"
#include "sysMan/SysNoteSetting.h"
#include "sysMan/SysOptions.h"
#include "sysMan/SysUserDefinedKey.h"
#include "sysMan/UpgradeMan.h"
#include "sysMan/UserSelect.h"
#include "ViewMain.h"

#include <EmpAuthorization.h>

#include "display/gui_func.h"

#define DEFALT_SCREEN_WIDTH   950
#define GRAY_WIDTH            20
#define TREE_WINDOW_WIDTH     260
#define CHECK_VALUE           0x74
#define UNCHECK_VALUE         0x66
#define OTHERS_M              TCD_M
#define SHOW_WINDOW_WIDTH     ((DEFALT_SCREEN_WIDTH - 40) - TREE_WINDOW_WIDTH - 3 * GRAY_WIDTH)

#define CustomReport_PATH     "./res/CustomReport.db"
#define CustomReport_PATH_BAK "./res/bak_CustomReport.db"

extern int g_cer_custom[];
extern int g_hl_custom[];
extern int g_bpd_custom[];
extern int g_fl_custom[];
extern int g_crl_custom[];
extern int g_gs_custom[];
extern int g_ac_custom[];
extern int g_hc_custom[];
extern int g_tad_custom[];
extern int g_apad_custom[];
extern int g_thd_custom[];
extern int g_ofd_custom[];
extern int g_fta_custom[];

enum {
  COL_GW,
  COL_GW1,
  COL_GW2,
  COL_GW3,
  COL_GW4,
  COL_GW5,
  COL_GW6,
  COL_GW7,
  COL_GW8,
  COL_GW9,
  COL_GW10,
  NUM_COLUMNS
};

enum {
  PRT_BRAND,
  PRT_MODEL,
  CNUM_COLS
};

enum {
  PRT_DEFAULT,
  PRT_NAME,
  PRT_STATUS,
  PRT_MESSAGE,
  SNUM_COLS
};

enum {
  NAME_COLUMN,
  INDEX_COLUMN,
  N_COLUMNS
};

int _system_(const string cmd) {
  if (cmd.empty()) {
    return -1;
  }

  pid_t pid = vfork();

  int status = 0;

  if (pid < 0) {
    perror("vfork error");

    status = -1;
  } else if (pid == 0) {
    if (execl("/bin/sh", "sh", "-c", cmd.c_str(), 0) == -1) {
      perror((string("execl error-") + strerror(errno)).c_str());

      _exit(127);
    }
  } else {
    while (waitpid(pid, &status, 0) < 0) {
      if (errno != EINTR) {
        status = -1;
        break;
      }
    }
  }

  return status;
}

// ---------------------------------------------------------
//
//  ViewCustomOB
//
// ---------------------------------------------------------

ViewCustomOB* ViewCustomOB::m_instance = NULL;

// ---------------------------------------------------------

ViewCustomOB* ViewCustomOB::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewCustomOB();
  }

  return m_instance;
}

ViewCustomOB::ViewCustomOB() {
  m_dialog = NULL;
  m_combobox_text = NULL;

  m_modelCustomOB = NULL;
  m_treeCustomOB = NULL;

  m_arrayGW = NULL;
}

ViewCustomOB::~ViewCustomOB() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewCustomOB::CreateWindow(GtkWidget* parent) {
  m_dialog = Utils::create_dialog(GTK_WINDOW(parent), _("OB Custom"), 800, 600);

  GtkButton* button_save = Utils::add_dialog_button(m_dialog, _("Save"), GTK_RESPONSE_OK, GTK_STOCK_SAVE);
  GtkButton* button_clear = Utils::add_dialog_button(m_dialog, _("Clear"), GTK_RESPONSE_CANCEL, GTK_STOCK_CLEAR);
  GtkButton* button_exit = Utils::add_dialog_button(m_dialog, _("Exit"), GTK_RESPONSE_CLOSE, GTK_STOCK_QUIT);

  g_signal_connect(button_save, "clicked", G_CALLBACK(signal_button_clicked_save), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_clear), this);
  g_signal_connect(button_exit, "clicked", G_CALLBACK(signal_button_clicked_exit), this);

  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), CreateCustomOB());

  MeaCalcFun::InitCustomTable();
  InitCustomOB();

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ViewCustomOB::DestroyWindow() {
  if(GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();
    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }

    m_dialog = NULL;
    m_combobox_text = NULL;

    m_modelCustomOB = NULL;
    m_treeCustomOB = NULL;

    m_arrayGW = NULL;
  }
}

// ---------------------------------------------------------

void ViewCustomOB::ButtonClickedSave(GtkButton* button) {
  MeaCalcFun::SaveCustomTable();
}

void ViewCustomOB::ButtonClickedClear(GtkButton* button) {
  gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text));
  int* ptr = NULL;

  switch (type) {
  case 0:   // CER
    ptr = g_cer_custom;
    break;
  case 1:   // HL
    ptr = g_hl_custom;
    break;
  case 2:   // BPD
    ptr = g_bpd_custom;
    break;
  case 3:   // FL
    ptr = g_fl_custom;
    break;
  case 4:   // CRL
    ptr = g_crl_custom;
    break;
  case 5:   // GS
    ptr = g_gs_custom;
    break;
  case 6:   // AC
    ptr = g_ac_custom;
    break;
  case 7:   // HC
    ptr = g_hc_custom;
    break;
  case 8:   // TAD
    ptr = g_tad_custom;
    break;
  case 9:   // APAD
    ptr = g_apad_custom;
    break;
  case 10:  // THD
    ptr = g_thd_custom;
    break;
  case 11:  // OFD
    ptr = g_ofd_custom;
    break;
  case 12:  // FTA
    ptr = g_fta_custom;
    break;
  }

  for (int i = 0; i < 272; i++) {
    *ptr = 0;
    ptr++;
  }

  UpdateList(type);
}

void ViewCustomOB::ButtonClickedExit(GtkButton* button) {
  g_keyInterface.Pop();
  gtk_widget_destroy(GTK_WIDGET(m_dialog));

  if (g_keyInterface.Size() == 1) {
    SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
  }
}

void ViewCustomOB::ComboBoxChanged(GtkComboBox* combobox) {
  gint type = gtk_combo_box_get_active(combobox);
  UpdateList(type);
}

void ViewCustomOB::CellEdited(GtkCellRenderer* cell, gchar* path_string, gchar* new_text) {
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);

  GtkTreeIter iter;
  gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text));

  gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
  gtk_tree_model_get_iter(m_modelCustomOB, &iter, path);

  if(!IsNum(new_text) && strcmp(new_text, "")) {
    PRINTF("It's not a num!\n");
    return;
  }

  float new_value = atof(new_text);
  if (new_value < 0.0 || new_value > 500.0) {
    PRINTF("Out of range!\n");
    return;
  }

  gint i = gtk_tree_path_get_indices(path)[0];

  switch(column) {
  case COL_GW1:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw1);
      g_array_index(m_arrayGW, CustomData, i).gw1 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw1, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW2:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw2);
      g_array_index(m_arrayGW, CustomData, i).gw2 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw2, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW3:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw3);
      g_array_index(m_arrayGW, CustomData, i).gw3 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw3, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW4:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw4);
      g_array_index(m_arrayGW, CustomData, i).gw4 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw4, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW5:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw5);
      g_array_index(m_arrayGW, CustomData, i).gw5 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw5, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW6:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw6);
      g_array_index(m_arrayGW, CustomData, i).gw6 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw6, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW7:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw7);
      g_array_index(m_arrayGW, CustomData, i).gw7 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw7, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW8:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw8);
      g_array_index(m_arrayGW, CustomData, i).gw8 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw8, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW9:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw9);
      g_array_index(m_arrayGW, CustomData, i).gw9 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw9, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  case COL_GW10:
    {
      g_free(g_array_index(m_arrayGW, CustomData, i).gw10);
      g_array_index(m_arrayGW, CustomData, i).gw10 = g_strdup(new_text);

      gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
        g_array_index(m_arrayGW, CustomData, i).gw10, -1);
      AddToTable(type, (i*10 + column)-1, new_value);
    }
    break;
  }
}

void ViewCustomOB::CellEditingStarted(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path) {
  if(GTK_IS_ENTRY(editable)) {
    g_signal_connect(G_OBJECT(editable), "insert_text", G_CALLBACK(signal_entry_insert), this);
  }
}

void ViewCustomOB::EntryInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }
}

void ViewCustomOB::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  if (keyValue == KEY_ESC) {
    g_timeout_add(100, signal_callback_exitwindow, this);

    FakeEscKey();
  }
}

GtkWidget* ViewCustomOB::CreateCustomOB() {
  GtkTable* table = Utils::create_table(10, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);

  // GW, Value, ComboBoxText

  GtkLabel* label_gw = Utils::create_label(_("GW:31~300days"));
  GtkLabel* label_value = Utils::create_label(_("Value:0.0~500.0mm (Click to enter a value)"));
  m_combobox_text = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_gw), 0, 1, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_value), 1, 3, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_text), 4, 5, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  string StringOBType[] = {
    {"CER"},  {"HL"},   {"BPD"},  {"FL"},   {"CRL"},
    {"GS"},   {"AC"},   {"HC"},   {"TAD"},  {"APAD"},
    {"THD"},  {"OFD"},  {"FTA"}
  };

  int size = sizeof(StringOBType) / sizeof(StringOBType[0]);

  for (int i = 0; i < size; i++) {
    gtk_combo_box_text_append_text(m_combobox_text, StringOBType[i].c_str());
  }

  g_signal_connect(GTK_OBJECT(m_combobox_text), "changed", G_CALLBACK(signal_combobox_changed), this);

  // scrolled_window

  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window), 0, 5, 1, 10);

  m_modelCustomOB = GTK_TREE_MODEL(gtk_list_store_new(NUM_COLUMNS,
    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
    G_TYPE_STRING)
  );

  m_treeCustomOB = Utils::create_tree_view(m_modelCustomOB);

  gtk_tree_view_set_enable_search (m_treeCustomOB, FALSE);
  gtk_tree_view_set_rules_hint(m_treeCustomOB, TRUE);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(m_treeCustomOB), GTK_SELECTION_SINGLE);

  for (int i = 0; i < NUM_COLUMNS; i++) {
    stringstream ss;

    if (i != COL_GW) {
      ss << i;
    }

    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(i));
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(ss.str().c_str(), renderer, "text", i, NULL);

    if (i == COL_GW) {
      g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);
    } else {
      g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
      g_signal_connect(renderer, "edited", G_CALLBACK(signal_cell_edited), this);
      g_signal_connect(renderer, "editing_started", G_CALLBACK(signal_cell_editing_started), this);

      g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 60, NULL);
    }

    gtk_tree_view_append_column(m_treeCustomOB, column);
  }

  m_arrayGW = g_array_new(TRUE, TRUE, sizeof(CustomData));

  for (int i = 1; i < 28; i++) {
    int k = (i+2)*10+1;

    stringstream ss;
    ss << k << "~" << k + 9;

    m_data.gw = g_strdup(ss.str().c_str());
    m_data.gw1 = NULL;
    m_data.gw2 = NULL;
    m_data.gw3 = NULL;
    m_data.gw4 = NULL;
    m_data.gw5 = NULL;
    m_data.gw6 = NULL;
    m_data.gw7 = NULL;
    m_data.gw8 = NULL;
    m_data.gw9 = NULL;
    m_data.gw10 = NULL;
    g_array_append_vals(m_arrayGW, &m_data, 1);
  }

  gtk_list_store_clear(GTK_LIST_STORE(m_modelCustomOB));

  GtkTreeIter iter;

  for (int i = 0; i < m_arrayGW->len; i++) {
    gtk_list_store_append(GTK_LIST_STORE(m_modelCustomOB), &iter);
    gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter,
      COL_GW, g_array_index(m_arrayGW, CustomData, i).gw,
      COL_GW1, g_array_index(m_arrayGW, CustomData, i).gw1,
      COL_GW2, g_array_index(m_arrayGW, CustomData, i).gw2,
      COL_GW3, g_array_index(m_arrayGW, CustomData, i).gw3,
      COL_GW4, g_array_index(m_arrayGW, CustomData, i).gw4,
      COL_GW5, g_array_index(m_arrayGW, CustomData, i).gw5,
      COL_GW6, g_array_index(m_arrayGW, CustomData, i).gw6,
      COL_GW7, g_array_index(m_arrayGW, CustomData, i).gw7,
      COL_GW8, g_array_index(m_arrayGW, CustomData, i).gw8,
      COL_GW9, g_array_index(m_arrayGW, CustomData, i).gw9,
      COL_GW10, g_array_index(m_arrayGW, CustomData, i).gw10,
      -1);
  }

  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeCustomOB));

  return GTK_WIDGET(table);
}

void ViewCustomOB::InitCustomOB() {
  UpdateList(gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text)));
}

void ViewCustomOB::UpdateList(int type) {
  PRINTF("type = %d\n", type);

  switch (type) {
  case 0:   // CER
    InsertList(m_modelCustomOB, g_cer_custom);
    break;
  case 1:   // HL
    InsertList(m_modelCustomOB, g_hl_custom);
    break;
  case 2:   // BPD
    InsertList(m_modelCustomOB, g_bpd_custom);
    break;
  case 3:   // FL
    InsertList(m_modelCustomOB, g_fl_custom);
    break;
  case 4:   // CRL
    InsertList(m_modelCustomOB, g_crl_custom);
    break;
  case 5:   // GS
    InsertList(m_modelCustomOB, g_gs_custom);
    break;
  case 6:   // AC
    InsertList(m_modelCustomOB, g_ac_custom);
    break;
  case 7:   // HC
    InsertList(m_modelCustomOB, g_hc_custom);
    break;
  case 8:   // TAD
    InsertList(m_modelCustomOB, g_tad_custom);
    break;
  case 9:   // APAD
    InsertList(m_modelCustomOB, g_apad_custom);
    break;
  case 10:  // THD
    InsertList(m_modelCustomOB, g_thd_custom);
    break;
  case 11:  // OFD
    InsertList(m_modelCustomOB, g_ofd_custom);
    break;
  case 12:  // FTA
    InsertList(m_modelCustomOB, g_fta_custom);
    break;
  }
}

void ViewCustomOB::InsertList(GtkTreeModel* model, int table[]) {
  for(int row = 0; row < 27; row++) {
    stringstream ss;
    ss << row;

    GtkTreePath* path = gtk_tree_path_new_from_string(ss.str().c_str());

    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);

    for (int col = 1; col < 11; col++) {
      PRINTF("%d  ", table[10*row + col - 1]);

      char buf[5] = {0};

      sprintf(buf, "%.1f", (float)table[10*row+(col-1)]/10);
      if (strcmp(buf, "0.0") == 0) {
        strcpy(buf, "");
      }

      gtk_list_store_set(GTK_LIST_STORE(model), &iter, col, buf, -1);
    }
  }
  PRINTF("\n");
}

void ViewCustomOB::AddToTable(gint type, int temp1, float temp2) {
  float val = temp2 * 10.0;

  switch(type) {
  case 0:   // CER
    g_cer_custom[temp1] = val;
    break;
  case 1:   // HL
    g_hl_custom[temp1] = val;
    break;
  case 2:   // BPD
    g_bpd_custom[temp1] = val;
    break;
  case 3:   // FL
    g_fl_custom[temp1] = val;
    break;
  case 4:   // CRL
    g_crl_custom[temp1] = val;
    break;
  case 5:   // GS
    g_gs_custom[temp1] = val;
    break;
  case 6:   // AC
    g_ac_custom[temp1] = val;
    break;
  case 7:   // HC
    g_hc_custom[temp1] = val;
    break;
  case 8:   // TAD
    g_tad_custom[temp1] = val;
    break;
  case 9:   // APAD
    g_apad_custom[temp1] = val;
    break;
  case 10:  // THD
    g_thd_custom[temp1] = val;
    break;
  case 11:  // OFD
    g_ofd_custom[temp1] = val;
    break;
  case 12:  // FTA
    g_fta_custom[temp1] = val;
    break;
  }
}

// ---------------------------------------------------------
//
//  ViewSystem
//
// ---------------------------------------------------------

ViewSystem* ViewSystem::m_instance = NULL;

extern bool g_authorizationOn;
extern bool g_review_pic;

string g_user_configure = "ItemPara.ini";
bool g_system_save = false;

sqlite3* CustomReport_db = NULL;
char ShowArr[MAX_CALCARR_COUNT+1] = {0x66};

const string DEFAULT_FLAG = "\u221a";
const string DEFAULT_STR = "System Default";

const string TEMPLET1_STR[] = {
  "Template 1",
  "\u6a21\u677f1",
  "\u0428\u0430\u0431\u043b\u043e\u043d 1",
  "szablon 1",
  "Modššle 1",
  "Templete 1",
  "Template 1"
};

const string CUSTOM_STR[] = {
  "Custom Template",
  "\u81ea\u5B9a\u4e49\u6a21\u677f",
  "\u041f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044c\u0441\u043a\u0438\u0435 \u0448\u0430\u0431\u043b\u043e\u043d\u044b",
  "Custom Template",
  "Modššle auto-dšŠfini",
  "Custom Templete",
  "Custom Template"
};

const string SECTION_STR[][LANG_MAX] = {
  {"Abdomen", "\u6210\u4eba\u8179\u90e8", "\u0410\u0431\u0434\u043e\u043c.", "Jama brzuszna", "Abdomen","Abdomen", "Abdomen"},
  {"Cardiac", "\u5fc3\u810f\u79d1", "\u041a\u0430\u0440\u0434\u0438\u0430\u043b\u044c\u043d\u044b\u0439", "Serce","Cardiaque", "Cardiaco",  "Kardial"},
  {"Urology", "\u6ccc\u5c3f\u79d1", "\u0423\u0440\u043e\u043b\u043e\u0433\u0438\u044f", "Urologia", "Urologie", "Urologico", "Urologie"},
  {"Obstetrics", "\u4ea7\u79d1", "\u0410\u043a\u0443\u0448\u0435\u0440\u0441\u0442\u0432\u043e", "Po\u0142o\u017cnictwo", "Obstétrique","Obstetrico",  "Geburtshilfe"},
  {"Gynecology", "\u5987\u79d1", "\u0413\u0438\u043d\u0435\u043a\u043e\u043b\u043e\u0433\u0438\u044f", "Ginekologia", "Gynécologie", "Gynecologico",  "Gynäkologie"},
  {"Small Part", "\u5c0f\u5668\u5b98", "\u0414\u0435\u0442\u043e\u043b\u044c\u043d\u043e", "Ma\u0142e narz\u0105dy", "Petite partie", "Pequeñas partes",  "Small-Parts"},
  {"Fetal Cardio", "\u80ce\u513f\u5fc3\u810f", "\u041a\u0430\u0440\u0434\u0438\u043e. \u043f\u043b\u043e\u0434\u0430", "Kardio. P\u0142odu", "Cardio f\u0153tal","Cardio Fetal", "fötale Kardio"},
  {"Vascular", "\u8840\u7ba1", "\u0421\u043e\u0441\u0443\u0434\u044b", "Naczynia","Vasculaire","Vascular", "Vaskulär"},
  {"Others", "其他", "Другие", "Inne", "Autres","Otro",  "Andere"}
};

const string KeyFunctionList[] = {
  N_("NULL"),
  N_("TSI"),
  N_("Save Video"),
  N_("Print Screen"),
  N_("Steer"),
  N_("PIP"),
  N_("Spectrum Sound"),
  N_("HPRF"),
  N_("Image Preset"),
  N_("Biopsy")
};

const CommonPrinter COMMON_PRINTER_DATA[] = {
  {"HP",    "LaserJet Series"},
  {"HP",    "DeskJet Series"},
  {"HP",    "New DeskJet Series"},
  {"EPSON", "9-Pin Series"},
  {"EPSON", "24-Pin Series"},
  {"EPSON", "Stylus Color Series"},
  {"EPSON", "New Stylus Color Series"},
  {"EPSON", "Stylus Photo Series"},
  {"EPSON", "New Stylus Photo Series"},
};

const ExamPara ExamItemArray[] = {
  {N_("Abdomen"), N_("Adult Abdomen"), ExamItem::ABDO_ADULT},
  {N_("Abdomen"), N_("Adult Liver"), ExamItem::ABDO_LIVER},
  {N_("Abdomen"), N_("Kid Abdomen"), ExamItem::ABDO_KID},

  {N_("Cardiac"), N_("Adult Cardio"), ExamItem::CAR_ADULT},
  {N_("Cardiac"), N_("Kid Cardio"), ExamItem::CAR_KID},

  {N_("Small Part"), N_("Mammary Glands"), ExamItem::GLANDS},
  {N_("Small Part"), N_("Thyroid"), ExamItem::THYROID},
  {N_("Small Part"), N_("Eye Ball"), ExamItem::EYE},
  {N_("Small Part"), N_("Testicle"), ExamItem::SMALL_PART},

  {N_("Gynecology"), N_("Gynecology"), ExamItem::GYN},

  {N_("Obstetrics"), N_("Early Pregnancy"), ExamItem::EARLY_PREG},
  {N_("Obstetrics"), N_("Middle-late Pregnancy"), ExamItem::LATER_PREG},
  {N_("Obstetrics"), N_("Fetal Cardio"), ExamItem::CAR_FETUS},

  {N_("Urology"), N_("Kidney Ureter"), ExamItem::KIDNEY},
  {N_("Urology"), N_("Bladder Prostate"), ExamItem::BLADDER},

  {N_("Vascular"), N_("Carotid"), ExamItem::CAROTID},
  {N_("Vascular"), N_("Jugular"), ExamItem::JUGULAR},
  {N_("Vascular"), N_("Periphery Artery"), ExamItem::PERI_ARTERY},
  {N_("Vascular"), N_("Periphery Vein"), ExamItem::PERI_VEIN},

  {N_("Orthopedic"), N_("Hip Joint"), ExamItem::HIP_JOINT},
  {N_("Orthopedic"), N_("Meniscus"), ExamItem::MENISCUS},
  {N_("Orthopedic"), N_("Joint Cavity"), ExamItem::JOINT_CAVITY},
  {N_("Orthopedic"), N_("Spine"), ExamItem::SPINE},
  {N_("TCD"), "", ExamItem::TCD},

  {N_("User Defined"), N_("User defined 1"), ExamItem::USER1},
  {N_("User Defined"), N_("User defined 2"), ExamItem::USER2},
  {N_("User Defined"), N_("User defined 3"), ExamItem::USER3},
  {N_("User Defined"), N_("User defined 4"), ExamItem::USER4}
};

GtkWidget* CheckButtonArr[MAX_CALCARR_COUNT] = {NULL};

// ---------------------------------------------------------

ViewSystem* ViewSystem::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewSystem();
  }

  return m_instance;
}

ViewSystem::ViewSystem() {
  m_flag_notebook_image = 0;
  m_flag_notebook_tvout = 0;
  m_flag_notebook_coustomreport = 0;


  m_p1Selected = true;
  m_p1_func_index = 0;
  m_p2_func_index = 0;
  m_imageItemSensitive = false;
  m_itemIndex = -1;
  m_itemName = NULL;
  m_save_or_new_flag = 0;
  m_frameRegisterInfo = NULL;
  m_powerOffFlag = false;
  m_vgaInterl=true;
}

ViewSystem::~ViewSystem() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewSystem::CreateWindow() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  m_dialog = Utils::create_dialog(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), _("System Setup"), 840, 600);

  m_button_apply = Utils::add_dialog_button(m_dialog, _("Apply"), GTK_RESPONSE_APPLY, GTK_STOCK_APPLY);
  m_button_save = Utils::add_dialog_button(m_dialog, _("Save"), GTK_RESPONSE_OK, GTK_STOCK_SAVE);
  m_button_exit = Utils::add_dialog_button(m_dialog, _("Exit"), GTK_RESPONSE_CLOSE, GTK_STOCK_QUIT);

  g_signal_connect(m_button_apply, "clicked", G_CALLBACK(signal_button_clicked_apply), this);
  g_signal_connect(m_button_save, "clicked", G_CALLBACK(signal_button_clicked_save), this);
  g_signal_connect(m_button_exit, "clicked", G_CALLBACK(signal_button_clicked_exit), this);

  GtkNotebook* m_notebook = Utils::create_notebook();
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(m_notebook));

  // General
  gtk_notebook_append_page(m_notebook, create_note_general(), GTK_WIDGET(Utils::create_label(_("General"))));
  init_general_setting();

  // Options
  gtk_notebook_append_page(m_notebook, create_note_options(), GTK_WIDGET(Utils::create_label(_("Options"))));
  init_option_setting(NULL);

  // Image Preset
  m_flag_notebook_image = gtk_notebook_get_n_pages(m_notebook);
  gtk_notebook_append_page(m_notebook, create_note_image(), GTK_WIDGET(Utils::create_label(_("Image"))));

  // Measure
  gtk_notebook_append_page(m_notebook, create_note_measure(), GTK_WIDGET(Utils::create_label(_("Measure"))));
  init_measure_setting(NULL);

  // Calculate And Measure
  gtk_notebook_append_page(m_notebook, create_note_calc_measure(), GTK_WIDGET(Utils::create_label(_("Calc & Measure"))));

  // OB
  gtk_notebook_append_page(m_notebook, create_note_calc(), GTK_WIDGET(Utils::create_label(_("OB"))));
  init_calc_setting(NULL);

  // Comment
  gtk_notebook_append_page(m_notebook, create_note_comment(), GTK_WIDGET(Utils::create_label(_("Comment"))));
  init_comment_setting(NULL);

  // Peripheral
  m_flag_notebook_tvout = gtk_notebook_get_n_pages(m_notebook);
  gtk_notebook_append_page(m_notebook, create_note_tvout(), GTK_WIDGET(Utils::create_label(_("Peripheral"))));

  // Custom Report
  m_flag_notebook_coustomreport = gtk_notebook_get_n_pages(m_notebook);
  gtk_notebook_append_page(m_notebook, create_set_report(), GTK_WIDGET(Utils::create_label(_("Report"))));

  // DICOM
  if (CDCMRegister::GetMe()->IsAuthorize()) {
    gtk_notebook_append_page(m_notebook, create_note_dicom(), GTK_WIDGET(Utils::create_label(_("DICOM"))));
  }

  // P1-P3
  gtk_notebook_append_page(m_notebook, create_note_key_config(), GTK_WIDGET(Utils::create_label(_("P1-P3"))));
  init_key_config();

  // System Info
  gtk_notebook_append_page(m_notebook, create_note_info(), GTK_WIDGET(Utils::create_label(_("System"))));
  init_info_setting();

  g_signal_connect(G_OBJECT(m_notebook), "switch-page", G_CALLBACK(signal_notebook_switch_page), this);
  gtk_notebook_set_current_page(m_notebook, 0);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  init_image_setting();
  init_tvout_setting(NULL);
  init_report_setting();

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ViewSystem::DestroyWindow() {
  if (GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();
    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }

    m_dialog = NULL;
  }

  // 注册了新功能，关机重启
  if(m_powerOffFlag) {
    g_timeout_add(500, signal_callback_poweroff, this);
  }
}

GtkWidget* ViewSystem::GetWindow() {
  return GTK_WIDGET(m_dialog);
}

string ViewSystem::GetUserName() {
  return gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxentry_user_select))));
}

bool ViewSystem::CheckFlagFromReportTemplet(int id) {
  SysGeneralSetting sys;
  init_language = sys.GetLanguage();

  InitDB();
  OpenDB();

  ostringstream stream;
  stream << "SELECT record FROM CustomReport WHERE sections = '" << DEFAULT_STR << "' AND templet = '" << DEFAULT_STR << "' AND childsection = '" << DEFAULT_STR << "'";
  string sql = stream.str();
  sqlite3_stmt* stmt = NULL;

  if(sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Database error!"), NULL);
    if ((id == OTHERS_COMMENT_SHOW_ID) || (id == OTHERS_INDICTION_SHOW_ID)) {
      return true;
    }

    return false;
  }

  string record;

  while (sqlite3_step(stmt) != SQLITE_DONE) {
    if(strcmp(sqlite3_column_name(stmt, 0), "record") == 0) {
      record.assign((const char*)sqlite3_column_text(stmt, 0));
    }
  }

  sqlite3_finalize(stmt);

  string templet;

  if(record.length() > 0) {
    templet = record;
  } else {
    templet = TEMPLET1_STR[init_language];
  }

  CloseDB();

  memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
  int maxsection = sizeof(SECTION_STR)/sizeof(SECTION_STR[0]);

  for(int t = 0; t < maxsection; t++) {
    bool ret = false;
    ret = ReadRecordFromDB(CUSTOM_STR[init_language], templet, SECTION_STR[t][init_language]);

    if(false == ret && OTHERS_M == t && ((id == OTHERS_COMMENT_SHOW_ID)||(id == OTHERS_INDICTION_SHOW_ID))) {
      return true;
    }
  }

  if(CHECK_VALUE == ShowArr[id]) {
    return true;
  } else {
    return false;
  }
}

void ViewSystem::CreateDefineItemFormUsbToHost(string name) {
  stringstream ss;

  if (name == "System Default") {
    ss << CFG_RES_PATH << EXAM_ITEM_FILE << "SystemDefault" << ".ini";
  } else {
    ss << CFG_RES_PATH << EXAM_ITEM_FILE << name <<  ".ini";
  }

  string path_defined = ss.str();

  FILE* fp;
  if ((fp = fopen(path_defined.c_str(), "r")) !=NULL) {
    fclose(fp);
  }

  int fd = open(path_defined.c_str(), O_RDWR |O_CREAT |O_TRUNC, 0666);
  close(fd);

  IniFile ini_defined(path_defined);

  ss.str("");

  if (name == "System Default") {
    ss << CFG_RES_PATH << EXAM_FILE_OTHER;
  } else {
    ss << CFG_RES_PATH << EXAM_FILE_DIR << "userconfig/" << name << ".ini";
  }

  IniFile ini(ss.str());
  vector<string> useritemgroup = ini.GetGroupName();

  for (int i = 0; i < useritemgroup.size(); i++) {
    string userselect = ini.ReadString(useritemgroup[i], "UserSelect");

    if (userselect != "") {
      string probelist = ini.ReadString(useritemgroup[i], "ProbeType");
      string useritem = ini.ReadString(useritemgroup[i], "ExamItem");
      string department = ini.ReadString(useritemgroup[i], "Department");
      string firstgenitem = ini.ReadString(useritemgroup[i], "GenFirstItem");

      ini_defined.WriteString(useritemgroup[i], "UserSelect", name);
      ini_defined.WriteString(useritemgroup[i], "ProbeType", probelist);
      ini_defined.WriteString(useritemgroup[i], "ExamItem", useritem);
      ini_defined.WriteString(useritemgroup[i], "Department", department);
      ini_defined.WriteString(useritemgroup[i], "GenFirstItem", firstgenitem);

      ini_defined.SyncConfigFile();
    }
  }
}

void ViewSystem::ChangeNoteBookPage(int page) {
  gtk_notebook_set_current_page(m_notebook, page);
}

void ViewSystem::SetActiveUser(gint num) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_user_select), num);
}

void ViewSystem::ShowList(const string name) {
  gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxentry_user_select), name.c_str());
}

void ViewSystem::UpdateUserItem() {
  string name = GetUserName();

  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" && name == "Sistema por defecto") {
    stringstream ss;
    ss << "userconfig/" << name << ".ini";

    g_user_configure = ss.str();
  } else {
    g_user_configure = "ItemPara.ini";
  }

  set_image_item_sensitive(false);
  save_itemIndex(-1);

  // rebuild the exam type treeview, in order to disppear the select box in gtk
  int index = GetProbeType();
  if (index == -1) {
    return;
  }

  ExamItem examItem;
  vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);
  GtkTreeModel* model = create_exam_item_model(itemIndex);
  gtk_tree_view_set_model(m_treeview_exam_type, model);
  gtk_tree_view_expand_all(m_treeview_exam_type);
}

void ViewSystem::UpdateSpecificPrinterModel() {
  GtkTreeModel* model = create_specific_print_model();

  if (model != NULL) {
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_specific_print), model);
  }

  g_object_unref(model);
}

// ---------------------------------------------------------

void ViewSystem::ButtonClickedApply(GtkButton* button) {
  SysGeneralSetting sys;

  int langage = sys.GetLanguage();
  int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));

  if (langage != index_lang) {
    InitDB();
    LocalizationDB();
  }

  apply_report_setting();
  save_general_setting();
  save_option_setting();
  save_comment_setting();

  if (gtk_notebook_get_current_page(m_notebook) == m_flag_notebook_tvout) {
    save_tvout_setting();
  }

  save_measure_setting();
  save_key_config();
  save_calc_setting();
}

void ViewSystem::ButtonClickedSave(GtkButton* button) {
  SysGeneralSetting sys;

  int langage = sys.GetLanguage();
  int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));

  if(langage != index_lang) {
    InitDB();
    LocalizationDB();
  }

  BtnSave2Clicked(NULL);
  save_general_setting();
  save_option_setting();
  save_comment_setting();

  if (gtk_notebook_get_current_page(m_notebook) == m_flag_notebook_tvout) {
    save_tvout_setting();
  }

  save_measure_setting();
  save_key_config();
  save_calc_setting();

  g_timeout_add(500, signal_callback_exit_window_system, this);
  g_system_save = true;

  FreezeMode::GetInstance()->PressUnFreeze();
  MultiFuncUndo();
}

void ViewSystem::ButtonClickedExit(GtkButton* button) {
  DestroyWindow();
}

void ViewSystem::NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num) {
  if (page_num == m_flag_notebook_coustomreport) {
    GtkTreeIter iter;
    GtkTreeModel* model = NULL;

    GtkTreeSelection* selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);

    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) == TRUE) {
      GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
      gint tree_depth = gtk_tree_path_get_depth(path);
      gtk_tree_path_free (path);

      if(2 == tree_depth) {
        gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), TRUE);
      } else {
        gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), FALSE);
      }

      gtk_widget_set_sensitive(GTK_WIDGET(m_button_save), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(m_button_exit), TRUE);
    }
  } else if(page_num == m_flag_notebook_image) {
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_save), FALSE);
  } else if(page_num == m_flag_notebook_tvout) {
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_save), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_exit), TRUE);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_save), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_exit), TRUE);
  }
}

// General
void ViewSystem::EntryHospitalInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

  if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
    gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
  }
}

void ViewSystem::SpinbuttonInsertTime(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text)) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }

  string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
  string new_str = new_text;
  old_str.insert(*position, new_str);

  int time = atoi(old_str.c_str());
  if (time < 0 || time > 59) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }
}

void ViewSystem::SpinbuttonInsertHour(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text)) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }

  string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
  string new_str = new_text;
  old_str.insert(*position, new_str);

  int hour = atoi(old_str.c_str());
  if (hour < 0 || hour > 23) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }
}

void ViewSystem::BtnAdjustTimeClicked(GtkButton* button) {
  guint year, month, day;
  gtk_calendar_get_date(GTK_CALENDAR(m_calendar), &year, &month, &day);

  int hour, minute, second;
  hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_hour));
  minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_minute));
  second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_second));

  char setdatetime[50];
  char hctosys[20];
  sprintf(setdatetime, "hwclock --set --date=\"%d/%d/%d %d:%d:%d\"", month+1, day, year, hour, minute, second);

  // 同步时间
  sprintf(hctosys, "hwclock --hctosys");

  _system_(setdatetime);
  _system_(hctosys);
}

void ViewSystem::LanguageChanged(GtkComboBox* combobox) {
  int indexLang = gtk_combo_box_get_active(combobox);

  if (indexLang == ZH) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), 0); // set date format to y:m:d
  } else {
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), 2); // set date format to d:m:y
  }
}

void ViewSystem::VgaChanged(GtkComboBox* combobox) {
  int indexVga = gtk_combo_box_get_active(combobox);

  FpgaCtrl2D* fpga = new FpgaCtrl2D();
  fpga->VGADisplay((INT16U)(indexVga));

  if (0==indexVga) {
    m_vgaInterl=true;
  } else {
    m_vgaInterl=false;
  }

  delete fpga;
}

void ViewSystem::BtnGeneralDefaultClicked(GtkButton* button) {
  SysGeneralSetting sysGeneralSetting;
  sysGeneralSetting.DefaultFactory();

  int date_format = sysGeneralSetting.GetDateFormat();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), date_format);

  int index_lang = sysGeneralSetting.GetLanguage();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_language), index_lang);

  int screen_saver = sysGeneralSetting.GetScreenProtect();
  int index_screen_saver = 0;

  switch (screen_saver) {
  case 0:
    index_screen_saver = 0;
    break;
  case 5:
    index_screen_saver = 1;
    break;
  case 10:
    index_screen_saver = 2;
    break;
  case 20:
    index_screen_saver = 3;
    break;
  case 30:
    index_screen_saver = 4;
    break;
  case 45:
    index_screen_saver = 5;
    break;
  case 60:
    index_screen_saver = 6;
    break;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_screen_saver), index_screen_saver);
}

// Options
void ViewSystem::BtnOptionsDefaultClicked(GtkButton* button) {
  SysOptions* sysOpt = new SysOptions;
  sysOpt->DefaultFactory();
  init_option_setting(sysOpt);
}

// Image Preset
void ViewSystem::EntryNameInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

  if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
    gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
  }
}

void ViewSystem::UserSelectFocusOut(GtkWidget* widget, GdkEventFocus* event) {
  string name = gtk_entry_get_text(GTK_ENTRY(widget));

  if (name != "System Default" && name != "Умолчан системы" &&
    name != "系统默认" && name != "Domyślne Systemu" && name != "Par défaut du sys." &&
    name != "Systemvorgabe" && name != "Sistema por defecto") {
    UserSelect::GetInstance()->write_username(GTK_COMBO_BOX(m_comboboxentry_user_select), USERNAME_DB, name);

    stringstream ss;
    ss << "userconfig/" << name << ".ini";

    g_user_configure = ss.str();

    ss.str("");
    ss << CFG_RES_PATH << EXAM_ITEM_FILE << name << ".ini";

    string path = ss.str();

    int fd = open(path.c_str(), O_RDWR |O_CREAT |O_TRUNC, 0666);
    close(fd);
    save_itemIndex(-1);

    // 注释预设 所用
    char path1[256];
    char userselectname[256];
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, name.c_str(), ".ini");
    sprintf(userselectname, "%s%s", CFG_RES_PATH, COMMENT_COPY );
    FileMan f;
    f.CopyFile(userselectname, path1);

    // 计算预设已选所用
    char path2[256];
    char userselectname2[256];
    sprintf(path2, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, name.c_str(), ".ini");
    sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_CALC_FILE);
    f.CopyFile(userselectname2, path2);

    //计算预设词库所用
    char path3[256];
    char userselectname3[256];
    sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, name.c_str(), ".ini");
    sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE );
    f.CopyFile(userselectname3, path3);

    // 测量预设已选所用
    sprintf(path2, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, name.c_str(), ".ini");
    sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_FILE);
    f.CopyFile(userselectname2, path2);

    //测量预设词库所用
    sprintf(path3, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, name.c_str(), ".ini");
    sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_ITEM_FILE );
    f.CopyFile(userselectname3, path3);

    // rebuild the exam type treeview, in order to disppear the select box in gtk
    int index = GetProbeType();
    if (index == -1) {
      return;
    }

    ExamItem examItem;
    vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);
    GtkTreeModel* model = create_exam_item_model(itemIndex);
    gtk_tree_view_set_model(m_treeview_exam_type, model);
    gtk_tree_view_expand_all(m_treeview_exam_type);
  }
}

void ViewSystem::BtnDeleteUserClicked(GtkButton* button) {
  string name = GetUserName();
  char path[256];

  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" && name == "Sistema por defecto") {
    // avoid to delete a config which is in use
    char *username = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_user_select));

    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string cur_username;
    cur_username = exam.ReadDefaultUserSelect(&ini);
    if(strcmp(username,_(cur_username.c_str()))==0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("User configuration being in use!"), NULL);
      return ;
    }

    UserSelect::GetInstance()->delete_username_db(USERNAME_DB, name);

    // delete file named by username
    sprintf(path, "%s%s%s", USERCONFIG_PATH, name.c_str(), ".ini");
    remove(path);

    char path_define[256];
    sprintf(path_define, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name.c_str(),".ini");
    remove(path_define);

    //delete defined comment file
    char path_comment_user[256];
    sprintf(path_comment_user, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, name.c_str(), ".ini");
    remove(path_comment_user);

    // append text to combo_box
    ClearComboBox(GTK_COMBO_BOX(m_comboboxentry_user_select));
    UserSelect::GetInstance()->read_default_username(GTK_COMBO_BOX(m_comboboxentry_user_select));
    UserSelect::GetInstance()->read_username_db(USERNAME_DB, GTK_COMBO_BOX(m_comboboxentry_user_select));

    // set System Default is the active user after delete a user
    UserSelect::GetInstance()->set_active_user(GTK_COMBO_BOX(m_comboboxentry_user_select), 0);
    g_user_configure = "ItemPara.ini";

    // set parameter zoom disable
    set_image_item_sensitive(false);
    save_itemIndex(-1);

    // rebuild the exam type treeview, in order to disppear the select box in gtk
    int index = GetProbeType();
    if (index == -1) {
      return;
    }

    ExamItem examItem;
    vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);
    GtkTreeModel* model = create_exam_item_model(itemIndex);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("System Default is protected!"), NULL);
  }
}

void ViewSystem::UserChanged(GtkComboBox* combobox) {
  int select = gtk_combo_box_get_active(combobox);

  if (select >= 0) {
    gchar* name = gtk_combo_box_get_active_text(combobox);

    if (select > 0) {
      stringstream ss;
      ss << "userconfig/" << name << ".ini";

      g_user_configure = ss.str();
    } else {
      g_user_configure = "ItemPara.ini";
    }
    set_image_item_sensitive(false);
    save_itemIndex(-1);

    // rebuild the exam type treeview, in order to disppear the select box in gtk
    int index = GetProbeType();
    if (index == -1) {
      return;
    }

    ExamItem examItem;
    vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);
    GtkTreeModel* model = create_exam_item_model(itemIndex);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
  }
}

void ViewSystem::ProbeTypeChanged(GtkComboBox* combobox) {
  int index = GetProbeType();

  if (index == -1) {
    return;
  }

  ExamItem examItem;
  vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);

  GtkTreeModel* model = create_exam_item_model(itemIndex);
  gtk_tree_view_set_model(m_treeview_exam_type, model);

  if (m_str_index.empty()) {
    GtkTreePath* path_tmp = gtk_tree_path_new_from_string("0:0");
    gtk_tree_view_expand_all(m_treeview_exam_type);
    gtk_tree_view_set_cursor(m_treeview_exam_type, path_tmp, NULL, TRUE);
  } else {
    GtkTreePath* path_tmp = gtk_tree_path_new_from_string(m_str_index.c_str());
    gtk_tree_view_expand_all(m_treeview_exam_type);
    gtk_tree_view_set_cursor(m_treeview_exam_type, path_tmp, NULL, TRUE);
  }

  int probeIndex = 0;
  int examIndex = 0;
  char* itemName = NULL;

  GetImageNoteSelection(probeIndex, examIndex, itemName);

  if (probeIndex != -1 && probeIndex < NUM_PROBE) {
    char buf[50] = {0};

    // update probe 2D freq
    ClearComboBox(GTK_COMBO_BOX(m_combobox_2d_freq));

    for (int i = 0; i < ProbeSocket::FREQ2D_SUM[probeIndex]; i ++) {
      sprintf(buf , "%.1fMHz", (float)ProbeSocket::FREQ2D[probeIndex][i].emit / 20);
      gtk_combo_box_append_text(GTK_COMBO_BOX (m_combobox_2d_freq), buf);
    }

    // update doppler freq
    ClearComboBox(GTK_COMBO_BOX(m_combobox_pw_freq));

    for (int i = 0; i < ProbeSocket::FREQ_DOPPLER_SUM[probeIndex]; i ++) {
      sprintf(buf , "%.1f", (float)ProbeSocket::FREQ_DOPPLER[probeIndex][i] / 20);
      gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_pw_freq), buf);
    }

    ClearComboBox(GTK_COMBO_BOX(m_combobox_thi_freq));

    for (int i = 0; i < ProbeSocket::FREQ_THI_SUM[probeIndex]; i ++) {
      if(ProbeSocket::FREQ_THI[i] != 0) {
        sprintf(buf , "%.1fMHz", (float)ProbeSocket::FREQ_THI[probeIndex][i] / 10);
        gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_thi_freq), buf);
      }
    }
  }

  string probeType = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_probe_type));

  if (!probeType.empty()) {
    if (probeType == "30P16A") {
      ClearComboBox(GTK_COMBO_BOX(m_combobox_space_compound));
      gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));
    } else {
      ClearComboBox(GTK_COMBO_BOX(m_combobox_space_compound));
      gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));

      char buf[50] = {0};

      for(int i = 1; i <Img2D::MAX_SPACE_COMPOUND; i ++) {
        sprintf(buf, "%d", i);
        gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_space_compound), buf);
      }
    }
  }
}

void ViewSystem::EntryItemInsert(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path) {
  if(GTK_IS_ENTRY(editable)) {
    g_signal_connect(G_OBJECT(editable), "insert_text", G_CALLBACK(on_entry_insert_custom_item), this);
  }
}

void ViewSystem::CellRendererRename(GtkCellRendererText* renderer, gchar* path_str, gchar* new_text) {
  char *old_text;

  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_exam_type));
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, 0, &old_text, -1);

  // gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free(path);

  //如果重命名前后renderertext中的文本没有改变,则返回
  if (strcmp(new_text, old_text) == 0) {
    return;
  }
  if (strcmp(new_text, "") == 0) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

    return;
  }

  // except the user defined1 to user defined4

  int size = sizeof(ExamItemArray)/sizeof(ExamPara) - 4;

  for(int i = 0; i < size/*24*/; i++) {
    if (new_text==ExamItemArray[i].name) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Fail to rename! The item name has existed!"), NULL); //重命名失败!该结点已存在
      return;
    }

    if (old_text==ExamItemArray[i].name) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Operation is invaild."), NULL); //
      return;
    }
  }

  int probe_type_index = GetProbeType();
  char *dialognotice=_("Fail to rename! The item name has existed!");
  int group_length(0);
  bool renameflag=TRUE;
  renameflag=RenameNotice(probe_type_index, new_text, dialognotice, group_length);
  if(renameflag) {
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);
    GtkTreeIter iter_parent;
    GtkTreeIter iter_child;

    char *strname;
    char *str_indexname;
    gtk_tree_model_iter_parent(model, &iter_parent, &iter);

    gtk_tree_model_get(model, &iter_parent, 0, &strname, -1);
    gtk_tree_model_iter_children(model, &iter_child, &iter_parent);
    gtk_tree_model_get(model, &iter_child, 0, &str_indexname, -1);

    char str[256];
    char str_index[256];

    TransEnglish(strname, str, str_indexname, str_index);

    ExamItem examItem;
    int itemIndex;
    int probeIndex;
    probeIndex = GetProbeType();

    if ((probeIndex >= 0)) {
      examItem.DeleteNewItemFile(probeIndex, old_text, str);
      examItem.DeleteItemParaFile(probeIndex, old_text, str);
      examItem.DeleteNewItemForCommentFile(probeIndex, old_text, str);
      examItem.DeleteNewItemForCalcFile(probeIndex, old_text, str);
      examItem.DeleteNewItemForMeasureFile(probeIndex, old_text, str);

      examItem.WriteNewItemFile(probeIndex, new_text, str,str_index);
      examItem.WriteDefinedItemPara(probeIndex, new_text, str, str_index);
      examItem.WriteNewItemToCommentFile(probeIndex, new_text, str);
      examItem.WriteNewItemToCalcFile(probeIndex, new_text, str);
      examItem.WriteNewItemToMeasureFile(probeIndex, new_text, str);
    }
  }
}

void ViewSystem::ExamTypeChanged(GtkTreeSelection* selection) {
  set_image_item_sensitive(false);

  GtkTreeModel *model;
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    return;
  }

  ExamItem::EItem itemIndex;
  gtk_tree_model_get(model, &iter, INDEX_COLUMN, &itemIndex, -1);
  char *itemName;
  gtk_tree_model_get(model, &iter, 0, &itemName, -1);
  save_itemName(itemName);
  save_itemIndex(itemIndex);

  if (itemIndex != -1) {
    set_image_item_sensitive(true);

    int probeIndex = GetProbeType();
    if (probeIndex != -1) {
      ExamItem::ParaItem paraItem;
      ExamItem examItem;
      examItem.ReadExamItemPara(probeIndex, (int)itemIndex, &paraItem, itemName);
      SetImagePara(paraItem);
      image_para_restrict(probeIndex);
    }
  }
}

void ViewSystem::BtnExamDepartmentClicked(GtkWidget* widget, GdkEventButton* event) {
  if (event->button == 1) {
    if (event->type == GDK_BUTTON_RELEASE) {
      GtkTreeIter iter;
      GtkTreeModel *model;

      GtkTreeSelection *selection;
      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

      if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE) {
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        int tree_depth = gtk_tree_path_get_depth(path);
        gtk_tree_path_free (path);

        if(gtk_tree_model_iter_has_child(model, &iter)) {
          GtkTreePath *path=gtk_tree_model_get_path(model, &iter);
          tree_auto_scroll(GTK_TREE_VIEW(widget), &iter, path, (gpointer)0);
          gtk_tree_path_free(path);
        }
      }
    }
  }
}

void ViewSystem::AddItemClicked(GtkButton* button) {
  char *new_string = _("New Item");//临时插入结点

  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
  GtkTreeModel *model;
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
      _("Please select a department before inserting!"), NULL); //请先选择待插入结点的父结点
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);
  if(tree_depth==1) {
    int probe_type_index = GetProbeType();
    int group_length(0);
    char *dialognotice= _("Fail to insert! 'New Item' has existed in the probe type. please rename it before inserting!");
    RenameNotice(probe_type_index, new_string, dialognotice, group_length);
    if(group_length>MAXNUMBER) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
        _("The defined items have reached the maximum!"), NULL);
      return;
    }

    char *strname;
    char *str_indexname;
    GtkTreeIter childern;

    gtk_tree_model_get(model, &iter, 0, &strname, -1);
    gtk_tree_model_iter_children(model, &childern, &iter);
    gtk_tree_model_get(model, &childern, 0, &str_indexname, -1);

    char str[256];
    char str_index[256];
    TransEnglish(strname, str, str_indexname, str_index);

    ExamItem examItem;
    int itemIndex;
    int probeIndex;
    probeIndex = GetProbeType();
    if ((probeIndex >= 0)) {
      examItem.WriteNewItemFile(probeIndex, new_string, str,str_index);
      examItem.WriteDefinedItemPara(probeIndex, new_string, str, str_index);
      examItem.WriteNewItemToCommentFile(probeIndex, new_string, str);
      examItem.WriteNewItemToCalcFile(probeIndex, new_string, str);
      examItem.WriteNewItemToMeasureFile(probeIndex, new_string, str);
    }
  } else if(tree_depth==2) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
      _("Fail to insert! Please select a department before inserting!"), NULL);
    return;
  }

  GtkTreeIter iter_new;
  gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
  gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
  GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

  //刷新treeview
  int probeIndex = GetProbeType();

  ExamItem examItem;
  vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[probeIndex]);
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(itemIndex));
  gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_exam_type), new_path);
  //设置结点为编辑状态
  GtkTreeViewColumn *column_tree_view;
  column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_exam_type), 0);

  gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_exam_type), new_path, column_tree_view, m_renderer, TRUE);
  gtk_tree_path_free (new_path);
}

void ViewSystem::DeleteItemClicked(GtkButton* button) {
  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));

  GtkTreeModel *model;
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
      _("Please select one item to be delete!"), NULL); //请先选择待删除的结点!
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free(path);

  if (tree_depth == 1) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
      _("Fail to delete! Can not delete the department!"), NULL); //删除失败!不能删除根结点!
    return;
  }

  if (tree_depth == 2) {
    char *str0name, *str1name;
    string str0;
    string str1;
    GtkTreeIter parent;
    gtk_tree_model_get(model, &iter, 0, &str0name, -1);

    gtk_tree_model_iter_parent(model, &parent, &iter);
    gtk_tree_model_get(model, &parent, 0, &str1name, -1);

    str0=str0name;
    str1=str1name;

    ExamItem examItem;
    int itemIndex;
    int probeIndex;
    probeIndex = GetProbeType();
    if ((probeIndex >= 0)) {
      int size = sizeof(ExamItemArray)/sizeof(ExamPara) - 4;

      for(int i = 0; i < size/*24*/; i++) {
        if (str0 == ExamItemArray[i].name) {
          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
            _("Fail to delete! Can not delete the defaulted exam item!"), NULL);
          return;
        }
      }

      string useritemname;
      char path[256];
      sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
      IniFile ini(path);
      IniFile *ptrIni= &ini;

      bool userflag= ptrIni->ReadBool("UserItemFlag", "UserFlag");
      useritemname = ptrIni->ReadString("UserItemName", "ItemName");
      string userselect = ptrIni->ReadString("UserSelect", "DefaultUser");
      string userprobe = ptrIni->ReadString("ProbeModel", "ProbeModel");
      string username = GetUserName();

      if (useritemname == str0 && userflag==true && userprobe == g_probe_list[probeIndex] && username == userselect) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
          _("Fail to delete! The item is under using!"), NULL); //删除失败!
        return;
      } else {
        examItem.DeleteNewItemFile(probeIndex, str0.c_str(), str1.c_str());
        examItem.DeleteItemParaFile(probeIndex, str0.c_str(), str1.c_str());

        //记录当前所选结点的父结点,刷新删除节点之后的树
        examItem.DeleteNewItemForCommentFile(probeIndex, str0.c_str(), str1.c_str());
        examItem.DeleteNewItemForCalcFile(probeIndex, str0.c_str(), str1.c_str());
        examItem.DeleteNewItemForMeasureFile(probeIndex, str0.c_str(), str1.c_str());
        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        if(username == GetUserName()) {
          CalcSetting::GetInstance()->ChangeExamBoxDelete();
          MeasureSetting::GetInstance()->ChangeExamBoxDelete();
          ChangeCommentExamBoxDelete(probeIndex);
        }

        GtkTreeIter parent_iter;
        GtkTreeIter child_iter;
        gtk_tree_model_iter_parent(model, &parent_iter, &iter);
        GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);

        ExamItem examItem;
        vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[probeIndex]);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(itemIndex));

        //展开被删除结点的父结点
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
        gboolean has_node = gtk_tree_model_iter_children(model, &child_iter, &parent_iter);
        if(has_node) {
          parent_path = gtk_tree_model_get_path(model, &child_iter);
          char *str_path = gtk_tree_path_to_string(parent_path);
          gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), parent_path, NULL, TRUE);
        }

        gtk_tree_path_free (parent_path);
      }
    }
  }
}

void ViewSystem::BtnImageSaveClicked(GtkButton* button) {
  gtk_label_set_text(GTK_LABEL(m_label_check_part), (_("Clicking OK will override the previous data, whether\n to continue the operation.")));
  gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_check_part), PANGO_WRAP_WORD);

  m_save_or_new_flag = 1;
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
  GtkTreeModel *model;
  GtkTreeIter iter;
  if ((gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)) {
    GtkTreePath *path_tmp;
    if(m_str_index.empty()) {
      path_tmp = gtk_tree_path_new_from_string("0:0");
      gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
      gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    } else {
      GtkTreePath *path_tmp = gtk_tree_path_new_from_string(m_str_index.c_str());
      gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
      gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    }
  }
}

void ViewSystem::BtnImageNewClicked(GtkButton* button) {
  m_save_or_new_flag = 0;
  gtk_label_set_text(m_label_check_part, (_("Exam Type:")));
  gtk_entry_set_text(m_entry_new_check_part, "");
  gtk_widget_show_all(GTK_WIDGET(m_frame_new_check_part));
  gtk_widget_grab_focus(GTK_WIDGET(m_entry_new_check_part));

  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
  GtkTreeModel *model;
  GtkTreeIter iter;
  if ((gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)) {
    GtkTreePath *path_tmp;
    if(m_str_index.empty()) {
      path_tmp = gtk_tree_path_new_from_string("0:0");
      gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
      gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    } else {
      GtkTreePath *path_tmp = gtk_tree_path_new_from_string(m_str_index.c_str());
      gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
      gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    }
  }
}

void ViewSystem::BtnImageExportToUSBClicked(GtkButton* button) {
  ConfigToUSB::GetInstance()->CreateWindow(GetWindow());
}

void ViewSystem::BtnImageImportFromUSBClicked(GtkButton* button) {
  PeripheralMan *ptr = PeripheralMan::GetInstance();

  if(!ptr->CheckUsbStorageState()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
      MessageDialog::DLG_ERROR, _("No USB storage found!"), NULL);
    return;
  } else {
    if(!ptr->MountUsbStorage()) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
        MessageDialog::DLG_ERROR, _("Failed to mount USB storage!"), NULL);
      return;
    }
  }

  ConfigToHost::GetInstance()->CreateWindow(GetWindow());
  ptr->UmountUsbStorage();
}

void ViewSystem::BtnImageDefaultClicked(GtkButton* button) {
  m_save_or_new_flag = 2;
  gtk_label_set_text(GTK_LABEL(m_label_check_part), (_("Clicking OK will override the previous data, whether\n to continue the operation.")));
  gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_check_part), PANGO_WRAP_WORD);
}

void ViewSystem::BtnNewCheckPartOkClicked(GtkButton* button) {
  if(m_save_or_new_flag == 1) {
    get_current_and_save_image_para();
  } else if (m_save_or_new_flag == 2) {
    image_default_setting();
  } else {
    char *check_part = (char*) gtk_entry_get_text(GTK_ENTRY(m_entry_new_check_part));
    AddCheckPart(check_part);
  }

  gtk_widget_hide(GTK_WIDGET(m_frame_new_check_part));
}

void ViewSystem::BtnNewCheckPartCancelClicked(GtkButton* button) {
  gtk_widget_hide(GTK_WIDGET(m_frame_new_check_part));
}

// Measure
void ViewSystem::ChkBtnTAmaxToggled(GtkToggleButton* togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (!press) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi), FALSE);
  }
}

void ViewSystem::ChkBtnPIToggled(GtkToggleButton* togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax), TRUE);
  }
}

void ViewSystem::ChkBtnHRToggled(GtkToggleButton* togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_time), FALSE);
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_time), TRUE);
  }
}

void ViewSystem::BtnMeasureDefaultClicked(GtkButton* button) {
  SysMeasurementSetting* sysMea = new SysMeasurementSetting();
  sysMea->DefaultFactory();
  init_measure_setting(sysMea);
}

// Calculate And Measure

// OB
void ViewSystem::BtnOBCustomClicked(GtkButton* button) {
  ViewCustomOB::GetInstance()->CreateWindow(GTK_WIDGET(m_dialog));
}

void ViewSystem::BtnCalculateDefaultClicked(GtkButton* button) {
  SysCalculateSetting* sysCalc = new SysCalculateSetting();
  sysCalc->DefaultFactory();
  init_calc_setting(sysCalc);
}

// Comment
void ViewSystem::ProbeCommentChanged(GtkComboBox* combobox) {
  int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

  if (index == -1) {
    return;
  }

  ExamItem examItem;
  vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[index]);
  create_exam_comment_model(itemIndex);
}

void ViewSystem::ExamCommentChanged(GtkComboBox* combobox) {
  GtkTreeModel* model = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), model);

  int department_index = DepartmentIndex();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), department_index);
}

void ViewSystem::DepartmentCommentChanged(GtkComboBox* combobox) {
  int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));

  if(index == -1) {
    return;
  }

  GtkTreeModel *model = create_item_comment_model(index);
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), model);
}

void ViewSystem::ButtonSelectOneCommentClicked(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  gboolean isSelect = FALSE;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please select a item before add!"), NULL);
    return;
  }

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  if(!exam_type_name) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;
  vector<ExamPara> vecItem_Comment;
  vecItem_Comment.clear();
  CreateItemList_Comment1(probe_exam, vecItem_Comment);

  int item_length(0);
  item_length = vecItem_Comment.size();
  if(!vecItem_Comment.empty()) {
    for(int i=0; i<item_length; i++) {
      SysGeneralSetting sysGS;

      if(sysGS.GetLanguage() ==ZH) {
        if(strcmp(select_name, _(vecItem_Comment[i].name.c_str())) == 0) {
          GtkTreeModel *model_tmp;
          GtkTreeSelection *selection;
          GtkTreeIter iter_tmp;
          selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
          model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));
          //高亮要插入的词条，并更新滚动条
          iter_tmp= InsertUniqueComment(model_tmp, select_name);
          gtk_tree_selection_select_iter(selection, &iter_tmp);

          GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
          gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path_scroll, NULL, FALSE, 1.0, 1.0);
          gtk_tree_path_free (path_scroll);

          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _(" Item has existed. Please select item again!"), NULL);
          return;
        }
      } else {
        if(strcmp(select_name, vecItem_Comment[i].name.c_str()) == 0) {
          GtkTreeModel *model_tmp;
          GtkTreeSelection *selection;
          GtkTreeIter iter_tmp;
          selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
          model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));
          //高亮要插入的词条，并更新滚动条
          iter_tmp= InsertUniqueComment(model_tmp, select_name);
          gtk_tree_selection_select_iter(selection, &iter_tmp);

          GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
          gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path_scroll, NULL, FALSE, 1.0, 1.0);
          gtk_tree_path_free (path_scroll);

          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _(" Item has existed. Please select item again!"), NULL);
          return;
        }
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
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni = &ini;

  int number;
  char SelectNum[256];
  number = ptrIni->ReadInt(probe_exam.c_str(), "Number");

  sprintf(SelectNum, "Note%d",number+1);
  ptrIni->WriteString(probe_exam.c_str(), SelectNum, select_name);
  ptrIni->WriteInt(probe_exam.c_str(), "Number", number+1);
  ptrIni->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);
  // 高亮插入的词条，并更新滚动条
  GtkTreeSelection *new_selection;
  GtkTreeIter iter_new;
  new_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
  iter_new= InsertUniqueComment(new_model1, select_name);
  gtk_tree_selection_select_iter(new_selection, &iter_new);

  GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path_scroll, NULL, TRUE, 1.0, 1.0);
  gtk_tree_path_free (path_scroll);
}

void ViewSystem::ButtonSelectAllCommentClicked(GtkButton* button) {
  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  if(!exam_type_name) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini11(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini11);
  char path[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path);
  IniFile *ptrIni = &ini;

  const char* ptrSection = probe_exam.c_str();

  int Num;
  Num = ptrIni->ReadInt(ptrSection, "Number");

  vector<ExamPara> vecSelect_Comment;
  vecSelect_Comment.clear();
  CreateItemList_Comment1(ptrSection, vecSelect_Comment);

  int select_length(0);
  select_length = vecSelect_Comment.size();

  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
  IniFile new_ini(path1);
  IniFile *new_ptrIni= &new_ini;
  int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
  string department_name = DepartmentName(department_index);
  int number;
  number = new_ptrIni->ReadInt(department_name, "Number");

  for(int i=1; i<=number; i++) {
    int writeNote=1;
    char NoteNumber[256];
    sprintf(NoteNumber, "Note%d", i);
    string Note_name;
    Note_name = new_ptrIni->ReadString(department_name, NoteNumber);

    if(!vecSelect_Comment.empty()) {
      for(int j=0; j<select_length; j++) {
        SysGeneralSetting sysGS;
        if(sysGS.GetLanguage() ==ZH) {
          if(strcmp(_(Note_name.c_str()), vecSelect_Comment[j].name.c_str()) ==0) {
            writeNote=0;
          }
        } else {
          if(strcmp(Note_name.c_str(), vecSelect_Comment[j].name.c_str()) ==0) {
            writeNote=0;
          }
        }
      }
    }

    if(writeNote) {
      char NoteNum[256];
      sprintf(NoteNum, "Note%d", ++Num);
      SysGeneralSetting sysGS;
      if(sysGS.GetLanguage() ==ZH) {
        ptrIni->WriteString(ptrSection, NoteNum, _(Note_name.c_str()));
      } else {
        ptrIni->WriteString(ptrSection, NoteNum, Note_name.c_str());
      }
    }
  }

  ptrIni->WriteInt(ptrSection, "Number", Num);
  ptrIni->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);
}

void ViewSystem::ButtonAddClicked(GtkButton* button) {
  char *new_string = _("New Item");
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
  IniFile new_ini(path1);
  IniFile *ptrIni= &new_ini;
  int number;
  int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
  string department = DepartmentName(department_index);
  number = ptrIni->ReadInt(department, "Number");
  vector<ExamPara> vecItemComment;
  vecItemComment.clear();
  CreateItemList_Comment(department, vecItemComment, number);

  int item_length(0);
  item_length = vecItemComment.size();
  if(!vecItemComment.empty()) {
    for(int i=0; i<item_length; i++) {
      SysGeneralSetting sysGS;

      if(sysGS.GetLanguage() ==ZH) {
        if(strcmp(new_string, _(vecItemComment[i].name.c_str())) == 0) {
          GtkTreeIter iter_tmp = InsertUniqueComment(model, new_string);
          gtk_tree_selection_select_iter(selection, &iter_tmp);
          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _(" 'New Item' has existed. Please rename item !"), NULL);
          return;
        }
      } else {
        if(strcmp(new_string, vecItemComment[i].name.c_str()) == 0) {
          GtkTreeIter iter_tmp = InsertUniqueComment(model, new_string);
          gtk_tree_selection_select_iter(selection, &iter_tmp);
          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _(" 'New Item' has existed. Please rename item !"), NULL);
          return;
        }
      }
    }
  }

  gtk_tree_selection_get_selected(selection, &model, &iter);
  if(gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    char SelectNum[256];
    sprintf(SelectNum, "Note%d",number+1);

    ptrIni->WriteString(department, SelectNum, new_string);
    ptrIni->WriteInt(department, "Number", number+1);
    ptrIni->SyncConfigFile();

    GtkTreeIter iter_new;
    gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, NULL);
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
    GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

    //刷新treeview
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment),  create_item_comment_model(0));
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_item_comment), new_path);
    //设置结点为编辑状态

    GtkTreeViewColumn *column_tree_view;
    column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_item_comment), 0);

    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_item_comment),
      new_path, column_tree_view, m_cellrenderer_comment_text, TRUE);

    //更新滚动条到底部
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment), new_path, NULL, TRUE, 1.0, 1.0);
    gtk_tree_path_free (new_path);
  } else {
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);
    ptrIni->RemoveGroup(department);
    ptrIni->SyncConfigFile();
    ExamPara exampara1;
    exampara1.dept_name="";
    exampara1.name = new_string;
    exampara1.index=ExamItem::USERNAME;
    vecItemComment.insert(vecItemComment.begin()+path_num+1, exampara1);

    for(int i=0; i<item_length+1; i++) {
      char NoteNumber[256];
      sprintf(NoteNumber, "Note%d", i+1);
      ptrIni->WriteString(department, NoteNumber, vecItemComment[i].name.c_str());
    }

    ptrIni->WriteInt(department, "Number", item_length+1);
    ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model(0);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), new_model1);

    g_free(path_string);
    gtk_tree_path_next(path);
    GtkTreeViewColumn *column_tree_view;
    column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_item_comment), 0);

    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_item_comment),
      path, column_tree_view, m_cellrenderer_comment_text, TRUE);

    //更新滚动条
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment), path, NULL, FALSE, 1.0, 1.0);
    gtk_tree_path_free (path);
  }
}

void ViewSystem::ButtonDeleteSelectClicked(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);
  int  department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
  string department = DepartmentName(department_index);
  char path2[256];
  sprintf(path2, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
  IniFile ini(path2);
  IniFile *ptrIni_comp= &ini;
  int number;
  number = ptrIni_comp->ReadInt(department, "Number");
  vector<ExamPara> vecItemNote;
  vecItemNote.clear();
  CreateItemList_Note(department, vecItemNote, number);

  int note_item_length(0);
  note_item_length = vecItemNote.size();

  if(!vecItemNote.empty()) {
    for(int i=0; i<note_item_length; i++) {
      SysGeneralSetting sysGS;

      if(sysGS.GetLanguage() ==ZH) {
        if( strcmp(select_name, _(vecItemNote[i].name.c_str())) == 0) {
          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
            MessageDialog::DLG_INFO,  _(" Only Userdefined items can be deleted!"), NULL);
          return;
        }
      } else {
        if( strcmp(select_name, vecItemNote[i].name.c_str()) == 0) {
          MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
            MessageDialog::DLG_INFO,  _(" Only Userdefined items can be deleted!"), NULL);
          return;
        }
      }
    }
  }

  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
  IniFile new_ini(path1);
  IniFile *ptrIni_comment= &new_ini;

  vector<ExamPara> vecDeleteComment;
  vecDeleteComment.clear();
  CreateItemList_Delete_Comment(select_name, department,vecDeleteComment);

  int item_length(0);
  item_length = vecDeleteComment.size();
  ptrIni_comment->RemoveGroup(department);
  ptrIni_comment->SyncConfigFile();

  for(int i=0; i<item_length; i++) {
    char NoteNumber[256];
    sprintf(NoteNumber, "Note%d", i+1);
    ptrIni_comment->WriteString(department, NoteNumber, vecDeleteComment[i].name.c_str());
  }

  ptrIni_comment->WriteInt(department, "Number", item_length);
  ptrIni_comment->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model(0);
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), new_model1);
  //设置光标，更新滚动条的值
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment), path, NULL, TRUE);
  gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_scrolledwindow_item_comment)));

  gtk_tree_path_free (path);
}

void ViewSystem::ButtonUpClicked(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
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

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;

  vector<ExamPara> vecDelete_Comment;
  vecDelete_Comment.clear();
  CreateItemList_Comment1(probe_exam, vecDelete_Comment);

  int item_length(0);
  item_length = vecDelete_Comment.size();

  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);
  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni_comment = &ini;

  ptrIni_comment->RemoveGroup(probe_exam.c_str());
  ptrIni_comment->SyncConfigFile();

  swap(vecDelete_Comment[path_num], vecDelete_Comment[path_num-1]);

  for(int i=0; i<item_length; i++) {
    char NoteNumber[256];
    sprintf(NoteNumber, "Note%d", i+1);
    ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
  }

  ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
  ptrIni_comment->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);

  g_free(path_string);
  if(gtk_tree_path_prev(path)) {
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, TRUE);
    //在词条上移时，更新滚动条

    if(item_length-path_num >12) {
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolledwindow_item_comment_selected));//保证在最上端的词条移动时能够正确显示
    }
  }

  gtk_tree_path_free (path);
}

void ViewSystem::ButtonDownClicked(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please select a item before down!"), NULL);
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  char *path_string = gtk_tree_path_to_string(path);
  int path_num = atoi(path_string);

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;
  vector<ExamPara> vecDelete_Comment;
  vecDelete_Comment.clear();
  CreateItemList_Comment1(probe_exam, vecDelete_Comment);

  int item_length(0);
  item_length = vecDelete_Comment.size();

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
      sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    } else {
      sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }

    IniFile ini(path1);
    IniFile *ptrIni_comment = &ini;

    ptrIni_comment->RemoveGroup(probe_exam.c_str());
    ptrIni_comment->SyncConfigFile();

    swap(vecDelete_Comment[path_num], vecDelete_Comment[path_num+1]);

    for(int i=0; i<item_length; i++) {
      char NoteNumber[256];
      sprintf(NoteNumber, "Note%d", i+1);
      ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
    }

    ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
    ptrIni_comment->SyncConfigFile();
    //更新树
    GtkTreeModel *new_model1 = create_item_comment_model_selected();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);

    g_free(path_string);
    gtk_tree_path_next(path);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, TRUE);
    //在词条下移时，更新滚动条

    if(item_length-path_num >2) {
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
    } else {
      gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolledwindow_item_comment_selected));//保证在最下端的词条移动后能够正常显示
    }

    gtk_tree_path_free (path);
  }
}

void ViewSystem::ButtonDeleteClicked(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment_selected));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));

  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

  char* select_name;
  gtk_tree_model_get(model, &iter, 0, &select_name, -1);

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  if(!exam_type_name) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;
  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni_comment = &ini;

  const char* ptrSection = probe_exam.c_str();

  vector<ExamPara> vecDelete_Comment;
  vecDelete_Comment.clear();
  CreateItemList_Delete_Comment1(select_name, probe_exam, vecDelete_Comment);

  int item_length(0);
  item_length = vecDelete_Comment.size();
  string department_copy;
  department_copy= ptrIni_comment->ReadString(probe_exam.c_str(), "Department");
  ptrIni_comment->RemoveGroup(probe_exam.c_str());
  ptrIni_comment->SyncConfigFile();

  for(int i=0; i<item_length; i++) {
    char NoteNumber[256];
    sprintf(NoteNumber, "Note%d", i+1);
    ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
  }

  ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
  ptrIni_comment->WriteString(probe_exam.c_str(), "Department", department_copy.c_str());
  ptrIni_comment->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);
  //设置光标，更新滚动条的值
  gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, TRUE);
  gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(m_scrolledwindow_item_comment_selected));
  //gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment_selected), path, NULL, FALSE, 1.0, 1.0);
  gtk_tree_path_free (path);
}

void ViewSystem::ButtonDeleteAllClicked(GtkButton* button) {
  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));
  if(!exam_type_name) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index1] + "-" +exam_type;

  char path[256];
  sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path1[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini(path1);
  IniFile *ptrIni = &ini;

  const char* ptrSection = probe_exam.c_str();
  string department_copy;
  department_copy= ptrIni->ReadString(ptrSection, "Department");

  ptrIni->RemoveGroup(ptrSection);

  ptrIni->WriteInt(ptrSection, "Number", 0);
  ptrIni->WriteString(ptrSection, "Department", department_copy.c_str());
  ptrIni->SyncConfigFile();

  GtkTreeModel *new_model1 = create_item_comment_model_selected();
  gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment_selected), new_model1);
}

void ViewSystem::BtnCommentDefaultClicked(GtkButton* button) {
  SysNoteSetting* sysNote = new SysNoteSetting();
  sysNote->DefaultFactory();
  init_comment_setting(sysNote);
}

void ViewSystem::CellRendererRenameSelectComment(GtkCellRendererText* cellrenderer, gchar* path_str, gchar* new_text) {
  char *old_text;

  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, 0, &old_text, -1);
  gtk_tree_path_free(path);

  //如果重命名前后renderertext中的文本没有改变,则返回
  if (strcmp(new_text, old_text) == 0) {
    return;
  }

  if (strcmp(new_text, "") == 0) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

    return;
  }

  int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
  if(department_index ==-1) {
    return;
  }

  string department = DepartmentName(department_index);
  char path2[256];
  sprintf(path2, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
  IniFile ini(path2);
  IniFile *ptrIni_comp= &ini;
  int num;
  num= ptrIni_comp->ReadInt(department, "Number");
  vector<ExamPara> vecItemNote;
  vecItemNote.clear();
  CreateItemList_Note(department, vecItemNote, num);

  int note_item_length(0);
  note_item_length = vecItemNote.size();

  if(!vecItemNote.empty()) {
    for(int i=0; i<note_item_length; i++) {
      if(( strcmp(old_text, vecItemNote[i].name.c_str()) == 0)||(strcmp(old_text, _(vecItemNote[i].name.c_str())) == 0)) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
          MessageDialog::DLG_INFO,  _("Only Userdefined items can be renamed!"), NULL);
        return;
      }
    }
  }

  char path1[256];
  sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
  IniFile new_ini(path1);
  IniFile *ptrIni= &new_ini;
  int number;
  number = ptrIni->ReadInt(department, "Number");
  vector<ExamPara> vecItemComment;
  vecItemComment.clear();
  CreateItemList_Comment(department, vecItemComment, number);

  int item_length(0);
  item_length = vecItemComment.size();

  int Num_tmp;

  if(!vecItemComment.empty()) {
    for(int i=0; i<item_length; i++) {
      if(strcmp(new_text, vecItemComment[i].name.c_str()) == 0) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO,  _("Fail to rename! The item name has existed!"), NULL);
        return;
      }
    }

    for(int i=0; i<item_length; i++) {
      if( strcmp(old_text, vecItemComment[i].name.c_str()) == 0) {
        Num_tmp=i+1;
      }
    }
  }

  gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);

  vector<ExamPara> vecDelete_Comment;
  vecDelete_Comment.clear();
  CreateItemList_Delete_Comment(old_text, department, vecDelete_Comment);

  int delete_length(0);
  delete_length = vecDelete_Comment.size();
  ptrIni->RemoveGroup(department);
  ptrIni->SyncConfigFile();

  for(int i=1; i<number; i++) {
    char NoteNumber[256];
    if(Num_tmp > i) {
      sprintf(NoteNumber, "Note%d", i);
      ptrIni->WriteString(department, NoteNumber, vecDelete_Comment[i-1].name.c_str());

    } else {
      sprintf(NoteNumber, "Note%d", i+1);
      ptrIni->WriteString(department, NoteNumber, vecDelete_Comment[i-1].name.c_str());
    }
  }

  char New_Note[256];
  sprintf(New_Note, "Note%d", Num_tmp);
  ptrIni->WriteString(department, New_Note, new_text);
  ptrIni->WriteInt(department, "Number", number);
  ptrIni->SyncConfigFile();
}

void ViewSystem::CellRendererRenameComment(GtkCellRendererText* cellrenderer, gchar* path_str, gchar* new_text) {
  char *old_text;

  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment_selected));
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, 0, &old_text, -1);
  gtk_tree_path_free(path);

  if (strcmp(new_text, old_text) == 0) {
    return;
  }

  if (strcmp(new_text, "") == 0) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

    return;
  }

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  if(!exam_type_name) {
    return;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);

  string probe_exam = g_probe_list[index1] + "-" +exam_type;
  vector<ExamPara> vecItem_Comment;
  vecItem_Comment.clear();
  CreateItemList_Comment1(probe_exam, vecItem_Comment);

  int item_length(0);
  item_length = vecItem_Comment.size();

  int Num_tmp;

  if(!vecItem_Comment.empty()) {
    for(int i=0; i<item_length; i++) {
      if( strcmp(new_text, vecItem_Comment[i].name.c_str()) == 0) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO,  _("Fail to rename! The item name has existed!"), NULL);
        return;
      }
    }

    for(int i=0; i<item_length; i++) {
      if( strcmp(old_text, vecItem_Comment[i].name.c_str()) == 0) {
        Num_tmp=i+1;
      }
    }
  }

  gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);

  char path11[256];
  sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
  IniFile ini1(path11);
  string username;
  username = exam.ReadDefaultUserSelect(&ini1);

  char path_comment[256];
  if(strcmp(username.c_str(), "System Default") == 0) {
    sprintf(path_comment, "%s%s", CFG_RES_PATH, COMMENT_FILE);
  } else {
    sprintf(path_comment, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
  }

  IniFile ini_comment(path_comment);
  IniFile *ptrIni_comment= &ini_comment;

  int DeleteNum;
  DeleteNum = ptrIni_comment->ReadInt(probe_exam.c_str(), "Number");

  vector<ExamPara> vecDelete_Comment;
  vecDelete_Comment.clear();
  CreateItemList_Delete_Comment1(old_text, probe_exam, vecDelete_Comment);

  int delete_length(0);
  delete_length = vecDelete_Comment.size();
  ptrIni_comment->RemoveGroup(probe_exam.c_str());
  ptrIni_comment->SyncConfigFile();

  for(int i=1; i<DeleteNum; i++) {
    char NoteNumber[256];
    if(Num_tmp > i) {
      sprintf(NoteNumber, "Note%d", i);
      ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i-1].name.c_str());

    } else {
      sprintf(NoteNumber, "Note%d", i+1);
      ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i-1].name.c_str());
    }
  }

  char New_Note[256];
  sprintf(New_Note, "Note%d", Num_tmp);
  ptrIni_comment->WriteString(probe_exam.c_str(), New_Note, new_text);
  ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", DeleteNum);
  ptrIni_comment->SyncConfigFile();
}

// Peripheral
void ViewSystem::BtnComboVideoChanged() {
  FpgaCtrl2D* fpga2d=new FpgaCtrl2D;
  gint data=gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video));
  fpga2d->VideoStandard((INT16U)(data));
}

void ViewSystem::BtnTVOutDefaultClicked(GtkButton* button) {
  SysGeneralSetting *sysGeneralSetting = new SysGeneralSetting();

  char printer_path[3];
  int index_printer = sysGeneralSetting->GetPrinter();
  sprintf(printer_path, "%d", index_printer);
  GtkTreePath *path = gtk_tree_path_new_from_string(printer_path);
  gtk_tree_selection_select_path(m_selected_common_printer, path);

  sysGeneralSetting->TVOutDefaultFactory();
  init_tvout_setting(sysGeneralSetting);
}

void ViewSystem::CommonRadioToggled(GtkToggleButton* togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    gtk_widget_show_all(GTK_WIDGET(m_scrolledwindow_common));
    gtk_widget_hide_all(GTK_WIDGET(m_scrolledwindow_specific));
  }
}

void ViewSystem::SpecificRadioToggled(GtkToggleButton* togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    gtk_widget_hide_all(GTK_WIDGET(m_scrolledwindow_common));
    gtk_widget_show_all(GTK_WIDGET(m_scrolledwindow_specific));
  }
}

void ViewSystem::BtnAddPrinterClicked(GtkButton* button) {
  ViewPrinterAdd::GetInstance()->CreateWindow(GTK_WINDOW(m_dialog));
}

void ViewSystem::BtnDelPrinterClicked(GtkButton* button) {
  string prnt_name = specific_printer_selection();
  if (!prnt_name.empty()) {
    Printer prnt;
    prnt.DeletePrinter(prnt_name.c_str());
    PeripheralMan::GetInstance()->SwitchPrinterDriver();
  }

  UpdateSpecificPrinterModel();
}

// Custom Report
void ViewSystem::BtnRecoveryClicked(GtkButton* button) {
  bool ret = false;
  if (!remove(CustomReport_PATH) && InitDB()) {
    ret = true;
  }

  gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));

  gtk_widget_show(GTK_WIDGET(m_button_report_add));
  gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
  gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  gtk_widget_hide(GTK_WIDGET(m_label_childsection));
  gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
  gtk_widget_hide(GTK_WIDGET(m_label_templet));
  gtk_widget_hide(GTK_WIDGET(m_entry_templet));
  gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
  gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));

  GtkTreeModel *modelnew = CreateTreeModel();
  gtk_tree_view_set_model(m_treeviewReportSet, modelnew);
  g_object_unref (modelnew);

  GtkTreePath *path = gtk_tree_path_new_first();
  gtk_tree_view_set_cursor(m_treeviewReportSet, path, NULL, TRUE);
  gtk_tree_path_free(path);

  if(ret) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),  MessageDialog::DLG_INFO,  _("Recovery succeeded!"), NULL); //Recovery succeed! Restart request!
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Recovery failed!"), NULL);
  }
}

void ViewSystem::BtnAddClicked(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  if(tree_depth == 2) {
    gtk_widget_hide(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    gtk_widget_hide(GTK_WIDGET(m_label_templet));
    gtk_widget_hide(GTK_WIDGET(m_entry_templet));
    gtk_widget_show(GTK_WIDGET(m_label_childsection));
    gtk_widget_show(GTK_WIDGET(m_combobox_childsection));
    gtk_widget_show(GTK_WIDGET(m_button_report_ok));
    gtk_widget_show(GTK_WIDGET(m_button_report_cancel));
  } else if(tree_depth == 1) {
    gtk_widget_hide(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    gtk_widget_hide(GTK_WIDGET(m_label_childsection));
    gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
    gtk_widget_show(GTK_WIDGET(m_label_templet));
    gtk_widget_show(GTK_WIDGET(m_entry_templet));
    gtk_widget_grab_focus(GTK_WIDGET(m_entry_templet));
    gtk_widget_show(GTK_WIDGET(m_button_report_ok));
    gtk_widget_show(GTK_WIDGET(m_button_report_cancel));
  }
}

void ViewSystem::BtnSave2Clicked(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) return;

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  char *sections, *templet, *childsection, *temp, record[1024];
  int start, end;
  if (tree_depth == 3) {
    GtkTreeIter iter_parent0;
    GtkTreeIter iter_parent1;
    gtk_tree_model_get(model, &iter, 0, &childsection, -1);
    gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
    gtk_tree_model_get(model, &iter_parent0, 0, &temp, -1);
    gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
    gtk_tree_model_get(model, &iter_parent1, 0, &sections, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str())) {
      templet = &temp[DEFAULT_FLAG.size() + 1];
    } else {
      templet = temp;
    }

    ostringstream stream;
    memset(record, 0, sizeof(record));
    if(!InitRecordFromShowArr(childsection, &start, &end))return;
    memcpy(record, &ShowArr[start], end - start);

    if(StrCmpSectionString(OB_M, childsection, NULL)) {
      //Fetal 2
      memcpy(&record[OB_CALC_COUNT], &ShowArr[MAX_CALC_COUNT],  OB_CALC_COUNT);
      //EFW
      memcpy(&record[(OB_CALC_COUNT)*2], &ShowArr[EFW_START_ID], EFW_COUNT);
      //average
      memcpy(&record[(OB_CALC_COUNT)*2 + EFW_COUNT], &ShowArr[AVERRES_START_ID], AverRes_COUNT);
    }

    if(StrCmpSectionString(UR_M, childsection, NULL)) {
      //PSAD
      memcpy(&record[end-start], &ShowArr[PSAD_START_ID], PSAD_COUNT);
    }

    if(StrCmpSectionString(OTHERS_M, childsection, NULL)) {
      //Ohters
      memcpy(&record[end-start], &ShowArr[OTHERS_START_ID], OTHERS_COUNT);
    }

    stream << "UPDATE CustomReport SET record = '" << record << "' WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
    string sql = stream.str();
    char *errmsg = NULL;
    OpenDB();

    if (sqlite3_exec(CustomReport_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Database error!"), NULL);
      PRINTF("save error:%s!\n", errmsg);
    }

    CloseDB();
  }
}

void ViewSystem::BtnDeleteClicked(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) return;
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  char *sections, *templet, *childsection, *temp;
  ostringstream stream;

  if (tree_depth == 1) {
    return;
  } else if (tree_depth == 2) {
    GtkTreeIter parent;
    gtk_tree_model_get(model, &iter, 0, &temp, -1);
    gtk_tree_model_iter_parent(model, &parent, &iter);
    gtk_tree_model_get(model, &parent, 0, &sections, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str())) {
      templet = &temp[DEFAULT_FLAG.size() + 1];
    } else {
      templet = temp;
    }

    int nChild = gtk_tree_model_iter_n_children(model, &parent);
    if (nChild == 1) {
      stream << "UPDATE CustomReport SET templet = '' " << "WHERE sections = '" << sections << "'";
    } else if (nChild > 1) {
      stream << "DELETE FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "'";
    } else {
      PRINTF("no node to delete\n");
    }
  } else if (tree_depth == 3) {
    GtkTreeIter parent1, parent2;
    gtk_tree_model_get(model, &iter, 0, &childsection, -1);
    gtk_tree_model_iter_parent(model, &parent1, &iter);
    gtk_tree_model_get(model, &parent1, 0, &temp, -1);
    gtk_tree_model_iter_parent(model, &parent2, &parent1);
    gtk_tree_model_get(model, &parent2, 0, &sections, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str())) {
      templet = &temp[DEFAULT_FLAG.size() + 1];
    } else {
      templet = temp;
    }

    int nChild = gtk_tree_model_iter_n_children(model, &parent1);
    if (nChild == 1) {
      stream << "UPDATE CustomReport SET childsection = '' " << "WHERE sections = '" << sections << "' AND templet = '" << templet << "'";
    } else if (nChild > 1) {
      stream << "DELETE FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
    } else {
      PRINTF("no node to delete\n");
    }
  }

  char *errmsg = NULL;
  string sql = stream.str();

  OpenDB();
  if (sqlite3_exec(CustomReport_db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
    PRINTF("DELETE ERROR:%s!\n", errmsg);
    return;
  }

  CloseDB();

  if (gtk_tree_model_iter_has_child(model, &iter) == FALSE) {
    gtk_widget_show_all(GTK_WIDGET(m_scrolled_window_show));
  }

  GtkTreeIter parent_iter;
  gtk_tree_model_iter_parent(model, &parent_iter, &iter);
  GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);
  gtk_tree_store_remove(GTK_TREE_STORE(model), &topIter);

  GtkTreeModel *modelnew = CreateTreeModel();
  gtk_tree_view_set_model(m_treeviewReportSet, modelnew);
  g_object_unref (modelnew);

  gtk_tree_view_expand_to_path(m_treeviewReportSet, parent_path);
  gtk_tree_view_set_cursor(m_treeviewReportSet, parent_path, NULL, TRUE);
  gint parent_depth = gtk_tree_path_get_depth(parent_path);
  gtk_tree_path_free (parent_path);

  gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));

  if(parent_depth == 1) {
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
  } else if(parent_depth == 2) {
    gtk_widget_show(GTK_WIDGET(m_button_report_delete));
  }

  gtk_widget_show(GTK_WIDGET(m_button_report_add));
  gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  gtk_widget_hide(GTK_WIDGET(m_label_childsection));
  gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));

  gtk_widget_hide(GTK_WIDGET(m_label_templet));
  gtk_widget_hide(GTK_WIDGET(m_entry_templet));
  gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
  gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));
}

void ViewSystem::BtnOkClicked(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeModel *model;

  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  char *sections, *templet, *temp, *new_string, buf[256];
  ostringstream stream;

  if (tree_depth == 1) {
    gtk_tree_model_get(model, &iter, 0, &sections, -1);
    templet = (char *)gtk_entry_get_text(GTK_ENTRY(m_entry_templet));

    memset(buf, 0, sizeof(buf));
    if(strlen(templet)) {
      int k, t;
      char buffer[256];
      sprintf(buffer, "%s", templet);

      for(k = 0,t = 0; '\0' != buffer[k]; k++) {
        // ascii code 32 is space
        if(32 != buffer[k]) {
          buf[t++] = buffer[k];
        } else if(0 != t) {
          if(32 != buf[t-1])buf[t++] = buffer[k];
        }
      }

      if(t > 1) {
        if(32 == buf[t-1])buf[t-1] = '\0';
      }

      buf[t] = '\0';
    }

    new_string = buf;

    if(!(strlen(new_string) > 0) || (!UniqueItem(sections, new_string))) {
      if(strlen(new_string) > 0) {
        GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
        gtk_tree_selection_select_iter(selected_node, &iter_tmp);
      }

      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
        MessageDialog::DLG_INFO,  _("Template name has existed or the input format is incorrect.Please input again!"), NULL);
      return;
    }

    if (gtk_tree_model_iter_has_child(model, &iter) == FALSE) {
      stream << "UPDATE CustomReport SET templet = '" << new_string << "' WHERE sections = '" << sections << "'";
    } else {
      stream << "INSERT INTO CustomReport VALUES('" << sections << "', '" << new_string << "', '', '')";
    }
  } else if (tree_depth == 2) {
    GtkTreeIter parent;
    gtk_tree_model_get(model, &iter, 0, &temp, -1);
    gtk_tree_model_iter_parent(model, &parent, &iter);
    gtk_tree_model_get(model, &parent, 0, &sections, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str())) {
      templet = &temp[DEFAULT_FLAG.size() + 1];
    } else {
      templet = temp;
    }

    unsigned int child = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_childsection));
    new_string = (char*)SECTION_STR[child][init_language].c_str();
    for(int language = 0; language < LANG_MAX; language++) {
      if(!UniqueItem(sections, templet, SECTION_STR[child][language])) {
        GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
        gtk_tree_selection_select_iter(selected_node, &iter_tmp);
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO,  _("Selected exam type has existed!"), NULL);
        return;
      }
    }

    if (gtk_tree_model_iter_has_child(model, &iter) == FALSE) {
      stream << "UPDATE CustomReport SET childsection = '" << new_string  << "' WHERE sections = '" << sections  << "' AND templet = '" << templet << "'";
    } else {
      stream << "INSERT INTO CustomReport VALUES('"  << sections << "', '"  << templet << "', '"  << SECTION_STR[child][init_language] << "', '')";
    }
  }

  char *errmsg = NULL;
  string sql = stream.str();
  OpenDB();
  if (sqlite3_exec(CustomReport_db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
    PRINTF("INSERT or UPDATE error:%s!\n", errmsg);
    return;
  }

  CloseDB();

  GtkTreeIter iter_new;
  gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
  gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
  GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);
  gtk_tree_view_expand_to_path(m_treeviewReportSet, new_path);
  GtkTreeViewColumn *column_tree_view = gtk_tree_view_get_column(m_treeviewReportSet, 0);
  gtk_tree_view_set_cursor_on_cell(m_treeviewReportSet,  new_path, column_tree_view,  m_renderer,  TRUE);
  gtk_tree_path_free (new_path);

  selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
    return;
  }

  path = gtk_tree_model_get_path(model, &iter);
  tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  if(tree_depth == 1) {
    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  } else if(tree_depth == 2) {
    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_show(GTK_WIDGET(m_button_report_delete));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  } else {
    gtk_widget_hide(GTK_WIDGET(m_button_report_add));
    gtk_widget_show(GTK_WIDGET(m_button_report_delete));
    gtk_widget_show(GTK_WIDGET(m_button_report_save));
  }

  gtk_widget_hide(GTK_WIDGET(m_label_childsection));
  gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
  gtk_widget_hide(GTK_WIDGET(m_label_templet));
  gtk_widget_hide(GTK_WIDGET(m_entry_templet));
  gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
  gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));
}

void ViewSystem::BtnCancelClicked(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
  if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  gint tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  if(tree_depth == 1) {
    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
  } else if(tree_depth == 2) {
    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_show(GTK_WIDGET(m_button_report_delete));
  } else {
    gtk_widget_hide(GTK_WIDGET(m_button_report_add));
    gtk_widget_show(GTK_WIDGET(m_button_report_delete));
  }

  gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  gtk_widget_hide(GTK_WIDGET(m_label_childsection));
  gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
  gtk_widget_hide(GTK_WIDGET(m_label_templet));
  gtk_widget_hide(GTK_WIDGET(m_entry_templet));
  gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
  gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));
}

void ViewSystem::EntrytempletInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

  if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
    gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
  }
}

void ViewSystem::TreeSelectionChanged(GtkTreeSelection* selection) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
    return;
  }

  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
  int tree_depth = gtk_tree_path_get_depth(path);
  gtk_tree_path_free (path);

  char *sections = NULL, *templet = NULL, *childsection = NULL, *temp = NULL;
  ostringstream stream;

  if(tree_depth == 1) {
    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
    gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    gtk_widget_hide(GTK_WIDGET(m_label_childsection));
    gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
    gtk_widget_hide(GTK_WIDGET(m_label_templet));
    gtk_widget_hide(GTK_WIDGET(m_entry_templet));
    gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
    gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));

    gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));

    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), FALSE);
  } else if(tree_depth == 2) {
    gtk_tree_model_get(model, &iter, 0, &temp, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str()) || StrCmpTemplet1String(temp, NULL)) {
      gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    } else {
      gtk_widget_show(GTK_WIDGET(m_button_report_delete));
    }

    gtk_widget_show(GTK_WIDGET(m_button_report_add));
    gtk_widget_hide(GTK_WIDGET(m_button_report_save));
    gtk_widget_hide(GTK_WIDGET(m_label_childsection));
    gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
    gtk_widget_hide(GTK_WIDGET(m_label_templet));
    gtk_widget_hide(GTK_WIDGET(m_entry_templet));
    gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
    gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));

    gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));

    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), TRUE);
  } else if (tree_depth == 3) {
    GtkTreeIter iter_parent0;
    GtkTreeIter iter_parent1;
    gtk_tree_model_get(model, &iter, 0, &childsection, -1);
    gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
    gtk_tree_model_get(model, &iter_parent0, 0, &temp, -1);
    gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
    gtk_tree_model_get(model, &iter_parent1, 0, &sections, -1);

    if(NULL != strstr(temp, DEFAULT_FLAG.c_str())) {
      templet = &temp[DEFAULT_FLAG.size() + 1];
    } else {
      templet = temp;
    }

    ReadRecordFromDB(sections, templet, childsection);
    gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));

    if (m_show_window != NULL) {
      gtk_container_remove(GTK_CONTAINER(m_scrolled_window_show), m_show_window);
      m_show_window = NULL;
    }

    if(StrCmpSectionString(ABDO_M, childsection, NULL)) {
      m_show_window = CreatWindowABD();
    }

    if(StrCmpSectionString(ADULT_M, childsection, NULL)) {
      m_show_window = CreatWindowAE();
    }

    if(StrCmpSectionString(UR_M, childsection, NULL)) {
      m_show_window = CreatWindowUR();
    }

    if(StrCmpSectionString(OB_M, childsection, NULL)) {
      m_show_window = CreatWindowOB();
    }

    if(StrCmpSectionString(GYN_M, childsection, NULL)) {
      m_show_window = CreatWindowGYN();
    }

    if(StrCmpSectionString(SP_M, childsection, NULL)) {
      m_show_window = CreatWindowSP();
    }

    if(StrCmpSectionString(VS_M, childsection, NULL)) {
      m_show_window = CreatWindowFE();
    }

    if(StrCmpSectionString(FETAL_M, childsection, NULL)) {
      m_show_window = CreatWindowVES();
    }

    if(StrCmpSectionString(OTHERS_M, childsection, NULL)) {
      m_show_window = CreatWindowOTHERS();
    }

    if(m_show_window != NULL) {
      gtk_container_add(GTK_CONTAINER(m_scrolled_window_show), m_show_window);
      gtk_widget_show_all(GTK_WIDGET(m_scrolled_window_show));
    }

    int start, end;
    if(InitRecordFromShowArr(childsection, &start, &end)) {
      for(int id = start; id < end; id++) {
        if(CheckButtonArr[id]) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
        }
      }
    }

    if(StrCmpSectionString(OB_M, childsection, NULL)) {
      //Fetal 2
      for(int id = MAX_CALC_COUNT; id < MAX_CALC_COUNT+OB_CALC_COUNT; id++) {
        if(CheckButtonArr[id]) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
        }
      }

      //EFW
      for(int id = EFW_START_ID; id < EFW_START_ID + EFW_COUNT; id++) {
        if(CheckButtonArr[id]) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
        }
      }

      //Average
      for(int id = AVERRES_START_ID; id < AVERRES_START_ID + AverRes_COUNT; id++) {
        if(CheckButtonArr[id]) {
          if(id == AVERRES_START_ID + 2 || id == AVERRES_START_ID + 3) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id+2]));
          } else {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
          }
        }
      }
    }

    //PSAD
    if(StrCmpSectionString(UR_M, childsection, NULL)) {
      for(int id = PSAD_START_ID; id < PSAD_START_ID + PSAD_COUNT; id++) {
        if(CheckButtonArr[id]) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
        }
      }
    }

    //Others
    if(StrCmpSectionString(OTHERS_M, childsection, NULL)) {
      for(int id = OTHERS_START_ID; id < OTHERS_END_ID; id++) {
        if(CheckButtonArr[id]) {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
        }
      }
    }

    if(StrCmpTemplet1String(temp, NULL)) {
      gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
    } else {
      gtk_widget_show(GTK_WIDGET(m_button_report_delete));
    }

    gtk_widget_hide(GTK_WIDGET(m_button_report_add));
    gtk_widget_show(GTK_WIDGET(m_button_report_save));
    gtk_widget_hide(GTK_WIDGET(m_label_childsection));
    gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));
    gtk_widget_hide(GTK_WIDGET(m_label_templet));
    gtk_widget_hide(GTK_WIDGET(m_entry_templet));
    gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
    gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_apply), FALSE);
  }
}

// DICOM
void ViewSystem::DicomnotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num) {
  switch(page_num) {
  case 0:
    DicomLocalSetting::GetInstance()->InitLocalSetting();
    break;
  case 1:
    DicomServerSetting::GetInstance()-> InitServerSetting();
    break;
  case 2:
    DicomServiceSetting::GetInstance()->InitServiceSetting();
    break;
  default:
    break;
  }
}

// P1-P3

// System Info
void ViewSystem::BtnUpgradeClicked(GtkButton* button) {
  UpgradeMan::GetInstance()->Upgrade();
}

void ViewSystem::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  switch(keyValue) {
  case KEY_ESC:
  case KEY_SYSTEM:
    if (!m_vgaInterl) {
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_vga), 0);
      break;
    }
  case KEY_P1:
    {
      g_timeout_add(100, signal_callback_exit_window_system, this);
      FakeEscKey();

      FreezeMode::GetInstance()->PressUnFreeze();
      MultiFuncUndo();
    }

    break;
  default:
    break;
  }
}

GtkWidget* ViewSystem::create_note_general() {
  GtkTable* table = Utils::create_table(10, 6);

  // Hospital Name
  GtkLabel* label_hospital = Utils::create_label(_("Hospital Name:"));
  m_entry_hospital = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_hospital), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_hospital), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_hospital, 46);
  g_signal_connect(G_OBJECT(m_entry_hospital), "insert_text", G_CALLBACK(signal_entry_insert_hospital), this);

  // Date and Time
  GtkFrame* frame_date_time = Utils::create_frame(_("Date and Time"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_date_time), 0, 6, 1, 6);

  GtkTable* table_frame = Utils::create_table(5, 6);
  gtk_container_set_border_width(GTK_CONTAINER(table_frame), 5);
  gtk_container_add(GTK_CONTAINER(frame_date_time), GTK_WIDGET(table_frame));

  m_calendar = gtk_calendar_new();
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_calendar), 0, 3, 0, 5);

  GtkLabel* label_time_setting = Utils::create_label(_("Time Setting:"));

  GtkAdjustment* adjustment_hour = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 23, 1, 1, 0));
  m_spinbutton_hour = Utils::create_spin_button(adjustment_hour, 1, 0);

  GtkAdjustment* adjustment_minute = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 59, 1, 1, 0));
  m_spinbutton_minute = Utils::create_spin_button(adjustment_minute, 1, 0);

  GtkAdjustment* adjustment_second = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 59, 1, 1, 0));
  m_spinbutton_second = Utils::create_spin_button(adjustment_second, 1, 0);

  GtkLabel* label_time_format = Utils::create_label(_("HH:MM:SS"));
  gtk_misc_set_alignment(GTK_MISC(label_time_format), 0, 0);

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_time_setting), 3, 5, 0, 1);
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_spinbutton_hour), 3, 4, 1, 2);
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_spinbutton_minute), 4, 5, 1, 2);
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_spinbutton_second), 5, 6, 1, 2);
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_time_format), 3, 5, 2, 3);

  time_t now;
  time(&now);
  tm* now_tm = localtime(&now);

  gtk_spin_button_set_value(m_spinbutton_hour, now_tm->tm_hour);
  g_signal_connect(G_OBJECT(m_spinbutton_hour), "insert_text", G_CALLBACK(signal_spinbutton_insert_hour), this);

  gtk_spin_button_set_value(m_spinbutton_minute, now_tm->tm_min);
  g_signal_connect(G_OBJECT(m_spinbutton_minute), "insert_text", G_CALLBACK(signal_spinbutton_insert_time), this);

  gtk_spin_button_set_value(m_spinbutton_second, now_tm->tm_sec);
  g_signal_connect(G_OBJECT(m_spinbutton_second), "insert_text", G_CALLBACK(signal_spinbutton_insert_time), this);

  GtkButton* button_adjust_time = Utils::create_button(_("Adjust Time&Date"));
  gtk_table_attach(table_frame, GTK_WIDGET(button_adjust_time), 3, 5, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_adjust_time, "clicked", G_CALLBACK (signal_button_clicked_adjust_time), this);

  // Language
  GtkLabel* label_language = Utils::create_label(_("Language:"));
  m_combobox_language = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_language), 0, 1, 6, 7);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_language), 1, 2, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_language, "English");
  gtk_combo_box_text_append_text(m_combobox_language, "中文");
  gtk_combo_box_text_append_text(m_combobox_language, "Русский язык");
  gtk_combo_box_text_append_text(m_combobox_language, "Polski");
  gtk_combo_box_text_append_text(m_combobox_language, "Français");
  gtk_combo_box_text_append_text(m_combobox_language, "Español");
  gtk_combo_box_text_append_text(m_combobox_language, "Deutsch");

  g_signal_connect(GTK_COMBO_BOX(m_combobox_language), "changed", G_CALLBACK (signal_combobox_changed_language), this);

  // VGA Source
  GtkLabel* label_vga = Utils::create_label(_("VGA Source:"));
  m_combobox_vga = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_vga), 2, 3, 6, 7);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_vga), 3, 4, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_vga, _("Internal"));
  gtk_combo_box_text_append_text(m_combobox_vga, _("External"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_vga), 0);
  g_signal_connect(GTK_COMBO_BOX(m_combobox_vga), "changed", G_CALLBACK (signal_combobox_changed_vga), this);

  // Date Format
  GtkLabel* label_date_format = Utils::create_label(_("Date Format:"));
  m_combobox_date_format = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_date_format), 0, 1, 7, 8);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_date_format), 1, 2, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_date_format, _("Y/M/D"));
  gtk_combo_box_text_append_text(m_combobox_date_format, _("M/D/Y"));
  gtk_combo_box_text_append_text(m_combobox_date_format, _("D/M/Y"));

  // Screen saver
  GtkLabel* label_screen_saver = Utils::create_label(_("Screen Saver:"));
  m_combobox_screen_saver = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_screen_saver), 2, 3, 7, 8);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_screen_saver), 3, 4, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("none"));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("5 Min."));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("10 Min."));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("20 Min."));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("30 Min."));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("45 Min."));
  gtk_combo_box_text_append_text(m_combobox_screen_saver, _("60 Min."));

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 1, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_general_default), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_general_setting() {
  SysGeneralSetting setting;

  gtk_entry_set_text(m_entry_hospital, setting.GetHospital().c_str());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_language), setting.GetLanguage());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), setting.GetDateFormat());

  int index_screen_saver = 0;

  switch (setting.GetScreenProtect()) {
  case 0:
    index_screen_saver = 0;
    break;
  case 5:
    index_screen_saver = 1;
    break;
  case 10:
    index_screen_saver = 2;
    break;
  case 20:
    index_screen_saver = 3;
    break;
  case 30:
    index_screen_saver = 4;
    break;
  case 45:
    index_screen_saver = 5;
    break;
  case 60:
    index_screen_saver = 6;
    break;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_screen_saver), index_screen_saver);
}

void ViewSystem::save_general_setting() {
  SysGeneralSetting setting;

  string hospital_name = gtk_entry_get_text(m_entry_hospital);

  if (!hospital_name.empty()) {
    setting.SetHospital(hospital_name);
  }

  int langage = setting.GetLanguage();
  int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));

  if (langage != index_lang) {
    string str_cmd = "setxkbmap -layout ";

    if (index_lang == ZH) {
      setenv("LANG", "zh_CN.UTF-8", 1);
      setenv("LANGUAGE", "zh_CN:zh", 1);
      setlocale(LC_ALL, "zh_CN.UTF-8");
      _system_(str_cmd + "us");
    } else if (index_lang == RU) {
      setenv("LANG", "ru_RU.UTF-8", 1);
      setenv("LANGUAGE", "ru_RU:ru", 1);
      setlocale(LC_ALL, "ru_RU.UTF-8");
      _system_(str_cmd + "ru");
    } else if (index_lang == PL) {
      setenv("LANG", "pl_PL.UTF-8", 1);
      setenv("LANGUAGE", "pl_PL:pl", 1);
      setlocale(LC_ALL, "pl_PL.UTF-8");
      _system_(str_cmd + "pl qwertz");
    } else if (index_lang == ES) {
      setenv("LANG", "es_ES.UTF-8", 1);
      setenv("LANGUAGE", "es_ES:es", 1);
      setlocale(LC_ALL, "es_ES.UTF-8");
      _system_(str_cmd + "es");
    } else if (index_lang == FR) {
      setenv("LANG", "fr_FR.UTF-8", 1);
      setenv("LANGUAGE", "fr_FR:fr", 1);
      setlocale(LC_ALL, "fr_FR.UTF-8");
      _system_(str_cmd + "fr");
    } else if (index_lang == DE) {
      setenv("LANG", "de_DE.UTF-8", 1);
      setenv("LANGUAGE", "de_DE:de", 1);
      setlocale(LC_ALL, "de_DE.UTF-8");
      _system_(str_cmd + "de");
    } else {
      setenv("LANG", "en_US.UTF-8", 1);
      setenv("LANGUAGE", "en_US:en", 1);
      setlocale(LC_ALL, "en_US.UTF-8");
      _system_(str_cmd + "us");
    }

    ChangeKeymap();
    setting.SetLanguage(index_lang);
  }

  int date_format = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_date_format));
  setting.SetDateFormat(date_format);
  MenuArea::GetInstance()->UpdateLabel();
  KnobMenu::GetInstance()->Update();
  if(!g_review_pic) {
    UpdateHospitalandpart(date_format, hospital_name);
  }

  int screenSaverIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_screen_saver));
  setting.SetScreenProtect(screenSaverIndex);

  int screenSaver = 0;

  switch (screenSaverIndex) {
  case 0:
    screenSaver = 0;
    break;
  case 1:
    screenSaver = 5;
    break;
  case 2:
    screenSaver = 10;
    break;
  case 3:
    screenSaver = 20;
    break;
  case 4:
    screenSaver = 30;
    break;
  case 5:
    screenSaver = 45;
    break;
  case 6:
    screenSaver = 60;
    break;
  }

  ScreenSaver::GetInstance()->SetPeriod(screenSaver * 60);

  setting.SyncFile();

  // update calc menu
  g_menuCalc.UpdateLabel();
  g_menuMeasure.UpdateLabel();
  ViewNewPat::GetInstance()->UpdateNameLength();
}

GtkWidget* ViewSystem::create_note_options() {
  GtkTable* table = Utils::create_table(10, 6);

  // TI
  GtkFrame* frame_ti = Utils::create_frame(_("TI"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_ti), 0, 2, 0, 2);

  gtk_widget_set_sensitive(GTK_WIDGET(frame_ti), FALSE);

  GtkTable* table_ti = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_ti), 5);
  gtk_container_add(GTK_CONTAINER(frame_ti), GTK_WIDGET(table_ti));

  m_radiobutton_tic = Utils::create_radio_button(NULL, _("TIC"));
  GSList* radiobutton_ti_group = gtk_radio_button_get_group(m_radiobutton_tic);
  m_radiobutton_tib = Utils::create_radio_button(radiobutton_ti_group, _("TIB"));
  radiobutton_ti_group = gtk_radio_button_get_group(m_radiobutton_tib);
  m_radiobutton_tis = Utils::create_radio_button(radiobutton_ti_group, _("TIS"));

  gtk_table_attach_defaults(table_ti, GTK_WIDGET(m_radiobutton_tic), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_ti, GTK_WIDGET(m_radiobutton_tib), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_ti, GTK_WIDGET(m_radiobutton_tis), 2, 3, 0, 1);

  // Keyboard Sound
  GtkFrame* frame_keyboard_sound = Utils::create_frame(_("Keyboard Sound"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_keyboard_sound), 2, 4, 0, 2);

  GtkTable* table_keyboard_sound = Utils::create_table(1, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_keyboard_sound), 5);
  gtk_container_add(GTK_CONTAINER(frame_keyboard_sound), GTK_WIDGET(table_keyboard_sound));

  m_radiobutton_keyboard_sound_on = Utils::create_radio_button(NULL, _("ON"));
  GSList* radiobutton_keyboard_sound_group = gtk_radio_button_get_group(m_radiobutton_keyboard_sound_on);
  m_radiobutton_keyboard_sound_off = Utils::create_radio_button(radiobutton_keyboard_sound_group, _("OFF"));

  gtk_table_attach_defaults(table_keyboard_sound, GTK_WIDGET(m_radiobutton_keyboard_sound_on), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_keyboard_sound, GTK_WIDGET(m_radiobutton_keyboard_sound_off), 1, 2, 0, 1);

  // Center Line
  GtkFrame* frame_center_line = Utils::create_frame(_("Center Line"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_center_line), 4, 6, 0, 2);

  GtkTable* table_center_line = Utils::create_table(1, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_center_line), 5);
  gtk_container_add(GTK_CONTAINER(frame_center_line), GTK_WIDGET(table_center_line));

  m_radiobutton_center_line_on = Utils::create_radio_button(NULL, _("ON"));
  GSList* radiobutton_center_line_group = gtk_radio_button_get_group(m_radiobutton_center_line_on);
  m_radiobutton_center_line_off = Utils::create_radio_button(radiobutton_center_line_group, _("OFF"));

  gtk_table_attach_defaults(table_center_line, GTK_WIDGET(m_radiobutton_center_line_on), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_center_line, GTK_WIDGET(m_radiobutton_center_line_off), 1, 2, 0, 1);

  // Display Format
  GtkFrame* frame_display_format = Utils::create_frame(_("Display Format"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_display_format), 0, 4, 2, 6);

  GtkTable* table_display_format = Utils::create_table(2, 6);
  gtk_container_set_border_width(GTK_CONTAINER(table_display_format), 5);
  gtk_container_add(GTK_CONTAINER(frame_display_format), GTK_WIDGET(table_display_format));

  GtkLabel* label_display_format_m = Utils::create_label(_("M:"));

  m_radiobutton_display_format_m_1 = Utils::create_radio_button(NULL);
  GSList* radiobutton_display_format_m_group = gtk_radio_button_get_group(m_radiobutton_display_format_m_1);
  m_radiobutton_display_format_m_2 = Utils::create_radio_button(radiobutton_display_format_m_group);
  radiobutton_display_format_m_group = gtk_radio_button_get_group(m_radiobutton_display_format_m_2);
  m_radiobutton_display_format_m_3 = Utils::create_radio_button(radiobutton_display_format_m_group);
  radiobutton_display_format_m_group = gtk_radio_button_get_group(m_radiobutton_display_format_m_3);
  m_radiobutton_display_format_m_4 = Utils::create_radio_button(radiobutton_display_format_m_group);
  radiobutton_display_format_m_group = gtk_radio_button_get_group(m_radiobutton_display_format_m_4);
  m_radiobutton_display_format_m_5 = Utils::create_radio_button(radiobutton_display_format_m_group);

  GtkLabel* label_display_format_pw_cw = Utils::create_label(_("PW/CW"));

  m_radiobutton_display_format_pw_cw_1 = Utils::create_radio_button(NULL);
  GSList* radiobutton_display_format_pw_cw_group = gtk_radio_button_get_group(m_radiobutton_display_format_pw_cw_1);
  m_radiobutton_display_format_pw_cw_2 = Utils::create_radio_button(radiobutton_display_format_pw_cw_group);
  radiobutton_display_format_pw_cw_group = gtk_radio_button_get_group(m_radiobutton_display_format_pw_cw_2);
  m_radiobutton_display_format_pw_cw_3 = Utils::create_radio_button(radiobutton_display_format_pw_cw_group);
  radiobutton_display_format_pw_cw_group = gtk_radio_button_get_group(m_radiobutton_display_format_pw_cw_3);
  m_radiobutton_display_format_pw_cw_4 = Utils::create_radio_button(radiobutton_display_format_pw_cw_group);
  radiobutton_display_format_pw_cw_group = gtk_radio_button_get_group(m_radiobutton_display_format_pw_cw_4);
  m_radiobutton_display_format_pw_cw_5 = Utils::create_radio_button(radiobutton_display_format_pw_cw_group);

  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(label_display_format_m), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_m_1), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_m_2), 2, 3, 0, 1);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_m_3), 3, 4, 0, 1);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_m_4), 4, 5, 0, 1);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_m_5), 5, 6, 0, 1);

  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(label_display_format_pw_cw), 0, 1, 1, 2);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_pw_cw_1), 1, 2, 1, 2);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_pw_cw_2), 2, 3, 1, 2);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_pw_cw_3), 3, 4, 1, 2);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_pw_cw_4), 4, 5, 1, 2);
  gtk_table_attach_defaults(table_display_format, GTK_WIDGET(m_radiobutton_display_format_pw_cw_5), 5, 6, 1, 2);

  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_m_1), Utils::create_image("./res/btn_format/1.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_m_2), Utils::create_image("./res/btn_format/2.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_m_3), Utils::create_image("./res/btn_format/3.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_m_4), Utils::create_image("./res/btn_format/4.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_m_5), Utils::create_image("./res/btn_format/5.jpg", 30, 30));

  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_pw_cw_1), Utils::create_image("./res/btn_format/1.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_pw_cw_2), Utils::create_image("./res/btn_format/2.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_pw_cw_3), Utils::create_image("./res/btn_format/3.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_pw_cw_4), Utils::create_image("./res/btn_format/4.jpg", 30, 30));
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_display_format_pw_cw_5), Utils::create_image("./res/btn_format/5.jpg", 30, 30));

  // Mouse Speed
  GtkFrame* frame_mouse_speed = Utils::create_frame(_("Mouse Speed"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_mouse_speed), 4, 6, 2, 6);

  GtkTable* table_mouse_speed = Utils::create_table(3, 1);
  gtk_container_set_border_width(GTK_CONTAINER(table_mouse_speed), 5);
  gtk_container_add(GTK_CONTAINER(frame_mouse_speed), GTK_WIDGET(table_mouse_speed));

  m_radiobutton_mouse_speed_high = Utils::create_radio_button(NULL, _("High"));
  GSList* radiobutton_mouse_speed_group = gtk_radio_button_get_group(m_radiobutton_mouse_speed_high);
  m_radiobutton_mouse_speed_middle = Utils::create_radio_button(radiobutton_mouse_speed_group, _("Middle"));
  radiobutton_mouse_speed_group = gtk_radio_button_get_group(m_radiobutton_mouse_speed_middle);
  m_radiobutton_mouse_speed_low = Utils::create_radio_button(radiobutton_mouse_speed_group, _("Low"));

  gtk_table_attach_defaults(table_mouse_speed, GTK_WIDGET(m_radiobutton_mouse_speed_high), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_mouse_speed, GTK_WIDGET(m_radiobutton_mouse_speed_middle), 0, 1, 1, 2);
  gtk_table_attach_defaults(table_mouse_speed, GTK_WIDGET(m_radiobutton_mouse_speed_low), 0, 1, 2, 3);

  // Image Options
  GtkFrame* frame_image_options = Utils::create_frame(_("Image Options"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_image_options), 0, 3, 6, 9);

  GtkTable* table_image_options = Utils::create_table(2, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_image_options), 5);
  gtk_container_add(GTK_CONTAINER(frame_image_options), GTK_WIDGET(table_image_options));

  GtkLabel* label_img_format = Utils::create_label(_("Storage format:"));
  m_combobox_img_format = Utils::create_combobox_text();
  GtkLabel* label_img_file = Utils::create_label(_("File name:"));
  m_combobox_img_file = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_image_options, GTK_WIDGET(label_img_format), 0, 1, 0, 1);
  gtk_table_attach(table_image_options, GTK_WIDGET(m_combobox_img_format), 1, 2, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_image_options, GTK_WIDGET(label_img_file), 0, 1, 1, 2);
  gtk_table_attach(table_image_options, GTK_WIDGET(m_combobox_img_file), 1, 2, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_img_format, _("BMP"));
  gtk_combo_box_text_append_text(m_combobox_img_format, _("JPEG"));
  gtk_combo_box_text_append_text(m_combobox_img_format, _("EMP"));

  gtk_combo_box_text_append_text(m_combobox_img_file, _("Manual"));
  gtk_combo_box_text_append_text(m_combobox_img_file, _("Auto"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_file), 0);

  // Video Options
  GtkFrame* frame_video_options = Utils::create_frame(_("Video Options"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_video_options), 3, 6, 6, 9);

  GtkTable* table_video_options = Utils::create_table(2, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_video_options), 5);
  gtk_container_add(GTK_CONTAINER(frame_video_options), GTK_WIDGET(table_video_options));

  GtkLabel* label_video_format = Utils::create_label(_("Storage format:"));
  m_combobox_video_format = Utils::create_combobox_text();
  GtkLabel* label_video_file = Utils::create_label(_("File name:"));
  m_combobox_video_file = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_video_options, GTK_WIDGET(label_video_format), 0, 1, 0, 1);
  gtk_table_attach(table_video_options, GTK_WIDGET(m_combobox_video_format), 1, 2, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_video_options, GTK_WIDGET(label_video_file), 0, 1, 1, 2);
  gtk_table_attach(table_video_options, GTK_WIDGET(m_combobox_video_file), 1, 2, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_video_format, _("AVI"));
  gtk_combo_box_text_append_text(m_combobox_video_format, _("CINE"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_format), 1);

  gtk_combo_box_text_append_text(m_combobox_video_file, _("Manual"));
  gtk_combo_box_text_append_text(m_combobox_video_file, _("Auto"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_file), 0);

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 1, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_options_default), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_option_setting(SysOptions* sysOptions) {
  if (sysOptions == NULL) {
    sysOptions = new SysOptions();
  }

  int index_TI_type = sysOptions->GetTI();

  if (index_TI_type == 0) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_tic), TRUE);
  } else if (index_TI_type == 1) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_tib), TRUE);
  } else if (index_TI_type == 2) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_tis), TRUE);
  }

  int index_keyboard_sound = sysOptions->GetKeyboardSound();

  if (index_keyboard_sound == 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_keyboard_sound_on), TRUE);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_keyboard_sound_off), TRUE);
  }

  int index_center_display = sysOptions->GetCenterLine();

  if (index_center_display == 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_line_on), TRUE);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_line_off), TRUE);
  }

  int mIndex = sysOptions->GetDisplayFormatM();
  int pwIndex = sysOptions->GetDisplayFormatPW();

  if (mIndex == 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_1), TRUE);
  } else if (mIndex == 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_2), TRUE);
  } else if (mIndex == 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_3), TRUE);
  } else if (mIndex == 3) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_4), TRUE);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_5), TRUE);
  }

  if (pwIndex == 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_1), TRUE);
  } else if (pwIndex == 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_2), TRUE);
  } else if (pwIndex == 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_3), TRUE);
  } else if (pwIndex == 3) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_4), TRUE);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_5), TRUE);
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_format), sysOptions->GetImageFormat());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_file), sysOptions->GetImageAutoName());

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_format), sysOptions->GetCineFormat());
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_file), sysOptions->GetCineAutoName());

  int mouse_speed = sysOptions->GetMouseSpeed();

  if (mouse_speed <= 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_high), TRUE);
  } else if (mouse_speed == 1) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_middle), TRUE);
  } else if (mouse_speed >= 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_low), TRUE);
  }

  delete sysOptions;
}

void ViewSystem::save_option_setting() {
  SysOptions sysOptions;

  int TIIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_tic))) {
    TIIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_tib))) {
    TIIndex = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_tis))) {
    TIIndex = 2;
  } else {
  }

  sysOptions.SetTI(TIIndex);

  int kbSoundIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_keyboard_sound_on))) {
    kbSoundIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_keyboard_sound_off))) {
    kbSoundIndex = 1;
  } else {
  }

  sysOptions.SetKeyboardSound(kbSoundIndex);

  if (kbSoundIndex) {
    KeyboardSound(false);
  } else {
    KeyboardSound(true);
  }

  int centerLineIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_line_on))) {
    centerLineIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_line_off))) {
    centerLineIndex = 1;
  } else {
  }

  sysOptions.SetCenterLine(centerLineIndex);

  int mouseSpeed = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_high))) {
    mouseSpeed = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_middle))) {
    mouseSpeed = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_low))) {
    mouseSpeed = 2;
  }

  sysOptions.SetMouseSpeed(mouseSpeed);
  g_keyInterface.SetMouseSpeed(mouseSpeed);

  int mIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_1))) {
    mIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_2))) {
    mIndex = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_3))) {
    mIndex = 2;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_4))) {
    mIndex = 3;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_m_5))) {
    mIndex = 4;
  } else {
  }

  sysOptions.SetDisplayFormatM(mIndex);

  int pwIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_1))) {
    pwIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_2))) {
    pwIndex = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_3))) {
    pwIndex = 2;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_4))) {
    pwIndex = 3;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_display_format_pw_cw_5))) {
    pwIndex = 4;
  }

  sysOptions.SetDisplayFormatPW(pwIndex);

  int imgFormatIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_img_format));
  int imgFileIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_img_file));
  sysOptions.SetImageFormat(imgFormatIndex);
  sysOptions.SetImageAutoName(imgFileIndex);

  int videoFormatIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video_format));
  int videoFileIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video_file));
  sysOptions.SetCineFormat(videoFormatIndex);
  sysOptions.SetCineAutoName(videoFileIndex);

  sysOptions.SyncFile();

  ImgMan::GetInstance()->SetImgFormat(imgFormatIndex);
  ImgMan::GetInstance()->SetImgNameMode(imgFileIndex);
  VideoMan::GetInstance()->SetVideoFormat(videoFormatIndex);
  VideoMan::GetInstance()->SetVideoNameMode(videoFileIndex);
}

GtkWidget* ViewSystem::create_note_image() {
  GtkTable* table = Utils::create_table(10, 18);

  // User Select
  GtkLabel* label_user_select = Utils::create_label(_("User Select:"));
  m_comboboxentry_user_select = Utils::create_combobox_entry();
  GtkButton* button_delete_user = Utils::create_button();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_user_select), 6, 9, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_comboboxentry_user_select), 9, 15, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_delete_user), 15, 17, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkWidget* bin_entry_dia = gtk_bin_get_child (GTK_BIN(m_comboboxentry_user_select));
  gtk_entry_set_max_length(GTK_ENTRY(bin_entry_dia), 40);
  g_signal_connect(G_OBJECT(bin_entry_dia), "insert_text", G_CALLBACK(signal_entry_insert_text_userselect), this);
  g_signal_connect(G_OBJECT(bin_entry_dia), "focus-out-event", G_CALLBACK(signal_entry_focusout_userselect), this);

  Utils::set_button_image(button_delete_user, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_BUTTON)));
  g_signal_connect(button_delete_user, "clicked", G_CALLBACK(signal_button_clicked_delete_user), this);

  UserSelect::GetInstance()->read_default_username(GTK_COMBO_BOX(m_comboboxentry_user_select));
  UserSelect::GetInstance()->read_username_db(USERNAME_DB, GTK_COMBO_BOX(m_comboboxentry_user_select));
  UserSelect::GetInstance()->set_active_user(GTK_COMBO_BOX(m_comboboxentry_user_select), UserSelect::GetInstance()->get_active_user());

  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_user_select), exam.ReadDefaultUserIndex(&ini));
  g_signal_connect(GTK_COMBO_BOX(m_comboboxentry_user_select), "changed", G_CALLBACK(signal_combobox_changed_user_select), this);

  string default_probe = exam.ReadDefaultProbe(&ini);

  for (int i = 0; i < NUM_PROBE; i++) {
    if (default_probe == g_probe_list[i]) {
      save_itemIndex(i);

      break;
    }
  }

  // Probe Type
  GtkLabel* label_probe_type = Utils::create_label(_("Probe Type:"));
  m_combobox_probe_type = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_probe_type), 0, 3, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_probe_type), 3, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  for (int i = 0; i < NUM_PROBE; i++) {
    string probeName = ProbeMan::GetInstance()->VerifyProbeName(g_probe_list[i]);
    gtk_combo_box_text_append_text(m_combobox_probe_type, probeName.c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), 0);
  g_signal_connect(m_combobox_probe_type, "changed", G_CALLBACK(signal_combobox_changed_probe_type), this);

  // scrolled window
  GtkScrolledWindow* scrolledwindow_exam_type = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolledwindow_exam_type), 0, 6, 1, 8);

  vector<ExamItem::EItem> indexVec;
  GtkTreeModel* model = create_exam_item_model(indexVec);

  m_treeview_exam_type = Utils::create_tree_view(model);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_exam_type), GTK_WIDGET(m_treeview_exam_type));

  add_exam_item_column(m_treeview_exam_type);
  GtkTreeSelection* select = gtk_tree_view_get_selection(m_treeview_exam_type);
  gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

  g_signal_connect(select, "changed", G_CALLBACK(signal_combobox_changed_exam_type), this);

  gtk_widget_add_events(GTK_WIDGET(m_treeview_exam_type), gtk_widget_get_events(GTK_WIDGET(m_treeview_exam_type)) | GDK_BUTTON_RELEASE_MASK);
  g_signal_connect_after(m_treeview_exam_type, "button_release_event", G_CALLBACK(signal_treeview_button_release_exam_department), this);

  GtkButton* button_add_item = Utils::create_button();
  GtkButton* button_delete_item = Utils::create_button();

  gtk_table_attach(table, GTK_WIDGET(button_add_item), 0, 2, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_delete_item), 2, 4, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  Utils::set_button_image(button_add_item, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON)));
  Utils::set_button_image(button_delete_item, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON)));

  g_signal_connect(button_add_item, "clicked", G_CALLBACK(signal_button_clicked_add_item), this);
  g_signal_connect(button_delete_item, "clicked", G_CALLBACK(signal_button_clicked_delete_item), this);

  // Button

  GtkButton* button_save = Utils::create_button(_("Save"));
  GtkButton* button_new = Utils::create_button(_("New"));
  GtkButton* button_delete = Utils::create_button(_("Delete"));

  gtk_table_attach(table, GTK_WIDGET(button_save), 6, 10, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_new), 10, 14, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_delete), 14, 18, 7, 8, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_save, "clicked", G_CALLBACK(signal_button_clicked_image_save), this);
  g_signal_connect(button_new, "clicked", G_CALLBACK(signal_button_clicked_image_new), this);
  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_delete_item), this);

  GtkButton* button_export = Utils::create_button(_("Export To USB"));
  GtkButton* button_import = Utils::create_button(_("Import From USB"));
  GtkButton* button_default = Utils::create_button(_("Default Factory"));

  gtk_table_attach(table, GTK_WIDGET(button_export), 6, 10, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_import), 10, 14, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(button_default), 14, 18, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_export, "clicked", G_CALLBACK(signal_button_clicked_image_export), this);
  g_signal_connect(button_import, "clicked", G_CALLBACK(signal_button_clicked_image_import), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_image_default), this);

  // Exam Type

  m_frame_new_check_part = Utils::create_frame();
  gtk_table_attach_defaults(table, GTK_WIDGET(m_frame_new_check_part), 6, 18, 3, 6);

  GtkTable* table_new_check_part = Utils::create_table(2, 3);
  gtk_container_add(GTK_CONTAINER(m_frame_new_check_part), GTK_WIDGET(table_new_check_part));

  m_label_check_part = Utils::create_label(_("Exam Type:"));
  m_entry_new_check_part = Utils::create_entry();
  GtkButton* button_ok = Utils::create_button(_("OK"));
  GtkButton* button_cancel = Utils::create_button(_("Cancel"));

  gtk_table_attach_defaults(table_new_check_part, GTK_WIDGET(m_label_check_part), 0, 3, 0, 1);
  gtk_table_attach(table_new_check_part, GTK_WIDGET(m_entry_new_check_part), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_new_check_part, GTK_WIDGET(button_ok), 0, 1, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_new_check_part, GTK_WIDGET(button_cancel), 2, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  Utils::set_button_image(button_ok, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON)));
  Utils::set_button_image(button_cancel, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON)));

  g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_newcheckpart_ok), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_newcheckpart_cancel), this);

  // scrolled window

  //GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  //GtkWidget* viewport_img_set = gtk_viewport_new(NULL, NULL);
  //gtk_container_add(GTK_CONTAINER(scrolled_window), viewport_img_set);

  return GTK_WIDGET(table);


  /*
    // scroll window

    viewport_img_set = gtk_viewport_new(NULL, NULL);
    gtk_widget_hide (viewport_img_set);
    //gtk_widget_show (viewport_img_set);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_img_set), viewport_img_set);

    fixed_img_set = gtk_fixed_new ();
    gtk_widget_show (fixed_img_set);
    gtk_container_add (GTK_CONTAINER (viewport_img_set), fixed_img_set);

    // comment
    frame_comment = gtk_frame_new (NULL);
    gtk_widget_show (frame_comment);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_comment, 5, 5);
    gtk_widget_set_size_request (frame_comment, 520, 70);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_comment), GTK_SHADOW_OUT);

    label_comment = gtk_label_new (_("<b>Common</b>"));
    gtk_widget_show (label_comment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_comment), label_comment);
    gtk_label_set_use_markup (GTK_LABEL (label_comment), TRUE);

    fixed_comment = gtk_fixed_new ();
    gtk_widget_show (fixed_comment);
    gtk_container_add (GTK_CONTAINER (frame_comment), fixed_comment);

    table_comment = gtk_table_new (1, 4, FALSE);
    gtk_widget_show (table_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), table_comment, 20, 5);
    gtk_widget_set_size_request (table_comment, 480, 30);
    gtk_table_set_row_spacings (GTK_TABLE(table_comment), 10);

    // MBP
    label_mbp = gtk_label_new (_("MBP :"));
    gtk_widget_show (label_mbp);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), label_mbp, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), label_mbp, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_mbp, -1, 30);

    m_combobox_mbp = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_mbp);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_mbp, 105, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), m_combobox_mbp, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_combobox_mbp, 100, 30);
    for (i = 0; i < Img2D::MAX_MBP_INDEX; i ++) {
        sprintf(buf, "%d", Img2D::MBP[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_mbp), buf);
    }

    label_ao_power = gtk_label_new (_("AO / Power :"));
    gtk_widget_show (label_ao_power);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), label_ao_power, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), label_ao_power, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_ao_power, -1, 30);

    m_combobox_ao_power = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ao_power);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_ao_power, 370, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), m_combobox_ao_power, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_combobox_ao_power, 100, 30);
    for (i = 0; i < Img2D::MAX_POWER_INDEX; i ++) {
        sprintf(buf, "%d", Img2D::POWER_DATA[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ao_power), buf);
    }

    // 2d and m mode
    frame_2d_m_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_2d_m_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_2d_m_mode, 5, 90);//90
    gtk_widget_set_size_request (frame_2d_m_mode, 520, 700); //550
    //gtk_widget_set_size_request (frame_2d_m_mode, 520, 605 - 40); //550
    gtk_frame_set_shadow_type (GTK_FRAME (frame_2d_m_mode), GTK_SHADOW_OUT);

    fixed_2d_m_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_2d_m_mode);
    gtk_container_add (GTK_CONTAINER (frame_2d_m_mode), fixed_2d_m_mode);

    label_2d_m_mode = gtk_label_new (_("<b>2D and M mode</b>"));
    gtk_widget_show (label_2d_m_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_2d_m_mode), label_2d_m_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_2d_m_mode), TRUE);

    table_2d_m_mode = gtk_table_new (16, 4, FALSE); //14
    //table_2d_m_mode = gtk_table_new (15, 4, FALSE); //14
    gtk_widget_show (table_2d_m_mode);
    gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), table_2d_m_mode, 20, 5);
    gtk_widget_set_size_request (table_2d_m_mode, 480, 520);//520
    gtk_table_set_row_spacings (GTK_TABLE(table_2d_m_mode), 10);

    // 2D gain
    label_2d_gain = gtk_label_new (_("2D Gain:"));
    gtk_widget_show (label_2d_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_gain, 5, 10);
    gtk_widget_set_size_request (label_2d_gain, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_gain, 0, 1, 0, 1);

    spinbutton_2d_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_2d_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_2d_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_2d_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_spinbutton_2d_gain, 105, 12);
    gtk_widget_set_size_request (m_spinbutton_2d_gain, -1, 27);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_spinbutton_2d_gain, 1, 2, 0, 1);
    g_signal_connect(G_OBJECT(m_spinbutton_2d_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    // 2D freq index
    label_2d_freq = gtk_label_new (_("Freq. :"));
    gtk_widget_show (label_2d_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_freq, 270, 10);
    gtk_widget_set_size_request (label_2d_freq, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_freq, 2, 3, 0, 1);

    m_combobox_2d_freq = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_freq, 370, 10);
    gtk_widget_set_size_request (m_combobox_2d_freq, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_freq, 3, 4, 0, 1);

    // focus sum
    label_focus_sum = gtk_label_new (_("Focus sum:"));
    gtk_widget_show (label_focus_sum);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_focus_sum, 5, 50);
    gtk_widget_set_size_request (label_focus_sum, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_focus_sum , 0, 1, 1, 2);

    m_combobox_focus_sum = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_focus_sum);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_focus_sum, 105, 50);
    gtk_widget_set_size_request (m_combobox_focus_sum, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_focus_sum, 1, 2, 1, 2);
    for (i = 1; i <= Img2D::MAX_FOCUS; i ++) {
        sprintf(buf, "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_focus_sum), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_focus_sum), "changed", G_CALLBACK (HandleComboFocusSum), this);

    // focus position
    label_focus_pos = gtk_label_new (_("Focus Pos. Index:"));
    gtk_widget_show (label_focus_pos);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_focus_pos, 260, 50);
    gtk_widget_set_size_request (label_focus_pos, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_focus_pos, 2, 3, 1, 2);

    m_combobox_focus_pos = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_focus_pos);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_focus_pos, 370, 50);
    gtk_widget_set_size_request (m_combobox_focus_pos, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_focus_pos, 3, 4, 1, 2);

    // scan range
    label_scan_range = gtk_label_new (_("Scan Range:"));
    gtk_widget_show (label_scan_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_scan_range, 5, 90);
    gtk_widget_set_size_request (label_scan_range, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_scan_range, 0, 1, 2, 3);

    m_combobox_scan_range = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_scan_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_scan_range, 105, 90);
    gtk_widget_set_size_request (m_combobox_scan_range, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_scan_range, 1, 2, 2, 3);
    for (i = 0; i < Img2D::MAX_ANGLE; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_scan_range), buf);
    }

    // dyanamic range
    label_dynamic_range = gtk_label_new (_("Dynamic Range:"));
    gtk_widget_show (label_dynamic_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_dynamic_range, 260, 90);
    gtk_widget_set_size_request (label_dynamic_range, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_dynamic_range, 2, 3, 2, 3);

    m_combobox_dynamic_range = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_dynamic_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_dynamic_range, 370, 90);
    gtk_widget_set_size_request (m_combobox_dynamic_range, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_dynamic_range, 3, 4, 2, 3);
    for (i = 0; i < Calc2D::MAX_DYNAMIC_INDEX; i ++)  {
        sprintf(buf , "%ddB", Img2D::DYNAMIC_DATA_D[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_dynamic_range), buf);
    }

    // line density
    label_2d_line_density = gtk_label_new (_("Line Density:"));
    gtk_widget_show (label_2d_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_line_density, 5, 130);
    gtk_widget_set_size_request (label_2d_line_density, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_line_density, 0, 1, 3, 4);

    m_combobox_2d_line_density = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_line_density, 105, 130);
    gtk_widget_set_size_request (m_combobox_2d_line_density, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_line_density, 1, 2, 3, 4);
    for (i = 0; i < Img2D::MAX_LINE_DENSITY; i ++) {
        sprintf(buf , "%s", _(Img2D::LINE_DENSITY_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_2d_line_density), buf);
    }

    // depth
    label_depth = gtk_label_new (_("Depth Scale:"));
    gtk_widget_show (label_depth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_depth, 270, 130);
    gtk_widget_set_size_request (label_depth, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_depth, 2, 3, 3, 4);

    m_combobox_depth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_depth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_depth, 370, 130);
    gtk_widget_set_size_request (m_combobox_depth, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_depth, 3, 4, 3, 4);
    for (i = 0; i < Img2D::MAX_SCALE_INDEX; i ++) {
        sprintf(buf, "%.1f", (float)Img2D::IMG_SCALE[i] / 10);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_depth), buf);
    }

    // steer
    label_steer = gtk_label_new (_("Steer:"));
    gtk_widget_show (label_steer);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_steer, 5, 170);
    gtk_widget_set_size_request (label_steer, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_steer, 0, 1, 4, 5);

    m_combobox_steer = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_steer);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_steer, 105, 170);
    gtk_widget_set_size_request (m_combobox_steer, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_steer, 1, 2, 4, 5);
    for (i = 0; i < Img2D::MAX_STEER; i ++) {
        sprintf(buf, "%d°", Img2D::STEER_ANGLE[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_steer), buf);
    }

    // AGC
    label_agc = gtk_label_new (_("AGC:"));
    gtk_widget_show (label_agc);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_agc, 270, 170);
    gtk_widget_set_size_request (label_agc, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_agc, 2, 3, 4, 5);

    m_combobox_agc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_agc);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_agc, 370, 170);
    gtk_widget_set_size_request (m_combobox_agc, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_agc, 3, 4, 4, 5);
    for (i = 0; i < Img2D::MAX_AGC; i++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_agc), buf);
    }

    // edge enhancement
    label_edge_enh = gtk_label_new (_("Edge enh. :"));
    gtk_widget_show (label_edge_enh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_edge_enh, 5, 210);
    gtk_widget_set_size_request (label_edge_enh, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_edge_enh, 0, 1, 5, 6);

    m_combobox_edge_enh = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_edge_enh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_edge_enh, 105, 210);
    gtk_widget_set_size_request (m_combobox_edge_enh, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_edge_enh, 1, 2, 5, 6);
    for (i = 0; i < Img2D::MAX_EDGE; i++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_edge_enh), buf);
    }

    // chroma
    label_chroma = gtk_label_new (_("Chroma:"));
    gtk_widget_show (label_chroma);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_chroma, 270, 210);
    gtk_widget_set_size_request (label_chroma, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_chroma, 2, 3, 5, 6);

    m_combobox_chroma = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_chroma);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_chroma, 370, 210);
    gtk_widget_set_size_request (m_combobox_chroma, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_chroma, 3, 4, 5, 6);
    for (i = 0; i < ImgProc2D::MAX_CHROMA; i++) {
        sprintf(buf , "%s", _(ImgProc2D::CHROMA_TYPE[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_chroma), buf);
    }

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), hseparator1, 16, 250);
    //gtk_widget_set_size_request (hseparator1, 472, 16);
    gtk_table_attach (GTK_TABLE (table_2d_m_mode), hseparator1, 0, 4, 6, 7,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_SHRINK), 0, 0);
    gtk_widget_set_size_request (hseparator1, -1, 5);

    // left-right
    label_lr = gtk_label_new (_("L / R:"));
    gtk_widget_show (label_lr);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_lr, 5, 270);
    gtk_widget_set_size_request (label_lr, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_lr, 0, 1, 7, 8);

    m_combobox_lr = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_lr);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_lr, 105, 270);
    gtk_widget_set_size_request (m_combobox_lr, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_lr, 1, 2, 7, 8);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_lr), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_lr), _("ON"));

    // up-down
    label_ud = gtk_label_new (_("U / D:"));
    gtk_widget_show (label_ud);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_ud, 270, 270);
    gtk_widget_set_size_request (label_ud, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_ud, 2, 3, 7, 8);

    m_combobox_ud = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ud);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_ud, 370, 270);
    gtk_widget_set_size_request (m_combobox_ud, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_ud, 3, 4, 7, 8);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ud), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ud), _("ON"));

    // polarity
    label_polarity = gtk_label_new (_("Polarity:"));
    gtk_widget_show (label_polarity);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_polarity, 5, 310);
    gtk_widget_set_size_request (label_polarity, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_polarity, 0, 1, 8, 9);

    m_combobox_polarity = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_polarity);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_polarity, 105, 310);
    gtk_widget_set_size_request (m_combobox_polarity, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_polarity, 1, 2, 8, 9);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_polarity), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_polarity), _("ON"));

    // frame aver
    label_frame = gtk_label_new (_("Frame aver. :"));
    gtk_widget_show (label_frame);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_frame, 5, 350);
    gtk_widget_set_size_request (label_frame, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_frame, 2, 3, 8, 9);
    m_combobox_frame = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_frame);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_frame, 105, 350);
    gtk_widget_set_size_request (m_combobox_frame, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_frame, 3, 4, 8, 9);
    for (i = 0; i < ImgProc2D::MAX_FRAME; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_frame), buf);
    }

    // line aver
    label_line_aver = gtk_label_new (_("Line aver. :"));
    gtk_widget_show (label_line_aver);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_line_aver, 270, 350);
    gtk_widget_set_size_request (label_line_aver, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_line_aver, 0, 1, 9, 10);

    m_combobox_line_aver = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_line_aver);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_line_aver, 370, 350);
    gtk_widget_set_size_request (m_combobox_line_aver, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_line_aver, 1, 2, 9, 10);
    for (i = 0; i < ImgProc2D::MAX_LINE; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_line_aver), buf);
    }

    // smooth
    label_2d_smooth = gtk_label_new (_("Smooth:"));
    gtk_widget_show (label_2d_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_smooth, 5, 390);
    gtk_widget_set_size_request (label_2d_smooth, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_smooth, 2, 3, 9, 10);

    m_combobox_2d_smooth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_smooth, 105, 390);
    gtk_widget_set_size_request (m_combobox_2d_smooth, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_smooth, 3, 4, 9, 10);
    for (i = 0; i < ImgProc2D::MAX_SMOOTH; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_2d_smooth), buf);
    }

    // restric
    label_restric = gtk_label_new (_("Reject:"));
    gtk_widget_show (label_restric);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_restric, 270, 390);
    gtk_widget_set_size_request (label_restric, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_restric, 0, 1, 10, 11);

    m_combobox_restric = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_restric);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_restric, 370, 390);
    gtk_widget_set_size_request (m_combobox_restric, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_restric, 1, 2, 10, 11);
    for (i = 0; i < ImgProc2D::MAX_REJECT; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_restric), buf);
    }

    // gray transform
    label_gray_transform = gtk_label_new (_("GrayTrans:"));
    gtk_widget_show (label_gray_transform);
    gtk_widget_set_size_request (label_gray_transform, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_gray_transform, 2, 3, 10, 11);

    m_combobox_gray_trans = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_gray_trans);
    gtk_widget_set_size_request (m_combobox_gray_trans, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_gray_trans, 3, 4, 10, 11);
    //float value = 1.0;
    //sprintf(buf , "%.1f", value);
    //gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_gray_trans), 0, "C0");
    for (i = 0; i < ImgProc2D::MAX_TRANS_CURVE; i ++) {
        sprintf(buf , "C%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_gray_trans), buf);
    }

    hseparator2 = gtk_hseparator_new ();
    gtk_widget_show (hseparator2);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), hseparator2, 10, 430);
    //gtk_widget_set_size_request (hseparator2, 470, 16);
    gtk_table_attach (GTK_TABLE (table_2d_m_mode), hseparator2, 0, 4, 11, 12,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_SHRINK), 0, 0);
    gtk_widget_set_size_request (hseparator2, -1, 5);

    // THI
    label_thi = gtk_label_new (_("THI:"));
    gtk_widget_show (label_thi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_thi, 5, 450);
    gtk_widget_set_size_request (label_thi, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_thi, 0, 1, 12, 13);

    m_combobox_thi = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_thi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_thi, 105, 450);
    gtk_widget_set_size_request (m_combobox_thi, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_thi, 1, 2, 12, 13);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_thi), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_thi), _("ON"));

    // TSI
    label_tsi = gtk_label_new (_("TSI:"));
    gtk_widget_show (label_tsi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_tsi, 270, 450);
    gtk_widget_set_size_request (label_tsi, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_tsi, 2, 3, 12, 13);

    m_combobox_tsi = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_tsi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_tsi, 370, 450);
    gtk_widget_set_size_request (m_combobox_tsi, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_tsi, 3, 4, 12, 13);
    for (i = 0; i < Img2D::MAX_TSI; i ++) {
        sprintf(buf , "%s", _(Img2D::TSI_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_tsi), buf);
    }

    // image enhance
    label_imgEnh = gtk_label_new (_("ePure:"));
    gtk_widget_show (label_imgEnh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_imgEnh, 5, 490);
    gtk_widget_set_size_request (label_imgEnh, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_imgEnh, 0, 1, 13, 14);

    m_combobox_imgEnh = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_imgEnh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_imgEnh, 105, 490);
    gtk_widget_set_size_request (m_combobox_imgEnh, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_imgEnh, 1, 2, 13, 14);
    for (i = 0; i < ImgProc2D::MAX_IMG_EHN; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_imgEnh), buf);
    }

    // M gain
    label_m_gain = gtk_label_new (_("M Gain:"));
    gtk_widget_show (label_m_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_m_gain, 270, 490);
    gtk_widget_set_size_request (label_m_gain, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_m_gain, 2, 3, 13, 14);

    spinbutton_m_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_m_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_m_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_m_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_spinbutton_m_gain, 370, 490);
    gtk_widget_set_size_request (m_spinbutton_m_gain, 100, 27);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_spinbutton_m_gain, 3, 4, 13, 14);
    g_signal_connect(G_OBJECT(m_spinbutton_m_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    //space compound
    label_space_compound = gtk_label_new(_("SpaceCompound:"));
    gtk_widget_show(label_space_compound);
    gtk_widget_set_size_request(label_space_compound, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_space_compound, 0, 1, 14, 15);
    m_combobox_space_compound = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_space_compound);
    gtk_widget_set_size_request(m_combobox_space_compound, -1, 30);
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));
    for(i = 1; i <Img2D::MAX_SPACE_COMPOUND; i ++) {
        sprintf(buf, "%d", i);
        gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_space_compound), buf);
    }
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_space_compound, 1, 2, 14, 15);

    //freq compound
    label_freq_compound = gtk_label_new(_("FreqCompound:"));
    gtk_widget_show(label_freq_compound);
    gtk_widget_set_size_request(label_freq_compound, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_freq_compound, 2, 3, 14, 15);
    m_combobox_freq_compound = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_freq_compound);
    gtk_widget_set_size_request(m_combobox_freq_compound, -1, 30);
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_freq_compound), 0, _("OFF"));
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_freq_compound), 1, _("ON"));
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_freq_compound, 3, 4, 14, 15);

    //thi freq
    label_thi_freq = gtk_label_new(_("FH:"));
    gtk_widget_show(label_thi_freq);
    gtk_widget_set_size_request(label_thi_freq, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_thi_freq, 0, 1, 15, 16);
    m_combobox_thi_freq = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_thi_freq);
    gtk_widget_set_size_request(m_combobox_thi_freq, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_thi_freq, 1, 2, 15, 16);

    // pw mode
    frame_pw_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_pw_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_pw_mode, 5, 715); //660
    //gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_pw_mode, 5, 715 - 40); //660
    gtk_widget_set_size_request (frame_pw_mode, 520, 200);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_pw_mode), GTK_SHADOW_OUT);

    fixed_pw_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_pw_mode);
    gtk_container_add (GTK_CONTAINER (frame_pw_mode), fixed_pw_mode);

    label_pw_mode = gtk_label_new (_("<b>PW mode</b>"));
    gtk_widget_show (label_pw_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_pw_mode), label_pw_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_pw_mode), TRUE);

    table_pw_mode = gtk_table_new (5, 4, FALSE);
    gtk_widget_show (table_pw_mode);
    gtk_fixed_put (GTK_FIXED (fixed_pw_mode), table_pw_mode, 20, 5);
    gtk_widget_set_size_request (table_pw_mode, 480, 170);
    gtk_table_set_row_spacings (GTK_TABLE(table_pw_mode), 10);

    // doppler freq
    label_pw_freq = gtk_label_new (_("Freq. :"));
    gtk_widget_show (label_pw_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_freq, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_freq, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_pw_freq, -1, 30);

    m_combobox_pw_freq = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_freq, 105, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_freq, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_combobox_pw_freq, 100, 30);

    // pw gain
    label_pw_gain = gtk_label_new (_("PW Gain:"));
    gtk_widget_show (label_pw_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_gain, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_gain, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_pw_gain, -1, 30);

    spinbutton_pw_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_pw_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_pw_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_pw_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_spinbutton_pw_gain, 370, 12);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_spinbutton_pw_gain, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_spinbutton_pw_gain, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    // pw scale
    label_pw_scale_prf = gtk_label_new (_("Scale PRF:"));
    gtk_widget_show (label_pw_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_scale_prf, 5, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_scale_prf, 0, 1, 1, 2);
    gtk_widget_set_size_request (label_pw_scale_prf, -1, 30);

    m_combobox_pw_scale_prf = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_scale_prf, 105, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_scale_prf, 1, 2, 1, 2);
    gtk_widget_set_size_request (m_combobox_pw_scale_prf, 100, 30);
    for (i = 0; i < ImgPw::MAX_PRF; i ++) {
        sprintf(buf , "%dHz", ImgPw::PW_PRF[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_scale_prf), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_pw_scale_prf ), "changed", G_CALLBACK (HandleComboPwPrf), this);

    // wall filter
    label_pw_wallfilter = gtk_label_new (_("Wall Filter:"));
    gtk_widget_show (label_pw_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_wallfilter, 270, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_wallfilter, 2, 3, 1, 2);
    gtk_widget_set_size_request (label_pw_wallfilter, -1, 30);

    m_combobox_pw_wallfilter = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_wallfilter, 370, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_wallfilter, 3, 4, 1, 2);
    gtk_widget_set_size_request (m_combobox_pw_wallfilter, 100, 30);

    // angle
    label_pw_angle = gtk_label_new (_("Angle:"));
    gtk_widget_show (label_pw_angle);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_angle, 0, 1, 2, 3);
    gtk_widget_set_size_request (label_pw_angle , -1, 30);

    spinbutton_pw_angle_adj = gtk_adjustment_new (0, -85, 85, 5, 1, 0);
    m_spinbutton_pw_angle = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_pw_angle_adj), 1, 0);
    gtk_widget_show (m_spinbutton_pw_angle);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_spinbutton_pw_angle, 1, 2, 2, 3);
    gtk_widget_set_size_request (m_spinbutton_pw_angle, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_angle), "insert_text", G_CALLBACK(on_spinbutton_insert_angle), this);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_angle), "output", G_CALLBACK(on_spinbutton_output_angle), this);

    hseparator3 = gtk_hseparator_new ();
    gtk_widget_show (hseparator3);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), hseparator3, 8, 88);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), hseparator3, 0, 4, 3, 4);
    gtk_widget_set_size_request (hseparator3, -1, 5);

    // invert
    label_pw_invert = gtk_label_new (_("Spectrum Invert:"));
    gtk_widget_show (label_pw_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_invert, 5, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_invert, 0, 1, 4, 5);
    gtk_widget_set_size_request (label_pw_invert, -1, 30);

    m_combobox_pw_invert = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_invert, 105, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_invert, 1, 2, 4, 5);
    gtk_widget_set_size_request (m_combobox_pw_invert, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_invert), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_invert), _("ON"));

    // time resolustion
    label_time_resolution = gtk_label_new (_("Time Resolution:"));
    gtk_widget_show (label_time_resolution);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_time_resolution, 260, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_time_resolution, 2, 3, 4, 5);
    gtk_widget_set_size_request (label_time_resolution, -1, 30);

    m_combobox_time_resolution = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_time_resolution);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_time_resolution, 370, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_time_resolution, 3, 4, 4, 5);
    gtk_widget_set_size_request (m_combobox_time_resolution, 100, 30);
    for (i = 0; i < ImgProcPw::MAX_TIME_RES; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_time_resolution), buf);
    }

    // cfm mode
    frame_cfm_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_cfm_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_cfm_mode, 5, 935); //880
    //gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_cfm_mode, 5, 935 - 40); //880
    gtk_widget_set_size_request (frame_cfm_mode, 520, 260);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_cfm_mode), GTK_SHADOW_OUT);

    fixed_cfm_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_cfm_mode);
    gtk_container_add (GTK_CONTAINER (frame_cfm_mode), fixed_cfm_mode);

    label_cfm_mode = gtk_label_new (_("<b>CFM mode</b>"));
    gtk_widget_show (label_cfm_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_cfm_mode), label_cfm_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_cfm_mode), TRUE);

    table_cfm_mode = gtk_table_new (6, 4, FALSE);
    gtk_widget_show (table_cfm_mode);
    gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), table_cfm_mode, 20, 5);
    gtk_widget_set_size_request (table_cfm_mode, 480, 230);
    gtk_table_set_row_spacings (GTK_TABLE(table_cfm_mode), 10);

    // cfm gain
    label_color_gain = gtk_label_new (_("Color Gain:"));
    gtk_widget_show (label_color_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_color_gain, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_color_gain, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_color_gain, -1, 30);

    spinbutton_cfm_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_cfm_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_cfm_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_cfm_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_spinbutton_cfm_gain, 105, 12);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_spinbutton_cfm_gain, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_spinbutton_cfm_gain, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_cfm_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    // cfm scale
    label_cfm_scale_prf = gtk_label_new (_("Scale PRF:"));
    gtk_widget_show (label_cfm_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_scale_prf, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_scale_prf, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_cfm_scale_prf, -1, 30);

    m_combobox_cfm_scale_prf = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_scale_prf, 370, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_scale_prf, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_combobox_cfm_scale_prf, 100, 30);
    for (i = 0; i < ImgCfm::MAX_PRF_INDEX; i ++) {
        sprintf(buf , "%dHz", ImgCfm::CFM_PRF[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_scale_prf), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_cfm_scale_prf), "changed", G_CALLBACK (HandleComboCfmPrf), this);

    // sensitive
    label_sensitivity = gtk_label_new (_("Sensitivity:"));
    gtk_widget_show (label_sensitivity);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_sensitivity, 5, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_sensitivity, 0, 1, 1, 2);
    gtk_widget_set_size_request (label_sensitivity, -1, 30);
    m_combobox_sensitivity = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_sensitivity);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_sensitivity, 105, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_sensitivity, 1, 2, 1, 2);
    gtk_widget_set_size_request (m_combobox_sensitivity, 100, 30);

    g_signal_connect (G_OBJECT(m_combobox_sensitivity), "changed", G_CALLBACK (HandleComboCfmSensitivity), this);

    // cfm wall filter
    label_cfm_wallfilter = gtk_label_new (_("Wall Filter:"));
    gtk_widget_show (label_cfm_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_wallfilter, 5, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_wallfilter, 2, 3, 1, 2);
    gtk_widget_set_size_request (label_cfm_wallfilter, -1, 30);

    m_combobox_cfm_wallfilter = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_wallfilter, 105, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_wallfilter, 3, 4, 1, 2);
    gtk_widget_set_size_request (m_combobox_cfm_wallfilter, 100, 30);

    // color line density
    label_cfm_line_density = gtk_label_new (_("Line Density:"));
    gtk_widget_show (label_cfm_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_line_density, 270, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_line_density, 0, 1, 2, 3);
    gtk_widget_set_size_request (label_cfm_line_density, -1, 30);

    m_combobox_cfm_line_density = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_line_density, 370, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_line_density, 1, 2, 2, 3);
    gtk_widget_set_size_request (m_combobox_cfm_line_density, 100, 30);
    for (i = 0; i < ImgCfm::MAX_LINE_DENSITY; i ++) {
        sprintf(buf , "%s", _(ImgCfm::LINE_DENSITY_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_line_density), buf);
    }

    // variance
    label_variance = gtk_label_new (_("Variance:"));
    gtk_widget_show (label_variance);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_variance, 270, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_variance, 2, 3, 2, 3);
    gtk_widget_set_size_request (label_variance, -1, 30);

    m_combobox_variance = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_variance);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_variance, 370, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_variance, 3, 4, 2, 3);
    gtk_widget_set_size_request (m_combobox_variance, -1, 30);
    for (i = 0; i < ImgCfm::MAX_TURB; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_variance), buf);
    }

    hseparator4 = gtk_hseparator_new ();
    gtk_widget_show (hseparator4);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), hseparator4, 5, 130);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), hseparator4, 0, 4, 3, 4);
    gtk_widget_set_size_request (hseparator4, -1, 5);

    // invert
    label_cfm_invert = gtk_label_new (_("Color Invert:"));
    gtk_widget_show (label_cfm_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_invert, 5, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_invert, 0, 1, 4, 5);
    gtk_widget_set_size_request (label_cfm_invert, -1, 30);

    m_combobox_cfm_invert = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_invert, 105, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_invert, 1, 2, 4, 5);
    gtk_widget_set_size_request (m_combobox_cfm_invert, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_invert), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_invert), _("ON"));

    // persistence
    label_persistence = gtk_label_new (_("Persistence:"));
    gtk_widget_show (label_persistence);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_persistence, 270, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_persistence, 2, 3, 4, 5);
    gtk_widget_set_size_request (label_persistence, -1, 30);

    m_combobox_persistence = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_persistence);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_persistence, 370, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_persistence, 3, 4, 4, 5);
    gtk_widget_set_size_request (m_combobox_persistence, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_PERSIST; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_persistence), buf);
    }

    // color rejection
    label_color_rejection = gtk_label_new (_("Color Reject:"));
    gtk_widget_show (label_color_rejection);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_color_rejection, 5, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_color_rejection, 0, 1, 5, 6);
    gtk_widget_set_size_request (label_color_rejection, -1, 30);

    m_combobox_color_rejection = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_color_rejection);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_color_rejection, 105, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_color_rejection, 1, 2, 5, 6);
    gtk_widget_set_size_request (m_combobox_color_rejection, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_REJECT; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_color_rejection), buf);
    }

    // cfm smooth
    label_cfm_smooth = gtk_label_new (_("Smooth:"));
    gtk_widget_show (label_cfm_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_smooth, 270, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_smooth, 2, 3, 5, 6);
    gtk_widget_set_size_request (label_cfm_smooth, -1, 30);

    m_combobox_cfm_smooth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_smooth, 370, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_smooth, 3, 4, 5, 6);
    gtk_widget_set_size_request (m_combobox_cfm_smooth, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_SMOOTH; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_smooth), buf);
    }

    return fixed_image;
  */
}

void ViewSystem::init_image_setting() {
  string probe_type = TopArea::GetInstance()->GetProbeType();

  for (int i = 0; i < NUM_PROBE; ++i) {
    if (probe_type == g_probe_list[i]) {
      string newProbeName = ProbeMan::GetInstance()->VerifyProbeName(probe_type);
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), i);
    }
  }

  gtk_widget_hide_all(GTK_WIDGET(m_frame_new_check_part));

  set_image_item_sensitive(false);
}

void ViewSystem::save_image_setting() {
  string name = GetUserName();
  if (name == "System Default" && name == "Умолчан системы" &&
    name == "系统默认" && name == "Domyślne Systemu" && name == "Par défaut du sys." &&
    name == "Systemvorgabe" && name == "Sistema por defecto") {
    UserSelect::GetInstance()->write_username(GTK_COMBO_BOX(m_comboboxentry_user_select), USERNAME_DB, name);
  }

  ExamItem::ParaItem paraItem;
  GetImagePara(paraItem);

  int probeIndex = 0;
  int itemIndex = 0;
  char* itemName = NULL;
  GetImageNoteSelection(probeIndex, itemIndex, itemName);

  if (probeIndex >= 0 && itemIndex >= 0) {
    ExamItem examItem;
    examItem.WriteExamItemPara(probeIndex, itemIndex, &paraItem, itemName);
  }
}

GtkWidget* ViewSystem::create_note_measure() {
  GtkTable* table = Utils::create_table(13, 6);

  // Unit Setting
  GtkFrame* frame_unit_setting = Utils::create_frame(_("Unit Setting"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_unit_setting), 0, 2, 0, 11);

  GtkTable* table_unit_setting = Utils::create_table(7, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_unit_setting), 5);
  gtk_container_add(GTK_CONTAINER(frame_unit_setting), GTK_WIDGET(table_unit_setting));

  GtkLabel* label_unit_dist = Utils::create_label(_("Distance"));
  m_combobox_unit_dist = Utils::create_combobox_text();

  GtkLabel* label_unit_area = Utils::create_label(_("Area"));
  m_combobox_unit_area = Utils::create_combobox_text();

  GtkLabel* label_unit_vol = Utils::create_label(_("Volume"));
  m_combobox_unit_vol = Utils::create_combobox_text();

  GtkLabel* label_unit_time = Utils::create_label(_("Time"));
  m_combobox_unit_time = Utils::create_combobox_text();

  GtkLabel* label_unit_vel = Utils::create_label(_("Velocity"));
  m_combobox_unit_vel = Utils::create_combobox_text();

  GtkLabel* label_unit_accel = Utils::create_label(_("Accel"));
  m_combobox_unit_accel = Utils::create_combobox_text();

  GtkLabel* label_unit_efw = Utils::create_label(_("EFW"));
  m_combobox_unit_efw = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_dist), 0, 1, 0, 1);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_dist), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_area), 0, 1, 1, 2);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_area), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_vol), 0, 1, 2, 3);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_vol), 1, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_time), 0, 1, 3, 4);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_time), 1, 3, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_vel), 0, 1, 4, 5);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_vel), 1, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_accel), 0, 1, 5, 6);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_accel), 1, 3, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_unit_setting, GTK_WIDGET(label_unit_efw), 0, 1, 6, 7);
  gtk_table_attach(table_unit_setting, GTK_WIDGET(m_combobox_unit_efw), 1, 3, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_unit_dist, "cm");
  gtk_combo_box_text_append_text(m_combobox_unit_dist, "mm");
  gtk_combo_box_text_append_text(m_combobox_unit_area, "cm²");
  gtk_combo_box_text_append_text(m_combobox_unit_area,"mm²");
  gtk_combo_box_text_append_text(m_combobox_unit_vol,"cm³");
  gtk_combo_box_text_append_text(m_combobox_unit_vol, "mm³");
  gtk_combo_box_text_append_text(m_combobox_unit_time, "s");
  gtk_combo_box_text_append_text(m_combobox_unit_time, "ms");
  gtk_combo_box_text_append_text(m_combobox_unit_vel, "cm/s");
  gtk_combo_box_text_append_text(m_combobox_unit_vel, "mm/s");
  gtk_combo_box_text_append_text(m_combobox_unit_vel, "m/s");
  gtk_combo_box_text_append_text(m_combobox_unit_accel, "cm/s²");
  gtk_combo_box_text_append_text(m_combobox_unit_accel, "mm/s²");
  gtk_combo_box_text_append_text(m_combobox_unit_accel, "m/s²");
  gtk_combo_box_text_append_text(m_combobox_unit_efw, "kg");
  gtk_combo_box_text_append_text(m_combobox_unit_efw, "g");

  // Heart Beat
  GtkLabel* label_heart_beat = Utils::create_label(_("Heart Beat:"));
  m_combobox_heart_beat = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_heart_beat), 0, 1, 11, 12);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_heart_beat), 1, 2, 11, 12, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_heart_beat, "1");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "2");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "3");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "4");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "5");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "6");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "7");
  gtk_combo_box_text_append_text(m_combobox_heart_beat, "8");

  // Auto Spectrum Calculation Result
  GtkFrame* frame_auto_spectrum = Utils::create_frame(_("Auto Spectrum Calculation Result"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_auto_spectrum), 2, 6, 0, 4);

  GtkTable* table_auto_spectrum = Utils::create_table(3, 4);
  gtk_container_set_border_width(GTK_CONTAINER(table_auto_spectrum), 5);
  gtk_container_add(GTK_CONTAINER(frame_auto_spectrum), GTK_WIDGET(table_auto_spectrum));

  m_checkbutton_autocalc_ps = Utils::create_check_button(_("PS"));
  m_checkbutton_autocalc_ed = Utils::create_check_button(_("ED"));
  m_checkbutton_autocalc_ri = Utils::create_check_button(_("RI"));
  m_checkbutton_autocalc_sd = Utils::create_check_button(_("SD"));
  m_checkbutton_autocalc_tamax = Utils::create_check_button(_("TA Max"));
  m_checkbutton_autocalc_pi = Utils::create_check_button(_("PI"));
  m_checkbutton_autocalc_time = Utils::create_check_button(_("Time"));
  m_checkbutton_autocalc_hr = Utils::create_check_button(_("HR"));
  m_checkbutton_autocalc_pgmax = Utils::create_check_button(_("PG Max"));
  m_checkbutton_autocalc_pgmean = Utils::create_check_button(_("PG Mean"));

  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_ps), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_ed), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_ri), 2, 3, 0, 1);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_sd), 3, 4, 0, 1);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_tamax), 0, 1, 1, 2);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_pi), 1, 2, 1, 2);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_time), 2, 3, 1, 2);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_hr), 3, 4, 1, 2);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_pgmax), 0, 1, 2, 3);
  gtk_table_attach_defaults(table_auto_spectrum, GTK_WIDGET(m_checkbutton_autocalc_pgmean), 1, 2, 2, 3);

  gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_ps), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_ed), FALSE);
  g_signal_connect(G_OBJECT(m_checkbutton_autocalc_tamax), "toggled", G_CALLBACK(signal_togglebutton_toggled_tamax), this);
  g_signal_connect(G_OBJECT(m_checkbutton_autocalc_pi), "toggled", G_CALLBACK(signal_togglebutton_toggled_pi), this);
  g_signal_connect(G_OBJECT(m_checkbutton_autocalc_hr), "toggled", G_CALLBACK(signal_togglebutton_toggled_hr), this);

  // Line Color
  GtkFrame* frame_line_color = Utils::create_frame(_("Line Color"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_line_color), 2, 6, 4, 6);

  GtkTable* table_line_color = Utils::create_table(1, 4);
  gtk_container_set_border_width(GTK_CONTAINER(table_line_color), 5);
  gtk_container_add(GTK_CONTAINER(frame_line_color), GTK_WIDGET(table_line_color));

  GtkLabel* label_current_line = Utils::create_label(_("Current Line:"));
  m_combobox_current_line = Utils::create_combobox_text();
  GtkLabel* label_confirmed_line = Utils::create_label(_("Confirmed Line:"));
  m_combobox_confirmed_line = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_line_color, GTK_WIDGET(label_current_line), 0, 1, 0, 1);
  gtk_table_attach(table_line_color, GTK_WIDGET(m_combobox_current_line), 1, 2, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_line_color, GTK_WIDGET(label_confirmed_line), 2, 3, 0, 1);
  gtk_table_attach(table_line_color, GTK_WIDGET(m_combobox_confirmed_line), 3, 4, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_current_line, _("Red"));
  gtk_combo_box_text_append_text(m_combobox_current_line, _("Yellow"));
  gtk_combo_box_text_append_text(m_combobox_current_line, _("Green"));
  gtk_combo_box_text_append_text(m_combobox_current_line, _("Blue"));
  gtk_combo_box_text_append_text(m_combobox_current_line, _("White"));

  gtk_combo_box_text_append_text(m_combobox_confirmed_line, _("Red"));
  gtk_combo_box_text_append_text(m_combobox_confirmed_line, _("Yellow"));
  gtk_combo_box_text_append_text(m_combobox_confirmed_line, _("Green"));
  gtk_combo_box_text_append_text(m_combobox_confirmed_line, _("Blue"));
  gtk_combo_box_text_append_text(m_combobox_confirmed_line, _("White"));

  // Report Result Method
  GtkFrame* frame_report_result = Utils::create_frame(_("Report Result Method"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_report_result), 2, 4, 6, 8);

  GtkTable* table_report_result = Utils::create_table(1, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_report_result), 5);
  gtk_container_add(GTK_CONTAINER(frame_report_result), GTK_WIDGET(table_report_result));

  m_radiobutton_report_last = Utils::create_radio_button(NULL, _("Last"));
  GSList* radiobutton_report_group = gtk_radio_button_get_group(m_radiobutton_report_last);
  m_radiobutton_report_average = Utils::create_radio_button(radiobutton_report_group, _("Average"));

  gtk_table_attach_defaults(table_report_result, GTK_WIDGET(m_radiobutton_report_last), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_report_result, GTK_WIDGET(m_radiobutton_report_average), 1, 2, 0, 1);

  // Measure Result Font Size
  GtkFrame* frame_measure_result = Utils::create_frame(_("Measure Result Font Size"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_measure_result), 4, 6, 6, 8);

  GtkTable* table_measure_result = Utils::create_table(1, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_measure_result), 5);
  gtk_container_add(GTK_CONTAINER(frame_measure_result), GTK_WIDGET(table_measure_result));

  m_radiobutton_result_small = Utils::create_radio_button(NULL, _("Small"));
  GSList* radiobutton_result_group = gtk_radio_button_get_group(m_radiobutton_result_small);
  m_radiobutton_result_big = Utils::create_radio_button(radiobutton_result_group, _("Big"));

  gtk_table_attach_defaults(table_measure_result, GTK_WIDGET(m_radiobutton_result_small), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_measure_result, GTK_WIDGET(m_radiobutton_result_big), 1, 2, 0, 1);

  // Trace Method
  GtkFrame* frame_trace_method = Utils::create_frame(_("Trace Method"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_trace_method), 2, 4, 8, 10);

  GtkTable* table_trace_method = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_trace_method), 5);
  gtk_container_add(GTK_CONTAINER(frame_trace_method), GTK_WIDGET(table_trace_method));

  m_radiobutton_trace_point = Utils::create_radio_button(NULL, _("Point"));
  GSList* radiobutton_trace_group = gtk_radio_button_get_group(m_radiobutton_trace_point);
  m_radiobutton_trace_track = Utils::create_radio_button(radiobutton_trace_group, _("Track"));
  radiobutton_trace_group = gtk_radio_button_get_group(m_radiobutton_trace_track);
  m_radiobutton_trace_auto = Utils::create_radio_button(radiobutton_trace_group, _("Auto"));

  gtk_table_attach_defaults(table_trace_method, GTK_WIDGET(m_radiobutton_trace_point), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_trace_method, GTK_WIDGET(m_radiobutton_trace_track), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_trace_method, GTK_WIDGET(m_radiobutton_trace_auto), 2, 3, 0, 1);

  // Cursor Size
  GtkFrame* frame_cursor_size = Utils::create_frame(_("Cursor Size"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_cursor_size), 4, 6, 8, 10);

  GtkTable* table_cursor_size = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_cursor_size), 5);
  gtk_container_add(GTK_CONTAINER(frame_cursor_size), GTK_WIDGET(table_cursor_size));

  m_radiobutton_cursor_big = Utils::create_radio_button(NULL, _("Big"));
  GSList* radiobutton_cursor_size_group = gtk_radio_button_get_group(m_radiobutton_cursor_big);
  m_radiobutton_cursor_mid = Utils::create_radio_button(radiobutton_cursor_size_group, _("Mid"));
  radiobutton_cursor_size_group = gtk_radio_button_get_group(m_radiobutton_cursor_mid);
  m_radiobutton_cursor_small = Utils::create_radio_button(radiobutton_cursor_size_group, _("Small"));

  gtk_table_attach_defaults(table_cursor_size, GTK_WIDGET(m_radiobutton_cursor_big), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_cursor_size, GTK_WIDGET(m_radiobutton_cursor_mid), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_cursor_size, GTK_WIDGET(m_radiobutton_cursor_small), 2, 3, 0, 1);

  // Measure Line
  GtkFrame* frame_measure_line = Utils::create_frame(_("Measure Line"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_measure_line), 2, 4, 10, 12);

  GtkTable* table_measure_line = Utils::create_table(1, 2);
  gtk_container_set_border_width(GTK_CONTAINER(table_measure_line), 5);
  gtk_container_add(GTK_CONTAINER(frame_measure_line), GTK_WIDGET(table_measure_line));

  m_radiobutton_measure_line_on = Utils::create_radio_button(NULL, _("ON"));
  GSList* radiobutton_measure_line_group = gtk_radio_button_get_group(m_radiobutton_measure_line_on);
  m_radiobutton_measure_line_off = Utils::create_radio_button(radiobutton_measure_line_group, _("OFF"));

  gtk_table_attach_defaults(table_measure_line, GTK_WIDGET(m_radiobutton_measure_line_on), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_measure_line, GTK_WIDGET(m_radiobutton_measure_line_off), 1, 2, 0, 1);

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 1, 12, 13, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK (signal_button_clicked_measure_default), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_measure_setting(SysMeasurementSetting* sysMeasure) {
  if (sysMeasure == NULL) {
    sysMeasure = new SysMeasurementSetting();
  }

  // Unit Setting

  int unitDist = sysMeasure->GetUnitDist();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_dist), unitDist);

  int unitArea = sysMeasure->GetUnitArea();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_area), unitArea);

  int unitVol = sysMeasure->GetUnitVol();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_vol), unitVol);

  int unitTime = sysMeasure->GetUnitTime();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_time), unitTime);

  int unitVel = sysMeasure->GetUnitVel();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_vel), unitVel);

  int unitAccel = sysMeasure->GetUnitAccel();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_accel), unitAccel);

  int unitEfw = sysMeasure->GetUnitEfw();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_efw), unitEfw);

  // Heart Beat

  int heartBeatCycleIndex = sysMeasure->GetHeartBeatCycle() - 1;
  if (heartBeatCycleIndex < 0) {
    heartBeatCycleIndex = 0;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_heart_beat), heartBeatCycleIndex);

  // Auto Spectrum Calculation Result

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ps), TRUE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ed), TRUE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ri), sysMeasure->GetAutoCalcRI());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_sd), sysMeasure->GetAutoCalcSD());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax), sysMeasure->GetAutoCalcTAmax());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi), sysMeasure->GetAutoCalcPI());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time), sysMeasure->GetAutoCalcTime());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_hr), sysMeasure->GetAutoCalcHR());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmax), sysMeasure->GetAutoCalcPGmax());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmean), sysMeasure->GetAutoCalcPGmean());

  // Line Color

  int lineColor = sysMeasure->GetMeasureColorCur();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_current_line), lineColor);
  lineColor = sysMeasure->GetMeasureColorConfirm();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_confirmed_line), lineColor);

  // Report Result Method

  int reportResult = sysMeasure->GetReportResult();
  switch (reportResult) {
  case 0:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_report_last), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_report_average), TRUE);
    break;
  }

  // Measure Result Font Size

  int meaResult = sysMeasure->GetMeasureResult();

  switch (meaResult) {
  case 0:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_small), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_big), TRUE);
    break;
  }

  // Trace Method

  int traceManual = sysMeasure->GetTraceMethod();

  switch (traceManual) {
  case 0:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_point), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_track), TRUE);
    break;
  case 2:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto), TRUE);
    break;
  }

  // Cursor Size
  int cursorSize = sysMeasure->GetMeasureCursorSize();

  switch (cursorSize) {
  case 0:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_big), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_mid), TRUE);
    break;
  case 2:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_small), TRUE);
    break;
  }

  // Measure Line

  int measureLine = sysMeasure->GetMeasureLineDisplay();

  switch (measureLine) {
  case 0:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_measure_line_on), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_measure_line_off), TRUE);
    break;
  }

  delete sysMeasure;
}

void ViewSystem::save_measure_setting() {
  SysMeasurementSetting sysMeasure;

  // Unit Setting

  int unitDist = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_dist));
  sysMeasure.SetUnitDist(unitDist);

  int unitArea = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_area));
  sysMeasure.SetUnitArea(unitArea);

  int unitVol = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_vol));
  sysMeasure.SetUnitVol(unitVol);

  int unitTime = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_time));
  sysMeasure.SetUnitTime(unitTime);

  int unitVel = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_vel));
  sysMeasure.SetUnitVel(unitVel);

  int unitAccel = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_accel));
  sysMeasure.SetUnitAccel(unitAccel);

  int unitEfw = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_efw));
  sysMeasure.SetUnitEfw(unitEfw);

  // Heart Beat

  int heartBeatCycle = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_heart_beat));
  sysMeasure.SetHeartBeatCycle(heartBeatCycle + 1);

  // Auto Spectrum Calculation Result

  sysMeasure.SetAutoCalcPS(true);
  sysMeasure.SetAutoCalcED(true);
  sysMeasure.SetAutoCalcRI(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ri)));
  sysMeasure.SetAutoCalcSD(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_sd)));
  sysMeasure.SetAutoCalcTAmax(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax)));
  sysMeasure.SetAutoCalcPI(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi)));
  sysMeasure.SetAutoCalcTime(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time)));
  sysMeasure.SetAutoCalcHR(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_hr)));
  sysMeasure.SetAutoCalcPGmax(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmax)));
  sysMeasure.SetAutoCalcPGmean(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmean)));

  // Line Color

  int colorIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_current_line));
  sysMeasure.SetMeasureColorCur(colorIndex);
  colorIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_confirmed_line));
  sysMeasure.SetMeasureColorConfirm(colorIndex);

  // Report Result Method

  int reportResult = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_report_last))) {
    reportResult = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_report_average))) {
    reportResult = 1;
  }

  sysMeasure.SetReportResult(reportResult);

  // Measure Result Font Size

  int meaResult = 0; //小字体

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_small))) {
    meaResult = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_big))) {
    meaResult = 1;
  }

  sysMeasure.SetMeasureResult(meaResult);

  // Trace Method

  int trace = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_point))) {
    trace = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_track))) {
    trace = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto))) {
    trace = 2;
  }

  sysMeasure.SetTraceMethod(trace);

  // Cursor Size

  int cursorSize = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_big))) {
    cursorSize = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_mid))) {
    cursorSize = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_small))) {
    cursorSize = 2;
  }

  sysMeasure.SetMeasureCursorSize(cursorSize);

  // Measure Line

  int measureLine = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_measure_line_on))) {
    measureLine = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_measure_line_off))) {
    measureLine = 1;
  }

  sysMeasure.SetMeasureLineDisplay(measureLine);

  sysMeasure.SyncFile();
  sysMeasure.UpdateTraceItemSetting();
  ImageArea::GetInstance()->UpdateMeaResultArea(meaResult);
}

GtkWidget* ViewSystem::create_note_calc_measure() {
  GtkNotebook* notebook = Utils::create_notebook();
  gtk_notebook_set_tab_pos(notebook, GTK_POS_LEFT);

  GtkWidget* calcSetting = CalcSetting::GetInstance()->CreateCalcWindow(GTK_WIDGET(m_dialog));
  GtkWidget* measureSetting = MeasureSetting::GetInstance()->CreateMeasureWindow(GTK_WIDGET(m_dialog));

  gtk_notebook_append_page(notebook, GTK_WIDGET(calcSetting), GTK_WIDGET(Utils::create_label(_("Calculate"))));
  gtk_notebook_append_page(notebook, GTK_WIDGET(measureSetting), GTK_WIDGET(Utils::create_label(_("Measure"))));

  return GTK_WIDGET(notebook);
}

GtkWidget* ViewSystem::create_note_calc() {
  GtkTable* table = Utils::create_table(10, 9);

  // OB Calc Method

  GtkFrame* frame_ob_calc = Utils::create_frame(_("OB Calc Method"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_ob_calc), 0, 9, 0, 6);

  GtkTable* table_ob_calc = Utils::create_table(5, 9);
  gtk_container_set_border_width(GTK_CONTAINER(table_ob_calc), 5);
  gtk_container_add(GTK_CONTAINER(frame_ob_calc), GTK_WIDGET(table_ob_calc));

  GtkLabel* label_ob_cer = Utils::create_label(_("CER:"));
  m_combobox_ob_cer = Utils::create_combobox_text();
  GtkLabel* label_ob_hl = Utils::create_label(_("HL:"));
  m_combobox_ob_hl = Utils::create_combobox_text();
  GtkLabel* label_ob_bpd = Utils::create_label(_("BPD:"));
  m_combobox_ob_bpd = Utils::create_combobox_text();

  GtkLabel* label_ob_fl = Utils::create_label(_("FL:"));
  m_combobox_ob_fl = Utils::create_combobox_text();
  GtkLabel* label_ob_crl = Utils::create_label(_("CRL:"));
  m_combobox_ob_crl = Utils::create_combobox_text();
  GtkLabel* label_ob_gs = Utils::create_label(_("GS:"));
  m_combobox_ob_gs = Utils::create_combobox_text();

  GtkLabel* label_ob_ac = Utils::create_label(_("AC:"));
  m_combobox_ob_ac = Utils::create_combobox_text();
  GtkLabel* label_ob_hc = Utils::create_label(_("HC:"));
  m_combobox_ob_hc = Utils::create_combobox_text();
  GtkLabel* label_ob_tad = Utils::create_label(_("TAD:"));
  m_combobox_ob_tad = Utils::create_combobox_text();

  GtkLabel* label_ob_apad = Utils::create_label(_("APAD:"));
  m_combobox_ob_apad = Utils::create_combobox_text();
  GtkLabel* label_ob_thd = Utils::create_label(_("THD:"));
  m_combobox_ob_thd = Utils::create_combobox_text();
  GtkLabel* label_ob_ofd = Utils::create_label(_("OFD:"));
  m_combobox_ob_ofd = Utils::create_combobox_text();

  GtkLabel* label_ob_efw = Utils::create_label(_("EFW:"));
  m_combobox_ob_efw = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_cer), 0, 1, 0, 1);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_cer), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_hl), 3, 4, 0, 1);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_hl), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_bpd), 6, 7, 0, 1);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_bpd), 7, 9, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_fl), 0, 1, 1, 2);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_fl), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_crl), 3, 4, 1, 2);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_crl), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_gs), 6, 7, 1, 2);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_gs), 7, 9, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_ac), 0, 1, 2, 3);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_ac), 1, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_hc), 3, 4, 2, 3);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_hc), 4, 6, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_tad), 6, 7, 2, 3);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_tad), 7, 9, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_apad), 0, 1, 3, 4);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_apad), 1, 3, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_thd), 3, 4, 3, 4);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_thd), 4, 6, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_ofd), 6, 7, 3, 4);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_ofd), 7, 9, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach_defaults(table_ob_calc, GTK_WIDGET(label_ob_efw), 0, 1, 4, 5);
  gtk_table_attach(table_ob_calc, GTK_WIDGET(m_combobox_ob_efw), 1, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_ob_cer, _("Goldstein"));
  gtk_combo_box_text_append_text(m_combobox_ob_cer, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_hl, _("Jeanty"));
  gtk_combo_box_text_append_text(m_combobox_ob_hl, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Hadlock"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Lasser"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Rempen"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Tokyo"));
  gtk_combo_box_text_append_text(m_combobox_ob_bpd, _("Custom"));

  gtk_combo_box_text_append_text(m_combobox_ob_fl, _("Hadlock"));
  gtk_combo_box_text_append_text(m_combobox_ob_fl, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_fl, _("Jeanty"));
  gtk_combo_box_text_append_text(m_combobox_ob_fl, _("Tokyo"));
  gtk_combo_box_text_append_text(m_combobox_ob_fl, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Hadlock"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Robinson"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Hansmenn"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Lasser"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Tokyo"));
  gtk_combo_box_text_append_text(m_combobox_ob_crl, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_gs, _("Hellman"));
  gtk_combo_box_text_append_text(m_combobox_ob_gs, _("Rempen"));
  gtk_combo_box_text_append_text(m_combobox_ob_gs, _("Tokyo"));
  gtk_combo_box_text_append_text(m_combobox_ob_gs, _("Custom"));

  gtk_combo_box_text_append_text(m_combobox_ob_ac, _("Hadlock"));
  gtk_combo_box_text_append_text(m_combobox_ob_ac, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_ac, _("Lasser"));
  gtk_combo_box_text_append_text(m_combobox_ob_ac, _("Tokyo"));
  gtk_combo_box_text_append_text(m_combobox_ob_ac, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_hc, _("Hadlock"));
  gtk_combo_box_text_append_text(m_combobox_ob_hc, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_hc, _("Lasser"));
  gtk_combo_box_text_append_text(m_combobox_ob_hc, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_tad, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_tad, _("Custom"));

  gtk_combo_box_text_append_text(m_combobox_ob_apad, _("Merz"));
  gtk_combo_box_text_append_text(m_combobox_ob_apad, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_thd, _("Hansmenn"));
  gtk_combo_box_text_append_text(m_combobox_ob_thd, _("Custom"));
  gtk_combo_box_text_append_text(m_combobox_ob_ofd, _("Korean"));
  gtk_combo_box_text_append_text(m_combobox_ob_ofd, _("Custom"));

  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Hadlock1"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Hadlock2"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Hadlock3"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Hadlock4"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Shepard"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Hansmenn"));
  gtk_combo_box_text_append_text(m_combobox_ob_efw, _("Tokyo"));

  // OB Custom, BSA
  GtkButton* button_ob_custom = Utils::create_button(_("OB Custom"));
  GtkLabel* label_bsa = Utils::create_label(_("BSA:"));
  m_combobox_bsa = Utils::create_combobox_text();

  gtk_table_attach(table, GTK_WIDGET(button_ob_custom), 0, 2, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_bsa), 3, 4, 6, 7);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_bsa), 4, 6, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_ob_custom, "clicked", G_CALLBACK(signal_button_clicked_ob_custom), this);

  gtk_combo_box_text_append_text(m_combobox_bsa, _("Oriental"));
  gtk_combo_box_text_append_text(m_combobox_bsa, _("Occidental"));

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 2, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_calc_default), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_calc_setting(SysCalculateSetting* sysCalc) {
  if (sysCalc == NULL) {
    sysCalc = new SysCalculateSetting();
  }

  int cer = sysCalc->GetCerMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_cer), cer);

  int hl = sysCalc->GetHlMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_hl), hl);

  int bpd = sysCalc->GetBpdMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_bpd), bpd);

  int fl = sysCalc->GetFlMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_fl), fl);

  int crl = sysCalc->GetCrlMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_crl), crl);

  int gs = sysCalc->GetGsMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_gs), gs);

  int ac = sysCalc->GetAcMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_ac), ac);

  int hc = sysCalc->GetHcMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_hc), hc);

  int tad = sysCalc->GetTadMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_tad), tad);

  int apad = sysCalc->GetApadMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_apad), apad);

  int thd = sysCalc->GetThdMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_thd), thd);

  int ofd = sysCalc->GetOfdMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_ofd), ofd);

  int efw = sysCalc->GetEfwMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_efw), efw);

  int bsa = sysCalc->GetBSAMethod();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_bsa), bsa);

  delete sysCalc;
}

void ViewSystem::save_calc_setting() {
  SysCalculateSetting sysCalc;

  int cer = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_cer));
  sysCalc.SetCerMethod(cer);

  int hl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_hl));
  sysCalc.SetHlMethod(hl);

  int bpd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_bpd));
  sysCalc.SetBpdMethod(bpd);

  int fl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_fl));
  sysCalc.SetFlMethod(fl);

  int crl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_crl));
  sysCalc.SetCrlMethod(crl);

  int gs = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_gs));
  sysCalc.SetGsMethod(gs);

  int ac = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_ac));
  sysCalc.SetAcMethod(ac);

  int hc = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_hc));
  sysCalc.SetHcMethod(hc);

  int tad = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_tad));
  sysCalc.SetTadMethod(tad);

  int apad = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_apad));
  sysCalc.SetApadMethod(apad);

  int thd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_thd));
  sysCalc.SetThdMethod(thd);

  int ofd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_ofd));
  sysCalc.SetOfdMethod(ofd);

  int efw = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_efw));
  sysCalc.SetEfwMethod(efw);

  int bsa = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_bsa));
  sysCalc.SetBSAMethod(bsa);

  sysCalc.SyncFile();
  sysCalc.UpdateOBSetting();

  g_menuCalc.UpdateEfwItem(efw);
}

GtkWidget* ViewSystem::create_note_comment() {
  GtkTable* table = Utils::create_table(14, 10);

  // Probe Type
  GtkLabel* label_probe_comment = Utils::create_label(_("Probe Type:"));
  m_combobox_probe_comment = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_probe_comment), 0, 2, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_probe_comment), 2, 5, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  for (int i = 0; i < NUM_PROBE; i++) {
    string newProbeName = ProbeMan::GetInstance()->VerifyProbeName(g_probe_list[i]);
    gtk_combo_box_text_append_text(m_combobox_probe_comment, newProbeName.c_str());
  }

  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  string default_probe = exam.ReadDefaultProbe(&ini);
  int probe_comment_index = 0;

  for (int i = 0; i < NUM_PROBE; i++) {
    if (default_probe == g_probe_list[i]) {
      probe_comment_index = i;

      break;
    } else {
      probe_comment_index = 0;
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_comment), probe_comment_index);
  g_signal_connect(m_combobox_probe_comment, "changed", G_CALLBACK(signal_combobox_changed_probe_comment), this);

  // Exam Type
  GtkLabel* label_exam_comment = Utils::create_label(_("Exam Type:"));
  m_combobox_exam_comment = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_exam_comment), 5, 7, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_exam_comment), 7, 10, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  ExamItem examItem;
  vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[probe_comment_index]);
  create_exam_comment_model(itemIndex);

  g_signal_connect(m_combobox_exam_comment, "changed", G_CALLBACK(signal_combobox_changed_exam_comment), this);

  // frame
  GtkFrame* frame = Utils::create_frame();
  gtk_table_attach_defaults(table, GTK_WIDGET(frame), 0, 10, 1, 9);

  GtkTable* table_frame = Utils::create_table(7, 10);
  gtk_container_set_border_width(GTK_CONTAINER(table_frame), 5);
  gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(table_frame));

  // Select Lexicon
  GtkLabel* label_department_comment = Utils::create_label(_("Select Lexicon:"));
  m_combobox_department_comment = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_department_comment), 0, 2, 0, 1);
  gtk_table_attach(table_frame, GTK_WIDGET(m_combobox_department_comment), 2, 5, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Abdomen"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Urology"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Cardiac"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Obstetrics"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Gynecology"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Small Part"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Vascular"));
  gtk_combo_box_text_append_text(m_combobox_department_comment, _("Orthopedic"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), DepartmentIndex());
  g_signal_connect(m_combobox_department_comment, "changed", G_CALLBACK(signal_combobox_changed_department_comment), this);

  // left scrolled window
  GtkScrolledWindow* scrolledwindow_item = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(scrolledwindow_item), 0, 3, 1, 7);

  GtkTreeModel* model = create_item_comment_model(0);
  m_treeview_item_comment = Utils::create_tree_view(model);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_item), GTK_WIDGET(m_treeview_item_comment));

  add_columns_comment(m_treeview_item_comment);

  // button
  GtkButton* button_select_one = Utils::create_button(_(">"));
  GtkButton* button_select_all = Utils::create_button(_(">>"));
  GtkButton* button_comment_add = Utils::create_button(_("Add"));
  GtkButton* button_comment_delete_select = Utils::create_button(_("Delete"));

  gtk_table_attach(table_frame, GTK_WIDGET(button_select_one), 3, 5, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_select_all), 3, 5, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_add), 3, 5, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_delete_select), 3, 5, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_select_one, "clicked", G_CALLBACK(signal_button_clicked_selectone_comment), this);
  g_signal_connect(button_select_all, "clicked", G_CALLBACK(signal_button_clicked_selectall_comment), this);
  g_signal_connect(button_comment_add, "clicked", G_CALLBACK(signal_button_clicked_add), this);
  g_signal_connect(button_comment_delete_select, "clicked", G_CALLBACK(signal_button_clicked_deleteselect), this);

  // right scrolled window
  GtkScrolledWindow* scrolledwindow_item_right = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(scrolledwindow_item_right), 5, 8, 1, 7);

  GtkTreeModel* model_selected = create_item_comment_model_selected();
  m_treeview_item_comment_selected = Utils::create_tree_view(model_selected);
  gtk_container_add(GTK_CONTAINER(scrolledwindow_item_right), GTK_WIDGET(m_treeview_item_comment_selected));

  add_columns_comment_selected(m_treeview_item_comment_selected);

  // button
  GtkButton* button_comment_up = Utils::create_button(_("Up"));
  GtkButton* button_comment_down = Utils::create_button(_("Down"));
  GtkButton* button_comment_delete = Utils::create_button(_("Delete"));
  GtkButton* button_comment_delete_all = Utils::create_button(_("All Delete"));

  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_up), 8, 10, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_down), 8, 10, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_delete), 8, 10, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(button_comment_delete_all), 8, 10, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_comment_up, "clicked", G_CALLBACK(signal_button_clicked_up), this);
  g_signal_connect(button_comment_down, "clicked", G_CALLBACK(signal_button_clicked_down), this);
  g_signal_connect(button_comment_delete, "clicked", G_CALLBACK(signal_button_clicked_delete), this);
  g_signal_connect(button_comment_delete_all, "clicked", G_CALLBACK(signal_button_clicked_deleteall), this);

  // Font Size
  GtkFrame* frame_font_size = Utils::create_frame(_("Font Size"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_font_size), 0, 5, 9, 11);

  GtkTable* table_font_size = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_font_size), 5);
  gtk_container_add(GTK_CONTAINER(frame_font_size), GTK_WIDGET(table_font_size));

  m_radiobutton_font_big = Utils::create_radio_button(NULL, _("Big"));
  GSList* radiobutton_font_size_group = gtk_radio_button_get_group(m_radiobutton_font_big);
  m_radiobutton_font_mid = Utils::create_radio_button(radiobutton_font_size_group, _("Mid"));
  radiobutton_font_size_group = gtk_radio_button_get_group(m_radiobutton_font_mid);
  m_radiobutton_font_small = Utils::create_radio_button(radiobutton_font_size_group, _("Small"));

  gtk_table_attach_defaults(table_font_size, GTK_WIDGET(m_radiobutton_font_big), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_font_size, GTK_WIDGET(m_radiobutton_font_mid), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_font_size, GTK_WIDGET(m_radiobutton_font_small), 2, 3, 0, 1);

  // Bodymark Size
  GtkFrame* frame_bodymark_size = frame = Utils::create_frame(_("Bodymark Size"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_bodymark_size), 0, 5, 11, 13);

  GtkTable* table_bodymark_size = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_bodymark_size), 5);
  gtk_container_add(GTK_CONTAINER(frame_bodymark_size), GTK_WIDGET(table_bodymark_size));

  m_radiobutton_bodymark_big = Utils::create_radio_button(NULL, _("Big"));
  GSList* radiobutton_bodymark_size_group = gtk_radio_button_get_group(m_radiobutton_bodymark_big);
  m_radiobutton_bodymark_mid = Utils::create_radio_button(radiobutton_bodymark_size_group, _("Mid"));
  radiobutton_bodymark_size_group = gtk_radio_button_get_group(m_radiobutton_bodymark_mid);
  m_radiobutton_bodymark_small = Utils::create_radio_button(radiobutton_bodymark_size_group, _("Small"));

  gtk_table_attach_defaults(table_bodymark_size, GTK_WIDGET(m_radiobutton_bodymark_big), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_bodymark_size, GTK_WIDGET(m_radiobutton_bodymark_mid), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_bodymark_size, GTK_WIDGET(m_radiobutton_bodymark_small), 2, 3, 0, 1);

  // Font Color
  GtkLabel* label_font_color = Utils::create_label(_("Font Color:"));
  m_combobox_font_color = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_font_color), 5, 7, 9, 11);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_font_color), 7, 10, 9, 11, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_font_color, _("Red"));
  gtk_combo_box_text_append_text(m_combobox_font_color, _("Green"));
  gtk_combo_box_text_append_text(m_combobox_font_color, _("Blue"));
  gtk_combo_box_text_append_text(m_combobox_font_color, _("White"));

  // Probe Mark Color
  GtkLabel* label_bodymark_color = Utils::create_label(_("Probe Mark Color:"));
  m_combobox_bodymark_color = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_bodymark_color), 5, 7, 11, 13);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_bodymark_color), 7, 10, 11, 13, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("White"));
  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("Gray"));
  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("Red"));
  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("Green"));
  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("Yellow"));
  gtk_combo_box_text_append_text(m_combobox_bodymark_color, _("Blue"));

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 2, 13, 14, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK (on_button_comment_default_clicked), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_comment_setting(SysNoteSetting* sysNoteSetting) {
  if (sysNoteSetting == NULL) {
    sysNoteSetting = new SysNoteSetting();
  }

  int index_font_size = sysNoteSetting->GetFontSize();

  switch (index_font_size) {
  case 0:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_big), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_mid), TRUE);
    break;
  case 2:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_small), TRUE);
    break;
  }

  int index_bdmk_size = sysNoteSetting->GetBodyMarkSize();

  switch (index_bdmk_size) {
  case 2:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_big), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_mid), TRUE);
    break;
  case 0:
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_small), TRUE);
    break;
  }

  int index_font_color = sysNoteSetting->GetFontColor();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_font_color), index_font_color);

  int index_bdmk_color = sysNoteSetting->GetBodyMarkColor();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_bodymark_color), index_bdmk_color);

  delete sysNoteSetting;
}

void ViewSystem::save_comment_setting() {
  SysNoteSetting sysNoteSetting;

  int fontSizeIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_big))) {
    fontSizeIndex = 0;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_mid))) {
    fontSizeIndex = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_small))) {
    fontSizeIndex = 2;
  }

  sysNoteSetting.SetFontSize(fontSizeIndex);

  int bdmkSizeIndex = 0;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_big))) {
    bdmkSizeIndex = 2;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_mid))) {
    bdmkSizeIndex = 1;
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_small))) {
    bdmkSizeIndex = 0;
  }

  sysNoteSetting.SetBodyMarkSize(bdmkSizeIndex);

  int fontColorIndex  = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_font_color));
  sysNoteSetting.SetFontColor(fontColorIndex);

  int bdmkColorIndex  = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_bodymark_color));
  sysNoteSetting.SetBodyMarkColor(bdmkColorIndex);

  sysNoteSetting.SyncFile();
}

GtkWidget* ViewSystem::create_note_tvout() {
  GtkTable* table = Utils::create_table(12, 6);

  // Video Standard
  GtkLabel* label_video = Utils::create_label(_("Video Standard:"));
  m_combobox_video = Utils::create_combobox_text();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_video), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_video), 1, 2, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_video, _("NTSC"));
  gtk_combo_box_text_append_text(m_combobox_video, _("PAL"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video), 0);
  g_signal_connect(m_combobox_video, "changed", G_CALLBACK(signal_combobox_changed_video), this);

  // TV Out Format
  GtkFrame* frame_tvout = Utils::create_frame();
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_tvout), 0, 6, 1, 6);

  SysGeneralSetting sg;

  if (sg.GetConnectorType()) {
    gtk_frame_set_label(frame_tvout, _("TV Out Format (Restart to complete changes)"));
  } else {
    gtk_frame_set_label(frame_tvout, _("TV Out Format"));
  }

  GtkTable* table_tvout = Utils::create_table(1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_tvout), 5);
  gtk_container_add(GTK_CONTAINER(frame_tvout), GTK_WIDGET(table_tvout));

  m_radiobutton_format_1 = Utils::create_radio_button(NULL, _("Full Screen"));
  GSList* radiobutton_format_group = gtk_radio_button_get_group(m_radiobutton_format_1);
  m_radiobutton_format_2 = Utils::create_radio_button(radiobutton_format_group, _("Hide Menu Area"));
  radiobutton_format_group = gtk_radio_button_get_group(m_radiobutton_format_2);
  m_radiobutton_format_3 = Utils::create_radio_button(radiobutton_format_group, _("Image Area Only"));

  gtk_table_attach_defaults(table_tvout, GTK_WIDGET(m_radiobutton_format_1), 0, 1, 0, 1);
  gtk_table_attach_defaults(table_tvout, GTK_WIDGET(m_radiobutton_format_2), 1, 2, 0, 1);
  gtk_table_attach_defaults(table_tvout, GTK_WIDGET(m_radiobutton_format_3), 2, 3, 0, 1);

  Utils::set_button_image(GTK_BUTTON(m_radiobutton_format_1), Utils::create_image("./res/btn_format/tv-out-1.png"), GTK_POS_TOP);
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_format_2), Utils::create_image("./res/btn_format/tv-out-2.png"), GTK_POS_TOP);
  Utils::set_button_image(GTK_BUTTON(m_radiobutton_format_3), Utils::create_image("./res/btn_format/tv-out-3.png"), GTK_POS_TOP);

  // Printer
  GtkFrame* frame_printer = Utils::create_frame(_("Printer"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_printer), 0, 6, 6, 11);

  GtkTable* table_printer = Utils::create_table(4, 6);
  gtk_container_set_border_width(GTK_CONTAINER(table_printer), 5);
  gtk_container_add(GTK_CONTAINER(frame_printer), GTK_WIDGET(table_printer));

  m_scrolledwindow_common = Utils::create_scrolled_window();
  m_scrolledwindow_specific = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table_printer, GTK_WIDGET(m_scrolledwindow_common), 0, 4, 0, 4);
  gtk_table_attach_defaults(table_printer, GTK_WIDGET(m_scrolledwindow_specific), 0, 4, 0, 4);

  m_treeview_common_print = create_common_print_treeview();
  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_common), GTK_WIDGET(m_treeview_common_print));
  m_selected_common_printer = gtk_tree_view_get_selection(m_treeview_common_print);

  m_treeview_specific_print = create_specific_print_treeview();
  gtk_container_add(GTK_CONTAINER(m_scrolledwindow_specific), GTK_WIDGET(m_treeview_specific_print));
  m_selected_specific_printer = gtk_tree_view_get_selection(m_treeview_specific_print);

  g_signal_connect(m_selected_common_printer, "changed", G_CALLBACK(on_common_treeview_selection_changed), this);
  g_signal_connect(m_selected_specific_printer, "changed", G_CALLBACK(on_specific_treeview_selection_changed), this);

  m_radiobutton_common = Utils::create_radio_button(NULL, _("Common"));
  GSList* radiobutton_printer_group = gtk_radio_button_get_group(m_radiobutton_common);
  m_radiobutton_specific = Utils::create_radio_button(radiobutton_printer_group, _("Specific"));

  gtk_table_attach_defaults(table_printer, GTK_WIDGET(m_radiobutton_common), 4, 6, 0, 1);
  gtk_table_attach_defaults(table_printer, GTK_WIDGET(m_radiobutton_specific), 4, 6, 1, 2);

  g_signal_connect(m_radiobutton_common, "toggled", G_CALLBACK(signal_radio_button_toggled_common), this);
  g_signal_connect(m_radiobutton_specific, "toggled", G_CALLBACK(signal_radio_button_toggled_specific), this);

  GtkButton* button_add_printer = Utils::create_button(_("Add"));
  GtkButton* button_del_printer = Utils::create_button(_("Delete"));

  gtk_table_attach(table_printer, GTK_WIDGET(button_add_printer), 4, 5, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_printer, GTK_WIDGET(button_del_printer), 4, 5, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add_printer, "clicked", G_CALLBACK(signal_button_clicked_add_printer), this);
  g_signal_connect(button_del_printer, "clicked", G_CALLBACK(signal_button_clicked_del_printer), this);

  // Default Factory
  GtkButton* button_default = Utils::create_button(_("Default Factory"));
  gtk_table_attach(table, GTK_WIDGET(button_default), 0, 1, 11, 12, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_tvout_default), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_tvout_setting(SysGeneralSetting* sysGeneralSetting) {
  if (sysGeneralSetting == NULL) {
    sysGeneralSetting = new SysGeneralSetting();
  }

  int index_video_mode = sysGeneralSetting->GetVideoMode();
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video), index_video_mode);

  int tvout_format = sysGeneralSetting->GetVideoFormat();

  switch (tvout_format) {
  case 0:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_1), TRUE);
    break;
  case 1:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_2), TRUE);
    break;
  case 2:
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_3), TRUE);
    break;
  }

  if (sysGeneralSetting->GetPrinterMethod() == 0) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_common), TRUE);

    gtk_widget_show_all(GTK_WIDGET(m_scrolledwindow_common));
    gtk_widget_hide_all(GTK_WIDGET(m_scrolledwindow_specific));

    int index_printer = sysGeneralSetting->GetPrinter();

    stringstream ss;
    ss << index_printer;

    GtkTreePath* path = gtk_tree_path_new_from_string(ss.str().c_str());
    gtk_tree_selection_select_path(gtk_tree_view_get_selection(m_treeview_common_print), path);
  } else {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific), TRUE);

    gtk_widget_hide_all(GTK_WIDGET(m_scrolledwindow_common));
    gtk_widget_show_all(GTK_WIDGET(m_scrolledwindow_specific));
  }

  delete sysGeneralSetting;
}

void ViewSystem::save_tvout_setting() {
  SysGeneralSetting sysGeneralSetting;

  int tvout_format = 0;
  string command;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_1))) {
    tvout_format = 0;
    command = "--mode 1024x768 --pos 0x0";
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_2))) {
    tvout_format = 1;
    command = "--mode 848x660 --pos 180x0";
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_3))) {
    tvout_format = 2;
    command = "--mode 848x560 --pos 180x100";
  }

  int videoModeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video));

  if (sysGeneralSetting.GetVideoMode() != videoModeIndex || sysGeneralSetting.GetVideoFormat() != tvout_format) {
    if (videoModeIndex) {
      command = "xrandr --output TV1 --set mode NTSC-M " + command;
    } else {
      command = "xrandr --output TV1 --set mode PAL " + command;
    }

    if(!sysGeneralSetting.GetConnectorType()) {
      _system_(command.c_str());
    }

    sysGeneralSetting.SetVideoMode(videoModeIndex);
    sysGeneralSetting.SetVideoFormat(tvout_format);
  }

  // save printer
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_common))) {
    int printIndex = common_printer_selection();

    if (printIndex != -1) {
      sysGeneralSetting.SetPrinterMethod(0);
      sysGeneralSetting.SetPrinter(printIndex);
    }
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific))) {
    string default_print = specific_printer_selection();

    if (!default_print.empty()) {
      Printer print;
      sysGeneralSetting.SetPrinterMethod(1);
      print.SetDefault(default_print.c_str());
      PeripheralMan::GetInstance()->SwitchPrinterDriver();
      UpdateSpecificPrinterModel();
    }
  }

  sysGeneralSetting.SyncFile();
}

GtkWidget* ViewSystem::create_set_report() {
  InitReportVar();

  GtkTable* table = Utils::create_table(10, 7);

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window), 0, 2, 0, 9);

  m_treeviewReportSet = Utils::create_tree_view(CreateTreeModel());
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeviewReportSet));

  gtk_tree_view_set_enable_search(m_treeviewReportSet, FALSE);
  gtk_tree_view_set_rules_hint(m_treeviewReportSet, TRUE);
  gtk_tree_view_set_headers_visible(m_treeviewReportSet, FALSE);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(m_treeviewReportSet), GTK_SELECTION_SINGLE);

  GtkTreePath* path = gtk_tree_path_new_first();
  gtk_tree_view_set_cursor(m_treeviewReportSet, path, NULL, TRUE);
  gtk_tree_path_free(path);

  m_renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("", m_renderer, "text", 0, NULL);
  gtk_tree_view_append_column(m_treeviewReportSet, column);

  g_signal_connect(G_OBJECT(gtk_tree_view_get_selection(m_treeviewReportSet)), "changed", G_CALLBACK(signal_tree_selection_changed), this);
  gtk_widget_add_events(GTK_WIDGET(m_treeviewReportSet), (gtk_widget_get_events(GTK_WIDGET(m_treeviewReportSet)) | GDK_BUTTON_RELEASE_MASK));
  g_signal_connect_after(m_treeviewReportSet, "button_release_event", G_CALLBACK(signal_treeview_button_release_exam_department), this);

  // scrolled window show
  m_scrolled_window_show = Utils::create_scrolled_window();
  gtk_table_attach_defaults(table, GTK_WIDGET(m_scrolled_window_show), 2, 7, 0, 9);

  // button
  m_button_report_recovery = Utils::create_button(_("Recovery"));
  m_button_report_add = Utils::create_button(_("Insert"));

  m_label_templet = Utils::create_label(_("Please Input New Name:"));
  m_entry_templet = Utils::create_entry(9679);

  m_label_childsection = Utils::create_label(_("Please Select Exam Type:"));
  m_combobox_childsection = Utils::create_combobox_text();

  m_button_report_save = Utils::create_button(_("Save"));
  m_button_report_delete = Utils::create_button(_("Delete"));

  m_button_report_ok = Utils::create_button(_("OK"));
  m_button_report_cancel = Utils::create_button(_("Cancel"));

  gtk_table_attach(table, GTK_WIDGET(m_button_report_recovery), 0, 1, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(m_button_report_add), 1, 2, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_templet), 1, 3, 9, 10);
  gtk_table_attach(table, GTK_WIDGET(m_entry_templet), 3, 5, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_childsection), 1, 3, 9, 10);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_childsection), 3, 5, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_table_attach(table, GTK_WIDGET(m_button_report_save), 5, 6, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(m_button_report_delete), 6, 7, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(m_button_report_ok), 5, 6, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(m_button_report_cancel), 6, 7, 9, 10, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_templet, 46);

  gtk_combo_box_text_append_text(m_combobox_childsection, _("Abdomen"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Cardiac"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Urology"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Obstetrics"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Gynecology"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Small Part"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Fetal Cardio"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Vascular"));
  gtk_combo_box_text_append_text(m_combobox_childsection, _("Others"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_childsection), 0);

  g_signal_connect(m_button_report_recovery, "clicked", G_CALLBACK(signal_button_clicked_report_recovery), this);
  g_signal_connect(m_button_report_add, "clicked", G_CALLBACK(signal_button_clicked_report_add), this);
  g_signal_connect(m_button_report_save, "clicked", G_CALLBACK(signal_button_clicked_report_save), this);
  g_signal_connect(m_button_report_delete, "clicked", G_CALLBACK(signal_button_clicked_report_delete), this);
  g_signal_connect(m_button_report_ok, "clicked", G_CALLBACK (signal_button_clicked_report_ok), this);
  g_signal_connect(m_button_report_cancel, "clicked", G_CALLBACK (signal_button_clicked_report_cancel), this);
  g_signal_connect(G_OBJECT(m_entry_templet), "insert_text", G_CALLBACK(signal_entry_insert_templet), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_report_setting() {
  gtk_widget_hide(GTK_WIDGET(m_label_templet));
  gtk_widget_hide(GTK_WIDGET(m_entry_templet));

  gtk_widget_hide(GTK_WIDGET(m_label_childsection));
  gtk_widget_hide(GTK_WIDGET(m_combobox_childsection));

  gtk_widget_hide(GTK_WIDGET(m_button_report_save));
  gtk_widget_hide(GTK_WIDGET(m_button_report_delete));
  gtk_widget_hide(GTK_WIDGET(m_button_report_ok));
  gtk_widget_hide(GTK_WIDGET(m_button_report_cancel));

  gtk_widget_hide(GTK_WIDGET(m_scrolled_window_show));
}

void ViewSystem::InitReportVar() {
  m_show_window = NULL;

  m_WindowABD = NULL;
  m_WindowUR = NULL;
  m_WindowAE = NULL;
  m_WindowOB = NULL;
  m_WindowGYN = NULL;
  m_WindowSP = NULL;
  m_WindowFE = NULL;
  m_WindowOTHERS = NULL;
  m_WindowVES = NULL;
  m_WindowTD = NULL;
  m_WindowANOB = NULL;

  m_FixedReportSet = NULL;
  m_treeviewReportSet = NULL;
  memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
  memset(CheckButtonArr, 0, sizeof(CheckButtonArr));

  SysGeneralSetting sys;
  init_language = sys.GetLanguage();
}

GtkWidget* ViewSystem::CreatWindowABD() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowABD) {
    return m_WindowABD;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>General</b></i>"), ABDO_M, ABD_MEA_START,  ABD_DIST_AO, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Arterial</b></i>"),  ABDO_M, ABD_DIST_AO, ABD_PROX_IVC, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Venous</b></i>"), ABDO_M, ABD_PROX_IVC, ABD_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowABD = scrolledwindow;

  return m_WindowABD;
}

GtkWidget* ViewSystem::CreatWindowAE() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowAE) {
    return m_WindowAE;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>Dimensions(2D)</b></i>"), ADULT_M, ADULT_MEA_START, ADULT_AOR_DIAM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Vessels(2D)</b></i>"), ADULT_M, ADULT_AOR_DIAM, ADULT_A2CD, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>EF &amp; Volume(2D)</b></i>"), ADULT_M, ADULT_A2CD, ADULT_AV_AREA, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Valves(2D)</b></i>"), ADULT_M, ADULT_AV_AREA, ADULT_RVAWD_MM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Dimensions(M)</b></i>"), ADULT_M, ADULT_RVAWD_MM, ADULT_LV_ET, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Aortic Valve(M)</b></i>"), ADULT_M, ADULT_LV_ET, ADULT_MV_DE_EXC, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle( _("<i><b>Mitral Valve(M)</b></i>"), ADULT_M, ADULT_MV_DE_EXC, ADULT_TV_DE_EXC, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Tricuspid Valve(M)</b></i>"), ADULT_M, ADULT_TV_DE_EXC, ADULT_LATE_DIAS_SLOPE, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Pulmonic Valve(M)</b></i>"), ADULT_M, ADULT_LATE_DIAS_SLOPE, ADULT_AV_ACC_T, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Aortic Valve(D)</b></i>"), ADULT_M, ADULT_AV_ACC_T, ADULT_HR_MV, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Mitral Valve(D)</b></i>"), ADULT_M, ADULT_HR_MV, ADULT_HR_TV, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Tricuspid Valve(D)</b></i>"), ADULT_M, ADULT_HR_TV, ADULT_HR_PV, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Pulmonic Valve(D)</b></i>"), ADULT_M, ADULT_HR_PV, ADULT_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowAE = scrolledwindow;

  return m_WindowAE;
}

GtkWidget* ViewSystem::CreatWindowUR() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowUR) {
    return m_WindowUR;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>Bladder</b></i>"), UR_M, UR_MEA_START, UR_PROSTATE_VOL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Prostate</b></i>"), UR_M, UR_PROSTATE_VOL, UR_L_KID_VOL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Left Renal</b></i>"), UR_M, UR_L_KID_VOL, UR_R_KID_VOL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle( _("<i><b>Right Renal</b></i>"), UR_M, UR_R_KID_VOL, UR_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  // Show PSAD
  frame = AddFrameForOthers(_("PSAD"), &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowUR = scrolledwindow;

  return m_WindowUR;
}

GtkWidget* ViewSystem::CreatWindowOB() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowOB) {
    return m_WindowOB;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  char title[256];
  //add frames
  sprintf(title, "<i><b>%s</b></i>", _("Average"));
  frame = AddFrameForOthers(title, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //Fetal 1
  sprintf(title, "<i><b>%s%s</b></i>", _("Advanced"), _("(Fetus 1)"));
  frame = AddFrameByTitle(title, OB_M, OB_MEA_START, OB_AFI, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("AFI"), _("(Fetus 1)"));
  frame = AddFrameByTitle(title, OB_M, OB_AFI, OB_CRL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Early"), _("(Fetus 1)"));
  frame = AddFrameByTitle(title, OB_M, OB_CRL, OB_RA_DIMEN, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Fetal Heart"), _("(Fetus 1)"));
  frame = AddFrameByTitle( title, OB_M, OB_RA_DIMEN, OB_GS, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("General"), _("(Fetus 1)"));
  frame = AddFrameByTitle(title, OB_M, OB_GS, OB_EAR, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //EFW
  sprintf(title, "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 1)"));
  //AddOBEfwTable(fixed, &y, title, 1);
  frame = AddFrameForOthers(title, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Other"), _("(Fetus 1)"));
  frame = AddFrameByTitle(title, OB_M, OB_EAR, OB_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //Fetal 2
  sprintf(title, "<i><b>%s%s</b></i>", _("Advanced"), _("(Fetus 2)"));
  frame = AddFrameByTitle2(title, OB_M, OB_MEA_START, OB_AFI, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("AFI"), _("(Fetus 2)"));
  frame = AddFrameByTitle2(title, OB_M, OB_AFI, OB_CRL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Early"), _("(Fetus 2)"));
  frame = AddFrameByTitle2(title, OB_M, OB_CRL, OB_RA_DIMEN, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Fetal Heart"), _("(Fetus 2)"));
  frame = AddFrameByTitle2( title, OB_M, OB_RA_DIMEN, OB_GS, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("General"), _("(Fetus 2)"));
  frame = AddFrameByTitle2(title, OB_M, OB_GS, OB_EAR, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //EFW
  sprintf(title, "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 2)"));
  frame = AddFrameForOthers(title, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  sprintf(title, "<i><b>%s%s</b></i>", _("Other"), _("(Fetus 2)"));
  frame = AddFrameByTitle2(title, OB_M, OB_EAR, OB_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowOB = scrolledwindow;

  return m_WindowOB;
}

GtkWidget* ViewSystem::CreatWindowGYN() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowGYN) {
    return m_WindowGYN;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>General</b></i>"), GYN_M, GYN_MEA_START, GYN_L_FOLL1, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Fertility</b></i>"), GYN_M, GYN_L_FOLL1, GYN_FIBROID1, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Fibroid</b></i>"), GYN_M, GYN_FIBROID1, GYN_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowGYN = scrolledwindow;

  return m_WindowGYN;
}

GtkWidget* ViewSystem::CreatWindowSP() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowSP) {
    return m_WindowSP;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>Breast</b></i>"), SP_M, SP_MEA_START, SP_L_TESTIS_VOL, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Testicle</b></i>"), SP_M, SP_L_TESTIS_VOL, SP_ISTHMUS_AP, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Thyroid</b></i>"), SP_M, SP_ISTHMUS_AP, SP_L_EYE_OA, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Eyeball</b></i>"), SP_M, SP_L_EYE_OA, SP_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowSP = scrolledwindow;

  return m_WindowSP;
}

GtkWidget* ViewSystem::CreatWindowFE() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowFE) {
    return m_WindowFE;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>Dimen(2D)</b></i>"), FETAL_M, FETAL_MEA_START, FETAL_AOR_DIAM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Vessels(2D)</b></i>"), FETAL_M, FETAL_AOR_DIAM, FETAL_AO_AN_DIAM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Valves(2D)</b></i>"), FETAL_M, FETAL_AO_AN_DIAM, FETAL_RVAWD_MM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Dimen(M)</b></i>"), FETAL_M, FETAL_RVAWD_MM, FETAL_AOR_DIAM_MM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Vessels(M)</b></i>"), FETAL_M, FETAL_AOR_DIAM_MM, FETAL_LV_ET_MM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>AV &amp; MV(M)</b></i>"), FETAL_M, FETAL_LV_ET_MM, FETAL_LATE_DIAS_SLOPE_MM, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>PV &amp; TV(M)</b></i>"), FETAL_M, FETAL_LATE_DIAS_SLOPE_MM, FETAL_PLACENTA, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Utero &amp; Placenta(D)</b></i>"), FETAL_M, FETAL_PLACENTA, FETAL_FROAMEN_OVALE, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Vessels(D)</b></i>"), FETAL_M, FETAL_FROAMEN_OVALE, FETAL_LVOT_ACC_TIME, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>AV &amp; MV(D)</b></i>"), FETAL_M, FETAL_LVOT_ACC_TIME, FETAL_RVOT_ACC_TIME, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>PV &amp; TV(D)</b></i>"), FETAL_M, FETAL_RVOT_ACC_TIME, FETAL_THORACIC_AO, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>Peripheral Vasc(D)</b></i>"), FETAL_M, FETAL_THORACIC_AO, FETAL_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowFE = scrolledwindow;

  return m_WindowFE;
}

GtkWidget* ViewSystem::CreatWindowVES() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowVES) {
    return m_WindowVES;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  //add frames
  frame = AddFrameByTitle(_("<i><b>Carotid</b></i>"), VS_M, VS_MEA_START, VS_L_CFA, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>LE Art</b></i>"), VS_M,  VS_L_CFA, VS_L_ANTECUBE, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  frame = AddFrameByTitle(_("<i><b>UE Art</b></i>"), VS_M, VS_L_ANTECUBE, VS_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowVES = scrolledwindow;

  return m_WindowVES;
}

GtkWidget* ViewSystem::CreatWindowOTHERS() {
  GtkWidget* scrolledwindow;
  GtkWidget* frame;
  GtkWidget *viewport;
  GtkWidget* fixed;
  int x = GRAY_WIDTH, y = 20, high = 0;

  if(m_WindowOTHERS) {
    return m_WindowOTHERS;
  }

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (viewport), fixed);

  frame = AddFrameByTitle(_("TCD"), TCD_M, TCD_MEA_START, TCD_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //add frames
  frame = AddFrameByTitle(_("Orthopedic"), ORTHO_M, ORTHO_MEA_START, ORTHO_MEA_END, &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  //others
  frame = AddFrameForOthers(_("Others"), &high);
  gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
  y += high + 20;

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
  gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  m_WindowOTHERS = scrolledwindow;

  return m_WindowOTHERS;
}

GtkWidget* ViewSystem::create_note_dicom() {
  GtkNotebook* notebook = Utils::create_notebook();
  gtk_notebook_set_tab_pos(notebook, GTK_POS_LEFT);

  GtkWidget* dicomLocalSetting = DicomLocalSetting::GetInstance()->CreateDicomWindow(GTK_WIDGET(m_dialog));
  DicomLocalSetting::GetInstance()->InitLocalSetting();

  GtkWidget* dicomServerSetting = DicomServerSetting::GetInstance()->CreateDicomWindow(GTK_WIDGET(m_dialog));
  // DicomServerSetting::GetInstance()-> InitServerSetting();

  GtkWidget* dicomServiceSetting = DicomServiceSetting::GetInstance()->CreateDicomWindow(GTK_WIDGET(m_dialog));
  // DicomServiceSetting::GetInstance()->InitServiceSetting();

  gtk_notebook_append_page(notebook, GTK_WIDGET(dicomLocalSetting), GTK_WIDGET(Utils::create_label(_("Local"))));
  gtk_notebook_append_page(notebook, GTK_WIDGET(dicomServerSetting), GTK_WIDGET(Utils::create_label(_("Server"))));
  gtk_notebook_append_page(notebook, GTK_WIDGET(dicomServiceSetting), GTK_WIDGET(Utils::create_label(_("Service"))));

  g_signal_connect(G_OBJECT(notebook), "switch-page", G_CALLBACK(signal_notebook_switch_page_dicom), this);

  return GTK_WIDGET(notebook);
}

GtkWidget* ViewSystem::create_note_key_config() {
  GtkTable* table = Utils::create_table(10, 6);

  // P1
  GtkFrame* frame_key_p1 = Utils::create_frame(_("P1"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_key_p1), 0, 6, 0, 3);

  GtkTable* table_key_p1 = Utils::create_table(ceil(MAX_KEY / 5.0), 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_key_p1), 5);
  gtk_container_add(GTK_CONTAINER(frame_key_p1), GTK_WIDGET(table_key_p1));

  GSList* radiobutton_key_p1_group = NULL;

  for (int i= 0; i < MAX_KEY; i++) {
    if (KeyFunctionList[i] == "Print Screen") {
      m_radiobutton_key_p1[i] = Utils::create_radio_button(radiobutton_key_p1_group, _("Print"));
    } else {
      m_radiobutton_key_p1[i] = Utils::create_radio_button(radiobutton_key_p1_group, KeyFunctionList[i].c_str());
    }

    int raw = i / 5;
    int col = i % 5;

    radiobutton_key_p1_group = gtk_radio_button_get_group(m_radiobutton_key_p1[i]);
    gtk_table_attach_defaults(table_key_p1, GTK_WIDGET(m_radiobutton_key_p1[i]), col, col + 1, raw, raw + 1);
  }

  // P2
  GtkFrame* frame_key_p2 = Utils::create_frame(_("P2"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_key_p2), 0, 6, 3, 6);

  GtkTable* table_key_p2 = Utils::create_table(ceil(MAX_KEY / 5.0), 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_key_p2), 5);
  gtk_container_add(GTK_CONTAINER(frame_key_p2), GTK_WIDGET(table_key_p2));

  GSList* radiobutton_key_p2_group = NULL;

  for (int i= 0; i < MAX_KEY; i++) {
    if (KeyFunctionList[i] == "Print Screen") {
      m_radiobutton_key_p2[i] = Utils::create_radio_button(radiobutton_key_p2_group, _("Print"));
    } else {
      m_radiobutton_key_p2[i] = Utils::create_radio_button(radiobutton_key_p2_group, KeyFunctionList[i].c_str());
    }

    int raw = i / 5;
    int col = i % 5;

    radiobutton_key_p2_group = gtk_radio_button_get_group(m_radiobutton_key_p2[i]);
    gtk_table_attach_defaults(table_key_p2, GTK_WIDGET(m_radiobutton_key_p2[i]), col, col + 1, raw, raw + 1);
  }

  // P3
  GtkFrame* frame_key_p3 = Utils::create_frame(_("P3"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_key_p3), 0, 6, 6, 9);

  GtkTable* table_key_p3 = Utils::create_table(ceil(MAX_KEY / 5.0), 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_key_p3), 5);
  gtk_container_add(GTK_CONTAINER(frame_key_p3), GTK_WIDGET(table_key_p3));

  GSList* radiobutton_key_p3_group = NULL;

  for (int i= 0; i < MAX_KEY; i++) {
    if (KeyFunctionList[i] == "Print Screen") {
      m_radiobutton_key_p3[i] = Utils::create_radio_button(radiobutton_key_p3_group, _("Print"));
    } else {
      m_radiobutton_key_p3[i] = Utils::create_radio_button(radiobutton_key_p3_group, KeyFunctionList[i].c_str());
    }

    int raw = i / 5;
    int col = i % 5;

    radiobutton_key_p3_group = gtk_radio_button_get_group(m_radiobutton_key_p3[i]);
    gtk_table_attach_defaults(table_key_p3, GTK_WIDGET(m_radiobutton_key_p3[i]), col, col + 1, raw, raw + 1);
  }

  return GTK_WIDGET(table);
}

void ViewSystem::init_key_config() {
  SysUserDefinedKey sysDefine;
  m_p1_func_index = sysDefine.GetFuncP1();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p1[m_p1_func_index]), TRUE);

  m_p2_func_index = sysDefine.GetFuncP2();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p2[m_p2_func_index]), TRUE);

  m_p3_func_index = sysDefine.GetFuncP3();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p3[m_p3_func_index]), TRUE);
}

void ViewSystem::save_key_config() {
  SysUserDefinedKey sysDefine;

  for(int i = 0; i < MAX_KEY; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p1[i]))) {
      m_p1_func_index = i;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p2[i]))) {
      m_p2_func_index = i;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p3[i]))) {
      m_p3_func_index = i;
    }
  }

  sysDefine.SetFuncP1(m_p1_func_index);
  sysDefine.SetFuncP2(m_p2_func_index);
  sysDefine.SetFuncP3(m_p3_func_index);
  sysDefine.SyncFile();
}

GtkWidget* ViewSystem::create_note_info() {
  GtkTable* table = Utils::create_table(10, 6);

  // frame
  GtkFrame* frame_version = Utils::create_frame(_("Software and FPGA Version"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_version), 0, 4, 0, 4);

  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_container_add(GTK_CONTAINER(frame_version), GTK_WIDGET(scrolled_window));

  m_textview_version = Utils::create_text_view();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_textview_version));
  gtk_text_view_set_editable(m_textview_version, FALSE);

  // button
  GtkButton* button_upgrade = Utils::create_button(_("Upgrade"));
  gtk_table_attach(table, GTK_WIDGET(button_upgrade), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_upgrade, "clicked", G_CALLBACK (signal_button_clicked_upgrade), this);

  return GTK_WIDGET(table);
}

void ViewSystem::init_info_setting() {
  string machine = UpgradeMan::GetInstance()->GetMachineType();
  string soft = UpgradeMan::GetInstance()->GetSoftVersion();
  string fpga = UpgradeMan::GetInstance()->GetFpgaVersion();

  stringstream ss;

  string soft_version = _("Software version");
  string fpga_version = _("FPGA version");

  if (g_authorizationOn) {
    int rest = CEmpAuthorization::GetRestTime();

    if (rest == CEmpAuthorization::PERMANENTDAY) {
      ss << soft_version << ": " << soft << "\n" <<
        _("This is charging software, the period of validity is permanent.") << "\n\n" <<
        fpga_version << ": " << fpga;
    } else {
      ss << soft_version << ": " << soft << "\n" <<
        _("This is charging software, the period of validity is ") << rest << _("days.") << "\n\n" <<
        fpga_version << ": " << fpga;
    }
  } else {
    ss << soft_version << ": " << soft << "\n\n" <<
      fpga_version << ": " << fpga;
  }

  gtk_text_buffer_set_text(gtk_text_view_get_buffer(m_textview_version), ss.str().c_str(), -1);
}


#include "periDevice/DCMMan.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vector>
#include "imageControl/Update2D.h"
#include "probe/ExamItem.h"
#include "Def.h"



#include "display/gui_global.h"

#include "periDevice/NetworkMan.h"


#include "patient/PatientInfo.h"
#include "sysMan/SysDicomSetting.h"


#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"

#include "sysMan/UserDefineKey.h"

#include "probe/ProbeSelect.h"

#include "measure/MeasureMan.h"

#include "patient/FileMan.h"

#include "patient/ViewUdiskDataSelect.h"

#include "sysMan/ConfigToHost.h"



#include "Init.h"


#include "utils/Const.h"
#include "calcPeople/MeasureDef.h"
#include "calcPeople/ViewReport.h"

bool ViewSystem::StrCmpSectionString(int section, const string str, int* language) {
    int maxsection = sizeof(SECTION_STR)/sizeof(SECTION_STR[0]);
    PRINTF("maxSection==%d\n",maxsection);
    if(section < maxsection && section >= 0) {
        for(int lan = 0; lan < LANG_MAX; lan++) {
            if(SECTION_STR[section][lan] == str) {

                if(language != NULL)
                    *language = lan;
                return true;
            }
        }
    }
    return false;
}

bool ViewSystem::StrCmpCustomString(const string str, int* language) {
    for(int lan = 0; lan < LANG_MAX; lan++) {
        if(CUSTOM_STR[lan] == str) {
            if(language != NULL)
                *language = lan;
            return true;
        }
    }
    return false;
}

bool ViewSystem::StrCmpTemplet1String(const string str, int* language) {
    for(int lan = 0; lan < LANG_MAX; lan++) {
        if(TEMPLET1_STR[lan] == str) {
            if(language != NULL)
                *language = lan;
            return true;
        }
    }
    return false;
}

bool ViewSystem::OpenDB() {
    if(sqlite3_open(CustomReport_PATH, &CustomReport_db) != SQLITE_OK)return false;
    return true;
}

bool ViewSystem::CloseDB() {
    if (sqlite3_close(CustomReport_db) != SQLITE_OK) {
        PRINTF("Database Close Error:%s\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    CustomReport_db = 0;
    return true;
}

bool ViewSystem::InitDB() {
    if(access(CustomReport_PATH, F_OK)) {
        sqlite3 *db = 0;
        if (sqlite3_open(CustomReport_PATH, &db) != SQLITE_OK) {
            PRINTF("Open CustomReport  Database Error!\n");
            return false;
        }
        char *errmsg = NULL, buffer[1024*10], buf[1024];
        memset(buffer, 0, sizeof(buffer));
        //create table
        strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE CustomReport(sections, templet, childsection, record);\nCREATE TABLE test_cp(sections, templet, childsection, record);\n");
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", DEFAULT_STR.c_str(), DEFAULT_STR.c_str(), DEFAULT_STR.c_str(), TEMPLET1_STR[init_language].c_str());
        strcat(buffer, buf);

        int maxsection = sizeof(SECTION_STR)/sizeof(SECTION_STR[0]);
        for(int k = 0; k < maxsection; k++) {
            int start, end, count;
            char record[1024];
            memset(record, CHECK_VALUE, sizeof(record));
            memset(buf, 0, sizeof(buf));

            InitRecordFromShowArr(SECTION_STR[k][init_language], &start, &end);
            count = end - start;
            if(StrCmpSectionString(OB_M, SECTION_STR[k][init_language], NULL)) {
                //Fetal 2
                count += OB_CALC_COUNT;
                //EFW
                count += EFW_COUNT;
                //average
                count += AverRes_COUNT;
            }
            if(StrCmpSectionString(UR_M, SECTION_STR[k][init_language], NULL)) {
                //PSAD
                count += PSAD_COUNT;
            }
            if(StrCmpSectionString(OTHERS_M, SECTION_STR[k][init_language], NULL)) {
                //Ohters
                count += OTHERS_COUNT;
            }
            record[count] = '\0';
            sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", CUSTOM_STR[init_language].c_str(), TEMPLET1_STR[init_language].c_str(), SECTION_STR[k][init_language].c_str(), record);
            strcat(buffer, buf);
        }

        strcat(buffer, "COMMIT;\n");
        if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK) {
            PRINTF("Init CustomReport db  Error: %s\n", sqlite3_errmsg(db));
            sqlite3_free(errmsg);
            return false;
        }
        sqlite3_close(db);
    }
    return true;
}

bool ViewSystem::LocalizationDB() {
    char *errmsg = NULL;
    sqlite3_stmt *stmt_f1=NULL;
    const char *sections = NULL, *templet = NULL, *childsection = NULL;
    string sql_f1 = "SELECT DISTINCT sections FROM CustomReport";

    char buffer[1024*10];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE CustomReport(sections, templet, childsection, record);\nCREATE TABLE test_cp(sections, templet, childsection, record);\n");

    if(access(CustomReport_PATH, F_OK)) return true;
    OpenDB();

    if (sqlite3_prepare(CustomReport_db, sql_f1.c_str(), sql_f1.size(), &stmt_f1, 0) != SQLITE_OK) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
          MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
        PRINTF("LocalizationDB 1:%s\n", sqlite3_errmsg(CustomReport_db));
        CloseDB();
        return false;
    }

    while (sqlite3_step(stmt_f1) != SQLITE_DONE) {
        int language;
        const char *f1 = (const char *)sqlite3_column_text(stmt_f1, 0);
        if (strcmp(f1, "") == 0) continue;

        if(StrCmpCustomString(f1, &language)) {
            if(language > LANG_MAX) {
                MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                  MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
                sqlite3_finalize(stmt_f1);
                CloseDB();
                return false;
            }

            sections = CUSTOM_STR[init_language].c_str();
        } else {
            sections = f1;
        }

        sqlite3_stmt *stmt_f2 = NULL;
        string sql_f2 = "SELECT DISTINCT templet FROM CustomReport WHERE sections = ?";
        if (sqlite3_prepare(CustomReport_db, sql_f2.c_str(), sql_f2.size(), &stmt_f2, 0) != SQLITE_OK) {
            CloseDB();
            return false;
        }
        if (sqlite3_bind_text(stmt_f2, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
            sqlite3_finalize(stmt_f2);
            CloseDB();
            return false;
        }

        while (sqlite3_step(stmt_f2) != SQLITE_DONE) {
            const char *f2 = (const char *)sqlite3_column_text(stmt_f2, 0);
            if (strcmp(f2, "") == 0)continue;

            if(StrCmpTemplet1String(f2, &language)) {
                if(language > LANG_MAX) {
                    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                      MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
                    sqlite3_finalize(stmt_f2);
                    sqlite3_finalize(stmt_f1);
                    CloseDB();
                    return false;
                }

                templet = TEMPLET1_STR[init_language].c_str();
            } else {
                templet = f2;
            }

            sqlite3_stmt *stmt_f3 = NULL;
            string sql_f3 = "SELECT DISTINCT childsection FROM CustomReport WHERE sections = ? AND templet = ?";
            if(sqlite3_prepare(CustomReport_db, sql_f3.c_str(), sql_f3.size(), &stmt_f3, 0) != SQLITE_OK) {
                CloseDB();
                return false;
            }
            if(sqlite3_bind_text(stmt_f3, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
                PRINTF("LocalizationDB 5:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                CloseDB();
                return false;
            }
            if(sqlite3_bind_text(stmt_f3, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK) {
                PRINTF("LocalizationDB 6:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                CloseDB();
                return false;
            }
            while(sqlite3_step(stmt_f3) != SQLITE_DONE) {
                const char *f3 = (const char *)sqlite3_column_text(stmt_f3, 0);
                if (strcmp(f3, "") == 0)continue;

                int section = 0;
                int maxsection = sizeof(SECTION_STR)/sizeof(SECTION_STR[0]);
                for(section = 0; section < maxsection; section++) {
                    if(StrCmpSectionString(section, f3, &language)) {
                        if(language > LANG_MAX) {
                            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                              MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
                            sqlite3_finalize(stmt_f3);
                            sqlite3_finalize(stmt_f2);
                            sqlite3_finalize(stmt_f1);
                            CloseDB();
                            return false;
                        }

                        childsection = SECTION_STR[section][init_language].c_str();
                        break;
                    }
                }

                if(f3 == DEFAULT_STR) childsection = DEFAULT_STR.c_str();

                string record;
                ostringstream stream;
                stream << "SELECT record FROM CustomReport WHERE sections = '" << f1 << "' AND templet = '" << f2 << "' AND childsection = '" << f3 << "'";
                sqlite3_stmt *stmt = NULL;
                string sql = stream.str();

                if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
                    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                      MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
                    PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
                    CloseDB();
                    return false;
                }

                while (sqlite3_step(stmt) != SQLITE_DONE) {
                    if (strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
                        record.assign((const char *)sqlite3_column_text(stmt, 0));
                }
                sqlite3_finalize(stmt);

                //write record
                char buf[1024];
                memset(buf, 0, sizeof(buf));

                if(StrCmpTemplet1String(record.c_str(), NULL)) {
                    sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", sections, templet, childsection, TEMPLET1_STR[init_language].c_str());
                } else {
                    sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", sections, templet, childsection, record.c_str());
                }
                strcat(buffer, buf);
            }
            sqlite3_finalize(stmt_f3);
        }
        sqlite3_finalize(stmt_f2);
    }
    sqlite3_finalize(stmt_f1);

    CloseDB();
    remove(CustomReport_PATH_BAK);

    sqlite3 *CustomReport_db_bak;
    if (sqlite3_open(CustomReport_PATH_BAK, &CustomReport_db_bak) != SQLITE_OK) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
        PRINTF("LocalizationDB 7:%s\n", sqlite3_errmsg(CustomReport_db_bak));
        return false;
    }

    strcat(buffer, "COMMIT;\n");
    if (sqlite3_exec(CustomReport_db_bak, buffer, 0, 0, &errmsg) != SQLITE_OK) { // costed 100ms, slower than sqlite3.6
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
        PRINTF("Init CustomReport_db_bak Error: %s\n", sqlite3_errmsg(CustomReport_db_bak));
        sqlite3_free(errmsg);
        return false;
    }

    sqlite3_close(CustomReport_db_bak);
    if(rename(CustomReport_PATH_BAK, CustomReport_PATH) != 0) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
          MessageDialog::DLG_ERROR, _("Database error!"), NULL);
        PRINTF("rename(%s, %s) error\n", CustomReport_PATH_BAK, CustomReport_PATH);
        return false;
    }
    return true;
}

GtkTreeModel *ViewSystem:: CreateTreeModel() {
    InitDB();
    LocalizationDB();
    OpenDB();

    int count = 0;
    string recordname;
    ostringstream stream;
    stream << "SELECT record FROM CustomReport WHERE sections = '" << DEFAULT_STR << "' AND templet = '" << DEFAULT_STR << "' AND childsection = '" << DEFAULT_STR << "'";
    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();

    if(sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
          MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        if(strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
            recordname.assign((const char *)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);

    if(recordname.length() > 0) {
        ostringstream stream2;
        stream2 << "SELECT DISTINCT childsection FROM CustomReport WHERE sections = '" << CUSTOM_STR[init_language] <<"' AND templet = '" << recordname.c_str() << "'";
        sqlite3_stmt *stmt2 = NULL;
        string sql2 = stream2.str();

        if(sqlite3_prepare(CustomReport_db, sql2.c_str(), sql2.size(), &stmt2, 0) != SQLITE_OK) {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
              MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
            return false;
        }
        while (sqlite3_step(stmt2) != SQLITE_DONE) {
            const char *pp = (const char *)sqlite3_column_text(stmt2, 0);
            if (strcmp(pp, "") == 0) continue;
            count ++;
        }
        sqlite3_finalize(stmt2);
    }

    char templatestr[256];
    if(0 == count) {
        sprintf(templatestr, "%s", TEMPLET1_STR[init_language].c_str());
        ostringstream stream3;
        stream3 << "UPDATE CustomReport SET record = '" << templatestr << "' WHERE sections = '" << DEFAULT_STR << "' AND templet = '" << DEFAULT_STR << "' AND childsection = '" << DEFAULT_STR << "'";
        string sql3 = stream3.str();
        if (sqlite3_exec(CustomReport_db, sql3.c_str(), 0, 0, NULL) != SQLITE_OK) {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Database error!"), NULL);
            return false;
        }
    } else {
        sprintf(templatestr, "%s", recordname.c_str());
    }

    GtkTreeIter first_iter;
    GtkTreeIter second_iter;
    GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

    sqlite3_stmt *stmt_f1=NULL;
    string sql_f1 = "SELECT DISTINCT sections FROM CustomReport";
    if (sqlite3_prepare(CustomReport_db, sql_f1.c_str(), sql_f1.size(), &stmt_f1, 0) != SQLITE_OK) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,  _("Database error!"), NULL);
        PRINTF("CreateTreeModel 1:%s\n", sqlite3_errmsg(CustomReport_db));
        return NULL;
    }

    while(sqlite3_step(stmt_f1) != SQLITE_DONE) {
        const char *f1 = (const char *)sqlite3_column_text(stmt_f1, 0);
        if (strcmp(f1, "") == 0) continue;
        if(f1 == DEFAULT_STR)continue;//delet default system
        gtk_tree_store_append(store, &topIter, NULL);
        gtk_tree_store_set(store, &topIter, 0, f1, -1);

        sqlite3_stmt *stmt_f2 = NULL;
        string sql_f2 = "SELECT DISTINCT templet FROM CustomReport WHERE sections = ?";
        if (sqlite3_prepare(CustomReport_db, sql_f2.c_str(), sql_f2.size(), &stmt_f2, 0) != SQLITE_OK) {
            PRINTF("CreateTreeModel 2:%s\n", sqlite3_errmsg(CustomReport_db));
            return NULL;
        }
        if (sqlite3_bind_text(stmt_f2, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
            PRINTF("CreateTreeModel 3:%s\n", sqlite3_errmsg(CustomReport_db));
            sqlite3_finalize(stmt_f2);
            return NULL;
        }

        while (sqlite3_step(stmt_f2) != SQLITE_DONE) {
            const char *f2 = (const char *)sqlite3_column_text(stmt_f2, 0);
            if (strcmp(f2, "") == 0)continue;
            gtk_tree_store_append(store, &first_iter, &topIter);

            if(!strcmp(f2, templatestr)) {
                char buffer[256];
                sprintf(buffer, "%s", DEFAULT_FLAG.c_str());
                strcat(buffer, " ");
                strcat(buffer, f2);
                gtk_tree_store_set(store, &first_iter, 0, buffer, -1);
            } else {
                gtk_tree_store_set(store, &first_iter, 0, f2, -1);
            }

            sqlite3_stmt *stmt_f3 = NULL;
            string sql_f3 = "SELECT DISTINCT childsection FROM CustomReport WHERE sections = ? AND templet = ?";
            if (sqlite3_prepare(CustomReport_db, sql_f3.c_str(), sql_f3.size(), &stmt_f3, 0) != SQLITE_OK) {
                PRINTF("CreateTreeModel 4:%s\n", sqlite3_errmsg(CustomReport_db));
                return NULL;
            }
            if (sqlite3_bind_text(stmt_f3, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK) {
                PRINTF("CreateTreeModel 5:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                return NULL;
            }
            if (sqlite3_bind_text(stmt_f3, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK) {
                PRINTF("CreateTreeModel 6:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                return NULL;
            }
            while (sqlite3_step(stmt_f3) != SQLITE_DONE) {
                const char *f3 = (const char *)sqlite3_column_text(stmt_f3, 0);
                if (strcmp(f3, "") == 0)continue;
                gtk_tree_store_append(store, &second_iter, &first_iter);
                gtk_tree_store_set(store, &second_iter, 0, f3, -1);
            }
            sqlite3_finalize(stmt_f3);
        }
        sqlite3_finalize(stmt_f2);
    }
    sqlite3_finalize(stmt_f1);
    CloseDB();

    if(0 == count) {
        //MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Blank template cannot apply,switch to template 1!"), NULL);
    }
    return GTK_TREE_MODEL(store);
}



void ViewSystem::ChkBtnClicked(GtkButton *button) {
    for(int id = 0; id < MAX_CALCARR_COUNT; id++) {
        if((GtkWidget*)button == CheckButtonArr[id]) {
            //average
            if(id == AVERRES_START_ID + 2  || id == AVERRES_START_ID + 3) {
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
                    ShowArr[id + 2] = CHECK_VALUE;
                } else {
                    ShowArr[id + 2] = UNCHECK_VALUE;
                }
            } else {
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
                    ShowArr[id] = CHECK_VALUE;
                } else {
                    ShowArr[id] = UNCHECK_VALUE;
                }

                //average
                if(id == AVERRES_START_ID || id == AVERRES_START_ID + 1) {
                    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
                        ShowArr[id + 2] = CHECK_VALUE;
                    } else {
                        ShowArr[id + 2] = UNCHECK_VALUE;
                    }
                }
            }
        }
    }
}

GtkWidget* ViewSystem::AddFrameByTitle(char *title, int section, int startid, int endid, int *h) {
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;
    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10, count = endid - startid;
    int wide = (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    for(int t = 0; t < count; t++) {
        if(!(t%3)) {
            x = tab_x;
            if(t /3) {
                y += high;
            } else {
                y += tab_y;
            }
        } else {
            x += wide + tab_x;
        }

        int id = startid + t;

        switch (section) {
        case ABDO_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(AbdoInfo[id- ABD_MEA_START].title));
            break;
        case ADULT_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(AdultInfo[id - ADULT_MEA_START].title));
            break;
        case UR_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(URInfo[id - UR_MEA_START].title));
            break;
        case OB_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(OBInfo[id - OB_MEA_START].title));
            break;
        case GYN_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(GYNInfo[id - GYN_MEA_START].title));
            break;
        case SP_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(SPInfo[id - SP_MEA_START].title));
            break;
        case VS_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(VSInfo[id - VS_MEA_START].title));
            break;
        case FETAL_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(FetalInfo[id - FETAL_MEA_START].title));
            break;
        case TCD_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(TCDInfo[id - TCD_MEA_START].title));
            break;
        case ORTHO_M:
            checkbutton = gtk_check_button_new_with_mnemonic (_(OrthoInfo[id - ORTHO_MEA_START].title));
            break;
        default:
            break;
        }

        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;

    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}

GtkWidget* ViewSystem::AddFrameByTitle2(char *title, int section, int startid, int endid, int *h) {
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;

    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10, count = endid - startid;
    int wide = (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    for(int t = 0; t < count; t++) {
        if(!(t%3)) {
            x = tab_x;
            if(t /3) {
                y += high;
            } else {
                y += tab_y;
            }
        } else {
            x += wide + tab_x;
        }
        int id = startid + t;
        checkbutton = gtk_check_button_new_with_mnemonic (_(OBInfo[id - OB_MEA_START].title));

        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        id = (id - OB_MEA_START) + MAX_CALC_COUNT; //fetal 2, the id  should add to the array end.
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;
    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}

GtkWidget* ViewSystem::AddFrameForOthers(const char *title, int *h) {
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;
    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10;
    int wide =(SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    int count = 0, idstart;
    const char *labstr[6];
    char efwstr[2][256];
    sprintf(efwstr[0], "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 1)"));
    sprintf(efwstr[1], "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 2)"));

    char averagestr[256];
    sprintf(averagestr, "<i><b>%s</b></i>", _("Average"));

    memset(labstr, 0, sizeof(labstr));
    if(!strcmp(title, _("PSAD"))) {
        count = PSAD_COUNT;
        idstart = PSAD_START_ID;
        labstr[0] = _("SPSA");
        labstr[1] = _("PSAD");
    } else if(!strcmp(title, averagestr)) {
        int k = 0;
        count = AverRes_COUNT - 2;
        idstart = AVERRES_START_ID;
        labstr[k++] = _("AUA");
        labstr[k++] = _("EDD(AUA)");
        labstr[k++] = _("GA(LMP)");
        labstr[k++] = _("EDD(LMP)");
    } else if(!strcmp(title, _(efwstr[0]))) {
        count = EFW_COUNT/2;
        idstart = EFW_START_ID;
        labstr[0] = _("EFW");
    } else if(!strcmp(title, _(efwstr[1]))) {
        count = EFW_COUNT/2;
        idstart = EFW_START_ID + 1;
        labstr[0] = _("EFW");
    } else if(!strcmp(title, _("Others"))) {
        count = OTHERS_COUNT;
        idstart = OTHERS_START_ID;
        labstr[0] = _("Show Comments");
        labstr[1] = _("Show Descriptor");
    }

    for(int t = 0; t < count; t++) {
        if(!(t%3)) {
            x = tab_x;
            if(t /3) {
                y += high;
            } else {
                y += tab_y;
            }
        } else {
            x += wide + tab_x;
        }
        int id = idstart + t;
        checkbutton = gtk_check_button_new_with_mnemonic (labstr[t]);
        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;
    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}





bool ViewSystem::UniqueItem(const string sections, const string templet, const string childsection) {
    sqlite3_stmt *stmt = NULL;
    ostringstream stream;
    int result = 0;

    stream << "SELECT COUNT(*) AS num_result FROM CustomReport " << "WHERE sections LIKE '" << sections
           << "' " << "AND templet LIKE '" << templet << "' " << "AND childsection LIKE '" << childsection << "' ";
    string sql = stream.str();

    OpenDB();
    if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        if (strcmp(sqlite3_column_name(stmt, 0), "num_result") == 0)
            result = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    CloseDB();

    if (result)
        return false;
    else
        return true;
}

GtkTreeIter ViewSystem::InsertUniqueComment(GtkTreeModel *model, const string str) {
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
    while (has_node == TRUE) {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strtmp == str)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}

void ViewSystem::ChangeCommentExamBox(int probe_type, char *check_part) {
    int comment_probe = GetCommentProbeType();
    if(comment_probe == probe_type) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_comment), check_part);
    }
}

void ViewSystem::ChangeCommentExamBoxDelete(int probe_type) {
  int comment_probe = GetCommentProbeType();

  if(comment_probe == probe_type) {
    ExamItem examItem;
    vector<ExamItem::EItem> itemIndex = examItem.GetItemListOfProbe(g_probe_list[comment_probe]);

    vecExamItem_comment.clear();
    vector<int>::iterator iter_index;

    for (int i = 0; i < itemIndex.size(); i++) {
      vecExamItem_comment.push_back(ExamItemArray[itemIndex[i]].name);
    }

    CreateDefineItem_comment(comment_probe, vecExamItem_comment);

    int exam_size = vecExamItem_comment.size();

    for(int i = exam_size; i >= 0; i--) {
      gtk_combo_box_remove_text(GTK_COMBO_BOX (m_combobox_exam_comment), i);
    }

    for (int i = 0; i <exam_size; i++) {
      gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_comment), _(vecExamItem_comment[i].c_str()));
    }
  }
}

GtkTreeIter ViewSystem::InsertUnique(GtkTreeModel* model, GtkTreeIter* iter, const string str) {
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_iter_children(model, &tmp_iter, iter);
    while (has_node == TRUE) {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strtmp == str)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}



















bool ViewSystem::InitRecordFromShowArr(const string childsection, int* start, int* end) {
    bool ret = false;
    if(childsection.empty() ||!start ||!end) return ret;

    if(StrCmpSectionString(ABDO_M, childsection, NULL)) {
        *start = ABD_MEA_START;
        *end = ABD_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(ADULT_M, childsection, NULL)) {
        *start = ADULT_MEA_START;
        *end = ADULT_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(UR_M, childsection, NULL)) {
        *start = UR_MEA_START;
        *end = UR_MEA_END;
        ret = true;
    }

    if(StrCmpSectionString(OB_M, childsection, NULL)) {
        *start = OB_MEA_START;
        *end = OB_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(GYN_M, childsection, NULL)) {
        *start = GYN_MEA_START;
        *end = GYN_MEA_END;
        ret = true;
    }

    if(StrCmpSectionString(SP_M, childsection, NULL)) {
        *start = SP_MEA_START;
        *end = SP_MEA_END;
        ret = true;
    }

    if(StrCmpSectionString(VS_M, childsection, NULL)) {
        *start = VS_MEA_START;
        *end = VS_MEA_END;
        ret = true;
    }

    if(StrCmpSectionString(FETAL_M, childsection, NULL)) {
        *start = FETAL_MEA_START;
        *end = FETAL_MEA_END;
        ret = true;
    }

    if(StrCmpSectionString(OTHERS_M, childsection, NULL)) {
        *start = TCD_MEA_START;
        //*end = TCD_MEA_END;
        *end = ORTHO_MEA_END;
        ret = true;
    }

    return ret;
}
bool ViewSystem::ReadRecordFromDB(const string sections, const string templet, const string childsection) {
    if(sections.empty() || templet.empty() ||childsection.empty())return false;

    ostringstream stream;
    stream << "SELECT record FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();
    string record;
    OpenDB();

    if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        if (strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
            record.assign((const char *)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    CloseDB();

    if(record.length() == 0)return false;

    int start,  end;
    //memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
    if(InitRecordFromShowArr(childsection, &start, &end)) {
        memcpy(&ShowArr[start], record.c_str(), end-start);
    }

    if(StrCmpSectionString(OB_M, childsection, NULL)) {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //Fetal 2
        memcpy(&ShowArr[MAX_CALC_COUNT], &recordstr[OB_CALC_COUNT], OB_CALC_COUNT);
        //EFW
        memcpy(&ShowArr[EFW_START_ID], &recordstr[(OB_CALC_COUNT)*2], EFW_COUNT);
        //Average
        memcpy(&ShowArr[AVERRES_START_ID], &recordstr[(OB_CALC_COUNT)*2 + EFW_COUNT], AverRes_COUNT);
    }
    if(StrCmpSectionString(UR_M, childsection, NULL)) {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //PSAD
        memcpy(&ShowArr[PSAD_START_ID], &recordstr[end-start], PSAD_COUNT);
    }
    if(StrCmpSectionString(OTHERS_M, childsection, NULL)) {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //Ohters
        memcpy(&ShowArr[OTHERS_START_ID], &recordstr[end-start], OTHERS_COUNT);
    }

    int id;
    for(id = 0; id < MAX_CALCARR_COUNT; id++) {
        if(ShowArr[id] != CHECK_VALUE) {
            ShowArr[id] = UNCHECK_VALUE;
        }
    }
    ShowArr[id] = '\0';
    return true;
}

void ViewSystem::apply_report_setting() {
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *templet;

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(m_treeviewReportSet);
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);
    gtk_tree_model_get(model, &iter, 0, &templet, -1);

    if((tree_depth <= 1) || (tree_depth >= 3))return;
    if(NULL != strstr(templet, DEFAULT_FLAG.c_str()))return;

    int count = 0;
    if(strlen(templet) > 0) {
        OpenDB();
        ostringstream stream2;
        stream2 << "SELECT DISTINCT childsection FROM CustomReport WHERE sections = '" << CUSTOM_STR[init_language] <<"' AND templet = '" << templet << "'";
        sqlite3_stmt *stmt2 = NULL;
        string sql2 = stream2.str();

        if(sqlite3_prepare(CustomReport_db, sql2.c_str(), sql2.size(), &stmt2, 0) != SQLITE_OK) {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,   _("Database error!"), NULL);
            return;
        }
        while (sqlite3_step(stmt2) != SQLITE_DONE) {
            const char *pp = (const char *)sqlite3_column_text(stmt2, 0);
            if (strcmp(pp, "") == 0) continue;
            count ++;
        }
        sqlite3_finalize(stmt2);

        if(count > 0) {
            ostringstream stream3;
            stream3 << "UPDATE CustomReport SET record = '" << templet << "' WHERE sections = '" << DEFAULT_STR << "' AND templet = '" << DEFAULT_STR << "' AND childsection = '" << DEFAULT_STR << "'";
            string sql3 = stream3.str();
            if (sqlite3_exec(CustomReport_db, sql3.c_str(), 0, 0, NULL) != SQLITE_OK) {
                MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Database error!"), NULL);
            }

            GtkTreeIter parent_iter;
            gtk_tree_model_iter_parent(model, &parent_iter, &iter);
            GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);

            GtkTreeModel *modelnew = CreateTreeModel();
            gtk_tree_view_set_model(m_treeviewReportSet, modelnew);
            g_object_unref (modelnew);
            gtk_tree_view_expand_to_path(m_treeviewReportSet, parent_path);
            gtk_tree_view_set_cursor(m_treeviewReportSet, parent_path, NULL, TRUE);
            gtk_tree_path_free (parent_path);

            gtk_widget_hide_all(GTK_WIDGET(m_scrolled_window_show));
        } else {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,   _("Blank template can't apply!"), NULL);
        }

        CloseDB();
    }
}
































void ViewSystem::CommonTreeviewSelectionChanged(GtkTreeSelection *treeselection) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    bool common_selected = gtk_tree_selection_get_selected(m_selected_common_printer, &model, &iter);
    if (common_selected) {
        PRINTF("** unselect specific **\n");
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(m_selected_specific_printer));
    }
}

/*void ViewSystem::ModebmRadioToggled(GtkToggleButton* togglebutton) {
  SysOptions sysOp;
  int index_display_format = sysOp.GetDisplayFormatM();

  if (index_display_format == 0)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total), TRUE);
  else if (index_display_format == 1)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11), TRUE);
  else if (index_display_format == 2)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21), TRUE);
  else if (index_display_format == 3)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12), TRUE);
  else
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11), TRUE);
}

void ViewSystem::ModebpwRadioToggled(GtkToggleButton *togglebutton) {
    int index_display_format = 0;
    SysOptions sysOp;
    index_display_format = sysOp.GetDisplayFormatPW();
    if (index_display_format == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total_pw), TRUE);
    else if (index_display_format == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11_pw), TRUE);
    else if (index_display_format == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21_pw), TRUE);
    else if (index_display_format == 3)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12_pw), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11_pw), TRUE);
}*/

/*void ViewSystem::OnRadiobtnDisplay_total(GtkToggleButton *togglebutton) {
    SysOptions sysOp;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm))) {
        m_bmIndex = 0;
        sysOp.SetDisplayFormatM(m_bmIndex);
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw))) {
        m_bpwIndex = 0;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}

void ViewSystem::OnRadiobtnDisplay_ud11(GtkToggleButton *togglebutton) {
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm))) {
        m_bmIndex = 1;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw))) {
        m_bpwIndex = 1;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();

}

void ViewSystem::OnRadiobtnDisplay_ud21(GtkToggleButton *togglebutton) {
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm))) {
        m_bmIndex = 2;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw))) {
        m_bpwIndex = 2;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();

}

void ViewSystem::OnRadiobtnDisplay_ud12(GtkToggleButton *togglebutton) {
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm))) {
        m_bmIndex = 3;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw))) {
        m_bpwIndex = 3;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}

void ViewSystem::OnRadiobtnDisplay_lr11(GtkToggleButton *togglebutton) {
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm))) {
        m_bmIndex = 4;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw))) {
        m_bpwIndex = 4;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}*/

void ViewSystem::SpecificTreeviewSelectionChanged(GtkTreeSelection *treeselection) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    bool specific_selected = gtk_tree_selection_get_selected(m_selected_specific_printer, &model, &iter);
    if (specific_selected) {
        PRINTF("** unselect common **\n");
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(m_selected_common_printer));
    }
}









int ViewSystem::common_printer_selection() {
  GtkTreeModel* model = NULL;
  GtkTreeIter iter;

  if (gtk_tree_selection_get_selected(m_selected_common_printer, &model, &iter) != TRUE) {
    return -1;
  }

  GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
  gchar* index_str = gtk_tree_path_to_string(path);
  return atoi(index_str);
}

string ViewSystem::specific_printer_selection() {
  GtkTreeModel* model = NULL;
  GtkTreeIter iter;
  string ret_val;

  if (gtk_tree_selection_get_selected(m_selected_specific_printer, &model, &iter) != TRUE) {
    return ret_val;
  }

  char* value = NULL;
  gtk_tree_model_get(model, &iter, PRT_NAME, &value, -1);

  if (value != NULL) {
    ret_val = value;
  }

  return ret_val;
}








void ViewSystem::RenameItemClicked(GtkButton *button) {
    g_object_set(m_renderer, "editable", TRUE, NULL);
    g_signal_connect(m_renderer, "edited", G_CALLBACK(signal_renderer_rename), this);
}



void ViewSystem::EntryItemLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    if(old_text_length + new_text_length >20) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

void ViewSystem::CreateDefineItem(int probeIndex, vector<ExamPara>& vecExamItem) {
  ExamItem exam;
  string userselectname = exam.TransUserSelectForEng(GetUserName());

  ExamItem examitem;
  vector<string> useritemgroup = examitem.GetCurrentUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string firstgenitem;

    examitem.GetUserItem(useritemgroup[i], userselect, probelist, useritem, department, firstgenitem);

    ExamPara exampara;
    exampara.dept_name = department;
    exampara.name = useritem;
    exampara.index = ExamItem::USERNAME;

    cout << i << endl;

    if(userselectname == userselect ) {
      if (probeIndex >= 0 && probeIndex <= NUM_PROBE) {
        if (probelist == g_probe_list[probeIndex]) {
          vecExamItem.push_back(exampara);
        }
      }
    }
  }
}



void ViewSystem::CreateDefineItem_comment(int probeIndex, vector<string>& vecExamItem_comment) {
  IniFile ini_userselect(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));

  ExamItem examitem;
  string userselectname = examitem.ReadDefaultUserSelect(&ini_userselect);

  vector<string> useritemgroup = examitem.GetDefaultUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string firstgenitem;

    examitem.GetUserItem(useritemgroup[i], userselect, probelist, useritem, department, firstgenitem);

    ExamPara exampara;
    exampara.dept_name = department;
    exampara.name = useritem;
    exampara.index = ExamItem::USERNAME;

    if (userselectname == userselect) {
      if (probeIndex >= 0 && probeIndex <= NUM_PROBE) {
        if (probelist == g_probe_list[probeIndex]) {
          vecExamItem_comment.push_back(useritem);
        }
      }
    }
  }
}

bool ViewSystem::RenameNotice(int probe_type_index, char *new_text, char *dialognotice, int group_length) {
  ExamItem examitem;
  string userselectname = examitem.TransUserSelectForEng(GetUserName());

  vector<string> useritemgroup = examitem.GetCurrentUserGroup();

  for (int i= 0 ; i < useritemgroup.size(); i++) {
    string userselect;
    string probelist;
    string useritem;
    string department;
    string firstgenitem;

    examitem.GetUserItem(useritemgroup[i], userselect, probelist, useritem, department, firstgenitem);

    if (userselectname == userselect) {
      if (probe_type_index >= 0 && probe_type_index <= NUM_PROBE) {
        if (probelist == g_probe_list[probe_type_index]) {
          if (useritem == new_text) {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
              MessageDialog::DLG_ERROR, dialognotice, NULL); // 重命名失败!该结点已存在

            return false;
          }
        }
      }
    }
    }

    return true;
}

void ViewSystem::TransEnglish(char *strname, char str[256],char *str_indexname,char str_index[256])
{
    string newtext=_("New Item");
    string smallpart=_("Small Part");
    string gyney=_("Gynecology");
    string abdomen=_("Abdomen");
    string cardiac=_("Cardiac");
    string ortho=_("Orthopedic");
    string vas=_("Vascular");
    string ob=_("Obstetrics");
    string ur=_("Urology");

    string adbo=_("Adult Abdomen");
    string gyn=_("Gynecology");
    string earlypreg=_("Early Pregnancy");
    string kidney=_("Kidney Ureter");
    string hipjoint=_("Hip Joint");
    string mammary=_("Mammary Glands");
    string kidabdo=_("Kid Abdomen");
    string adultcar=_("Adult Cardio");
    string fetus=_("Fetal Cardio");
    string thyroid=_("Thyroid");
    string carotid=_("Carotid");

    if (strcmp(strname, smallpart.c_str())==0) {
        strcpy(str, "Small Part");
    } else if (strcmp(strname, gyney.c_str())==0) {
        strcpy(str, "Gynecology");
    } else if (strcmp(strname,abdomen.c_str())==0) {
        strcpy(str, "Abdomen");
    } else if (strcmp(strname, cardiac.c_str())==0) {
        strcpy(str, "Cardiac");
    } else if (strcmp(strname, ortho.c_str())==0) {
        strcpy(str, "Orthopedic");
    } else if (strcmp(strname, vas.c_str())==0) {
        strcpy(str, "Vascular");
    } else if (strcmp(strname,ob.c_str())==0) {
        strcpy(str, "Obstetrics");
    } else if (strcmp(strname, ur.c_str())==0) {
        strcpy(str, "Urology");
    }

    if (strcmp(str_indexname, newtext.c_str())==0) {
        strcpy(str_index, "New Item");
    }

    else if (strcmp(str_indexname, adbo.c_str())==0) {
        strcpy(str_index, "Adult Abdomen");
    } else if (strcmp(str_indexname, gyn.c_str())==0) {
        strcpy(str_index, "Gynecology");
    } else if (strcmp(str_indexname,earlypreg.c_str())==0) {
        strcpy(str_index, "Early Pregnancy");
    } else if (strcmp(str_indexname, kidney.c_str())==0) {
        strcpy(str_index, "Kidney Ureter");
    } else if (strcmp(str_indexname,hipjoint.c_str())==0) {
        strcpy(str_index, "Hip Joint");
    } else if (strcmp(str_indexname,mammary.c_str())==0) {
        strcpy(str_index, "Mammary Glands");
    } else if (strcmp(str_indexname,kidabdo.c_str())==0) {
        strcpy(str_index, "Kid Abdomen");
    } else if (strcmp(str_indexname,adultcar.c_str())==0) {
        strcpy(str_index, "Adult Cardio");
    } else if (strcmp(str_indexname,fetus.c_str())==0) {
        strcpy(str_index, "Fetal Cardio");

    } else if (strcmp(str_indexname,thyroid.c_str())==0) {
        strcpy(str_index, "Thyroid");

    } else if (strcmp(str_indexname,carotid.c_str())==0) {
        strcpy(str_index, "Carotid");
    }

}




void ViewSystem::AddCheckPart(char *checkpart) {
    if(strcmp(checkpart, "") == 0) {
        PRINTF("chekc_part is null\n");
        return;
    }

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR,
                                          _("Please select a department before inserting!"), NULL); //请先选择待插入结点的父结点
        return;
    }

    int probe_type_index = GetProbeType();
    char *dialognotice=_("Fail to rename! The item name has existed!");
    int group_length(0);
    bool renameflag=TRUE;

    renameflag=RenameNotice(probe_type_index, checkpart, dialognotice, group_length);
    if(renameflag) {

        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        gint tree_depth = gtk_tree_path_get_depth(path);
        gtk_tree_path_free (path);

        if(tree_depth > 1) {
            GtkTreeIter parent_iter;
            gtk_tree_model_iter_parent(model, &parent_iter, &iter);
            iter = parent_iter;
            tree_depth = 1;
        }

        char *strname;
        char *str_indexname;
        GtkTreeIter childern;

        gtk_tree_model_get(model, &iter, 0, &strname, -1);
        gtk_tree_model_iter_children(model, &childern, &iter);
        gtk_tree_model_get(model, &childern, 0, &str_indexname, -1);

        char str[256];
        char str_index[256];

        TransEnglish(strname, str, str_indexname, str_index);

        ExamItem examItem;
        int itemIndex;
        int probeIndex;
        probeIndex = GetProbeType();

        //先写入新建名称到文件
        if ((probeIndex >= 0)) {
            examItem.WriteNewItemFile(probeIndex, checkpart, str, str_index);
            examItem.WriteNewItemToCommentFile(probeIndex, checkpart, str);
            examItem.WriteNewItemToCalcFile(probeIndex, checkpart, str);
            examItem.WriteNewItemToMeasureFile(probeIndex, checkpart, str);
        }
        //change exam box in comment and calc

        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        if(username == GetUserName()) {
            CalcSetting::GetInstance()->ChangeExamBox(checkpart);
            MeasureSetting::GetInstance()->ChangeExamBox(checkpart);
            int probe_type_index = GetProbeType();
            ChangeCommentExamBox(probe_type_index, checkpart);
        }

        GtkTreeIter iter_new;
        gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, checkpart, -1);
        GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

        //刷新treeview
        vector<ExamItem::EItem> vecItemIndex = examItem.GetItemListOfProbe(g_probe_list[probeIndex]);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(vecItemIndex));
        //gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_exam_type), new_path);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), new_path, NULL, TRUE);

        gtk_tree_path_free (new_path);

        //保存当前参数值到新建检查科别
        if ((probeIndex >= 0)) {
            get_current_and_save_image_para();
        }

    }
}







void ViewSystem::SpinbuttonInsertGain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int gain = atoi(old_str.c_str());
    if (gain < 0 || gain > 100)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

void ViewSystem::SpinbuttonInsertAngle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    string new_str = new_text;
    string s1 = "°";
    string::size_type pos = 0;
    if((pos=new_str.find(s1, 0)) == string::npos) {
        if (!g_ascii_isdigit(*new_text))
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    old_str.insert(*position, new_str);
    int angle = atoi(old_str.c_str());
    if (angle < -85 || angle > 85)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

gboolean ViewSystem::SpinbuttonOutputAngle(GtkSpinButton *spin) {
    GtkAdjustment *adj;
    gchar *text;
    int value;
    adj = gtk_spin_button_get_adjustment (spin);
    value = (int)gtk_adjustment_get_value (adj);
    text = g_strdup_printf ("%d°", value);
    gtk_entry_set_text (GTK_ENTRY (spin), text);
    g_free (text);

    return TRUE;
}









GtkTreeModel* ViewSystem::create_item_comment_model_selected() {
  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

  if(!exam_type_name)
      return NULL;

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);

  vector<ExamPara> vecItemComment1;
  vecItemComment1.clear();
  CreateItemList_Comment1(g_probe_list[index1] + "-" +exam_type, vecItemComment1);
  int item_size(0);
  item_size = vecItemComment1.size();

  if(vecItemComment1.empty())
      return NULL;

  GtkTreeIter iter;
  GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
                        G_TYPE_STRING,
                        G_TYPE_INT);

  vector<ExamPara>::iterator iterItem;
  for (iterItem = vecItemComment1.begin(); iterItem != vecItemComment1.end(); iterItem++) {

      gtk_tree_store_append(store, &iter, NULL);

      SysGeneralSetting sysGS;
      if(sysGS.GetLanguage() ==ZH) {

          gtk_tree_store_set(store, &iter,
                             NAME_COLUMN, _(iterItem->name.c_str()),
                             INDEX_COLUMN, iterItem->index,
                             -1);
      } else {
          gtk_tree_store_set(store, &iter,
                             NAME_COLUMN, iterItem->name.c_str(),
                             INDEX_COLUMN, iterItem->index,
                             -1);
      }
  }

  return GTK_TREE_MODEL(store);
}

GtkTreeModel* ViewSystem::create_item_comment_model(int index) {
  int  department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));

  if(department_index== -1) {
    return NULL;
  }

  string department = DepartmentName(department_index);

  IniFile new_ini(string(CFG_RES_PATH) + string(NOTE_FILE));
  int number = new_ini.ReadInt(department, "Number");

  vector<ExamPara> vecItemComment;
  CreateItemList_Comment(department, vecItemComment, number);

  int item_size = vecItemComment.size();

  if(vecItemComment.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeStore* store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  vector<ExamPara>::iterator iterItem;
  for (iterItem = vecItemComment.begin(); iterItem != vecItemComment.end(); iterItem++) {
    gtk_tree_store_append(store, &iter, NULL);

    SysGeneralSetting sysGS;

    if (sysGS.GetLanguage() ==ZH) {
      gtk_tree_store_set(store, &iter,
        NAME_COLUMN, iterItem->name.c_str(),
        INDEX_COLUMN, iterItem->index, -1);
    } else {
      gtk_tree_store_set(store, &iter,
        NAME_COLUMN,  iterItem->name.c_str(),
        INDEX_COLUMN, iterItem->index, -1);
    }
  }

  return GTK_TREE_MODEL(store);
}

int ViewSystem::DepartmentIndex() {
  int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
  string exam_type_name = Utils::combobox_active_text(m_combobox_exam_comment);

  if(exam_type_name.empty()) {
    return -1;
  }

  ExamItem exam;
  string exam_type = exam.TransItemName(exam_type_name);
  string probe_exam = g_probe_list[index] + "-" + exam_type;

  IniFile ini_default(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string username = exam.ReadDefaultUserSelect(&ini_default);

  stringstream ss;

  if (username == "System Default") {
    ss << CFG_RES_PATH << COMMENT_FILE;
  } else {
    ss << CFG_RES_PATH << COMMENT_PATH << username << ".ini";
  }

  IniFile ini(ss.str());
  string department = ini.ReadString(probe_exam.c_str(), "Department");

  if(department, "Abdomen") {
    return 0;
  } else if(department == "Urology") {
    return 1;
  } else if(department == "Cardiac") {
    return 2;
  } else if(department == "Obstetrics") {
    return 3;
  } else if(department == "Gynecology") {
    return 4;
  } else if(department == "Small Part") {
    return 5;
  } else if(department == "Vascular") {
    return 6;
  } else if(department == "Orthopedic") {
    return 7;
  } else {
    return 0;
  }
}

string ViewSystem::DepartmentName(int index) {
  if (index == 0) {
    return "Abdomen";
  } else if(index == 1) {
    return "Urology";
  } else if(index == 2) {
    return "Cardiac";
  } else if(index == 3) {
    return "Obstetrics";
  } else if(index == 4) {
    return "Gynecology";
  } else if(index == 5) {
    return "Small Part";
  } else if(index == 6) {
    return "Vascular";
  } else {
    return "Orthopedic";
  }
}

void ViewSystem::CreateItemList_Delete_Comment1(string select_name, string probe_exam,vector<ExamPara>& vecItemComment1) {

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    SysGeneralSetting sysGS;
    for(int i=1; i<=number; i++) {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(probeExam, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;
        if(sysGS.GetLanguage() ==ZH) {
            if(select_name != exampara.name) {
                vecItemComment1.push_back(exampara);
            }

        } else {
            if(select_name != exampara.name) {
                vecItemComment1.push_back(exampara);
            }
        }
    }
}

void ViewSystem::CreateItemList_Delete_Comment(string select_name, string department, vector<ExamPara>& vecDeleteComment) {
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

    int number;
    number = ptrIni->ReadInt(department, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;
        SysGeneralSetting sysGS;
        printf("select_name = %s, exam_name = %s\n", select_name.c_str(), _(exampara.name.c_str()));
        if(select_name != exampara.name) {
            vecDeleteComment.push_back(exampara);
        }
    }
}

void ViewSystem::CreateItemList_Comment1(string probe_exam,vector<ExamPara>& vecItemComment1) {
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0) {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    } else {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    for(int i=1; i<=number; i++) {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(probeExam, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;

        vecItemComment1.push_back(exampara);
    }
}

void ViewSystem::CreateItemList_Note(string department,vector<ExamPara>& vecItemComment, int number) {
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

    for(int i=1; i<=number; i++) {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
        // exampara.index = ;
        exampara.index=ExamItem::USERNAME;

        vecItemComment.push_back(exampara);
    }
}

void ViewSystem::CreateItemList_Comment(string department,vector<ExamPara>& vecItemComment, int number) {
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

    for(int i=1; i<=number; i++) {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
        // exampara.index = ;
        exampara.index=ExamItem::USERNAME;

        vecItemComment.push_back(exampara);
    }
}









int ViewSystem::get_active_user() {
    return gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxentry_user_select));
}



int ViewSystem::GetProbeType() {
    return gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_type));
}

void ViewSystem::image_para_combo_box_set(GtkWidget *combobox, int value) {
    if (value < 0)
        gtk_widget_set_sensitive(combobox, FALSE);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), value);
}

void ViewSystem::SetImagePara(const ExamItem::ParaItem &item) {
    /*image_para_combo_box_set((m_combobox_mbp), item.common.MBP);
    image_para_combo_box_set((m_combobox_ao_power), item.common.powerIndex);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_2d_gain), (double)item.d2.gain2D);
    image_para_combo_box_set((m_combobox_agc), item.d2.AGC);
    image_para_combo_box_set((m_combobox_edge_enh), item.d2.edgeEnhance);
    image_para_combo_box_set((m_combobox_steer), item.d2.steerIndex);
    if(item.d2.focSum<=0)
        image_para_combo_box_set((m_combobox_focus_sum), (item.d2.focSum));
    else
        image_para_combo_box_set((m_combobox_focus_sum), (item.d2.focSum-1));
    if(item.d2.focPosIndex<0)
        image_para_combo_box_set((m_combobox_focus_pos), item.d2.focPosIndex+1);
    else
        image_para_combo_box_set((m_combobox_focus_pos), item.d2.focPosIndex);
    image_para_combo_box_set((m_combobox_depth), item.d2.imgScale);
    image_para_combo_box_set((m_combobox_chroma), item.d2.chroma);
    image_para_combo_box_set((m_combobox_lr), item.d2.leftRight);
    image_para_combo_box_set((m_combobox_ud), item.d2.upDown);
    //image_para_combo_box_set((m_combobox_rotation), item.d2.rotate);
    image_para_combo_box_set((m_combobox_polarity), item.d2.polarity);
    image_para_combo_box_set((m_combobox_frame), item.d2.frameAver);
    image_para_combo_box_set((m_combobox_line_aver), item.d2.lineAver);
    image_para_combo_box_set((m_combobox_restric), item.d2.noiseReject);
    image_para_combo_box_set((m_combobox_gray_trans), item.d2.grayTransIndex);
    image_para_combo_box_set((m_combobox_thi), item.d2.harmonic);
    image_para_combo_box_set((m_combobox_tsi), item.d2.TSI);
    image_para_combo_box_set((m_combobox_imgEnh), item.d2.imgEhn);
    image_para_combo_box_set((m_combobox_scan_range), item.d2.scanAngle);
    image_para_combo_box_set((m_combobox_dynamic_range), item.d2.dynamicRange);
    image_para_combo_box_set((m_combobox_2d_freq), item.d2.freqIndex);
    image_para_combo_box_set((m_combobox_thi_freq), item.d2.thiFreqIndex);
    image_para_combo_box_set((m_combobox_2d_line_density), item.d2.lineDensity);
    image_para_combo_box_set((m_combobox_2d_smooth), item.d2.smooth);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_m_gain), (double)item.d2.gainM);
    image_para_combo_box_set((m_combobox_space_compound), item.d2.spaceCompoundIndex);
    image_para_combo_box_set((m_combobox_freq_compound), item.d2.freqCompoundIndex);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_gain), (double)item.spectrum.gain);
    image_para_combo_box_set((m_combobox_pw_scale_prf), item.spectrum.PRF);
    image_para_combo_box_set((m_combobox_time_resolution), item.spectrum.timeSmooth);
    image_para_combo_box_set((m_combobox_pw_freq), item.spectrum.freq);
    image_para_combo_box_set((m_combobox_pw_wallfilter), item.spectrum.wallFilter);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_angle), (double)item.spectrum.correctAngle);
    image_para_combo_box_set((m_combobox_pw_invert), item.spectrum.invert);

    image_para_combo_box_set((m_combobox_cfm_line_density), item.color.lineDensity);
    image_para_combo_box_set((m_combobox_cfm_scale_prf), item.color.PRFIndex);
    image_para_combo_box_set((m_combobox_sensitivity), item.color.sensitive); //must before after prf index
    image_para_combo_box_set((m_combobox_cfm_wallfilter), item.color.wallFilter);//must after sensitive
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain), (double)item.color.gain);
    image_para_combo_box_set((m_combobox_variance), item.color.turb);
    image_para_combo_box_set((m_combobox_cfm_invert), item.color.invert);
    image_para_combo_box_set((m_combobox_persistence), item.color.persist);
    image_para_combo_box_set((m_combobox_cfm_smooth), item.color.smooth);
    image_para_combo_box_set((m_combobox_color_rejection), item.color.reject);*/
}

void ViewSystem::save_image_para(ExamItem::ParaItem &item) {
    string name = GetUserName();
    if (name == "System Default" && name == "Умолчан системы" &&
            name == "系统默认" && name == "Domyślne Systemu" &&
            name == "Par défaut du sys." && name == "Systemvorgabe" && name == "Sistema por defecto") {
        UserSelect::GetInstance()->write_username(GTK_COMBO_BOX(m_comboboxentry_user_select), USERNAME_DB, name);
    }

    ExamItem examItem;
    int probeIndex = 0;
    int itemIndex = 0;
    char *itemName = NULL;
    GetImageNoteSelection(probeIndex, itemIndex, itemName);
    PRINTF("probeIndex:%d itemIndex:%d itemName:%s\n", probeIndex, itemIndex, itemName);
    if ((probeIndex >= 0) && (itemIndex >= 0)) {
        examItem.WriteExamItemPara(probeIndex, itemIndex, &item, itemName);
    }
}

void ViewSystem::GetImagePara(ExamItem::ParaItem &item) {
    /*string gain;
    string angle;
    item.common.MBP = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_mbp));
    item.common.powerIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ao_power));
    // item.d2.gain2D = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_2d_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_2d_gain));
    item.d2.gain2D = atoi(gain.c_str());
    item.d2.AGC = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_agc));
    item.d2.edgeEnhance = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_edge_enh));
    item.d2.steerIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_steer));
    item.d2.focSum = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_sum)) + 1;
    item.d2.focPosIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_pos));
    item.d2.imgScale = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_depth));
    item.d2.chroma = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_chroma));
    item.d2.leftRight = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_lr));
    item.d2.upDown = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ud));
    //item.d2.rotate = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_rotation));
    item.d2.polarity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_polarity));
    item.d2.frameAver = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_frame));
    item.d2.lineAver = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_line_aver));
    item.d2.noiseReject = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_restric));
    item.d2.grayTransIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_gray_trans));
    item.d2.harmonic = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_thi));
    item.d2.TSI = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_tsi));
    item.d2.imgEhn = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_imgEnh));
    item.d2.scanAngle = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_scan_range));
    item.d2.dynamicRange = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_dynamic_range));
    item.d2.freqIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_freq));
    item.d2.thiFreqIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_thi_freq));
    item.d2.lineDensity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_line_density));
    item.d2.smooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_smooth));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_m_gain));
    item.d2.gainM = atoi(gain.c_str());
    item.d2.spaceCompoundIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_space_compound));
    item.d2.freqCompoundIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_freq_compound));

    // item.spectrum.gain = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_pw_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_pw_gain));
    item.spectrum.gain = atoi(gain.c_str());
    item.spectrum.PRF = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_scale_prf));
    item.spectrum.timeSmooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_time_resolution));
    item.spectrum.freq = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_freq));
    item.spectrum.wallFilter = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter));
    angle = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_pw_angle));
    item.spectrum.correctAngle = atoi(angle.c_str());
    item.spectrum.invert = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_invert));

    // item.color.gain = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_cfm_gain));
    item.color.gain = atoi(gain.c_str());
    item.color.lineDensity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_line_density));
    item.color.PRFIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf));
    item.color.wallFilter = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
    item.color.turb = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_variance));
    item.color.invert = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_invert));
    item.color.persist = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_persistence));
    item.color.smooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_smooth));
    item.color.reject = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_color_rejection));
    item.color.sensitive = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sensitivity));*/
}

void ViewSystem::image_para_restrict(int probeIndex) {
    /*switch (probeIndex) {
    case 0:                     // 35C40K
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        break;
    case 1:                     // 75L40K
        gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
        //            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
        break;
    case 2:                     // 65C10K
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        //            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
        break;
    case 3:
        // 35C20H
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        break;
    case 4:
        //65C15D
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        //            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
        break;
    case 5:
        // 30P16A
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        //            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
        gtk_widget_set_sensitive(m_combobox_space_compound, FALSE);
        break;
    case 6:
        //35D40J
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        break;
    case 7:
        //10L25K
        gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
        break;
    case 8:
        //35D40J
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        break;
    case 9:
        //65C10H
        gtk_widget_set_sensitive(m_combobox_steer, FALSE);
        //gtk_widget_set_sensitive(m_combobox_thi, FALSE);
        break;
    }*/
}

void ViewSystem::save_itemIndex(int itemIndex) {
    m_itemIndex = itemIndex;
}

void ViewSystem::save_itemName(char *itemName) {
    m_itemName=itemName;

}

int ViewSystem::get_itemIndex() {
    return m_itemIndex;
}

void ViewSystem::get_itemName(char *itemName) {
    strcpy(itemName,m_itemName);
}

void ViewSystem::tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data) {
    const char *str="";
    static char lastStr[30] = "";
    bool d_press = (bool)user_data;
    str = gtk_tree_path_to_string(path);
    if (d_press) {
        gtk_tree_view_collapse_row(tree_view,
                                   gtk_tree_path_new_from_string(str));
        return;
    }
    if((!strstr(str, ":")) && (strcmp(lastStr, str))) {
        if(strcmp(lastStr, ""))
            gtk_tree_view_collapse_row(tree_view,
                                       gtk_tree_path_new_from_string(lastStr));
        strcpy(lastStr, str);
    }

    gtk_tree_view_expand_row(tree_view, path, FALSE);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(tree_view), path, NULL, FALSE);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 0.5, 0.5);
}





bool ViewSystem::ExamTypeTestRowExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path) {
    gtk_tree_view_collapse_all(tree_view);
    return FALSE; //必须要有返回值,才能触发事件
}



void ViewSystem::init_image_para() {
    /*gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_mbp), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ao_power), -1);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_2d_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_agc), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_edge_enh), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_steer), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_sum), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_pos), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_depth), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_chroma), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_lr), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ud), -1);
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_rotation), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_polarity), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_frame), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_line_aver), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_restric), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_gray_trans), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_thi), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_tsi), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_imgEnh), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_scan_range), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_dynamic_range), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_thi_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_line_density), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_smooth), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_m_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_space_compound), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_freq_compound), -1);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_scale_prf), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_time_resolution), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_angle), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_invert), -1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_line_density), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sensitivity), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_variance), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_invert), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_persistence), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_smooth), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_color_rejection), -1);*/
}

void ViewSystem::set_image_item_sensitive(bool status) {
    m_imageItemSensitive = status;
    /*if (!status)
        init_image_para();

    gtk_widget_set_sensitive(m_combobox_mbp, status);
    gtk_widget_set_sensitive(m_combobox_ao_power, status);
    gtk_widget_set_sensitive(m_spinbutton_2d_gain, status);
    gtk_widget_set_sensitive(m_combobox_agc, status);
    gtk_widget_set_sensitive(m_combobox_edge_enh, status);
    gtk_widget_set_sensitive(m_combobox_steer, status);
    gtk_widget_set_sensitive(m_combobox_focus_sum, status);
    gtk_widget_set_sensitive(m_combobox_focus_pos, status);
    gtk_widget_set_sensitive(m_combobox_depth, status);
    gtk_widget_set_sensitive(m_combobox_chroma, status);
    gtk_widget_set_sensitive(m_combobox_lr, status);
    gtk_widget_set_sensitive(m_combobox_ud, status);
    //gtk_widget_set_sensitive(m_combobox_rotation, status);
    gtk_widget_set_sensitive(m_combobox_polarity, status);
    gtk_widget_set_sensitive(m_combobox_frame, status);
    gtk_widget_set_sensitive(m_combobox_line_aver, status);
    gtk_widget_set_sensitive(m_combobox_restric, status);
    gtk_widget_set_sensitive(m_combobox_gray_trans, status);
    gtk_widget_set_sensitive(m_combobox_thi, status);
    gtk_widget_set_sensitive(m_combobox_tsi, status);
    gtk_widget_set_sensitive(m_combobox_imgEnh, status);
    gtk_widget_set_sensitive(m_spinbutton_m_gain, status);
    gtk_widget_set_sensitive(m_combobox_scan_range, status);
    gtk_widget_set_sensitive(m_combobox_dynamic_range, status);
    gtk_widget_set_sensitive(m_combobox_2d_freq, status);
    gtk_widget_set_sensitive(m_combobox_thi_freq, status);
    gtk_widget_set_sensitive(m_combobox_2d_line_density, status);
    gtk_widget_set_sensitive(m_combobox_2d_smooth, status);
    gtk_widget_set_sensitive(m_combobox_space_compound, status);
    gtk_widget_set_sensitive(m_combobox_freq_compound, status);

    gtk_widget_set_sensitive(m_spinbutton_pw_gain, status);
    gtk_widget_set_sensitive(m_combobox_pw_scale_prf, status);
    gtk_widget_set_sensitive(m_combobox_time_resolution, status);
    gtk_widget_set_sensitive(m_combobox_pw_freq, status);
    gtk_widget_set_sensitive(m_combobox_pw_wallfilter, status);
    gtk_widget_set_sensitive(m_spinbutton_pw_angle, status);
    gtk_widget_set_sensitive(m_combobox_pw_invert, status);
    gtk_widget_set_sensitive(m_combobox_cfm_line_density, status);
    gtk_widget_set_sensitive(m_combobox_cfm_wallfilter, status);
    gtk_widget_set_sensitive(m_combobox_sensitivity, status);
    gtk_widget_set_sensitive(m_combobox_cfm_scale_prf, status);
    gtk_widget_set_sensitive(m_spinbutton_cfm_gain, status);
    gtk_widget_set_sensitive(m_combobox_variance, status);
    gtk_widget_set_sensitive(m_combobox_cfm_invert, status);
    gtk_widget_set_sensitive(m_combobox_persistence, status);
    gtk_widget_set_sensitive(m_combobox_cfm_smooth, status);
    gtk_widget_set_sensitive(m_combobox_color_rejection, status);*/
}






















int ViewSystem::GetCommentProbeType() {
    return  gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
}









/**
 * @brief: 光标改变，隐藏注册信息
 */
void ViewSystem::TreeCursorChanged(GtkTreeView *treeview) {
    gtk_label_set_text(GTK_LABEL(m_labelExport),"");
    gtk_label_set_text(GTK_LABEL(m_labelRegister),"");
    gtk_entry_set_text(GTK_ENTRY(m_entryRegisterKey), "");
    gtk_widget_hide(m_frameRegisterInfo);
}

/**
 * @brief: pop menu
 */
int ViewSystem::OptionOptEvent(GtkWidget *widget, GdkEvent *event) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &iter);
    if(!selected)
        return FALSE;
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    gtk_tree_path_free (path);

    //create popup-menu
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *installItem = gtk_menu_item_new_with_label(_("Install"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), installItem);
    g_signal_connect(G_OBJECT (installItem), "activate", G_CALLBACK(HandleMenuInstallActivate), this);
    gtk_widget_show(installItem);

    GtkWidget *uninstallItem = gtk_menu_item_new_with_label(_("Uninstall"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), uninstallItem);
    g_signal_connect(G_OBJECT (uninstallItem), "activate", G_CALLBACK(HandleMenuUninstallActivate), this);
    gtk_widget_show(uninstallItem);

    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row]))
        gtk_widget_set_sensitive(installItem, false);
    else
        gtk_widget_set_sensitive(uninstallItem, false);

    if (event->type == GDK_BUTTON_PRESS ) {
        GdkEventButton *bevent = (GdkEventButton *) event;
        gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
                        bevent->button, bevent->time);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief:注册/安装选配,获取选中配件的节点
 */
void ViewSystem::MenuInstallActivate(GtkMenuItem *menuitem) {
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeOption));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &m_optionIter);

    gtk_widget_show(m_frameRegisterInfo);
}

/**
 * @brief: 注销/卸载选配 直接删除密钥文件，同时设置标识为false
 */
void ViewSystem::MenuUninstallActivate(GtkMenuItem *menuitem) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeOption));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &iter);
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);

    gchar *optionName = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                       0, _(CManRegister::Optional[row].c_str()),
                       1, _("Disabled(* Restart to take effect)"),
                       -1);
    gtk_tree_path_free (path);
    m_powerOffFlag = true;

    ///>移除文件
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row])) {
        char filePath[256];
        sprintf(filePath, "%s/%s", OPTIONAL_DIR, CManRegister::Optional[row].c_str());
        if(remove(filePath) == -1)
            PRINTF("remove %s file error: %s\n", CManRegister::Optional[row].c_str(),  strerror(errno));
    }
}

/**
 * @brief: 更改选配类表中选配功能的状态
 */
void ViewSystem::UpdateOptionStatus(bool status) {
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    if(status)
        gtk_list_store_set(GTK_LIST_STORE(model), &m_optionIter,
                           0, _(CManRegister::Optional[row].c_str()),
                           1, _("Installed(* Restart to take effect)"),
                           -1);
    else
        gtk_list_store_set(GTK_LIST_STORE(model), &m_optionIter,
                           0, _(CManRegister::Optional[row].c_str()),
                           1, _("Disabled(* Restart to take effect)"),
                           -1);

    gtk_tree_path_free (path);
    m_powerOffFlag = true;
}

/**
 * @brief: export license file to usb
 */
void ViewSystem::BtnExportClicked(GtkButton *button) {
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    char info1[256];
    if(!ptr->CheckUsbStorageState()) {
        sprintf(info1,"%s",_("No USB storage found!"));
        gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
        gtk_widget_show (m_labelExport);

        return;
    } else {
        if(!ptr->MountUsbStorage()) {
            sprintf(info1,"%s",_("Failed to mount USB storage!"));
            gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
            gtk_widget_show (m_labelExport);
            return;
        }
    }

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    gtk_tree_path_free (path);

    if(row < 0 || row >= CManRegister::MAX_OPTIONAL_NUM)
        return;
    string destFileDir = UDISK_PATH;
    bool value(false);
    if(row == 0) {
        if(CDCMRegister::GetMe()==NULL)
            return;
        value = CDCMRegister::GetMe()->GenerateLicenseFile(destFileDir);
    } else {
        if(CManRegister::GetInstance()==NULL)
            return;
        value = CManRegister::GetInstance()->GenerateLicenseFile(destFileDir, CManRegister::Optional[row]);
    }
    char info[256];
    if(value) {
        sprintf(info,"%s",_("Succeed to export file!"));
    } else {
        sprintf(info,"%s",_("Fail to export file!"));
    }
    gtk_label_set_text(GTK_LABEL(m_labelExport),info);
    gtk_widget_show (m_labelExport);

    ptr->UmountUsbStorage();
}

/**
 * @brief: 校验序列号，验证注册是否成功
 */
void ViewSystem::BtnRegisterClicked(GtkButton *button) {
    const gchar *registerKey = gtk_entry_get_text(GTK_ENTRY(m_entryRegisterKey));
    if(CManRegister::GetInstance() == NULL)
        return;

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    gtk_tree_path_free (path);

    if(row < 0 || row >= CManRegister::MAX_OPTIONAL_NUM)
        return;

    bool value(false);
    if(row == 0) {
        value =CDCMRegister::GetMe()->CheckAuthorize(registerKey);
        CManRegister::GetInstance()->CheckAuthorize(registerKey, row);
    } else
        value =CManRegister::GetInstance()->CheckAuthorize(registerKey, row);

    char info[256];
    if(value) {
        sprintf(info,"%s",_("Succeed to register!"));
    } else {
        sprintf(info,"%s",_("Fail to register!"));
    }
    gtk_label_set_text(GTK_LABEL(m_labelRegister),info);
    gtk_widget_show (m_labelRegister);

    ///>设置选配状态
    if(!CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row]) && value)
        UpdateOptionStatus(value);
}

















void ViewSystem::GetImageNoteSelection(int &probeIndex, int &itemIndex, char* &itemName) {
    // get probeIndex
    probeIndex = GetProbeType();

    // get itemIndex
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        itemIndex = -1;
        return ;
    }
    gtk_tree_model_get(model, &iter,
                       0, &itemName,
                       -1);
    gtk_tree_model_get(model, &iter,
                       INDEX_COLUMN, &itemIndex,
                       -1);
}













GtkTreeView* ViewSystem::create_common_print_treeview() {
  GtkTreeView* treeview = Utils::create_tree_view(create_common_print_model());
  gtk_tree_view_set_enable_search(treeview, FALSE);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview), GTK_SELECTION_SINGLE);

  GtkCellRenderer* render_brand = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_brand = gtk_tree_view_column_new_with_attributes(_("Brand"), render_brand, "text", PRT_BRAND, NULL);
  gtk_tree_view_append_column(treeview, col_brand);

  GtkCellRenderer* render_model = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_model = gtk_tree_view_column_new_with_attributes(_("Model"), render_model, "text", PRT_MODEL, NULL);
  gtk_tree_view_append_column(treeview, col_model);

  return treeview;
}

GtkTreeModel* ViewSystem::create_common_print_model() {
  GtkTreeIter iter;

  GtkListStore* store = gtk_list_store_new(CNUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
  for (unsigned int i = 0; i < G_N_ELEMENTS(COMMON_PRINTER_DATA); i++) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set(store, &iter,
      PRT_BRAND, COMMON_PRINTER_DATA[i].brand.c_str(),
      PRT_MODEL, COMMON_PRINTER_DATA[i].model.c_str(),
      -1);
  }

  return GTK_TREE_MODEL(store);
}

GtkTreeView* ViewSystem::create_specific_print_treeview() {
  GtkTreeView* treeview = Utils::create_tree_view(create_specific_print_model());
  gtk_tree_view_set_enable_search(treeview, FALSE);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview), GTK_SELECTION_SINGLE);

  GtkCellRenderer* render_name = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_name = gtk_tree_view_column_new_with_attributes(_("Printer"), render_name, "text", PRT_NAME, NULL);
  gtk_tree_view_append_column(treeview, col_name);

  GtkCellRenderer* render_default = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_default = gtk_tree_view_column_new_with_attributes(_("Default"), render_default, "text", PRT_DEFAULT, NULL);
  gtk_tree_view_append_column(treeview, col_default);

  GtkCellRenderer* render_status = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_status = gtk_tree_view_column_new_with_attributes(_("Status"), render_status, "text", PRT_STATUS, NULL);
  gtk_tree_view_append_column(treeview, col_status);

  GtkCellRenderer* render_message = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* col_message = gtk_tree_view_column_new_with_attributes(_("Message"), render_message, "text", PRT_MESSAGE, NULL);
  gtk_tree_view_append_column(treeview, col_message);

  return treeview;
}

GtkTreeModel* ViewSystem::create_specific_print_model() {
  GtkTreeIter iter;

  GtkListStore* store = gtk_list_store_new(SNUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  Printer print_data;

  if (print_data.GetPrinters()) {
    const vector<Printer::printer_attr>* vec_prt = print_data.get_printers();
    vector<Printer::printer_attr>::const_iterator iter_prt;

    for (iter_prt = vec_prt->begin(); iter_prt != vec_prt->end(); ++iter_prt) {
      string tag_default;

      if (iter_prt->isDefault) {
        tag_default = "*";
      } else {
        tag_default = "";
      }

      gtk_list_store_append (store, &iter);
      gtk_list_store_set(store, &iter,
        PRT_NAME, iter_prt->name.c_str(),
        PRT_DEFAULT, tag_default.c_str(),
        PRT_STATUS, iter_prt->status.c_str(),
        PRT_MESSAGE, iter_prt->message.c_str(),
        -1);
    }
  }

  return GTK_TREE_MODEL(store);
}

GtkWidget * ViewSystem::create_key_function_treeview(const string function_list[], unsigned int size) {
    GtkWidget *treeview;
    GtkListStore *store;
    GtkTreeIter    iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    store = gtk_list_store_new(1, G_TYPE_STRING);
    for (unsigned int i = 0; i < size; i++) {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter,
                           0, _(function_list[i].c_str()),
                           -1);
    }
    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), FALSE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Function"), renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    return treeview;
}

void ViewSystem::create_exam_comment_model(vector<ExamItem::EItem> indexVec) {
  int probeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

  if (probeIndex == -1) {
    return;
  }

  vecExamItem_comment.clear();

  for (int i = 0; i < indexVec.size(); i++) {
    vecExamItem_comment.push_back(ExamItemArray[indexVec[i]].name);
  }

  CreateDefineItem_comment(probeIndex, vecExamItem_comment);

  Utils::combobox_clear(m_combobox_exam_comment);

  for (int i = 0; i < vecExamItem_comment.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_exam_comment, vecExamItem_comment[i].c_str());
  }

  ExamItem exam;
  IniFile ini(string(CFG_RES_PATH) + string(STORE_DEFAULT_ITEM_PATH));
  string current_exam = exam.ReadDefaultProbeDefaultItemName(&ini);

  int exam_type_index = 0;

  for (int i = 0; i < vecExamItem_comment.size(); i++) {
    if (current_exam == vecExamItem_comment[i]) {
      exam_type_index = i;

      break;
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_comment), exam_type_index);
}

GtkTreeModel* ViewSystem::create_exam_item_model(vector<ExamItem::EItem> indexVec) {
  string probe_type = TopArea::GetInstance()->GetProbeType();
  int probe_index = 0;

  for (int i = 0; i < NUM_PROBE; ++i) {
    if (probe_type == g_probe_list[i]) {
      probe_index = i;
    }
  }

  string str_check_part = TopArea::GetInstance()->GetCheckPart();

  vector<ExamPara> vecExamItem;
  for (int i = 0; i < indexVec.size(); i++) {
    vecExamItem.push_back(ExamItemArray[indexVec[i]]);
  }

  int probeIndex = GetProbeType();
  CreateDefineItem(probeIndex, vecExamItem);

  if (vecExamItem.empty()) {
    return NULL;
  }

  GtkTreeIter iter;
  GtkTreeIter child_iter;
  GtkTreeStore* store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  vector<ExamPara>::iterator iterItem;

  for (iterItem = vecExamItem.begin(); iterItem != vecExamItem.end(); iterItem++) {
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
      bool has_node = true;
      char* strtmp = NULL;

      while (has_node) {
        gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, NAME_COLUMN, &strtmp, -1);

        if (iterItem->dept_name == strtmp) {
          gtk_tree_store_append(store, &child_iter, &iter);
          gtk_tree_store_set(store, &child_iter,
            NAME_COLUMN, _(iterItem->name.c_str()),
            INDEX_COLUMN, iterItem->index,
            -1);

          if(str_check_part == iterItem->name && probe_index == probeIndex) {
            GtkTreePath* path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &child_iter);
            m_str_index = gtk_tree_path_to_string (path);

            gtk_tree_path_free(path);
          } else if(probe_index != probeIndex) {
            m_str_index = "";
          }

          goto next;
        } else {
          has_node = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
      }
    }

    if ((iterItem->name).empty()) {
      gtk_tree_store_append(store, &iter, NULL);
      gtk_tree_store_set(store, &iter,
        NAME_COLUMN, _(iterItem->dept_name.c_str()),
        INDEX_COLUMN, iterItem->index,
        -1);
    } else {
      gtk_tree_store_append(store, &iter, NULL);
      gtk_tree_store_set(store, &iter,
        NAME_COLUMN, _(iterItem->dept_name.c_str()),
        INDEX_COLUMN, -1,
        -1);
      gtk_tree_store_append(store, &child_iter, &iter);
      gtk_tree_store_set(store, &child_iter,
        NAME_COLUMN, _(iterItem->name.c_str()),
        INDEX_COLUMN, iterItem->index,
        -1);

      if(str_check_part == iterItem->name && probe_index == probeIndex) {
        GtkTreePath* path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &child_iter);
        m_str_index = gtk_tree_path_to_string (path);
        gtk_tree_path_free(path);
      } else if(probe_index != probeIndex) {
        m_str_index = "";
      }
    }

  next:
    continue;
  }

  return GTK_TREE_MODEL(store);
}

void ViewSystem::add_columns_comment(GtkTreeView* treeview) {
  m_cellrenderer_comment_text = gtk_cell_renderer_text_new();
  gint col_offset = gtk_tree_view_insert_column_with_attributes(treeview,
    -1, _("Select Item"), m_cellrenderer_comment_text, "text", 0, NULL);

  g_object_set(m_cellrenderer_comment_text, "editable", TRUE, NULL);
  g_signal_connect(m_cellrenderer_comment_text, "editing_started", G_CALLBACK(signal_renderer_insert_user_item), this);
  g_signal_connect(m_cellrenderer_comment_text, "edited", G_CALLBACK(signal_renderer_rename_selectcomment), this);

  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable(GTK_TREE_VIEW_COLUMN(column), TRUE);
}

void ViewSystem::add_columns_comment_selected(GtkTreeView* treeview) {
  m_cellrenderer_comment_text_selected = gtk_cell_renderer_text_new();
  gint col_offset = gtk_tree_view_insert_column_with_attributes(treeview,
    -1, _("Selected Item"), m_cellrenderer_comment_text_selected, "text", 0, NULL);

  g_object_set(m_cellrenderer_comment_text_selected, "editable", TRUE, NULL);
  g_signal_connect(m_cellrenderer_comment_text_selected, "editing_started", G_CALLBACK(signal_renderer_insert_user_item), this);
  g_signal_connect(m_cellrenderer_comment_text_selected, "edited", G_CALLBACK(signal_renderer_rename_comment), this);

  GtkTreeViewColumn* column = gtk_tree_view_get_column(treeview, col_offset - 1);
  gtk_tree_view_column_set_clickable(GTK_TREE_VIEW_COLUMN (column), TRUE);
}

void ViewSystem::add_exam_item_column(GtkTreeView* treeview) {
    m_renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(_("Department"), m_renderer, "text", NAME_COLUMN, NULL);

    g_object_set(m_renderer, "editable", TRUE, NULL);
    g_signal_connect(m_renderer, "editing_started", G_CALLBACK(signal_renderer_insert_user_item), this);
    g_signal_connect(m_renderer, "edited", G_CALLBACK(signal_renderer_rename), this);

    gtk_tree_view_append_column(treeview, column);
}







void ViewSystem::UpdateHospitalandpart(int date_format, const string hospital_name) {
    TopArea::GetInstance()->SetDateFormat(date_format);
    g_patientInfo.UpdateTopArea();
    TopArea::GetInstance()->UpdateHospitalName(hospital_name);

    ExamItem exam;
    char str_path[256];
    sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(str_path);
    bool userFlag = exam.ReadUserItemFlag(&ini);
    string userItemName = exam.ReadDefaultProbeDefaultItemName(&ini);
    if(userFlag) {
        TopArea::GetInstance()->UpdateCheckPart(_(userItemName.c_str()));
    } else {
        ExamItem::EItem index = ProbeSelect::GetItemIndex();

        TopArea::GetInstance()->UpdateCheckPart(_(EXAM_TYPES[index].c_str()));
    }
}











void ViewSystem::EntryDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}

void ViewSystem::EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}







void ViewSystem::image_default_setting() {
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    const char *name = exam.ReadDefaultUserSelect(&ini).c_str();
    bool flag = exam.ReadUserItemFlag(&ini);

    if (((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))||(flag)) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("Fail to default factory! Define user or New item is under using!"), NULL);

    } else {
        //delete define item
        char deletepath[256];
        sprintf(deletepath, "%s%s", CFG_RES_PATH, EXAM_ITEM_FILE);

        // remove all the custom image parameter config file
        remove(USERNAME_DB);
        FileMan f;
        f.DelDirectory(deletepath);
        f.DelDirectory(USERCONFIG_PATH);

        char itemparafile[256];
        sprintf(itemparafile, "%s%s", CFG_RES_PATH, EXAM_FILE);
        remove(itemparafile);
        char defaultfile[256];
        sprintf(defaultfile, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
        f.CopyFile(defaultfile, itemparafile);
        //itempara file of default dir
        sprintf(itemparafile, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        remove(itemparafile);
        sprintf(defaultfile, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
        f.CopyFile(defaultfile, itemparafile);

        UserSelect::GetInstance()->create_userdefined_dir();
        char path_defined[256];
        sprintf(path_defined, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, "SystemDefault", ".ini");
        int fd = open(path_defined, O_RDWR |O_CREAT |O_TRUNC, 0666);
        close(fd);

        UserSelect::GetInstance()->create_userconfig_dir();
        UserSelect::GetInstance()->creat_username_db(USERNAME_DB);
        ClearComboBox(GTK_COMBO_BOX(m_comboboxentry_user_select));
        UserSelect::GetInstance()->read_default_username(GTK_COMBO_BOX(m_comboboxentry_user_select));
        UserSelect::GetInstance()->set_active_user(GTK_COMBO_BOX(m_comboboxentry_user_select), 0);
    }

    CalcSetting::GetInstance()->ChangeExamBoxDelete();
    MeasureSetting::GetInstance()->ChangeExamBoxDelete();

    int probeIndex;
    probeIndex = GetProbeType();
    ChangeCommentExamBoxDelete(probeIndex);
}

void ViewSystem::BtnImageGetCurrentClicked(GtkButton *button) {
    if (m_imageItemSensitive) {
        ExamItem::ParaItem paraItem;

        // get current item para
        Img2D::GetInstance()->GetCurPara(&paraItem);
        ImgPw::GetInstance()->GetCurPara(&paraItem);
        ImgCfm::GetInstance()->GetCurPara(&paraItem);
        ImgProc2D::GetInstance()->GetCurPara(&paraItem);
        ImgProcPw::GetInstance()->GetCurPara(&paraItem);
        ImgProcCfm::GetInstance()->GetCurPara(&paraItem);

        // set current item para to dialog
        SetImagePara(paraItem);
    }
}



void ViewSystem::get_current_and_save_image_para() {
    ExamItem::ParaItem paraItem;

    // get current item para
    Img2D::GetInstance()->GetCurPara(&paraItem);
    ImgPw::GetInstance()->GetCurPara(&paraItem);
    ImgCfm::GetInstance()->GetCurPara(&paraItem);
    ImgProc2D::GetInstance()->GetCurPara(&paraItem);
    ImgProcPw::GetInstance()->GetCurPara(&paraItem);
    ImgProcCfm::GetInstance()->GetCurPara(&paraItem);

    save_image_para(paraItem);
}







void ViewSystem::ComboFocusSum(GtkComboBox *box) {
    int i;
    char buf[20];
    int indexFocOrder;
    int index = gtk_combo_box_get_active(box);

    /*if ((index != -1) && (index < Img2D::MAX_FOCUS)) {
        indexFocOrder = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_pos));

        ClearComboBox(GTK_COMBO_BOX(m_combobox_focus_pos));
        for (i = 0; i < Img2D::FOC_POS_INDEX_MAX[index]; i ++) {
            sprintf(buf, "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_focus_pos), buf);
        }

        if (indexFocOrder >= Img2D::FOC_POS_INDEX_MAX[index])
            indexFocOrder = Img2D::FOC_POS_INDEX_MAX[index] / 2;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_pos), indexFocOrder);
    }*/
}

void ViewSystem::ComboPwPrf(GtkComboBox *box) {
    char buf[50];
    int i;
    int indexWf;
    int index = gtk_combo_box_get_active(box);

    if ((index != -1) && (index < ImgPw::MAX_PRF)) {
        indexWf = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter));

        ClearComboBox(GTK_COMBO_BOX(m_combobox_pw_wallfilter));
        // wall filter
        for (i = 0; i < ImgPw::MAX_WALL_FILTER; i ++) {
            //sprintf(buf , "%dHz", ImgPw::WALL_FILTER_FREQ[index][i]);
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_wallfilter), buf);
        }

        if (indexWf >= ImgPw::MAX_WALL_FILTER)
            indexWf = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_pw_wallfilter), indexWf);
    }
}

void ViewSystem::ComboCfmPrf(GtkComboBox *box) {
    char buf[50];
    int i;
    int index = gtk_combo_box_get_active(box);

    if ((index != -1) && (index < ImgCfm::MAX_PRF_INDEX)) {
        int indexPkt = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sensitivity));

        // sensitive
        ClearComboBox(GTK_COMBO_BOX(m_combobox_sensitivity));
        for (i = 0; i < ImgCfm::MAX_PKTSIZE_INDEX; i ++) {
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sensitivity), buf);
        }

        if (indexPkt >= ImgCfm::MAX_PKTSIZE_INDEX)
            indexPkt = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sensitivity), indexPkt);
    }
}

void ViewSystem::ComboSensitivity(GtkComboBox *box) {
    char buf[50];
    int i;
    int indexPkt = gtk_combo_box_get_active(box);

    if ((indexPkt != -1) && (indexPkt < ImgCfm::MAX_PKTSIZE_INDEX)) {
        //int indexPrf = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf));
        int indexWF = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));

        // wall filter
        ClearComboBox(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
        for (i = 0; i < ImgCfm::MAX_WALLFILTER_INDEX; i ++) {
            //sprintf(buf , "%dHz", ImgCfm::GetWFDisplay(indexPrf, indexPkt, ImgCfm::WALL_FILTER_SELECT_IDNEX[indexPkt][i]));
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_wallfilter), buf);
        }

        if (indexWF >= ImgCfm::MAX_WALLFILTER_INDEX)
            indexWF = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter), indexWF);
    }
}

void ViewSystem::ClearComboBox(GtkComboBox *box) {
    GtkTreeModel *model;
    model = gtk_combo_box_get_model(box);
    gtk_list_store_clear(GTK_LIST_STORE(model));
}







void ViewSystem::RadioP1Toggled(GtkToggleButton *togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    m_p1Selected = true;
  }
}

void ViewSystem::RadioP2Toggled(GtkToggleButton *togglebutton) {
  gboolean press = gtk_toggle_button_get_active(togglebutton);

  if (press) {
    m_p1Selected = false;
  }
}

void ViewSystem::TreeFuncChanged(GtkTreeSelection *selection) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *value;

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
        return ;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gchar *index_str = gtk_tree_path_to_string (path);

    gtk_tree_model_get(model, &iter,
                       0, &value,
                       -1);
    if (m_p1Selected) {
        gtk_entry_set_text(GTK_ENTRY(m_entry_function_p1), value);
        m_p1_func_index = atoi(index_str);
    } else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_function_p2), value);
        m_p2_func_index = atoi(index_str);
    }
    gtk_tree_path_free (path);
    free(value);
}
