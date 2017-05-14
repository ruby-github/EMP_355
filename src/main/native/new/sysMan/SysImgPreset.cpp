#include "sysMan/SysImgPreset.h"

#include "Def.h"
#include "probe/ExamItem.h"
#include "sysMan/ViewSystem.h"

void ExecSetting(ExamItem::EItem itemIndex);

// ---------------------------------------------------------

void SetAdultAbdo() {
  ExecSetting(ExamItem::ABDO_ADULT);
}

void SetAdultLiver() {
  ExecSetting(ExamItem::ABDO_LIVER);
}

void SetKidAbdo() {
  ExecSetting(ExamItem::ABDO_KID);
}

void SetEarlyPreg() {
  ExecSetting(ExamItem::EARLY_PREG);
}

void SetMiddleLaterPreg() {
  ExecSetting(ExamItem::LATER_PREG);
}

void SetFetusCardio() {
  ExecSetting(ExamItem::CAR_FETUS);
}

void SetGyn() {
  ExecSetting(ExamItem::GYN);
}

void SetKidneyUreter() {
  ExecSetting(ExamItem::KIDNEY);
}

void SetBladderProstate() {
  ExecSetting(ExamItem::BLADDER);
}

void SetAdultCardio() {
  ExecSetting(ExamItem::CAR_ADULT);
}

void SetKidCardio() {
  ExecSetting(ExamItem::CAR_KID);
}

void SetMammaryGlands() {
  ExecSetting(ExamItem::GLANDS);
}

void SetThyroid() {
  ExecSetting(ExamItem::THYROID);
}

void SetEyeBall() {
  ExecSetting(ExamItem::EYE);
}

void SetSmallPart() {
  ExecSetting(ExamItem::SMALL_PART);
}

void SetCarotid() {
  ExecSetting(ExamItem::CAROTID);
}

void SetJugular() {
  ExecSetting(ExamItem::JUGULAR);
}

void SetPeripheryArtery() {
  ExecSetting(ExamItem::PERI_ARTERY);
}

void SetPeripheryVein() {
  ExecSetting(ExamItem::PERI_VEIN);
}

void SetHipJoint() {
  ExecSetting(ExamItem::HIP_JOINT);
}

void SetMeniscus() {
  ExecSetting(ExamItem::MENISCUS);
}

void SetJointCavity() {
  ExecSetting(ExamItem::JOINT_CAVITY);
}

void SetSpine() {
  ExecSetting(ExamItem::SPINE);
}

void SetUser1() {
  ExecSetting(ExamItem::USER1);
}

void SetUser2() {
  ExecSetting(ExamItem::USER2);
}

void SetUser3() {
  ExecSetting(ExamItem::USER3);
}

void SetUser4() {
  ExecSetting(ExamItem::USER4);
}

// ---------------------------------------------------------

void ExecSetting(ExamItem::EItem itemIndex) {
  ExamItem examItem;
  ExamItem::ParaItem paraItem;

  int probeIndex = ViewSystem::GetInstance()->GetProbeType();

  if (probeIndex != -1) {
    char* itemName = "Adult Abdomen";

    PRINTF("PROBE iNDEX = %d, itemIndex = %d\n", probeIndex, itemIndex);

    examItem.ReadExamItemPara(probeIndex, (int)itemIndex, &paraItem, itemName);
    ViewSystem::GetInstance()->SetImagePara(paraItem);
  }
}