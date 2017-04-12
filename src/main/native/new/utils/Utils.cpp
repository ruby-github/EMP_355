#include "utils/Utils.h"

#include <sstream>

using namespace std;

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

  gtk_widget_modify_bg(GTK_WIDGET(window), GTK_STATE_NORMAL, get_color("DimGrey"));

  return window;
}

GtkDialog* Utils::create_dialog(GtkWindow* parent, const string title, const int width, const int height) {
  GtkDialog* dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(title.c_str(), parent, GTK_DIALOG_MODAL, NULL));

  gtk_widget_set_size_request(GTK_WIDGET(dialog), width, height);

  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
  gtk_dialog_set_has_separator(dialog, TRUE);

  gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);

  gtk_widget_modify_bg(GTK_WIDGET(dialog), GTK_STATE_NORMAL, get_color("DimGrey"));

  return dialog;
}

GtkButton* Utils::add_dialog_button(GtkDialog* dialog, const string label, const GtkResponseType type, const string stock_id) {
  GtkButton* button = GTK_BUTTON(gtk_dialog_add_button(dialog, label.c_str(), type));

  if (stock_id.empty()) {
    GtkSettings* settings = gtk_widget_get_settings(GTK_WIDGET(button));
    g_object_set(settings, "gtk-button-images", TRUE, NULL);

    GtkWidget* image = gtk_image_new_from_stock(stock_id.c_str(), GTK_ICON_SIZE_BUTTON);

    gtk_button_set_image(button, image);
    gtk_button_set_image_position(button, GTK_POS_LEFT);
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 12);
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

  set_font(GTK_WIDGET(label), "", "", 12);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

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

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 12);
  gtk_button_set_alignment(button, 0.5, 0.5);

  /*
    GTK_STATE_NORMAL      // 正常状态
    GTK_STATE_ACTIVE      // 获得焦点时的状态
    GTK_STATE_PRELIGHT    // 鼠标移动到上方时的状态
    GTK_STATE_SELECTED    // 被选中时的状态
    GTK_STATE_INSENSITIVE // 失去功能时的状态
  */

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
    GtkSettings* settings = gtk_widget_get_settings(GTK_WIDGET(button));
    g_object_set(settings, "gtk-button-images", TRUE, NULL);

    gtk_button_set_image(button, GTK_WIDGET(create_image(filename, width, height)));
    gtk_button_set_image_position(button, GTK_POS_LEFT);
  }

  set_font(gtk_bin_get_child(GTK_BIN(button)), "", "", 12);
  gtk_button_set_alignment(button, 0.5, 0.5);

  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_ACTIVE, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_PRELIGHT, get_color("green"));
  gtk_widget_modify_bg(GTK_WIDGET(button), GTK_STATE_SELECTED, get_color("green"));

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_ACTIVE, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_PRELIGHT, get_color("white"));
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(button)), GTK_STATE_SELECTED, get_color("white"));

  return button;
}

GtkEntry* Utils::create_entry() {
  GtkEntry* entry = GTK_ENTRY(gtk_entry_new());

  set_font(GTK_WIDGET(entry), "", "", 12);

  return entry;
}

GtkComboBox* Utils::create_combobox() {
  GtkComboBox* combobox = GTK_COMBO_BOX(gtk_combo_box_new());

  set_font(GTK_WIDGET(combobox), "", "", 12);

  return combobox;
}

GtkNotebook* Utils::create_notebook() {
  GtkNotebook* notebook = GTK_NOTEBOOK(gtk_notebook_new());

  gtk_notebook_set_scrollable(notebook, TRUE);
  gtk_container_set_border_width(GTK_CONTAINER(notebook), 0);

  gtk_widget_modify_bg(GTK_WIDGET(notebook), GTK_STATE_NORMAL, get_color("gray"));

  return notebook;
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
    pango_font_description_set_family(font, "WenQuanYi Zen Hei");
  }

  if (size > 0) {
    pango_font_description_set_size(font, size * PANGO_SCALE);
  } else {
    pango_font_description_set_size(font, 12 * PANGO_SCALE);
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
          pango_font_description_set_family(font, "WenQuanYi Zen Hei");
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

void Utils::set_image(GtkImage* image, const string filename) {
  if (filename.empty()) {
    gtk_image_set_from_pixbuf(image, NULL);
  } else {
    gtk_image_set_from_file(image, filename.c_str());
  }
}

GdkColor* Utils::get_color(const string color_name) {
  if (m_color == NULL) {
    m_color = new GdkColor();
  }

  gdk_color_parse(color_name.c_str(), m_color);

  return m_color;
}

// ---------------------------------------------------------

#include "sysMan/CalcSetting.h"

void Utils::test(GtkWidget* widget) {
  GtkWidget* win = CustomCalc::GetInstance()->CreateCalcSettingWin(widget);

  gtk_widget_show_all(win);
}