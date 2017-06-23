#include "thirdparty/MyDCMRegister.h"

#include "Def.h"

#include <exception>
#include <iostream>

using namespace std;

CDCMRegister* MyDCMRegister::GetMe(bool init) {
  cout << "MyDCMRegister::GetMe" << endl;

  CDCMRegister* reg = CDCMRegister::GetMe();

  if (reg == NULL) {
    cout << "CDCMRegister::GetMe() is NULL" << endl;

    if (init) {
      CDCMRegister::Create(DCMRES_DIR);

      reg = CDCMRegister::GetMe();
    }
  }

  return reg;
}

void MyDCMRegister::Create(string resfiledir) {
  cout << "MyDCMRegister::Create start: " << resfiledir << endl;
  CDCMRegister::Create(resfiledir);
  cout << "MyDCMRegister::Create finish" << endl;
}

void MyDCMRegister::Destroy() {
  cout << "MyDCMRegister::Destroy start" << endl;
  CDCMRegister::Destroy();
  cout << "MyDCMRegister::Destroy finish" << endl;
}

bool MyDCMRegister::GenerateLicenseFile(string destFileDir) {
  cout << "MyDCMRegister::GenerateLicenseFile start: " << destFileDir << endl;
  bool ret = GetMe(true)->GenerateLicenseFile(destFileDir);
  cout << "MyDCMRegister::GenerateLicenseFile finish" << endl;

  return ret;
}

bool MyDCMRegister::CheckAuthorize(string registerKey) {
  cout << "MyDCMRegister::CheckAuthorize start: " << registerKey << endl;
  bool ret = GetMe(true)->CheckAuthorize(registerKey);
  cout << "MyDCMRegister::CheckAuthorize finish" << endl;

  return ret;
}

bool MyDCMRegister::IsAuthorize() {
  cout << "MyDCMRegister::IsAuthorize start" << endl;
  bool ret = GetMe(true)->IsAuthorize();
  cout << "MyDCMRegister::IsAuthorize finish" << endl;

  return ret;
}
