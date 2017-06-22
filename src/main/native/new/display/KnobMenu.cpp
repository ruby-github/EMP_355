#include "display/KnobMenu.h"

#include "imageProc/GlobalClassMan.h"
#include "utils/StringUtils.h"

KnobMenu* KnobMenu::m_instance = NULL;

// ---------------------------------------------------------

KnobMenu* KnobMenu::GetInstance() {
  if (m_instance == NULL) {
    m_instance  = new KnobMenu();
  }

  return m_instance;
}

KnobMenu::KnobMenu() {
  m_button_left = NULL;
  m_button_right = NULL;
  m_button_knob[KNOB_NUM] = {NULL};
  m_label_knob[KNOB_NUM] = {NULL};

  m_KnobItem = NULL;

  m_knobType = D2;
}

KnobMenu::~KnobMenu() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* KnobMenu::Create() {
  GtkTable* table = Utils::create_table(5, 14);

  gtk_container_set_border_width(GTK_CONTAINER(table), 1);
  gtk_table_set_row_spacings(table, 1);
  gtk_table_set_col_spacings(table, 1);

  m_label_knob[0] = Utils::create_label();
  m_label_knob[1] = Utils::create_label();
  m_label_knob[2] = Utils::create_label();
  m_label_knob[3] = Utils::create_label();
  m_label_knob[4] = Utils::create_label();
  m_label_knob[5] = Utils::create_label();

  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[0]), 1, 3, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[1]), 3, 5, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[2]), 5, 7, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[3]), 7, 9, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[4]), 9, 11, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_knob[5]), 11, 13, 0, 1);

  gtk_misc_set_alignment(GTK_MISC(m_label_knob[0]), 0.5, 0.5);
  gtk_misc_set_alignment(GTK_MISC(m_label_knob[1]), 0.5, 0.5);
  gtk_misc_set_alignment(GTK_MISC(m_label_knob[2]), 0.5, 0.5);
  gtk_misc_set_alignment(GTK_MISC(m_label_knob[3]), 0.5, 0.5);
  gtk_misc_set_alignment(GTK_MISC(m_label_knob[4]), 0.5, 0.5);
  gtk_misc_set_alignment(GTK_MISC(m_label_knob[5]), 0.5, 0.5);

  m_button_left = Utils::create_button("<<");
  m_button_knob[0] = Utils::create_button("knob1");
  m_button_knob[1] = Utils::create_button("knob2");
  m_button_knob[2] = Utils::create_button("knob3");
  m_button_knob[3] = Utils::create_button("knob4");
  m_button_knob[4] = Utils::create_button("knob5");
  m_button_knob[5] = Utils::create_button("knob6");
  m_button_right = Utils::create_button(">>");

  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_left), 0, 1, 0, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[0]), 1, 3, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[1]), 3, 5, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[2]), 5, 7, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[3]), 7, 9, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[4]), 9, 11, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_knob[5]), 11, 13, 1, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_button_right), 13, 14, 0, 5);

  Utils::set_button_image(m_button_knob[0], Utils::create_image("res/menu/menu1.png", 70, 60), GTK_POS_BOTTOM);
  Utils::set_button_image(m_button_knob[1], Utils::create_image("res/menu/menu2.png", 70, 60), GTK_POS_BOTTOM);
  Utils::set_button_image(m_button_knob[2], Utils::create_image("res/menu/menu3.png", 70, 60), GTK_POS_BOTTOM);
  Utils::set_button_image(m_button_knob[3], Utils::create_image("res/menu/menu4.png", 70, 60), GTK_POS_BOTTOM);
  Utils::set_button_image(m_button_knob[4], Utils::create_image("res/menu/menu5.png", 70, 60), GTK_POS_BOTTOM);
  Utils::set_button_image(m_button_knob[5], Utils::create_image("res/menu/menu6.png", 70, 60), GTK_POS_BOTTOM);

  GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(m_button_left), GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(m_button_right), GTK_CAN_FOCUS);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_left), GTK_STATE_NORMAL, Utils::get_color("black"));
  gtk_widget_modify_bg(GTK_WIDGET(m_button_right), GTK_STATE_NORMAL, Utils::get_color("black"));

  for (int i = 0; i < KNOB_NUM; i++) {
    GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(m_button_knob[i]), GTK_CAN_FOCUS);
    gtk_widget_modify_bg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("black"));
  }

  return GTK_WIDGET(table);
}

void KnobMenu::SetValue(int index, const string value, EKnobReturn status) {
  m_KnobItem[index].value = value;
  m_KnobItem[index].status = status;

  Update();
}

void KnobMenu::SetItem(KnobItem* item, unsigned int total_item, EKnobType type) {
  m_KnobItem = item;

  if (total_item % KNOB_NUM == 0)
    m_MaxLevel = (total_item / KNOB_NUM) - 1;
  else
    m_MaxLevel = total_item / KNOB_NUM;

  m_CurLevel = 0;
  m_knobType = type;
  Update();
}

void KnobMenu::Update() {
  string tmp;

  for (int i = 0; i < KNOB_NUM; i ++) {
    if (m_KnobItem[m_CurLevel * KNOB_NUM + i].status == MIN ||
      m_KnobItem[m_CurLevel * KNOB_NUM + i].status == MAX ||
      m_KnobItem[m_CurLevel * KNOB_NUM + i].status == OK ||
      m_KnobItem[m_CurLevel * KNOB_NUM + i].status == PRESS ||
      m_KnobItem[m_CurLevel * KNOB_NUM + i].status == ERROR) {
      string name = m_KnobItem[m_CurLevel * KNOB_NUM + i].name;
      string value = m_KnobItem[m_CurLevel * KNOB_NUM + i].value;

      if (m_KnobItem[m_CurLevel * KNOB_NUM + i].status == PRESS) {
        value = "【 " + value + " 】";
      }

      if (name.empty()) {
        gtk_label_set_text(m_label_knob[i], "");
        gtk_button_set_label(m_button_knob[i], "");
      } else {
        gtk_label_set_text(m_label_knob[i], name.c_str());
        gtk_button_set_label(m_button_knob[i], value.c_str());
      }
    } else {
      gtk_label_set_text(m_label_knob[i], "");
      gtk_button_set_label(m_button_knob[i], "");
    }
  }

  if (m_CurLevel == 0 && m_MaxLevel == 0) {
    gtk_button_set_label(m_button_left, "");
    gtk_button_set_label(m_button_right, "");
  } else if (m_CurLevel == 0 && m_MaxLevel > 0) {
    gtk_button_set_label(m_button_left, "");
    gtk_button_set_label(m_button_right, ">>");
  } else if (m_CurLevel > 0 && m_CurLevel == m_MaxLevel) {
    gtk_button_set_label(m_button_left, "<<");
    gtk_button_set_label(m_button_right, "");
  } else if (m_CurLevel > 0 && m_CurLevel < m_MaxLevel) {
    gtk_button_set_label(m_button_left, "<<");
    gtk_button_set_label(m_button_right, ">>");
  }

  Refresh();
}

void KnobMenu::PageUp() {
  if (m_CurLevel > 0) {
    --m_CurLevel;
  }

  Update();
}

void KnobMenu::PageDown() {
  if (m_CurLevel < m_MaxLevel) {
    ++m_CurLevel;
  }

  Update();
}

void KnobMenu::Knob1_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 0;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob2_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 1;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob3_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 2;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob4_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 3;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob5_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 4;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob6_Press() {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 5;

  if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  (*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob1_Screw(int cw) {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 0;

  if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
  } else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
  }
}

void KnobMenu::Knob2_Screw(int cw) {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 1;

  if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
  } else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
  }
}

void KnobMenu::Knob3_Screw(int cw) {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 2;

  if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
  } else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
  }
}

void KnobMenu::Knob4_Screw(int cw) {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 3;

  if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
  } else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
  }
}

void KnobMenu::Knob5_Screw(int cw) {
  int index = 0;
  index = KNOB_NUM * m_CurLevel + 4;

  if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR) {
    return;
  }

  if (cw == 1) {
    (*(m_KnobItem[index].pf))(ADD);
  } else if (cw == 0) {
    (*(m_KnobItem[index].pf))(SUB);
  }
}

// ---------------------------------------------------------

void KnobMenu::Refresh() {
  FormatCfm::EFormatCfm formatCfm = FormatCfm::GetInstance()->GetFormat();
  Format2D::EFormat2D format2D = Format2D::GetInstance()->GetFormat();

  DSCCONTROLATTRIBUTES* m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
  bool cfmIsDirection = m_ptrDscPara->dcaCFMIsDirection;

  gtk_widget_queue_draw(GTK_WIDGET(m_button_left));
  gtk_widget_queue_draw(GTK_WIDGET(m_button_right));

  char type = GlobalClassMan::GetInstance()->GetProbeType();

  for (int i = 0; i < KNOB_NUM; i ++) {
    gtk_widget_queue_draw(GTK_WIDGET(m_button_knob[i]));

    gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("white"));

    if (type == 'L' || type == 'l') {
      if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "Scan Angle") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      } else if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "EFVI") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      } else {
      }
    } else {
      if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "Steer") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      } else if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "TP-View") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      } else {
      }
    }

    if (type != 'P' && type != 'p') {
      if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "Scan Line") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      }
    } else {
    }

    if (!cfmIsDirection && (ScanMode::GetInstance()->GetScanMode() != ScanMode::PWPDI)) {
      if(m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "Baseline") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      }
    }

    if (ScanMode::GetInstance()->GetScanMode() == ScanMode::CFM
      || ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI
      || ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI_VS_2D) {
      if (m_KnobItem[m_CurLevel * KNOB_NUM + i].name == "Simult") {
        gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
      }
    }

    if (m_KnobItem[m_CurLevel * KNOB_NUM + i].status == PRESS) {
      gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("blue"));
    }

    if (m_KnobItem[m_CurLevel * KNOB_NUM + i].status == ERROR) {
      gtk_widget_modify_fg(GTK_WIDGET(m_button_knob[i]), GTK_STATE_NORMAL, Utils::get_color("DimGrey"));
    }
  }
}
