#include "thirdparty/MyDCMMan.h"

#include "Def.h"

#include <exception>
#include <iostream>

using namespace std;

CDCMMan* MyDCMMan::GetMe(bool init) {
  cout << "MyDCMMan::GetMe" << endl;

  CDCMMan* reg = GetMe(true);

  if (reg == NULL) {
    if (init) {
      CDCMMan::Create(DCMDB_DIR);

      reg = GetMe(true);
    }
  }

  return reg;
}

void MyDCMMan::Create(string dcm_database_dir) {
  cout << "MyDCMMan::Create: " << dcm_database_dir << endl;

  CDCMMan::Create(dcm_database_dir);
}

void MyDCMMan::Destroy() {
  cout << "MyDCMMan::Destroy" << endl;

  CDCMMan::Destroy();
}

void MyDCMMan::AddImage(DCMIMAGEELEMENT element) {
  cout << "MyDCMMan::AddImage" << endl;

  GetMe(true)->AddImage(element);
}

bool MyDCMMan::AddMPPSService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddMPPSService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

  return GetMe(true)->AddMPPSService(device, serviceName, aeTitle, port);
}

bool MyDCMMan::AddQueryRetrieveService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddQueryRetrieveService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

  return GetMe(true)->AddQueryRetrieveService(device, serviceName, aeTitle, port);
}

bool MyDCMMan::AddServer(string device, string ip) {
  cout << "MyDCMMan::AddServer: " << device << ", " << ip << endl;

  return GetMe(true)->AddServer(device, ip);
}

bool MyDCMMan::AddStorageCommitService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddStorageCommitService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

  return GetMe(true)->AddStorageCommitService(device, serviceName, aeTitle, port);
}

bool MyDCMMan::AddStorageService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddStorageService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

  return GetMe(true)->AddStorageService(device, serviceName, aeTitle, port);
}

bool MyDCMMan::AddWorklistService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddWorklistService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

  return GetMe(true)->AddWorklistService(device, serviceName, aeTitle, port);
}

EDCMReturnStatus MyDCMMan::BrowseDICOMDIR(string destDirStorageMedia, vector<DCMDICOMDIRSTUDYLEVEL>& vecStudyLevel) {
  cout << "MyDCMMan::BrowseDICOMDIR: " << destDirStorageMedia << endl;

  return GetMe(true)->BrowseDICOMDIR(destDirStorageMedia, vecStudyLevel);
}

void MyDCMMan::DeleteImageBeforeEndStudy(string filename) {
  cout << "MyDCMMan::DeleteImageBeforeEndStudy: " << filename << endl;

  GetMe(true)->DeleteImageBeforeEndStudy(filename);
}

bool MyDCMMan::DeleteMPPSService(string device) {
  cout << "MyDCMMan::DeleteMPPSService: " << device << endl;

  return GetMe(true)->DeleteMPPSService(device);
}

bool MyDCMMan::DeleteQueryRetrieveService(string device) {
  cout << "MyDCMMan::DeleteQueryRetrieveService: " << device << endl;

  return GetMe(true)->DeleteQueryRetrieveService(device);
}

bool MyDCMMan::DeleteServer(string device) {
  cout << "MyDCMMan::DeleteServer: " << device << endl;

  return GetMe(true)->DeleteServer(device);
}

bool MyDCMMan::DeleteStorageCommitService(string device) {
  cout << "MyDCMMan::DeleteStorageCommitService: " << device << endl;

  return GetMe(true)->DeleteStorageCommitService(device);
}

bool MyDCMMan::DeleteStorageService(string device) {
  cout << "MyDCMMan::DeleteStorageService: " << device << endl;

  return GetMe(true)->DeleteStorageService(device);
}

bool MyDCMMan::DeleteStudy(unsigned int studyNo) {
  cout << "MyDCMMan::DeleteStudy: " << studyNo << endl;

  return GetMe(true)->DeleteStudy(studyNo);
}

bool MyDCMMan::DeleteWorklistService(string device) {
  cout << "MyDCMMan::DeleteWorklistService: " << device << endl;

  return GetMe(true)->DeleteWorklistService(device);
}

void MyDCMMan::EditSR(DCMSRELEMENT element) {
  cout << "MyDCMMan::EditSR" << endl;

  GetMe(true)->EditSR(element);
}

void MyDCMMan::EditStudyInfo(DCMSTUDYELEMENT element) {
  cout << "MyDCMMan::EditStudyInfo" << endl;

  GetMe(true)->EditStudyInfo(element);
}

bool MyDCMMan::EndMPPS(string studyEndDate, string studyEndTime) {
  cout << "MyDCMMan::EndMPPS: " << studyEndDate << ", " << studyEndTime << endl;

  return GetMe(true)->EndMPPS(studyEndDate, studyEndTime);
}

bool MyDCMMan::EndStudy() {
  cout << "MyDCMMan::EndStudy" << endl;

  return GetMe(true)->EndStudy();
}

EDCMReturnStatus MyDCMMan::ExportStudy(unsigned int studyNo, string srcDirImage, string destDirStorageMedia, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::ExportStudy: " << studyNo << ", " << srcDirImage << ", " << destDirStorageMedia << endl;

  return GetMe(true)->ExportStudy(studyNo, srcDirImage, destDirStorageMedia, psCallback);
}

vector<string> MyDCMMan::GetAllDevice() {
  cout << "MyDCMMan::GetAllDevice" << endl;

  return GetMe(true)->GetAllDevice();
}

bool MyDCMMan::GetAllMPPSService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllMPPSService" << endl;

  return GetMe(true)->GetAllMPPSService(callback, pData);
}

bool MyDCMMan::GetAllQueryRetrieveService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllQueryRetrieveService" << endl;

  return GetMe(true)->GetAllQueryRetrieveService(callback, pData);
}

vector<string> MyDCMMan::GetAllRetrieveDestinationServerDevice() {
  cout << "MyDCMMan::GetAllRetrieveDestinationServerDevice" << endl;

  return GetMe(true)->GetAllRetrieveDestinationServerDevice();
}

bool MyDCMMan::GetAllServer(GetSingleServerAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllServer" << endl;

  return GetMe(true)->GetAllServer(callback, pData);
}

bool MyDCMMan::GetAllStorageCommitService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllStorageCommitService" << endl;

  return GetMe(true)->GetAllStorageCommitService(callback, pData);
}

bool MyDCMMan::GetAllStorageService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllStorageService" << endl;

  return GetMe(true)->GetAllStorageService(callback, pData);
}

bool MyDCMMan::GetAllWorklistService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllWorklistService" << endl;

  return GetMe(true)->GetAllWorklistService(callback, pData);
}

string MyDCMMan::GetDefaultMPPSServiceDevice() {
  cout << "MyDCMMan::GetDefaultMPPSServiceDevice" << endl;

  return GetMe(true)->GetDefaultMPPSServiceDevice();
}

string MyDCMMan::GetDefaultQueryRetrieveServiceDevice() {
  cout << "MyDCMMan::GetDefaultQueryRetrieveServiceDevice" << endl;

  return GetMe(true)->GetDefaultQueryRetrieveServiceDevice();
}

string MyDCMMan::GetDefaultStorageCommitServiceDevice() {
  cout << "MyDCMMan::GetDefaultStorageCommitServiceDevice" << endl;

  return GetMe(true)->GetDefaultStorageCommitServiceDevice();
}

string MyDCMMan::GetDefaultStorageServiceDevice() {
  cout << "MyDCMMan::GetDefaultStorageServiceDevice" << endl;

  return GetMe(true)->GetDefaultStorageServiceDevice();
}

string MyDCMMan::GetDefaultWorklistServiceDevice() {
  cout << "MyDCMMan::GetDefaultWorklistServiceDevice" << endl;

  return GetMe(true)->GetDefaultWorklistServiceDevice();
}

string MyDCMMan::GetLocalAE() {
  cout << "MyDCMMan::GetLocalAE" << endl;

  return GetMe(true)->GetLocalAE();
}

int MyDCMMan::GetLocalPort() {
  cout << "MyDCMMan::GetLocalPort" << endl;

  return GetMe(true)->GetLocalPort();
}

bool MyDCMMan::GetStudyBackupStatus(unsigned int studyNo) {
  cout << "MyDCMMan::GetStudyBackupStatus: " << studyNo << endl;

  return GetMe(true)->GetStudyBackupStatus(studyNo);
}

EDCMReturnStatus MyDCMMan::ImportStudy(int studyIndex, string destDirStorageMedia,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::ImportStudy: " << studyIndex << ", " << destDirStorageMedia << endl;

  return GetMe(true)->ImportStudy(studyIndex, destDirStorageMedia, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
}

bool MyDCMMan::IsExistedWorklistStudy(DCMWORKLISTELEMENT wlElement) {
  cout << "MyDCMMan::IsExistedWorklistStudy" << endl;

  return GetMe(true)->IsExistedWorklistStudy(wlElement);
}

bool MyDCMMan::Ping(string ip) {
  cout << "MyDCMMan::Ping: " << ip << endl;

  return GetMe(true)->Ping((char*)ip.c_str());
}

EDCMReturnStatus MyDCMMan::Query(string patientName, string patientID,
  string accessionNumber, string startStudyDate, string endStudyDate, vector<DCMQRQUERYRES>& vQueryRes) {
  cout << "MyDCMMan::Query: " << patientName << ", " << patientID << ", "
    << accessionNumber << ", " << startStudyDate << ", " << endStudyDate << endl;

  return GetMe(true)->Query(patientName, patientID, accessionNumber, startStudyDate, endStudyDate, vQueryRes);
}

vector<DCMWORKLISTELEMENT> MyDCMMan::QueryWorklist(string patientID, string patientName,
  string accessionNumber, string requestedProcedureID, string startDate, string endDate) {
  cout << "MyDCMMan::QueryWorklist: " << patientID << ", " << patientName << ", "
    << accessionNumber << ", " << requestedProcedureID << ", " << startDate << ", " << endDate << endl;

  return GetMe(true)->QueryWorklist(patientID, patientName, accessionNumber, requestedProcedureID, startDate, endDate);
}

EDCMReturnStatus MyDCMMan::Retrieve(unsigned int itemIndex, string destDevice,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::Retrieve: " << itemIndex << ", " << destDevice << endl;

  return GetMe(true)->Retrieve(itemIndex, destDevice, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
}

bool MyDCMMan::SendStudy(unsigned int studyNo, string srcDirImage, string destDirImage,
  bool onStorageCommitment, bool enableStorageSR, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::SendStudy: " << studyNo << ", " << srcDirImage << ", " << destDirImage << ", "
    << onStorageCommitment << ", " << enableStorageSR << ", " << enableStorageMultiFrame << ", " << maxFrameNumber << endl;

  return GetMe(true)->SendStudy(studyNo, srcDirImage, destDirImage,
    onStorageCommitment, enableStorageSR, enableStorageMultiFrame, maxFrameNumber, psCallback);
}

bool MyDCMMan::SetDefaultMPPSService(string device) {
  cout << "MyDCMMan::SetDefaultMPPSService: " << device << endl;

  return GetMe(true)->SetDefaultMPPSService(device);
}

bool MyDCMMan::SetDefaultQueryRetrieveService(string device) {
  cout << "MyDCMMan::SetDefaultQueryRetrieveService: " << device << endl;

  return GetMe(true)->SetDefaultQueryRetrieveService(device);
}

bool MyDCMMan::SetDefaultStorageCommitService(string device) {
  cout << "MyDCMMan::SetDefaultStorageCommitService: " << device << endl;

  return GetMe(true)->SetDefaultStorageCommitService(device);
}

bool MyDCMMan::SetDefaultStorageService(string device) {
  cout << "MyDCMMan::SetDefaultStorageService: " << device << endl;

  return GetMe(true)->SetDefaultStorageService(device);
}

bool MyDCMMan::SetDefaultWorklistService(string device) {
  cout << "MyDCMMan::SetDefaultWorklistService: " << device << endl;

  return GetMe(true)->SetDefaultWorklistService(device);
}

void MyDCMMan::SetLocalAE(string strAE) {
  cout << "MyDCMMan::SetLocalAE: " << strAE << endl;

  GetMe(true)->SetLocalAE(strAE);
}

void MyDCMMan::SetLocalPort(int port) {
  cout << "MyDCMMan::SetLocalPort: " << port << endl;

  GetMe(true)->SetLocalPort(port);
}

void MyDCMMan::SetSRVerifierName(string name) {
  cout << "MyDCMMan::SetSRVerifierName: " << name << endl;

  GetMe(true)->SetSRVerifierName(name);
}

void MyDCMMan::SetSRVerifyFlag(bool flag) {
  cout << "MyDCMMan::SetSRVerifyFlag: " << flag << endl;

  GetMe(true)->SetSRVerifyFlag(flag);
}

void MyDCMMan::SetSRVerifyOrganization(string organization) {
  cout << "MyDCMMan::SetSRVerifyOrganization: " << organization << endl;

  GetMe(true)->SetSRVerifyOrganization(organization);
}

bool MyDCMMan::StartMPPS(DCMMPPSELEMENT mppsElement) {
  cout << "MyDCMMan::StartMPPS" << endl;

  return GetMe(true)->StartMPPS(mppsElement);
}

bool MyDCMMan::TestLinkDefaultWorklist() {
  cout << "MyDCMMan::TestLinkDefaultWorklist" << endl;

  return GetMe(true)->TestLinkDefaultWorklist();
}

bool MyDCMMan::TestLinkMPPS(string device) {
  cout << "MyDCMMan::TestLinkMPPS: " << device << endl;

  return GetMe(true)->TestLinkMPPS(device);
}

bool MyDCMMan::TestLinkQueryRetrieve(string device) {
  cout << "MyDCMMan::TestLinkQueryRetrieve: " << device << endl;

  return GetMe(true)->TestLinkQueryRetrieve(device);
}

bool MyDCMMan::TestLinkStorage(string device) {
  cout << "MyDCMMan::TestLinkStorage: " << device << endl;

  return GetMe(true)->TestLinkStorage(device);
}

bool MyDCMMan::TestLinkStorageCommit(string device) {
  cout << "MyDCMMan::TestLinkStorageCommit: " << device << endl;

  return GetMe(true)->TestLinkStorageCommit(device);
}

bool MyDCMMan::TestLinkWorklist(string device) {
  cout << "MyDCMMan::TestLinkWorklist: " << device << endl;

  return GetMe(true)->TestLinkWorklist(device);
}
