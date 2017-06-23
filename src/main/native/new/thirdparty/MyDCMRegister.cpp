#include "thirdparty/MyDCMRegister.h"

#include "Def.h"

#include <exception>
#include <iostream>

using namespace std;

CDCMRegister* MyDCMRegister::GetMe(bool init) {
  cout << "MyDCMRegister::GetMe" << endl;

  CDCMRegister* reg = CDCMRegister::GetMe();

  if (reg == NULL) {
    if (init) {
      CDCMRegister::Create(DCMRES_DIR);

      reg = CDCMRegister::GetMe();
    }
  }

  return reg;
}

void MyDCMRegister::Create(string resfiledir) {
  cout << "MyDCMRegister::Create: " << resfiledir << endl;

  CDCMRegister::Create(resfiledir);
}

void MyDCMRegister::Destroy() {
  cout << "MyDCMRegister::Destroy" << endl;

  CDCMRegister::Destroy();
}

bool MyDCMRegister::GenerateLicenseFile(string destFileDir) {
  cout << "MyDCMRegister::GenerateLicenseFile: " << destFileDir << endl;

  return GetMe(true)->GenerateLicenseFile(destFileDir);
}

bool MyDCMRegister::CheckAuthorize(string registerKey) {
  cout << "MyDCMRegister::CheckAuthorize: " << registerKey << endl;

  return GetMe(true)->CheckAuthorize(registerKey);
}

bool MyDCMRegister::IsAuthorize() {
  cout << "MyDCMRegister::IsAuthorize" << endl;

  return GetMe(true)->IsAuthorize();
}
