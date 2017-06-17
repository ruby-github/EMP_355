#include "imageControl/KnobCfm.h"
#include "imageControl/ImgCfm.h"
#include "imageControl/ImgPw.h"
#include "imageControl/Img2D.h"
#include "display/KnobMenu.h"
#include "display/gui_global.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobPw.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ScanMode.h"
#include "display/HintArea.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/Zoom.h"

KnobMenu::KnobItem KnobCfmMenu[] = {
    {N_("Scale"), "", MIN, CfmChgScale, NULL},
    {N_("Baseline"), "", MIN, CfmChgBaseline, NULL},
    {N_("Wall Filter"), "", MIN, CfmChgWallFilter, NULL},
    {N_("Steer"), "", MIN, PwChgSteer, NULL},
    {N_("Sensitivity"), "", MIN, CfmChgSensitive, NULL},
    {N_("Doppler Freq."), "", MIN, CfmChgDopFreq, NULL},

    {N_("Simult"), "", MIN, CfmChgSimult, NULL},
    {N_("Flow Opt."), "", MIN, CfmChgFlowOpt, NULL},
    {N_("Color Invert"), "", MIN, CfmChgInvert, NULL},
    {N_("Color Focus"), "", MIN, CfmChgColorFoc, NULL},
    {N_("Color Line Density"), "", MIN, CfmChgLineDensity, NULL},
    {N_("Color Map"), "", MIN, CfmChgMap, NULL},

    {N_("Scan Line"), "", MIN, CfmChgScanLines, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

void KnobCfmCreate() {
    KnobMenu::GetInstance()->SetItem(KnobCfmMenu, sizeof(KnobCfmMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::CFM);
}

///> sync
void SyncKnobCfm(EKnobCfm type, const char* s, EKnobReturn status, bool draw) {
    KnobCfmMenu[type].value = s;
    KnobCfmMenu[type].status = status;
    if (draw)
        KnobMenu::GetInstance()->Update();
}

///> call back
EKnobReturn CfmChgScale(EKnobOper oper) {
    EKnobReturn ret;
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();

    if ((mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
        ret = ImgPw::GetInstance()->ChangeScaleSimult3(oper);
    else
        ret = ImgCfm::GetInstance()->ChangeScale(oper);

    // change tis
    ChangeTis();

    return ret;
}

EKnobReturn CfmChgBaseline(EKnobOper oper) {
    EKnobReturn ret = ImgCfm::GetInstance()->ChangeBaseline(oper);
    if (ret == ERROR)
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid in current mode."), 1);

    return ret;
}

EKnobReturn CfmChgWallFilter(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeWallFilter(oper);
}

EKnobReturn CfmChgLineDensity(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeLineDensity(oper);
}

EKnobReturn CfmChgSensitive(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeSensitive(oper);
}

EKnobReturn CfmChgDopFreq(EKnobOper oper) {
    return PwChgDopFreq(oper);
}

EKnobReturn CfmChgMap(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeColorMap(oper);
}

EKnobReturn CfmChgSimult(EKnobOper oper) {
    ModeStatus ms;
    ScanMode::EScanMode mode = ms.GetScanMode();

    if (mode == ScanMode::PWCFM) {
        if (ImgPw::GetInstance()->GetHPRFStatus()) {
            HintArea::GetInstance()->UpdateHint(_("[Simult]: Can not be on when HPRF is on."), 2);
            return ERROR;
        } else {
            if (oper == ADD)
                return ScanMode::GetInstance()->EnterPwCfmSimult(TRUE);
        }
    } else if (mode == ScanMode::PWPDI) {
        if (ImgPw::GetInstance()->GetHPRFStatus()) {
            HintArea::GetInstance()->UpdateHint(_("[Simult]: Can not be on when HPRF is on."), 2);
            return ERROR;
        } else {
            if (oper == ADD)
                return ScanMode::GetInstance()->EnterPwPdiSimult(TRUE);
        }
    }

    else if (mode == ScanMode::PWCFM_SIMULT) {
        if (oper == SUB)
            return ScanMode::GetInstance()->EnterPwCfmSimult(FALSE);
    } else if (mode == ScanMode::PWPDI_SIMULT) {
        if (oper == SUB)
            return ScanMode::GetInstance()->EnterPwPdiSimult(FALSE);
    } else {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid in current mode."), 1);
        return ERROR;
    }

    return OK;
}

EKnobReturn CfmChgTurbo(EKnobOper oper) {
    //return ImgCfm::GetInstance()->ChangeTurbo(oper);
    return MIN;
}

EKnobReturn CfmChgInvert(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeInvert(oper);
}

EKnobReturn CfmChgFlowOpt(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeFlowOpt(oper);
}
EKnobReturn CfmChgColorFoc(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeColorFocus(oper);
}

//////////////////////   test    /////////////////////////
EKnobReturn CfmChgTissue(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeTissue(oper);
}

EKnobReturn CfmChgNoise(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeNoise(oper);
}

EKnobReturn CfmChgRate(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeSpeed(oper);
}

EKnobReturn CfmChgVariance(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeVariance(oper);
}

EKnobReturn CfmChgGainGate(EKnobOper oper) {
    return ImgCfm::GetInstance()->ChangeGainGate(oper);
}

EKnobReturn CfmChgScanLines(EKnobOper oper) {
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if(type == 'p' || type == 'P') {
        if (!Zoom::GetInstance()->GetLocalZoomStatus())
            return ImgCfm::GetInstance()->ChangeScanLines(oper);
        else {
            HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Invalid when localzoom is open!."), 1);
            return ERROR;
        }
    } else {
        HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Only valid when probe type is phase."), 1);
        return ERROR;
    }
}
