#ifndef __MULTI_FUNC_FACTORY_H__
#define __MULTI_FUNC_FACTORY_H__

#include "AbsMultiFunc.h"

class MultiFuncFactory {
public:
  static MultiFuncFactory* GetInstance();

public:
  ~MultiFuncFactory();

public:
  enum EMultiFunc {
    NONE,       FOCUS,      DEPTH,        LOCAL_ZOOM,     GLOBAL_ZOOM,
    PIP_ZOOM,   MEASURE,    CALC,         TEXT,           ARROW,
    BODYMARK,   GRAY_TRANS, GRAY_REJECT,  FREQ,           CHROMA,
    M_INIT,     M,          PW_INIT,      PW,             CFM,
    PWCFM_INIT, PWCFM,      PWPDI_INIT,   PWPDI,          ANATOMIC_M,
    REPLAY,     EFOV,       CW_INIT,      CW,             CWCFM_INIT,
    CWCFM,      CWPDI_INIT, CWPDI,        BIOPSY_VERIFY,  BIOPSY
  };

  AbsMultiFunc* Create(EMultiFunc type);
  AbsMultiFunc* GetObject();
  EMultiFunc GetMultiFuncType();
  EMultiFunc GetMultiFuncPreType() {
      return m_typePre;
  }

private:
  MultiFuncFactory();

private:
  static MultiFuncFactory* m_instance;
  static AbsMultiFunc* m_ptrMulti;

private:
  EMultiFunc m_type;
  EMultiFunc m_typePre;
};

extern void MultiFuncUndo();

inline AbsMultiFunc* MultiFuncFactory::GetObject() {
  return m_ptrMulti;
}

inline MultiFuncFactory::EMultiFunc MultiFuncFactory::GetMultiFuncType() {
  return m_type;
}

#endif
