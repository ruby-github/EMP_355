#include "utils/Utils.h"

#include "sysMan/SysGeneralSetting.h"
#include "utils/FakeXUtils.h"

#include <cstdlib>

GdkColor* Utils::m_color = NULL;

// ---------------------------------------------------------

GtkWindow* Utils::create_app_window(const string title, const int width, const int height) {
  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gtk_widget_set_size_request(GTK_WIDGET(window), width, height);

  gtk_window_set_title(window, title.c_str());
  gtk_window_set_position(window, GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (window, FALSE);

  gtk_container_set_border_width(GTK_CONTAINER(window), 0);

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_modify_bg(GTK_WIDGET(window), GTK_STATE_NORMAL, get_color("black"));

  return window;
}

GtkDialog* Utils::create_dialog(GtkWindow* parent, const string title, const int width, const int height) {
  GtkDialog* dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(title.c_str(), parent, GTK_DIALOG_MODAL, NULL));

  gtk_widget_set_size_request(GTK_WIDGET(dialog), width, height);

  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
  gtk_dialog_set_has_separator(dialog, TRUE);

  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  gtk_widget_modify_bg(GTK_WIDGET(dialog), GTK_STATE_NORMAL, get_color("#363636"));

  return dialog;
}

GtkButton* Utils::add_dialog_button(GtkDialog* dialog, const string label, const GtkResponseType type, const string stock_id) {
  GtkButton* button = GTK_BUTTON(gtk_dialog_add_button(dialog, label.c_str(), type));

  if (!stock_id.empty()) {
    GtkImage* image = GTK_IMAGE(gtk_image_new_from_stock(stock_id.c_str(), GTK_ICON_SIZE_BUTTON));
    set_button_image(button, image);
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 10);
  gtk_button_set_alignment(button, 0.5, 0.5);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

GtkLabel* Utils::create_label(const string text) {
  GtkLabel* label = GTK_LABEL(gtk_label_new(text.c_str()));

  set_font(GTK_WIDGET(label), "", "", 10);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
  gtk_label_set_use_markup(label, TRUE);

  gtk_label_set_line_wrap(label, TRUE);
  gtk_label_set_line_wrap_mode(label, PANGO_WRAP_WORD_CHAR);

  gtk_widget_modify_fg(GTK_WIDGET(label), GTK_STATE_NORMAL, get_color("white"));

  return label;
}

GtkButton* Utils::create_button(const string label) {
  GtkButton* button = NULL;

  if (label.empty()) {
    button = GTK_BUTTON(gtk_button_new());
  } else {
    button = GTK_BUTTON(gtk_button_new_with_label(label.c_str()));
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 10);
  gtk_button_set_alignment(button, 0.5, 0.5);

  /*
    GTK_STATE_NORMAL      // 正常状态
    GTK_STATE_ACTIVE      // 获得焦点时的状态
    GTK_STATE_PRELIGHT    // 鼠标移动到上方时的状态
    GTK_STATE_SELECTED    // 被选中时的状态
    GTK_STATE_INSENSITIVE // 失去功能时的状态
  */

  gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

GtkButton* Utils::create_button_with_image(const string filename, const int width, const int height) {
  GtkButton* button = GTK_BUTTON(gtk_button_new());

  if (!filename.empty()) {
    set_button_image(button, create_image(filename, width, height));
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 10);
  gtk_button_set_alignment(button, 0.5, 0.5);

  gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

void Utils::set_button_image(GtkButton* button, GtkImage* image, const GtkPositionType position) {
  GtkSettings* settings = gtk_widget_get_settings(GTK_WIDGET(button));
  g_object_set(settings, "gtk-button-images", TRUE, NULL);

  gtk_button_set_image(button, GTK_WIDGET(image));
  gtk_button_set_image_position(button, position);
}

GtkCheckButton* Utils::create_check_button(const string label) {
  GtkCheckButton* button = NULL;

  if (label.empty()) {
    button = GTK_CHECK_BUTTON(gtk_check_button_new());
  } else {
    button = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(label.c_str()));
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 10);
  gtk_button_set_alignment(GTK_BUTTON(button), 0, 0.5);

  gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

GtkRadioButton* Utils::create_radio_button(GSList* group, const string label) {
  GtkRadioButton* button = NULL;

  if (label.empty()) {
    button = GTK_RADIO_BUTTON(gtk_radio_button_new(group));
  } else {
    button = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(group, label.c_str()));
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 10);
  gtk_button_set_alignment(GTK_BUTTON(button), 0, 0.5);

  gtk_widget_set_size_request(GTK_WIDGET(button), -1, 30);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

GtkSpinButton* Utils::create_spin_button(GtkAdjustment* adjustment, gdouble climb_rate, guint digits) {
  GtkSpinButton* spin_button = GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment, climb_rate, digits));

  gtk_widget_set_size_request(GTK_WIDGET(spin_button), -1, 30);

  return spin_button;
}

GtkImage* Utils::create_image(const string filename, const int width, const int height) {
  GtkImage* image = NULL;

  if (filename.empty()) {
    image = GTK_IMAGE(gtk_image_new());
  } else {
    if (width == 0 || height == 0) {
      image = GTK_IMAGE(gtk_image_new_from_file(filename.c_str()));
    } else {
      GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), NULL);
      GdkPixbuf* pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);

      image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf_scale));
      g_object_unref(pixbuf_scale);
      g_object_unref(pixbuf);
    }
  }

  return image;
}

GtkEntry* Utils::create_entry(gunichar ch) {
  GtkEntry* entry = GTK_ENTRY(gtk_entry_new());

  set_font(GTK_WIDGET(entry), "", "", 10);
  gtk_widget_set_size_request(GTK_WIDGET(entry), -1, 25);

  if (ch > 0) {
    gtk_entry_set_invisible_char(entry, ch);
  }

  return entry;
}

GtkComboBoxText* Utils::create_combobox_text() {
  GtkComboBoxText* combobox = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());

  set_font(GTK_WIDGET(combobox), "", "", 10);
  gtk_widget_set_size_request(GTK_WIDGET(combobox), -1, 30);

  return combobox;
}

GtkComboBoxEntry* Utils::create_combobox_entry() {
  GtkComboBoxEntry* combobox = GTK_COMBO_BOX_ENTRY(gtk_combo_box_entry_new_text());

  set_font(GTK_WIDGET(combobox), "", "", 10);
  gtk_widget_set_size_request(GTK_WIDGET(combobox), -1, 30);

  return combobox;
}

GtkNotebook* Utils::create_notebook() {
  GtkNotebook* notebook = GTK_NOTEBOOK(gtk_notebook_new());

  gtk_notebook_set_scrollable(notebook, TRUE);
  gtk_container_set_border_width(GTK_CONTAINER(notebook), 0);

  gtk_widget_modify_bg(GTK_WIDGET(notebook), GTK_STATE_NORMAL, get_color("#545454"));

  return notebook;
}

GtkProgressBar* Utils::create_progress_bar() {
  GtkProgressBar* progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());

  gtk_widget_set_size_request(GTK_WIDGET(progress_bar), -1, 25);

  return progress_bar;
}

GtkTable* Utils::create_table(guint rows, guint columns) {
  GtkTable* table = GTK_TABLE(gtk_table_new(rows, columns, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table), 20);

  gtk_table_set_row_spacings(table, 10);
  gtk_table_set_col_spacings(table, 10);

  return table;
}

GtkScrolledWindow* Utils::create_scrolled_window() {
  GtkScrolledWindow* scrolled_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(scrolled_window, GTK_SHADOW_IN);

  return scrolled_window;
}

GtkTreeView* Utils::create_tree_view(GtkTreeModel* mode) {
  GtkTreeView* tree_view = NULL;

  if (mode == NULL) {
    tree_view = GTK_TREE_VIEW(gtk_tree_view_new());
  } else {
    tree_view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(mode));
  }

  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(tree_view), GTK_SELECTION_BROWSE);

  return tree_view;
}

GtkTextView* Utils::create_text_view() {
  GtkTextView* text_view = GTK_TEXT_VIEW(gtk_text_view_new());

  gtk_text_view_set_editable(text_view, FALSE);
  gtk_text_view_set_wrap_mode(text_view, GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_cursor_visible(text_view, FALSE);

  return text_view;
}

GtkFrame* Utils::create_frame(const string label) {
  GtkFrame* frame = NULL;

  if (label.empty()) {
    frame = GTK_FRAME(gtk_frame_new(NULL));

    gtk_frame_set_shadow_type(frame, GTK_SHADOW_OUT);
  } else {
    frame = GTK_FRAME(gtk_frame_new(label.c_str()));

    gtk_label_set_use_markup(GTK_LABEL(gtk_frame_get_label_widget(frame)), TRUE);
    gtk_frame_set_label_align(frame, 0.5, 0.5);

    gtk_frame_set_shadow_type(frame, GTK_SHADOW_IN);
  }

  return frame;
}

GtkHSeparator* Utils::create_hseparator() {
  GtkHSeparator* separator = GTK_HSEPARATOR(gtk_hseparator_new());

  return separator;
}

GtkVSeparator* Utils::create_vseparator() {
  GtkVSeparator* separator = GTK_VSEPARATOR(gtk_vseparator_new());

  return separator;
}

GtkBox* Utils::create_hbox(gboolean homogeneous, gint spacing) {
  GtkBox* box = GTK_BOX(gtk_hbox_new(homogeneous, spacing));

  return box;
}

GtkBox* Utils::create_vbox(gboolean homogeneous, gint spacing) {
  GtkBox* box = GTK_BOX(gtk_vbox_new(homogeneous, spacing));

  return box;
}

GtkMenuBar* Utils::create_menu_bar() {
  GtkMenuBar* menubar = GTK_MENU_BAR(gtk_menu_bar_new());

  return menubar;
}

GtkMenuItem* Utils::create_menu_item(const string label) {
  GtkMenuItem* menuitem = GTK_MENU_ITEM(gtk_menu_item_new_with_label(label.c_str()));

  return menuitem;
}

GooCanvas* Utils::create_canvas(const int width, const int height) {
  GooCanvas* canvas = GOO_CANVAS(goo_canvas_new());

  if (width > 0 && height > 0) {
    gtk_widget_set_size_request(GTK_WIDGET(canvas), width, height);
  }

  return canvas;
}

void Utils::set_font(GtkWidget* widget, const string family, const string sytle, const int size) {
  PangoFontDescription* font = NULL;

  if (sytle.empty()) {
    font = pango_font_description_new();
  } else {
    font = pango_font_description_from_string(sytle.c_str());
  }

  if (!family.empty()) {
    pango_font_description_set_family(font, family.c_str());
  } else {
    pango_font_description_set_family(font, DEFAULT_FONT);
  }

  if (size > 0) {
    pango_font_description_set_size(font, size * PANGO_SCALE);
  } else {
    pango_font_description_set_size(font, 10 * PANGO_SCALE);
  }

  gtk_widget_modify_font(widget, font);
  pango_font_description_free(font);
}

void Utils::adjust_font_size(GtkWidget* widget, const string family, const string sytle,
  const int size, const int max_width, const int max_height) {
  if (size > 0) {
    if (max_width > 0 && max_height > 0) {
      string text;

      if (GTK_IS_LABEL(widget) == TRUE) {
        text = gtk_label_get_text(GTK_LABEL(widget));
      } else if (GTK_IS_BUTTON(widget) == TRUE) {
        text = gtk_button_get_label(GTK_BUTTON(widget));
      } else {
      }

      if (!text.empty()) {
        PangoFontDescription* font = NULL;

        if (sytle.empty()) {
          font = pango_font_description_new();
        } else {
          font = pango_font_description_from_string(sytle.c_str());
        }

        if (!family.empty()) {
          pango_font_description_set_family(font, family.c_str());
        } else {
          pango_font_description_set_family(font, DEFAULT_FONT);
        }

        int width = 0;
        int height = 0;
        int font_size = size;

        PangoLayout* layout = gtk_widget_create_pango_layout(widget, text.c_str());

        do {
          pango_font_description_set_size(font, font_size * PANGO_SCALE);

          pango_layout_set_font_description(layout, font);
          pango_layout_get_pixel_size(layout, &width, &height);

          font_size -= 1;

          if (font_size <= 0) {
            break;
          }
        } while (width > max_width || height > max_height);

        g_object_unref(layout);

        if (font_size > 0) {
          gtk_widget_modify_font(widget, font);
        }

        pango_font_description_free(font);
      }
    }
  }
}

void Utils::combobox_clear(GtkComboBoxText* combobox_text) {
  GtkListStore* store = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(combobox_text)));
  gtk_list_store_clear(store);
}

string Utils::combobox_active_text(GtkComboBoxText* combobox_text) {
  if (combobox_text != NULL && gtk_combo_box_get_active(GTK_COMBO_BOX(combobox_text)) >= 0) {
    return gtk_combo_box_text_get_active_text(combobox_text);
  } else {
    return "";
  }
}

GdkColor* Utils::get_color(const string color_name) {
  if (m_color == NULL) {
    m_color = new GdkColor();
  }

  gdk_color_parse(color_name.c_str(), m_color);

  return m_color;
}

// 返回Combobox的index值, 失败返回-1
int Utils::GetComboBoxIndex(GtkComboBox* combobox, string name) {
  GtkTreeIter iter;
  GtkTreeModel* model = gtk_combo_box_get_model(combobox);

  if (gtk_tree_model_get_iter_first(model, &iter)) {
    char* new_name = NULL;
    gtk_tree_model_get(model, &iter, 0, &new_name, -1);
    string str_name = new_name;

    delete new_name;
    new_name = NULL;

    while (str_name != name) {
      if (gtk_tree_model_iter_next(model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &new_name, -1);
        str_name = new_name;

        delete new_name;
        new_name = NULL;
      } else {
        return -1;
      }
    }

    GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
    string path_str = gtk_tree_path_to_string(path);
    gtk_tree_path_free(path);

    return atoi(path_str.c_str());
  } else {
      return -1;
  }
}

void Utils::SetTheme(const string rc_path) {
  gtk_rc_parse(rc_path.c_str());
  gtk_rc_reparse_all();
  gtk_rc_reset_styles(gtk_settings_get_default());
}

void Utils::GetCurrentDate(int& year, int& month, int& day) {
  time_t now;
  time(&now);

  tm* timeinfo = localtime(&now);

  year = timeinfo->tm_year + 1900;
  month = timeinfo->tm_mon + 1;
  day = timeinfo->tm_mday;
}

void Utils::GetCurrentDateTime(int& year, int& month, int& day, int& hour, int& minute, int& second) {
  time_t now;
  time(&now);

  tm* timeinfo = localtime(&now);

  year = timeinfo->tm_year + 1900;
  month = timeinfo->tm_mon + 1;
  day = timeinfo->tm_mday;

  hour = timeinfo->tm_hour;
  minute = timeinfo->tm_min;
  second = timeinfo->tm_sec;
}

string Utils::CommaToDotLocaleNumeric(string dot) {
  SysGeneralSetting sysGS;
  int language= sysGS.GetLanguage();

  string tmp = dot;

  if (PL == language || RU == language || FR == language || DE == language || ES == language) {
    size_t pos = tmp.find(',');

    if (pos != string::npos) {
      tmp[pos] = '.';
    }
  }

  return tmp;
}

string Utils::DotToCommaLocaleNumeric(string comma) {
  SysGeneralSetting sysGS;
  int language= sysGS.GetLanguage();

  string tmp = comma;

  if (PL == language || RU == language || FR == language || DE == language || ES == language) {
    size_t pos = tmp.find('.');

    if (pos != string::npos) {
      tmp[pos] = ',';
    }
  }

  return tmp;
}

// ---------------------------------------------------------

#include "imageProc/Menu2D.h"

void Utils::test(GtkWidget* widget) {
  //ViewNewPat::GetInstance()->CreateWindow();
}