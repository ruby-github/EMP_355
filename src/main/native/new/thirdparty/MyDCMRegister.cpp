#include "thirdparty/MyDCMRegister.h"

#include <exception>
#include <iostream>

using namespace std;

CDCMRegister* MyDCMRegister::GetMe() {
  try {
    return CDCMRegister::GetMe();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::GetMe" << endl;

    return NULL;
  }
}

void MyDCMRegister::Create(string resfiledir) {
  try {
    CDCMRegister::Create(resfiledir);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::Create: " << resfiledir << endl;
  }
}

void MyDCMRegister::Destroy() {
  try {
    CDCMRegister::Destroy();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::Destroy" << endl;
  }
}

bool MyDCMRegister::GenerateLicenseFile(string destFileDir) {
  try {
    return CDCMRegister::GetMe()->GenerateLicenseFile(destFileDir);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::GenerateLicenseFile: " << destFileDir << endl;

    return false;
  }
}

bool MyDCMRegister::CheckAuthorize(string registerKey) {
  try {
    return CDCMRegister::GetMe()->CheckAuthorize(registerKey);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::CheckAuthorize: " << registerKey << endl;

    return false;
  }
}

bool MyDCMRegister::IsAuthorize() {
  try {
    return CDCMRegister::GetMe()->IsAuthorize();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMRegister::IsAuthorize" << endl;

    return false;
  }
}
