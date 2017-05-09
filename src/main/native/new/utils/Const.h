#ifndef __CONST_H__
#define __CONST_H__

#include <string>

using std::string;

#ifdef VET
  const string EXAM_TYPES[] = {
    N_("User1"),
    N_("User2"),
    N_("User3"),
    N_("User4"),
    N_("User5"),
    N_("User6"),
    N_("User7"),
    N_("User8"),
    N_("User9"),
    N_("User10")
  };
#else
  const string EXAM_TYPES[] = {
    N_("Adult Abdomen"),
    N_("Adult Liver"),
    N_("Kid Abdomen"),
    N_("Adult Cardio"),
    N_("Kid Cardio"),
    N_("Mammary Glands"),
    N_("Thyroid"),
    N_("Eye Ball"),
    N_("Testicle"),
    N_("Gynecology"),
    N_("Early Pregnancy"),
    N_("Middle-late Pregnancy"),
    N_("Fetal Cardio"),
    N_("Kidney Ureter"),
    N_("Bladder Prostate"),
    N_("Carotid"),
    N_("Jugular"),
    N_("Periphery Artery"),
    N_("Periphery Vein"),
    N_("Hip Joint"),
    N_("Meniscus"),
    N_("Joint Cavity"),
    N_("Spine"),
    N_("User defined 1"),
    N_("User defined 2"),
    N_("User defined 3"),
    N_("User defined 4")
  };
#endif

#endif