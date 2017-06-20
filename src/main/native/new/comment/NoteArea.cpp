#include "comment/NoteArea.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include "comment/MenuNote.h"
#include "display/ImageArea.h"
#include "display/KnobNone.h"
#include "display/MenuArea.h"
#include "keyboard/KeyDef.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/MultiFuncFactory.h"
#include "sysMan/SysNoteSetting.h"
#include "ViewMain.h"

enum {
  NORMAL, EDITING, DRAGING, SELECT
};

NoteArea* NoteArea::m_instance = NULL;

// ---------------------------------------------------------

NoteArea* NoteArea::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new NoteArea();
  }

  return m_instance;
}

NoteArea::NoteArea() {
  m_canvas = NULL;

  m_color = NULL;
  m_listItem = NULL;
  m_entry = NULL;

  m_itemDrag = NULL;
  m_itemEntry = NULL;
  m_itemImage = NULL;
  m_itemSel = NULL;

  m_dragX = 0.0;
  m_dragY = 0.0;
  m_itemX = 0.0;
  m_itemY = 0.0;

  GetNoteSetting();
}

NoteArea::~NoteArea() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* NoteArea::Create(const int width, const int height) {
  m_canvas = Utils::create_canvas(width, height);
  goo_canvas_set_bounds(m_canvas, 0, 0, width, height);

  return GTK_WIDGET(m_canvas);
}

void NoteArea::SetNewText(const string text) {
  int x = m_sysCursor.x - CANVAS_AREA_X;
  int y = m_sysCursor.y - CANVAS_AREA_Y;
  int h;

  GetNoteSetting();

  stringstream ss;
  ss << DEFAULT_FONT << ", " << m_sizeFont;

  PangoFontDescription* font = pango_font_description_from_string(ss.str().c_str());

  GtkEntry* entry = CreateEntry(font, m_color);
  PangoLayout *layout = gtk_widget_create_pango_layout(GTK_WIDGET(m_entry), NULL);
  pango_layout_get_pixel_size(layout, NULL, &h);
  g_object_unref(layout);

  if( y + h > CANVAS_AREA_H ) {
    y = CANVAS_AREA_H - h - 3;
  }

  // add new text to m_sysCursor
  GooCanvasItem* item = AddTextItem(text, x, y);

  gtk_widget_destroy(GTK_WIDGET(m_entry));

  // set system cusor
  SetSystemCursor(m_sysCursor.x, m_sysCursor.y);

  // enter drag state
  StartDrag(item);
}

void NoteArea::Enter() {
  if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(m_canvas))) {
    Show();
  }

  g_keyInterface.Push(this);

  GdkCursor* cursor = CustomCursor();
  //GdkCursor *cursor = gdk_cursor_new(GDK_XTERM);
  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
  gdk_cursor_unref(cursor);

  // set system cusor
  m_sysCursor.x = CANVAS_AREA_X + CANVAS_AREA_W / 2;
  m_sysCursor.y = CANVAS_AREA_Y + CANVAS_AREA_H / 2;
  SetSystemCursor(m_sysCursor.x, m_sysCursor.y);

  // show MenuNote
  MenuArea::GetInstance()->ShowNoteMenu();

  // Hide KnobMenu
  KnobNoneCreate();
}

void NoteArea::Exit() {
  if(m_state == EDITING) {
    Undo();
  } else if(m_state == DRAGING) {
    if(m_itemDrag) {
      EndDrag(m_itemDrag);
    }
  }

  m_state = NORMAL;

  g_keyInterface.Pop();

  // restore cursor type
  GdkCursor *cursor = gdk_cursor_new(GDK_LEFT_PTR);
  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
  gdk_cursor_unref(cursor);

  // restore Multi-func status
  MultiFuncUndo();

  // restore previous menu
  MenuShowUndo();
  KnobUndo();
}

void NoteArea::Show() {
  GdkPixbuf *pixbuf = NULL;
  GooCanvasItem *root = goo_canvas_get_root_item(m_canvas);

  pixbuf = ImageArea::GetInstance()->GetDrawingAreaImage();
  m_itemImage = goo_canvas_image_new(root, pixbuf, 0, 0, NULL);
  Focus();
  g_signal_connect(G_OBJECT(m_itemImage), "key-press-event", G_CALLBACK(signal_goocanvas_item_key_press_image), this);

  gtk_widget_show(GTK_WIDGET(m_canvas));

  if(G_IS_OBJECT(pixbuf)) {
    g_object_unref (pixbuf);
  }
}

void NoteArea::Hide() {
  TextInfo info;
  GdkColor *color;

  if(g_list_length(m_listItem) > 0) {
    GList *list = g_list_first(m_listItem);

    while(list) {
      info = (TextInfo)((TextItem*)list->data)->info;

      if(info.color == "red")
        color = Utils::get_color("red");
      else if(info.color == "lime")
        color = Utils::get_color("lime");
      else if(info.color == "blue")
        color = Utils::get_color("blue");
      else if(info.color == "white")
        color = Utils::get_color("white");
      else
        color = Utils::get_color("white");

      ImageArea::GetInstance()->DrawNoteText(info.str.c_str(), info.x-1, info.y-1, color, info.size);
      list = list->next;
    }
  }

  DeleteAllTextItem();

  if(G_IS_OBJECT(m_itemEntry)) {
    goo_canvas_item_remove(m_itemEntry);
  }

  m_itemEntry = NULL;

  if(G_IS_OBJECT(m_itemImage)) {
    goo_canvas_item_remove(m_itemImage);
  }

  m_itemImage = NULL;

  gtk_widget_hide(GTK_WIDGET(m_canvas));
}

void NoteArea::StartEdit(gdouble x, gdouble y) {
  GetNoteSetting();

  stringstream ss;
  ss << DEFAULT_FONT << ", " << m_sizeFont;

  PangoFontDescription *font = pango_font_description_from_string(ss.str().c_str());
  GooCanvasItem *root = goo_canvas_get_root_item(m_canvas);

  m_entry = CreateEntry(font, m_color);

  int h;
  PangoLayout *layout = gtk_widget_create_pango_layout(GTK_WIDGET(m_entry), NULL);
  pango_layout_get_pixel_size(layout, NULL, &h);
  g_object_unref(layout);

  if(m_sizeFont ==0) {
    if (y + h > CANVAS_AREA_H) {
      y = CANVAS_AREA_H - h - 20;
    }
  } else if(m_sizeFont == 1) {
    if (y + h > CANVAS_AREA_H ) {
      y = CANVAS_AREA_H - h - 1;
    }
  } else {
    if (y + h > CANVAS_AREA_H ) {
      y = CANVAS_AREA_H - h + 20;
    }
  }

  m_itemEntry = goo_canvas_widget_new(root, GTK_WIDGET(m_entry), x, y, -1, -1, NULL);
  gtk_widget_grab_focus(GTK_WIDGET(m_entry));
  m_state = EDITING;
}

void NoteArea::EndEdit() {
  if(!m_itemEntry || !m_entry) {
    return;
  }

  gdouble x;
  gdouble y;

  g_object_get(G_OBJECT(m_itemEntry), "x", &x, "y", &y, NULL);
  string str = gtk_entry_get_text(m_entry);

  if(!str.empty()) {
    AddTextItem(str, x-1, y+1);
  }

  if(G_IS_OBJECT(m_itemEntry)) {
    goo_canvas_item_remove(m_itemEntry);
  }

  m_itemEntry = NULL;

  if(G_IS_OBJECT(m_entry)) {
    gtk_widget_destroy(GTK_WIDGET(m_entry));
  }

  m_entry = NULL;

  m_state = NORMAL;
  Focus();
}

void NoteArea::Focus() {
  goo_canvas_grab_focus(m_canvas, m_itemImage);
}

void NoteArea::Undo() {
  if(m_state == NORMAL) {
    if(m_itemSel != NULL) {
      DeleteTextItem(m_itemSel);
      m_itemSel = NULL;

      GdkCursor* cursor = CustomCursor();

      gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
      gdk_cursor_unref(cursor);
    } else {
      // List为空时退出注释功能
      if(g_list_length(m_listItem) == 0) {
        Exit();
      }

      DeleteLastTextItem();
    }
  } else if(m_state == EDITING) {
    if(G_IS_OBJECT(m_itemEntry)) {
      goo_canvas_item_remove(m_itemEntry);
    }

    m_itemEntry = NULL;

    if(G_IS_OBJECT(m_entry)) {
      gtk_widget_destroy(GTK_WIDGET(m_entry));
    }

    m_entry = NULL;

    m_state = NORMAL;
    Focus();
  }
}

// ---------------------------------------------------------

gboolean NoteArea::ItemKeyPressImage(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventKey* event) {
  cout << "keyval = " << event->keyval << ", "
    << "string = " << event->string << ", "
    << "name = " << gdk_keyval_name(event->keyval) << endl;

  if(m_state == NORMAL) {
    StartEdit(m_sysCursor.x - CANVAS_AREA_X, m_sysCursor.y - CANVAS_AREA_Y - 1);

    if(strcmp(gdk_keyval_name(event->keyval), "Shift_L") != 0) {
      if(!FakeAlphabet(g_key_note)) {
        if(!FakeNumKey(g_key_note)) {
          FakePunctuation(g_key_note);  // 符号键
        }
      }
    }

    gtk_editable_set_position(GTK_EDITABLE(m_entry), -1);

    return FALSE;
  }

  return TRUE;
}

gboolean NoteArea::TextItemButtonPress(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventButton* event) {
  if (event->button == 1) { // left button
    if(event->type == GDK_BUTTON_PRESS) {
      if(m_state == NORMAL) {
        m_itemDrag = item;
        m_dragX = event->x;
        m_dragY = event->y;

        g_object_get(G_OBJECT (item), "x", &m_itemX, "y", &m_itemY, NULL);

        GdkCursor* cursor = gdk_cursor_new(GDK_HAND1);
        goo_canvas_pointer_grab(m_canvas, item,
          GdkEventMask(GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_PRESS_MASK),
          cursor, event->time);
        gdk_cursor_unref(cursor);

        m_state = DRAGING;

        return TRUE;
      } else if(m_state == DRAGING) {
        if (m_itemDrag == item && m_itemDrag != NULL) {
          goo_canvas_pointer_ungrab(m_canvas, m_itemDrag, event->time);
          g_object_get (G_OBJECT(item), "x", &m_itemX, "y", &m_itemY, NULL);

          // change the item in GList
          if(g_list_length(m_listItem) > 0) {
            GList *list = g_list_first(m_listItem);
            while(list) {
              if(item == ((TextItem*)list->data)->item) {
                gint pos = g_list_position(m_listItem, list);
                TextInfo pInfo = ((TextInfo)((TextItem*)list->data)->info);
                TextItem *ti = new TextItem;

                ti->info.str = pInfo.str;
                ti->info.x = m_itemX;
                ti->info.y = m_itemY;
                ti->info.size = pInfo.size;
                ti->info.color = pInfo.color;
                ti->item = item;

                delete (TextInfo*)&((((TextItem*)list->data)->info));
                m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
                m_listItem = g_list_insert(m_listItem, ti, pos);

                break;
              }

              list = list->next;
            }
          }

          m_itemDrag = NULL;
          m_state = NORMAL;
          Focus();

          return TRUE;
        }
      }
    }
  } else if(event->button == 3) { // right button
  }

  return FALSE;
}

gboolean NoteArea::TextItemNotifyMotion(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventMotion* event) {
  if (m_itemDrag == item && m_itemDrag != NULL) {
    gdouble rel_x = event->x - m_dragX;
    gdouble rel_y = event->y - m_dragY;

    g_object_set (G_OBJECT (item), "x", m_itemX + rel_x, "y", m_itemY + rel_y, NULL);

    return TRUE;
  }

  return FALSE;
}

gboolean NoteArea::TextItemNotifyEnter(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventCrossing* event) {
  GdkCursor* cursor = gdk_cursor_new(GDK_HAND2);
  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
  gdk_cursor_unref(cursor);

  m_itemSel = item;
  return TRUE;
}

gboolean NoteArea::TextItemNotifyLeave(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventCrossing* event) {
  GdkCursor* cursor = CustomCursor();
  gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
  gdk_cursor_unref(cursor);

  m_itemSel = NULL;
  return TRUE;
}

void NoteArea::EntryChanged(GtkEntry* entry) {
  int w;
  PangoLayout *layout = gtk_entry_get_layout(entry);
  pango_layout_get_pixel_size(layout, &w, NULL);
  g_object_set(G_OBJECT(m_itemEntry), "width", MAX(double(w), 10.0), NULL);
}

void NoteArea::StartDrag(GooCanvasItem* item) {
  m_state = NORMAL;

  GdkEventButton* event = (GdkEventButton*)gdk_event_new(GDK_BUTTON_PRESS);
  event->type = GDK_BUTTON_PRESS;
  event->button = 1;	 //left button
  event->x = m_sysCursor.x - CANVAS_AREA_X + 0.5;
  event->y = m_sysCursor.y - CANVAS_AREA_Y + 0.5;
  event->time = GDK_CURRENT_TIME;

  TextItemButtonPress(item, item, event);
  gdk_event_free((GdkEvent*)(event));
}

void NoteArea::EndDrag(GooCanvasItem* item) {
  m_state = DRAGING;

  GdkEventButton *event = (GdkEventButton*)gdk_event_new(GDK_BUTTON_PRESS);
  event->type = GDK_BUTTON_PRESS;
  event->button = 1;  // left button
  event->time = GDK_CURRENT_TIME;

  TextItemButtonPress(item, item, event);
  gdk_event_free((GdkEvent*)(event));
}

void NoteArea::KeyEvent(unsigned char keyValue) {
  if(keyValue==KEY_SHIFT_CTRL) {
    KeySwitchIM ksim;
    ksim.ExcuteChange(TRUE);
    return;
  }

  if(FakeMouseButton(keyValue))
    return;

  if(FakeNumKey(keyValue))
    return;

  if(FakeAlphabet(keyValue))
    return;

  if(FakePunctuation(keyValue))
    return;

  switch(keyValue) {
  case KEY_MENU:
    break;
  case KEY_ESC:
    Undo();
    break;
  case KEY_TEXT:
    Exit();
    break;
  case KEY_CURSOR:
    PressCursor();
    break;
  case KEY_CLEAR:
    {
      if(m_state == NORMAL) {
        DeleteAllTextItem();

        Hide();
        KeyClearScreen kcs;
        kcs.Execute();
        gdk_threads_enter();

        while(gtk_events_pending()) {
          gtk_main_iteration();
        }

        gdk_threads_leave();
        Show();
        GdkCursor *cursor = CustomCursor();
        gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(m_canvas)), cursor);
        gdk_cursor_unref(cursor);
      }
    }

    break;
  case KEY_SHIFT_ALT:
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
    break;
  case KEY_SHIFT_SPACE:
    break;
  case KEY_F1ADD:
  case KEY_F1SUB:
  case KEY_F2ADD:
  case KEY_F2SUB:
  case KEY_F3ADD:
  case KEY_F3SUB:
  case KEY_F4ADD:
  case KEY_F4SUB:
  case KEY_F5ADD:
  case KEY_F5SUB:
    break;
  default:
    Exit();
    ViewMain::GetInstance()->KeyEvent(keyValue);
    break;
  }
}

void NoteArea::MouseEvent(char offsetX, char offsetY) {
  if (m_state == SELECT) {
    FakeXMotionEventMenu(m_menuCursor.x, m_menuCursor.y, offsetX, offsetY);
  } else {
    FakeXMotionEventImage(m_sysCursor.x, m_sysCursor.y, offsetX, offsetY);

    if(m_state == EDITING) {
      EndEdit();
    }
  }
}

void NoteArea::PressCursor() {
  if (m_state == NORMAL) {
    m_state = SELECT;
    m_menuCursor.x = SYSCURSOR_X;
    m_menuCursor.y = SYSCUROSR_Y;
    SetSystemCursor(m_menuCursor.x, m_menuCursor.y);
    g_menuNote.Focus();
  } else if(m_state == SELECT) {
    m_state = NORMAL;
    SetSystemCursor(m_sysCursor.x, m_sysCursor.y);
    Focus();
  }
}

void NoteArea::DeleteAllTextItem() {
  if(g_list_length(m_listItem) > 0) {
    m_listItem = g_list_first(m_listItem);

    while(m_listItem) {
      goo_canvas_item_remove((GooCanvasItem*)((TextItem*)m_listItem->data)->item);
      delete (TextInfo*)(&(((TextItem*)m_listItem->data)->info));
      m_listItem = g_list_remove(m_listItem, (TextItem*)m_listItem->data);
    }
  }
}

void NoteArea::DeleteFirstTextItem() {
  GList *list = g_list_first(m_listItem);
  if(list) {
    goo_canvas_item_remove((GooCanvasItem*)((TextItem*)list->data)->item);
    delete (TextInfo*)&((((TextItem*)list->data)->info));
    m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
  }
}

void NoteArea::DeleteLastTextItem() {
  GList *list = g_list_last(m_listItem);
  if(list) {
    goo_canvas_item_remove((GooCanvasItem*)((TextItem*)list->data)->item);
    delete (TextInfo*)&((((TextItem*)list->data)->info));
    m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);
  }
}

void NoteArea::DeleteTextItem(GooCanvasItem* item) {
  if(g_list_length(m_listItem) > 0) {
    GList *list = g_list_first(m_listItem);
    while(list) {
      if((GooCanvasItem*)((TextItem*)list->data)->item == item) {
        goo_canvas_item_remove(item);
        delete (TextInfo*)&((((TextItem*)list->data)->info));
        m_listItem = g_list_remove(m_listItem, (TextItem*)list->data);

        break;
      }

      list = list->next;
    }
  }
}

void NoteArea::ListAllTextItem(GList* list) {
  if(g_list_length(list) > 0) {
    cout << "length = " << g_list_length(list) << endl;
    list = g_list_first(list);

    while(list) {
      cout << "string=" << ((TextInfo)((TextItem*)list->data)->info).str << endl;
      list = list->next;
    }
  } else {
    cout << "List is NULL" << endl;
  }
}

GooCanvasItem* NoteArea::AddTextItem(const string text, gdouble x, gdouble y) {
  char font_desc[100];

  GetNoteSetting();

  stringstream ss;
  ss << DEFAULT_FONT << ", " << m_sizeFont;

  PangoFontDescription *font = pango_font_description_from_string(ss.str().c_str());
  GooCanvasItem* root = goo_canvas_get_root_item(m_canvas);

  GooCanvasItem* item = goo_canvas_text_new(root, text.c_str(), x, y, -1,
    GTK_ANCHOR_NW,
    "font-desc", font,
    "fill-color", m_strColor.c_str(),
    NULL);

  SetupTextItemHandle(item);

  TextItem* ti = new TextItem();

  ti->info.str = text;
  ti->info.x = x;
  ti->info.y = y;
  ti->info.size = m_sizeFont;
  ti->info.color = m_strColor;
  ti->item = item;
  m_listItem = g_list_append(m_listItem, ti);

  return item;
}

void NoteArea::GetNoteSetting() {
  SysNoteSetting sns;

  switch(sns.GetFontSize()) {
  case 0:
    m_sizeFont = 32;
    break;
  case 1:
    m_sizeFont = 18;
    break;
  case 2:
    m_sizeFont = 12;
    break;
  default:
    m_sizeFont = 18;
    break;
  }

  switch(sns.GetFontColor()) {
  case 0:
    m_strColor = "red";
    break;
  case 1:
    m_strColor = "lime";
    break;
  case 2:
    m_strColor = "blue";
    break;
  case 3:
    m_strColor = "white";
    break;
  default:
    m_strColor = "red";
    break;
  }

  m_color = Utils::get_color(m_strColor);
}

void NoteArea::SetupTextItemHandle(GooCanvasItem* item) {
  g_signal_connect(G_OBJECT(item), "button-press-event", G_CALLBACK(signal_goocanvas_text_item_button_press), this);
  g_signal_connect(G_OBJECT(item), "motion-notify-event", G_CALLBACK(signal_goocanvas_text_item_notify_motion), this);
  g_signal_connect(G_OBJECT(item), "enter-notify-event", G_CALLBACK(signal_goocanvas_text_item_notify_enter), this);
  g_signal_connect(G_OBJECT(item), "leave-notify-event", G_CALLBACK(signal_goocanvas_text_item_notify_leave), this);
}

GtkEntry* NoteArea::CreateEntry(PangoFontDescription* font, GdkColor* color) {
  GtkEntry* entry = Utils::create_entry();

  g_object_set(G_OBJECT(entry), "has-frame", FALSE, NULL);
  gtk_widget_set_size_request(GTK_WIDGET(entry), 10, -1);
  gtk_widget_modify_font(GTK_WIDGET(entry), font);
  gtk_widget_modify_text(GTK_WIDGET(entry), GTK_STATE_NORMAL, m_color);

  g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(signal_entry_changed), this);

  return entry;
}

GdkCursor* NoteArea::CustomCursor() {
  GetNoteSetting();

  string logo_file;

  if (m_strColor == "red") {
    logo_file = string(CFG_RES_PATH) + string("res/icon/red.png");
  } else if (m_strColor == "blue") {
    logo_file = string(CFG_RES_PATH) + string("res/icon/blue.png");
  } else if (m_strColor == "lime") {
    logo_file = string(CFG_RES_PATH) + string("res/icon/green.png");
  } else {
    logo_file = string(CFG_RES_PATH) + string("res/icon/white.png");
  }

  GError* error = NULL;
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(logo_file.c_str(), &error);

  int width = 0;
  int height = 0;

  if(m_sizeFont==32) {
    width = gdk_pixbuf_get_width(pixbuf) *1.4;
    height = gdk_pixbuf_get_height(pixbuf) * 1.5;
  } else if(m_sizeFont==12) {
    width = gdk_pixbuf_get_width(pixbuf) *1.0;
    height = gdk_pixbuf_get_height(pixbuf) * 0.6;
  } else {
    width = gdk_pixbuf_get_width(pixbuf) *1.0;
    height = gdk_pixbuf_get_height(pixbuf) * 0.9;
  }

  GdkPixbuf* pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_NEAREST);
  GdkDisplay* display = gdk_display_get_default();

  GdkCursor* cursor = gdk_cursor_new_from_pixbuf(display, pixbuf_scale,2,2);
  return cursor;
}
