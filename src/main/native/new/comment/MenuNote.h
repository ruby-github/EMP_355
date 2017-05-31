#ifndef __MENU_NOTE_H__
#define __MENU_NOTE_H__

#include "utils/Utils.h"

class MenuNote {
public:
  MenuNote();
  ~MenuNote();

public:
  GtkWidget* Create();

  void Show();
  void Hide();
  void Focus();

  void UpdateLabel();

private:
  // signal

  static void signal_combobox_changed_dept(GtkComboBox* combobox, MenuNote* data) {
    if (data != NULL) {
      data->ComboBoxChangedDept(combobox);
    }
  }

  static gboolean signal_tree_button_press(GtkWidget* widget, GdkEventButton* event, MenuNote* data) {
    if (data != NULL) {
      data->TreeButtonPress(widget, event);
    }

    return FALSE;
  }

  // signal

  void ComboBoxChangedDept(GtkComboBox* combobox);
  void TreeButtonPress(GtkWidget* widget, GdkEventButton* event);

private:
  GtkTreeModel* CreateDeptModel();

  GtkScrolledWindow* CreateCurrentItem();
  GtkScrolledWindow* CreateAbdoItem();
  GtkScrolledWindow* CreateUroItem();
  GtkScrolledWindow* CreateCarItem();
  GtkScrolledWindow* CreateObItem();
  GtkScrolledWindow* CreateGynItem();
  GtkScrolledWindow* CreateSpItem();
  GtkScrolledWindow* CreateVsItem();
  GtkScrolledWindow* CreateOrthoItem();

  string DepartmentName(int index);

  void HideMenu();
  void ShowMenu(int index, string department);

  void UpdateScaleModel();

  GtkTreeModel* CreateItemNoteModel(int index, string department);
  GtkTreeView* CreateItemList(int index, string department);

  void AddColumnsComment(GtkTreeView* treeview);

private:
  GtkTable* m_table;

  GtkComboBoxText* m_comboDept;
  GtkTreeModel* m_modelDept;

  GtkScrolledWindow* m_scrolledwindow_item_current;
  GtkScrolledWindow* m_scrolledwindow_item_abdo;
  GtkScrolledWindow* m_scrolledwindow_item_uro;
  GtkScrolledWindow* m_scrolledwindow_item_car;
  GtkScrolledWindow* m_scrolledwindow_item_ob;
  GtkScrolledWindow* m_scrolledwindow_item_gyn;
  GtkScrolledWindow* m_scrolledwindow_item_sp;
  GtkScrolledWindow* m_scrolledwindow_item_vs;
  GtkScrolledWindow* m_scrolledwindow_item_ortho;

  GtkTreeView* m_treeview_item_current;
  GtkTreeView* m_treeview_item_abdo;
  GtkTreeView* m_treeview_item_uro;
  GtkTreeView* m_treeview_item_car;
  GtkTreeView* m_treeview_item_ob;
  GtkTreeView* m_treeview_item_gyn;
  GtkTreeView* m_treeview_item_sp;
  GtkTreeView* m_treeview_item_vs;
  GtkTreeView* m_treeview_item_ortho;
};

extern MenuNote g_menuNote;

#endif
