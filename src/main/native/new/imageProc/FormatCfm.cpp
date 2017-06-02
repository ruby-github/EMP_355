/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: FormatCfm.cpp
 * @brief: control cfm/pdi diplay format and current image
 *
 * version: V1.0
 * date: 2009-5-27
 * @author: zhanglei
 */

#include "imageProc/FormatCfm.h"
#include "imageProc/Format2D.h"
#include "imageProc/GlobalClassMan.h"

FormatCfm* FormatCfm::m_ptrInstance = NULL;

FormatCfm* FormatCfm::GetInstance() {
    if (m_ptrInstance == NULL)
        m_ptrInstance = new FormatCfm;

    return m_ptrInstance;
}

FormatCfm::FormatCfm() {
    GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
    m_ptrUpdate = ptrGcm->GetUpdateCfm();

    m_ptrDsc = DscMan::GetInstance();
    m_ptrDscPara = m_ptrDsc->GetDscPara();

    m_ptrReplay = Replay::GetInstance();
    m_ptrImg = ImgCfm::GetInstance();

    m_format = B;
    m_formatSnap = B;
    m_curB = 0;
}
FormatCfm::~FormatCfm() {
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

/*
 * @brief change cfm format to "format", when it changd 2D format will be changed too.
 *
 * @para format format that will be changed to
 */
void FormatCfm::ChangeFormat(enum EFormatCfm format) {
    m_format = format;

    m_curB = Format2D::GetInstance()->ChangeFormat((Format2D::EFormat2D)format);

    if (m_format == B) {
        m_ptrDsc->CreateDscObj(DscMan::CFMB);
    } else if (m_format == BB) {
        m_ptrDsc->CreateDscObj(DscMan::CFMBB);
    } else if (m_format == B4) {
        m_ptrDsc->CreateDscObj(DscMan::CFMB4);
    }

    m_ptrImg->SetBoxStatus(false);

    ///> update view
    m_ptrUpdate->ChangeFormatCfm(m_format);
}

/*
 * @brief switch current image in BB format, 2D current image of BB will be changed tof
 */
bool FormatCfm::SwitchBB(bool left, int &current) {
    m_ptrImg->SetBoxStatus(false);
    if (Format2D::GetInstance()->SwitchBB(left, current)) {
        m_ptrUpdate->ChangeCurrentImgBB(m_curB);
        return TRUE;
    }

    return FALSE;
}

int FormatCfm::SwitchBB(void) {
    m_ptrImg->SetBoxStatus(false);
    m_curB = Format2D::GetInstance()->SwitchBB();

    ///> update
    m_ptrUpdate->ChangeCurrentImgBB(m_curB);

    return m_curB;
}

int FormatCfm::SwitchB4() {
    m_ptrImg->SetBoxStatus(false);
    m_curB = Format2D::GetInstance()->SwitchB4();

    ///> update
    m_ptrUpdate->ChangeCurrentImg4B(m_curB);

    return m_curB;

}

FormatCfm::EFormatCfm FormatCfm::GetFormat() {
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
        return m_formatSnap;
    else
        return m_format;
}
