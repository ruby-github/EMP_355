#ifndef __NOTE_AREA_H__
#define __NOTE_AREA_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

#include "display/ImageArea.h"

class NoteArea: public FakeXEvent {
public:
  static NoteArea* GetInstance();

public:
  ~NoteArea();

  GtkWidget* Create();

  void SetNewText(const string text);
  void Enter();
  void Exit();
  void Show();
  void Hide();
  void StartEdit(gdouble x, gdouble y);
  void EndEdit();
  void Focus();
  void Undo();

public:
  struct TextInfo {
    string str;
    gdouble x;
    gdouble y;
    guint size;
    string color;
  };

  struct TextItem {
    TextInfo info;
    GooCanvasItem *item;
  };

private:
  // signal

  static gboolean signal_goocanvas_item_key_press_image(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventKey *event, NoteArea *data) {
    if (data != NULL) {
      return data->ItemKeyPressImage(item, target_item, event);
    }

    return FALSE;
  }

  static gboolean signal_goocanvas_text_item_button_press(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventButton *event, NoteArea *data) {
    if (data != NULL) {
      return data->TextItemButtonPress(item, target_item, event);
    }

    return FALSE;
  }

  static gboolean signal_goocanvas_text_item_notify_motion(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventMotion *event, NoteArea *data) {
    if (data != NULL) {
      return data->TextItemNotifyMotion(item, target_item, event);
    }

    return FALSE;
  }

  static gboolean signal_goocanvas_text_item_notify_enter(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventCrossing *event, NoteArea *data) {
    if (data != NULL) {
      return data->TextItemNotifyEnter(item, target_item, event);
    }

    return FALSE;
  }

  static gboolean signal_goocanvas_text_item_notify_leave(GooCanvasItem *item, GooCanvasItem *target_item, GdkEventCrossing *event, NoteArea *data) {
    if (data != NULL) {
      return data->TextItemNotifyLeave(item, target_item, event);
    }

    return FALSE;
  }

  static void signal_entry_changed(GtkEntry *entry, NoteArea *data) {
    if (data != NULL) {
      data->EntryChanged(entry);
    }
  }

  // signal

  void EntryChanged(GtkEntry* entry);

  gboolean ItemKeyPressImage(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventKey* event);
  gboolean TextItemButtonPress(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventButton* event);
  gboolean TextItemNotifyMotion(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventMotion* event);
  gboolean TextItemNotifyEnter(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventCrossing* event);
  gboolean TextItemNotifyLeave(GooCanvasItem* item, GooCanvasItem* target_item, GdkEventCrossing* event);

private:
  NoteArea();

  void StartDrag(GooCanvasItem* item);
  void EndDrag(GooCanvasItem* item);

  void KeyEvent(unsigned char keyValue);
  void MouseEvent(char offsetX, char offsetY);
  void PressCursor();

  void DeleteTextItem(GooCanvasItem* item);
  void DeleteFirstTextItem();
  void DeleteLastTextItem();
  void DeleteAllTextItem();
  void ListAllTextItem(GList* list);
  GooCanvasItem* AddTextItem(const string text, gdouble x, gdouble y);

  void GetNoteSetting();
  void SetupTextItemHandle(GooCanvasItem* item);

  GtkEntry* CreateEntry(PangoFontDescription* font, GdkColor* color);
  GdkCursor* CustomCursor();

private:
  static NoteArea* m_instance;

private:
  GooCanvas* m_canvas;
  ImageArea* m_ptrImgArea;

  GdkColor* m_color;
  GList* m_listItem;
  GtkEntry* m_entry;

  GooCanvasItem* m_itemDrag;
  GooCanvasItem* m_itemEntry;
  GooCanvasItem* m_itemImage;
  GooCanvasItem* m_itemSel;

  gdouble m_dragX;
  gdouble m_dragY;
  gdouble m_itemX;
  gdouble m_itemY;

  POINT m_menuCursor;
  POINT m_sysCursor;

  int m_sizeFont;
  guint m_state;
  string m_strColor;
};

#endif
