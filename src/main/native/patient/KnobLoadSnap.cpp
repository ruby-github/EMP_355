#include "patient/KnobLoadSnap.h"
#include "display/KnobMenu.h"
#include "Def.h"
#include "imageProc/MenuReview.h"

KnobMenu::KnobItem KnobLoadSnapMenu[6] = {
    {N_("Next Snap"), N_("Press"), PRESS, NULL, ReviewNextSnap},
    {N_("Previous Snap"), N_("Press"), PRESS, NULL, ReviewPreviousSnap},
    {N_("Slide Play"), "", MIN, ReviewSlidePlay, PressReviewSlidePlay},
    {N_("Slide Speed"), "", MIN, ReviewSlideSpeed, PressReviewSlideSpeed},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

void KnobLoadSnapCreate() {
    KnobMenu::GetInstance()->SetItem(KnobLoadSnapMenu, sizeof(KnobLoadSnapMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::SNAP);
}
///> knob menu need to be sync
void SyncKnobReview(EKnobReview type, const char* s, EKnobReturn status, bool update) {
    KnobLoadSnapMenu[type].value = s;
    KnobLoadSnapMenu[type].status = status;
    if (update)
        KnobMenu::GetInstance()->Update();
}

///> callback function
EKnobReturn ChgReviewSnap(EKnobOper oper) {
    return (g_menuReview.ChangeSnap(oper));
#if 0
    EKnobReturn ret = OK;
    if(oper == ADD) {
        g_menuReview.NextSnap();
    } else if(oper == SUB) {
        g_menuReview.PreviousSnap();
    }

    g_menuReview.ExitSlide();
    return ret;
#endif
}

EKnobReturn ReviewNextSnap(void) {
    g_menuReview.NextSnap();
    g_menuReview.ExitSlide();
    return PRESS;
}

EKnobReturn ReviewPreviousSnap(void) {
    g_menuReview.PreviousSnap();
    g_menuReview.ExitSlide();
    return PRESS;
}
//按键切换，改变幻灯片播放的开与关
EKnobReturn PressReviewSlidePlay(void) {
    EKnobOper oper = ROTATE;
    return g_menuReview.SlidePlay(oper);
}
EKnobReturn ReviewSlidePlay(EKnobOper oper) {
    return g_menuReview.SlidePlay(oper);
}
//按键切换，改变播放速度
EKnobReturn PressReviewSlideSpeed(void) {
    EKnobOper oper = ROTATE;
    return g_menuReview.SlideSpeed(oper);
}
EKnobReturn ReviewSlideSpeed(EKnobOper oper) {
    return g_menuReview.SlideSpeed(oper);
}
