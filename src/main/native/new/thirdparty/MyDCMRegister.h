#ifndef __MY_DCM_REGISTER_H__
#define __MY_DCM_REGISTER_H__

#include "thirdparty/DCMRegister.h"

class MyDCMRegister {
public:
  static CDCMRegister* GetMe();

  static void Create(string resfiledir);
  static void Destroy();

  static bool GenerateLicenseFile(string destFileDir);
  static bool CheckAuthorize(string registerKey);
  static bool IsAuthorize();
};

#endif
