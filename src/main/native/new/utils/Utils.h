#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib/gi18n.h>
#include <goocanvas.h>
#include <gtk/gtk.h>

#include "utils/Settings.h"

class Utils {
public:
  static GtkWindow* create_app_window(const string title = "", const int width = 800, const int height = 600);

  static GtkDialog* create_dialog(GtkWindow* parent, const string title = "", const int width = 400, const int height = 300);
  static GtkButton* add_dialog_button(GtkDialog* dialog, const string label, const GtkResponseType type, const string stock_id = "");

  static GtkLabel* create_label(const string text = "");

  static GtkButton* create_button(const string label = "");
  static GtkButton* create_button_with_image(const string filename = "", const int width = 0, const int height = 0);
  static void set_button_image(GtkButton* button, GtkImage* image, const GtkPositionType position = GTK_POS_LEFT);

  static GtkCheckButton* create_check_button(const string label = "");

  static GtkSpinButton* create_spin_button(GtkAdjustment* adjustment, gdouble climb_rate, guint digits);

  static GtkImage* create_image(const string filename = "", const int width = 0, const int height = 0);

  static GtkEntry* create_entry(gunichar ch = 0);
  static GtkComboBoxText* create_combobox_text();
  static GtkNotebook* create_notebook();
  static GtkProgressBar* create_progress_bar();

  static GtkTable* create_table(guint rows, guint columns);

  static GtkScrolledWindow* create_scrolled_window();
  static GtkTreeView* create_tree_view(GtkTreeModel* mode = NULL);
  static GtkFrame* create_frame(const string label = "");

  static GtkHSeparator* create_hseparator();
  static GtkVSeparator* create_vseparator();

  static GtkBox* create_hbox(gboolean homogeneous = FALSE, gint spacing = 0);
  static GtkBox* create_vbox(gboolean homogeneous = FALSE, gint spacing = 0);

  static GtkMenuBar* create_menu_bar();
  static GtkMenuItem* create_menu_item(const string label);

  static GooCanvas* create_canvas(const int width = 0, const int height = 0);

  static void set_font(GtkWidget* widget, const string family = "", const string sytle = "", const int size = 0);
  static void adjust_font_size(GtkWidget* widget, const string family, const string sytle,
    const int size, const int max_width, const int max_height);

  static void combobox_clear(GtkComboBoxText* combobox_text);
  static string combobox_active_text(GtkComboBoxText* combobox_text);

  static GdkColor* get_color(const string color_name);

private:
  static GdkColor* m_color;

public:
  static void test(GtkWidget* widget);
};

#endif
