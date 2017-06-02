#include "bodymark/MenuBDMK.h"

#include "bodymark/MultiFuncBodyMark.h"
#include "probe/ExamItem.h"
#include "probe/ProbeSelect.h"

#define BDMK_PATH "res/bdmk_scaled"

const string EXAM_ITEM[] = {
  N_("Abdomen"),
  N_("Obstetrics"),
  N_("Gynecology"),
  N_("Urology"),
  N_("Cardiac"),
  N_("Small Part"),
  N_("Vascular"),
  N_("Orthopedic"),
  N_("EXTRA")
};

MenuBDMK g_menuBDMK;

// ---------------------------------------------------------

MenuBDMK::MenuBDMK() {
  m_table = NULL;
  m_combobox_text = NULL;
  m_button_left = NULL;
  m_button_right = NULL;
  m_label_page = NULL;

  m_button_images[MAX_IMAGE] = {0};

  m_numTotal = 0;
  m_pageCur = 0;
  m_study = 0;
  m_dir = NULL;
  m_pixbufSel = NULL;
}

MenuBDMK::~MenuBDMK() {
}

GtkWidget* MenuBDMK::Create() {
  m_table = Utils::create_table(12, 1);

  // combobox_text

  m_combobox_text = Utils::create_combobox_text();
  gtk_table_attach_defaults(m_table, GTK_WIDGET(m_combobox_text), 0, 1, 0, 1);

  int size = sizeof(EXAM_ITEM) / sizeof(EXAM_ITEM[0]);

  for (int i = 0; i < size; i++) {
    gtk_combo_box_text_append_text(m_combobox_text, EXAM_ITEM[i].c_str());
  }

  g_signal_connect(G_OBJECT(m_combobox_text), "changed", G_CALLBACK(signal_combobox_changed), this);

  // image table

  GtkTable* table_image = Utils::create_table(MAX_ROWS, MAX_IMAGE / MAX_ROWS);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(table_image), 0, 1, 1, 11);

  int index = 0;

  for (int i = 0; i < MAX_ROWS; i++) {
    for (int j = 0; j < MAX_IMAGE/MAX_ROWS; j++) {
      m_button_images[index] = Utils::create_button();

      gtk_table_attach_defaults(table_image, GTK_WIDGET(m_button_images[index]), j, j + 1, i, i + 1);

      g_signal_connect(G_OBJECT(m_button_images[index]), "clicked", G_CALLBACK(signal_button_clicked_bdmk), this);
      g_object_set_data(G_OBJECT(m_button_images[index]), "SNO", GINT_TO_POINTER(index));

      index += 1;
    }
  }

  // hbox

  GtkBox* hbox = Utils::create_hbox(TRUE, 0);
  gtk_table_attach_defaults(m_table, GTK_WIDGET(hbox), 0, 1, 11, 12);

  m_button_left = Utils::create_button("◀");
  gtk_box_pack_start(hbox, GTK_WIDGET(m_button_left), FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(m_button_left), "clicked", G_CALLBACK(signal_button_clicked_left), this);

  m_label_page = Utils::create_label("[ 0 / 0 ]");
  gtk_box_pack_start(hbox, GTK_WIDGET(m_label_page), TRUE, TRUE, 0);

  m_button_right = Utils::create_button("◀");
  gtk_box_pack_start(hbox, GTK_WIDGET(m_button_right), FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(m_button_right), "clicked", G_CALLBACK(signal_button_clicked_right), this);

  return GTK_WIDGET(m_table);
}

void MenuBDMK::Hide() {
  if(m_dir != NULL) {
    g_dir_close(m_dir);
  }

  m_dir = NULL;

  if (m_table != NULL) {
    gtk_widget_hide_all(GTK_WIDGET(m_table));
  }
}

void MenuBDMK::Show() {
  if (m_table != NULL) {
    gtk_widget_show_all(GTK_WIDGET(m_table));

    // 根据不同的探头部位设置不同科别
    guchar study;
    ExamItem::EItem item = ProbeSelect::GetItemIndex();

    switch(item) {
    case ExamItem::ABDO_ADULT:
    case ExamItem::ABDO_LIVER:
    case ExamItem::ABDO_KID:
    case ExamItem::USER1:
    case ExamItem::USER2:
    case ExamItem::USER3:
    case ExamItem::USER4:
      study = ABD;
      break;
    case ExamItem::EARLY_PREG:
    case ExamItem::LATER_PREG:
      study = OB;
      break;
    case ExamItem::GYN:
      study = GYN;
      break;
    case ExamItem::KIDNEY:
    case ExamItem::BLADDER:
      study = UR;
      break;
    case ExamItem::CAR_KID:
    case ExamItem::CAR_ADULT:
    case ExamItem::CAR_FETUS:
      study = CAR;
      break;
    case ExamItem::SMALL_PART:
    case ExamItem::GLANDS:
    case ExamItem::THYROID:
    case ExamItem::EYE:
      study = SP;
      break;
    case ExamItem::CAROTID:
    case ExamItem::JUGULAR:
    case ExamItem::PERI_ARTERY:
    case ExamItem::PERI_VEIN:
      study = VES;
      break;
    case ExamItem::HIP_JOINT:
    case ExamItem::JOINT_CAVITY:
    case ExamItem::MENISCUS:
    case ExamItem::SPINE:
      study = ORT;
      break;
    default:
      study = ABD;
      break;
    }

    ChangeBDMKStudy(study);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text), study);
  }
}

// ---------------------------------------------------------

void MenuBDMK::ComboBoxChanged(GtkComboBox* combobox) {
  if (gtk_combo_box_get_active(combobox) >= 0) {
    string text = gtk_combo_box_get_active_text(combobox);

    guchar study = 0;

    if (text == EXAM_ITEM[0]) {
      study = ABD;
    } else if (text == EXAM_ITEM[1]) {
      study = OB;
    } else if (text == EXAM_ITEM[2]) {
      study = GYN;
    } else if (text == EXAM_ITEM[3]) {
      study = UR;
    } else if (text == EXAM_ITEM[4]) {
      study = CAR;
    } else if (text == EXAM_ITEM[5]) {
      study = SP;
    } else if (text == EXAM_ITEM[6]) {
      study = VES;
    } else if (text == EXAM_ITEM[7]) {
      study = ORT;
    } else if (text == EXAM_ITEM[8]) {
      study = EXTRA;
    } else {
      study = ABD;
    }

    ChangeBDMKStudy(study);
  }
}

void MenuBDMK::ButtonClickedBDMK(GtkButton* button) {
  string path;

  switch(m_study) {
  case ABD:
    path = GetBdmkPath("ABD");
    break;
  case OB:
    path = GetBdmkPath("OB");
    break;
  case GYN:
    path = GetBdmkPath("GYN");
    break;
  case UR:
    path = GetBdmkPath("UR");
    break;
  case CAR:
    path = GetBdmkPath("CAR");
    break;
  case SP:
    path = GetBdmkPath("SP");
    break;
  case VES:
    path = GetBdmkPath("VES");
    break;
  case ORT:
    path = GetBdmkPath("ORT");
    break;
  case EXTRA:
    path = GetBdmkPath("EXTRA");
    break;
  default:
    path = GetBdmkPath("ABD");
    break;
  }

  int no = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "SNO"));

  if (m_vecName.size() > no + (m_pageCur-1)*MAX_IMAGE) {
    string abspath = g_build_filename(path.c_str(), m_vecName[no + (m_pageCur-1)*MAX_IMAGE].c_str(), NULL);

    if(!g_file_test(abspath.c_str(), G_FILE_TEST_IS_DIR)) {
      m_pixbufSel = gdk_pixbuf_new_from_file(abspath.c_str(), NULL);

      //add code to draw bdmk to image area
      MultiFuncBodyMark::SetNewBodyMark(m_pixbufSel);
    }
  }
}

void MenuBDMK::ButtonClickedLeft(GtkButton* button) {
  m_pageCur--;
  SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

  string path;

  switch(m_study) {
  case ABD:
    path = GetBdmkPath("ABD");
    break;
  case OB:
    path = GetBdmkPath("OB");
    break;
  case GYN:
    path = GetBdmkPath("GYN");
    break;
  case UR:
    path = GetBdmkPath("UR");
    break;
  case CAR:
    path = GetBdmkPath("CAR");
    break;
  case SP:
    path = GetBdmkPath("SP");
    break;
  case VES:
    path = GetBdmkPath("VES");
    break;
  case ORT:
    path = GetBdmkPath("ORT");
    break;
  case EXTRA:
    path = GetBdmkPath("EXTRA");
    break;
  default:
    path = GetBdmkPath("ABD");
    break;
  }

  HideAllButton();
  UpdateImage(path, m_pageCur);
}

void MenuBDMK::ButtonClickedRight(GtkButton* button) {
  m_pageCur++;
  SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

  string path;

  switch(m_study) {
  case ABD:
    path = GetBdmkPath("ABD");
    break;
  case OB:
    path = GetBdmkPath("OB");
    break;
  case GYN:
    path = GetBdmkPath("GYN");
    break;
  case UR:
    path = GetBdmkPath("UR");
    break;
  case CAR:
    path = GetBdmkPath("CAR");
    break;
  case SP:
    path = GetBdmkPath("SP");
    break;
  case VES:
    path = GetBdmkPath("VES");
    break;
  case ORT:
    path = GetBdmkPath("ORT");
    break;
  case EXTRA:
    path = GetBdmkPath("EXTRA");
    break;
  default:
    path = GetBdmkPath("ABD");
    break;
  }

  HideAllButton();
  UpdateImage(path, m_pageCur);
}

string MenuBDMK::GetBdmkPath(const string item) {
  string path = g_build_filename(CFG_RES_PATH, BDMK_PATH, item.c_str(), NULL);

  if(g_file_test(path.c_str(), G_FILE_TEST_IS_DIR)) {
    return path;
  } else {
    GString* strUp = g_string_new(item.c_str());
    GString* strDown = g_string_ascii_down(strUp);  // After called g_string_ascii_down(), strUp==strDown, so just must free one
    path = g_build_filename(CFG_RES_PATH, BDMK_PATH, strDown->str, NULL);
    g_string_free(strDown, TRUE);

    return path;
  }
}

void MenuBDMK::ChangeBDMKStudy(guchar study) {
  //clear the list
  m_vecName.clear();

  // hide all image displayed
  HideAllButton();

  // read filename

  string path;

  switch(study) {
  case ABD:
    path = GetBdmkPath("ABD");
    break;
  case OB:
    path = GetBdmkPath("OB");
    break;
  case GYN:
    path = GetBdmkPath("GYN");
    break;
  case UR:
    path = GetBdmkPath("UR");
    break;
  case CAR:
    path = GetBdmkPath("CAR");
    break;
  case SP:
    path = GetBdmkPath("SP");
    break;
  case VES:
    path = GetBdmkPath("VES");
    break;
  case ORT:
    path = GetBdmkPath("ORT");
    break;
  case EXTRA:
    path = GetBdmkPath("EXTRA");
    break;
  default:
    path = GetBdmkPath("ABD");
    break;
  }

  m_study = study;
  m_dir = g_dir_open(path.c_str(), 0, NULL);

  if(!m_dir) {
    return;
  }

  string str;

  string name = g_dir_read_name(m_dir);
  if(name.empty()) {
    PRINTF("Error: The dir '%s' is empty!\n", path.c_str());

    return;
  }

  while(!name.empty()) {
    // Ignore hidden files that start with a '.'
    if(name[0] != '.') {
      m_vecName.push_back(name);
    }

    name = g_dir_read_name(m_dir);
  }

  g_dir_close(m_dir);
  m_dir = NULL;

  m_pageCur = 1;
  m_numTotal = m_vecName.size();
  SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

  // sort list by filename
  sort(m_vecName.begin(), m_vecName.end(), Sort);

  UpdateImage(path, m_pageCur);
}

void MenuBDMK::SetPageValue(guint page, guint total_page) {
  stringstream ss;
  ss << "[ " << page << " / " << total_page << " ]";

  gtk_label_set_text(m_label_page, ss.str().c_str());

  if(page <= 1 && total_page <= 1) {
    gtk_button_set_label(m_button_left, "");
    gtk_button_set_label(m_button_right, "");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_left), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_right), FALSE);
  } else if(page == 1 && total_page > 1) {
    gtk_button_set_label(m_button_left, "");
    gtk_button_set_label(m_button_right, "▶");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_left), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_right), TRUE);
  } else if(page > 1 && total_page > page) {
    gtk_button_set_label(m_button_left, "◀");
    gtk_button_set_label(m_button_right, "▶");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_left), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(m_button_right), TRUE);
  } else if(page > 1 && total_page == page) {
    gtk_button_set_label(m_button_left, "◀");
    gtk_button_set_label(m_button_right, "");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_left), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_right), FALSE);
  }
}

void MenuBDMK::HideAllButton() {
  for(int i = 0; i < MAX_IMAGE; i++) {
    gtk_widget_hide(GTK_WIDGET(m_button_images[i]));
  }
}

void MenuBDMK::UpdateImage(const string path, int page) {
  int size = m_vecName.size();
  int nth = (page - 1) * MAX_IMAGE;

  for(int i=nth, j=0; i<size; i++, j++) {
    if(j >= MAX_IMAGE) {
      break;
    }

    char* filepath = g_build_filename(path.c_str(), m_vecName[i].c_str(), NULL);
    if(!g_file_test(filepath, G_FILE_TEST_IS_DIR)) {
      GtkWidget* image = gtk_image_new();
      gtk_widget_show(image);
      gtk_image_set_from_file(GTK_IMAGE(image), filepath);
      gtk_button_set_image(m_button_images[j], image);
      gtk_widget_show(GTK_WIDGET(m_button_images[j]));
    }

    g_free(filepath);
  }
}

bool MenuBDMK::Sort(const string s1, const string s2) {
  return atof(s1.c_str()) < atof(s2.c_str());
}
