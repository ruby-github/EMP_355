#ifndef __ABS_DCMDATA_H__
#define __ABS_DCMDATA_H__

#include "thirdparty/DCMDef.h"

class CAbsDCMData {
public:
  virtual ~CAbsDCMData() {};
  virtual bool GetDCMData(bool (*)(DCMELEMENT &))=0;
};

#endif
