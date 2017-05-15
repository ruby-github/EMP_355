#include "sysMan/UserDefineKey.h"

#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageControl/Knob2D.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/KnobReplay.h"
#include "imageProc/Menu2D.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "periDevice/PrintScreen.h"
#include "sysMan/SysUserDefinedKey.h"
#include "sysMan/ViewSystem.h"

UserDefineKey::UserDefineKey() {
}

UserDefineKey::~UserDefineKey() {
}

void UserDefineKey::PressKeyP1(void) {
  SysUserDefinedKey ud;
  int func = ud.GetFuncP1();

  if(PRINT == func) {
    PrintScreen ps;
    ps.PrintP1();
  } else {
    FactoryCreate((EKeyFunc)func);
  }
}

void UserDefineKey::PressKeyP2(void) {
  SysUserDefinedKey ud;
  int func = ud.GetFuncP2();

  if(PRINT == func) {
    ModeStatus ms;

    if (ms.GetFreezeMode() == FreezeMode::UNFREEZE) {
      FreezeMode::GetInstance()->PressFreeze();
    }

    PrintScreen ps;
    ps.PrintP2();
  } else {
    FactoryCreate((EKeyFunc)func);
  }
}

void UserDefineKey::PressKeyP3(void) {
  SysUserDefinedKey ud;
  int func = ud.GetFuncP3();

  if(PRINT == func) {
    ModeStatus ms;

    if (ms.GetFreezeMode() == FreezeMode::UNFREEZE) {
      FreezeMode::GetInstance()->PressFreeze();
    }

    PrintScreen ps;
    ps.PrintP3();
  } else {
    FactoryCreate((EKeyFunc)func);
  }
}

// ---------------------------------------------------------

void UserDefineKey::FactoryCreate(EKeyFunc type) {
  ModeStatus ms;
  PrintScreen ps;

  switch (type) {
  case NONE:
    break;
  case TSI:
    if (ms.GetFreezeMode() == FreezeMode::UNFREEZE) {
      Img2D::GetInstance()->ChangeTSI(ROTATE);
    }

    break;
  case SAVE_CINE:
    if (ms.GetFreezeMode() != FreezeMode::UNFREEZE) {
      ReplayChgSaveCine();
    }

    break;
  case PRINT:
    break;
  case BIOPSY:
    {
      KeyBiopsy kb;
      kb.Execute();
    }

    break;
  #if defined(EMP_322)
    case PIP:
      g_menu2D.BtnPIP(NULL);

      break;
  #else
    case STEER:
      if (ms.GetFreezeMode() == FreezeMode::UNFREEZE) {
        D2ChgSteer(ROTATE);
      }

      break;
    case PIP:
      g_menu2D.BtnPIP(NULL);

      break;
    case PW_SOUND:
      ImgPw::GetInstance()->ChangeSoundStatus(ROTATE);

      break;
    case HPRF:
      if (ms.IsSpectrumMode() || ms.IsSpectrumColorMode()) {
        if (ImgPw::GetInstance()->GetHPRFStatus()) {
          ImgPw::GetInstance()->ChangeHPRF(FALSE);
        } else {
          ImgPw::GetInstance()->ChangeHPRF(TRUE);
        }
      }

      break;
    case IMAGE_PRESET:
      {
        if (ModeStatus::IsAutoReplayMode()) {
          FreezeMode::GetInstance()->ChangeAutoReplay();
        } else if (ModeStatus::IsUnFreezeMode()) {
          FreezeMode::GetInstance()->PressFreeze();
        }

        ViewSystem::GetInstance()->CreateWindow();

        #ifndef EMP_355        //按自定义键进入systemsetting时，保持冻结
          FreezeMode::GetInstance()->PressUnFreeze();
        #endif
      }

      break;
  #endif
  default:
    break;
  }
}
