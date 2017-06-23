#include "thirdparty/MyDCMMan.h"

#include "Def.h"

#include <exception>
#include <iostream>

using namespace std;

CDCMMan* MyDCMMan::GetMe(bool init) {
  cout << "MyDCMMan::GetMe" << endl;

  CDCMMan* reg = CDCMMan::GetMe();

  if (reg == NULL) {
    cout << "CDCMMan::GetMe() is NULL" << endl;

    if (init) {
      CDCMMan::Create(DCMDB_DIR);

      reg = CDCMMan::GetMe();
    }
  }

  return reg;
}

void MyDCMMan::Create(string dcm_database_dir) {
  cout << "MyDCMMan::Create start: " << dcm_database_dir << endl;
  CDCMMan::Create(dcm_database_dir);
  cout << "MyDCMMan::Create finish" << endl;
}

void MyDCMMan::Destroy() {
  cout << "MyDCMMan::Destroy start" << endl;
  CDCMMan::Destroy();
  cout << "MyDCMMan::Destroy finish" << endl;
}

void MyDCMMan::AddImage(DCMIMAGEELEMENT element) {
  cout << "MyDCMMan::AddImage start" << endl;
  GetMe(true)->AddImage(element);
  cout << "MyDCMMan::AddImage finish" << endl;
}

bool MyDCMMan::AddMPPSService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddMPPSService start: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;
  bool ret = GetMe(true)->AddMPPSService(device, serviceName, aeTitle, port);
  cout << "MyDCMMan::AddMPPSService finish" << endl;

  return ret;
}

bool MyDCMMan::AddQueryRetrieveService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddQueryRetrieveService start: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;
  bool ret = GetMe(true)->AddQueryRetrieveService(device, serviceName, aeTitle, port);
  cout << "MyDCMMan::AddQueryRetrieveService finish" << endl;

  return ret;
}

bool MyDCMMan::AddServer(string device, string ip) {
  cout << "MyDCMMan::AddServer start: " << device << ", " << ip << endl;
  bool ret = GetMe(true)->AddServer(device, ip);
  cout << "MyDCMMan::AddServer finish" << endl;

  return ret;
}

bool MyDCMMan::AddStorageCommitService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddStorageCommitService start: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;
  bool ret = GetMe(true)->AddStorageCommitService(device, serviceName, aeTitle, port);
  cout << "MyDCMMan::AddStorageCommitService finish" << endl;

  return ret;
}

bool MyDCMMan::AddStorageService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddStorageService start: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;
  bool ret = GetMe(true)->AddStorageService(device, serviceName, aeTitle, port);
  cout << "MyDCMMan::AddStorageService finish" << endl;

  return ret;
}

bool MyDCMMan::AddWorklistService(string device, string serviceName, string aeTitle, int port) {
  cout << "MyDCMMan::AddWorklistService start: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;
  bool ret = GetMe(true)->AddWorklistService(device, serviceName, aeTitle, port);
  cout << "MyDCMMan::AddWorklistService finish" << endl;

  return ret;
}

EDCMReturnStatus MyDCMMan::BrowseDICOMDIR(string destDirStorageMedia, vector<DCMDICOMDIRSTUDYLEVEL>& vecStudyLevel) {
  cout << "MyDCMMan::BrowseDICOMDIR start: " << destDirStorageMedia << endl;
  EDCMReturnStatus ret = GetMe(true)->BrowseDICOMDIR(destDirStorageMedia, vecStudyLevel);
  cout << "MyDCMMan::BrowseDICOMDIR finish" << endl;

  return ret;
}

void MyDCMMan::DeleteImageBeforeEndStudy(string filename) {
  cout << "MyDCMMan::DeleteImageBeforeEndStudy start: " << filename << endl;
  GetMe(true)->DeleteImageBeforeEndStudy(filename);
  cout << "MyDCMMan::DeleteImageBeforeEndStudy finish" << endl;
}

bool MyDCMMan::DeleteMPPSService(string device) {
  cout << "MyDCMMan::DeleteMPPSService start: " << device << endl;
  bool ret = GetMe(true)->DeleteMPPSService(device);
  cout << "MyDCMMan::DeleteMPPSService finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteQueryRetrieveService(string device) {
  cout << "MyDCMMan::DeleteQueryRetrieveService start: " << device << endl;
  bool ret = GetMe(true)->DeleteQueryRetrieveService(device);
  cout << "MyDCMMan::DeleteQueryRetrieveService finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteServer(string device) {
  cout << "MyDCMMan::DeleteServer start: " << device << endl;
  bool ret = GetMe(true)->DeleteServer(device);
  cout << "MyDCMMan::DeleteServer finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteStorageCommitService(string device) {
  cout << "MyDCMMan::DeleteStorageCommitService start: " << device << endl;
  bool ret = GetMe(true)->DeleteStorageCommitService(device);
  cout << "MyDCMMan::DeleteStorageCommitService finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteStorageService(string device) {
  cout << "MyDCMMan::DeleteStorageService start: " << device << endl;
  bool ret = GetMe(true)->DeleteStorageService(device);
  cout << "MyDCMMan::DeleteStorageService finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteStudy(unsigned int studyNo) {
  cout << "MyDCMMan::DeleteStudy start: " << studyNo << endl;
  bool ret = GetMe(true)->DeleteStudy(studyNo);
  cout << "MyDCMMan::DeleteStudy finish" << endl;

  return ret;
}

bool MyDCMMan::DeleteWorklistService(string device) {
  cout << "MyDCMMan::DeleteWorklistService start: " << device << endl;
  bool ret = GetMe(true)->DeleteWorklistService(device);
  cout << "MyDCMMan::DeleteWorklistService finish" << endl;

  return ret;
}

void MyDCMMan::EditSR(DCMSRELEMENT element) {
  cout << "MyDCMMan::EditSR start" << endl;
  GetMe(true)->EditSR(element);
  cout << "MyDCMMan::EditSR finish" << endl;
}

void MyDCMMan::EditStudyInfo(DCMSTUDYELEMENT element) {
  cout << "MyDCMMan::EditStudyInfo start" << endl;
  GetMe(true)->EditStudyInfo(element);
  cout << "MyDCMMan::EditStudyInfo finish" << endl;
}

bool MyDCMMan::EndMPPS(string studyEndDate, string studyEndTime) {
  cout << "MyDCMMan::EndMPPS start: " << studyEndDate << ", " << studyEndTime << endl;
  bool ret = GetMe(true)->EndMPPS(studyEndDate, studyEndTime);
  cout << "MyDCMMan::EndMPPS finish" << endl;

  return ret;
}

bool MyDCMMan::EndStudy() {
  cout << "MyDCMMan::EndStudy start" << endl;
  bool ret = GetMe(true)->EndStudy();
  cout << "MyDCMMan::EndStudy finish" << endl;

  return ret;
}

EDCMReturnStatus MyDCMMan::ExportStudy(unsigned int studyNo, string srcDirImage, string destDirStorageMedia, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::ExportStudy start: " << studyNo << ", " << srcDirImage << ", " << destDirStorageMedia << endl;
  EDCMReturnStatus ret = GetMe(true)->ExportStudy(studyNo, srcDirImage, destDirStorageMedia, psCallback);
  cout << "MyDCMMan::ExportStudy finish" << endl;

  return ret;
}

vector<string> MyDCMMan::GetAllDevice() {
  cout << "MyDCMMan::GetAllDevice start" << endl;
  vector<string> ret = GetMe(true)->GetAllDevice();
  cout << "MyDCMMan::GetAllDevice finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllMPPSService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllMPPSService start" << endl;
  bool ret = GetMe(true)->GetAllMPPSService(callback, pData);
  cout << "MyDCMMan::GetAllMPPSService finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllQueryRetrieveService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllQueryRetrieveService start" << endl;
  bool ret = GetMe(true)->GetAllQueryRetrieveService(callback, pData);
  cout << "MyDCMMan::GetAllQueryRetrieveService finish" << endl;

  return ret;
}

vector<string> MyDCMMan::GetAllRetrieveDestinationServerDevice() {
  cout << "MyDCMMan::GetAllRetrieveDestinationServerDevice start" << endl;
  vector<string> ret = GetMe(true)->GetAllRetrieveDestinationServerDevice();
  cout << "MyDCMMan::GetAllRetrieveDestinationServerDevice finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllServer(GetSingleServerAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllServer start" << endl;
  bool ret = GetMe(true)->GetAllServer(callback, pData);
  cout << "MyDCMMan::GetAllServer finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllStorageCommitService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllStorageCommitService start" << endl;
  bool ret = GetMe(true)->GetAllStorageCommitService(callback, pData);
  cout << "MyDCMMan::GetAllStorageCommitService finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllStorageService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllStorageService start" << endl;
  bool ret = GetMe(true)->GetAllStorageService(callback, pData);
  cout << "MyDCMMan::GetAllStorageService finish" << endl;

  return ret;
}

bool MyDCMMan::GetAllWorklistService(GetSingleServiceAttribute callback, void* pData) {
  cout << "MyDCMMan::GetAllWorklistService start" << endl;
  bool ret = GetMe(true)->GetAllWorklistService(callback, pData);
  cout << "MyDCMMan::GetAllWorklistService finish" << endl;

  return ret;
}

string MyDCMMan::GetDefaultMPPSServiceDevice() {
  cout << "MyDCMMan::GetDefaultMPPSServiceDevice start" << endl;
  string ret = GetMe(true)->GetDefaultMPPSServiceDevice();
  cout << "MyDCMMan::GetDefaultMPPSServiceDevice finish" << endl;

  return ret;
}

string MyDCMMan::GetDefaultQueryRetrieveServiceDevice() {
  cout << "MyDCMMan::GetDefaultQueryRetrieveServiceDevice start" << endl;
  string ret = GetMe(true)->GetDefaultQueryRetrieveServiceDevice();
  cout << "MyDCMMan::GetDefaultQueryRetrieveServiceDevice finish" << endl;

  return ret;
}

string MyDCMMan::GetDefaultStorageCommitServiceDevice() {
  cout << "MyDCMMan::GetDefaultStorageCommitServiceDevice start" << endl;
  string ret = GetMe(true)->GetDefaultStorageCommitServiceDevice();
  cout << "MyDCMMan::GetDefaultStorageCommitServiceDevice finish" << endl;

  return ret;
}

string MyDCMMan::GetDefaultStorageServiceDevice() {
  cout << "MyDCMMan::GetDefaultStorageServiceDevice start" << endl;
  string ret = GetMe(true)->GetDefaultStorageServiceDevice();
  cout << "MyDCMMan::GetDefaultStorageServiceDevice finish" << endl;

  return ret;
}

string MyDCMMan::GetDefaultWorklistServiceDevice() {
  cout << "MyDCMMan::GetDefaultWorklistServiceDevice start" << endl;
  string ret = GetMe(true)->GetDefaultWorklistServiceDevice();
  cout << "MyDCMMan::GetDefaultWorklistServiceDevice finish" << endl;

  return ret;
}

string MyDCMMan::GetLocalAE() {
  cout << "MyDCMMan::GetLocalAE start" << endl;
  string ret = GetMe(true)->GetLocalAE();
  cout << "MyDCMMan::GetLocalAE finish" << endl;

  return ret;
}

int MyDCMMan::GetLocalPort() {
  cout << "MyDCMMan::GetLocalPort start" << endl;
  int ret = GetMe(true)->GetLocalPort();
  cout << "MyDCMMan::GetLocalPort finish" << endl;

  return ret;
}

bool MyDCMMan::GetStudyBackupStatus(unsigned int studyNo) {
  cout << "MyDCMMan::GetStudyBackupStatus start: " << studyNo << endl;
  bool ret = GetMe(true)->GetStudyBackupStatus(studyNo);
  cout << "MyDCMMan::GetStudyBackupStatus finish" << endl;

  return ret;
}

EDCMReturnStatus MyDCMMan::ImportStudy(int studyIndex, string destDirStorageMedia,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::ImportStudy start: " << studyIndex << ", " << destDirStorageMedia << endl;
  EDCMReturnStatus ret = GetMe(true)->ImportStudy(studyIndex, destDirStorageMedia, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
  cout << "MyDCMMan::ImportStudy finish" << endl;

  return ret;
}

bool MyDCMMan::IsExistedWorklistStudy(DCMWORKLISTELEMENT wlElement) {
  cout << "MyDCMMan::IsExistedWorklistStudy start" << endl;
  bool ret = GetMe(true)->IsExistedWorklistStudy(wlElement);
  cout << "MyDCMMan::IsExistedWorklistStudy finish" << endl;

  return ret;
}

bool MyDCMMan::Ping(string ip) {
  cout << "MyDCMMan::Ping start: " << ip << endl;
  bool ret = GetMe(true)->Ping((char*)ip.c_str());
  cout << "MyDCMMan::Ping finish" << endl;

  return ret;
}

EDCMReturnStatus MyDCMMan::Query(string patientName, string patientID,
  string accessionNumber, string startStudyDate, string endStudyDate, vector<DCMQRQUERYRES>& vQueryRes) {
  cout << "MyDCMMan::Query start: " << patientName << ", " << patientID << ", "
    << accessionNumber << ", " << startStudyDate << ", " << endStudyDate << endl;
  EDCMReturnStatus ret = GetMe(true)->Query(patientName, patientID, accessionNumber, startStudyDate, endStudyDate, vQueryRes);
  cout << "MyDCMMan::Query finish" << endl;

  return ret;
}

vector<DCMWORKLISTELEMENT> MyDCMMan::QueryWorklist(string patientID, string patientName,
  string accessionNumber, string requestedProcedureID, string startDate, string endDate) {
  cout << "MyDCMMan::QueryWorklist start: " << patientID << ", " << patientName << ", "
    << accessionNumber << ", " << requestedProcedureID << ", " << startDate << ", " << endDate << endl;
  vector<DCMWORKLISTELEMENT> ret = GetMe(true)->QueryWorklist(patientID, patientName, accessionNumber, requestedProcedureID, startDate, endDate);
  cout << "MyDCMMan::QueryWorklist finish" << endl;

  return ret;
}

EDCMReturnStatus MyDCMMan::Retrieve(unsigned int itemIndex, string destDevice,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::Retrieve start: " << itemIndex << ", " << destDevice << endl;
  EDCMReturnStatus ret = GetMe(true)->Retrieve(itemIndex, destDevice, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
  cout << "MyDCMMan::Retrieve finish" << endl;

  return ret;
}

bool MyDCMMan::SendStudy(unsigned int studyNo, string srcDirImage, string destDirImage,
  bool onStorageCommitment, bool enableStorageSR, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback) {
  cout << "MyDCMMan::SendStudy start: " << studyNo << ", " << srcDirImage << ", " << destDirImage << ", "
    << onStorageCommitment << ", " << enableStorageSR << ", " << enableStorageMultiFrame << ", " << maxFrameNumber << endl;
  bool ret = GetMe(true)->SendStudy(studyNo, srcDirImage, destDirImage,
    onStorageCommitment, enableStorageSR, enableStorageMultiFrame, maxFrameNumber, psCallback);
  cout << "MyDCMMan::SendStudy finish" << endl;

  return ret;
}

bool MyDCMMan::SetDefaultMPPSService(string device) {
  cout << "MyDCMMan::SetDefaultMPPSService start: " << device << endl;
  bool ret = GetMe(true)->SetDefaultMPPSService(device);
  cout << "MyDCMMan::SetDefaultMPPSService finish" << endl;

  return ret;
}

bool MyDCMMan::SetDefaultQueryRetrieveService(string device) {
  cout << "MyDCMMan::SetDefaultQueryRetrieveService start: " << device << endl;
  bool ret = GetMe(true)->SetDefaultQueryRetrieveService(device);
  cout << "MyDCMMan::SetDefaultQueryRetrieveService finish" << endl;

  return ret;
}

bool MyDCMMan::SetDefaultStorageCommitService(string device) {
  cout << "MyDCMMan::SetDefaultStorageCommitService start: " << device << endl;
  bool ret = GetMe(true)->SetDefaultStorageCommitService(device);
  cout << "MyDCMMan::SetDefaultStorageCommitService finish" << endl;

  return ret;
}

bool MyDCMMan::SetDefaultStorageService(string device) {
  cout << "MyDCMMan::SetDefaultStorageService start: " << device << endl;
  bool ret = GetMe(true)->SetDefaultStorageService(device);
  cout << "MyDCMMan::SetDefaultStorageService finish" << endl;

  return ret;
}

bool MyDCMMan::SetDefaultWorklistService(string device) {
  cout << "MyDCMMan::SetDefaultWorklistService start: " << device << endl;
  bool ret = GetMe(true)->SetDefaultWorklistService(device);
  cout << "MyDCMMan::SetDefaultWorklistService finish" << endl;

  return ret;
}

void MyDCMMan::SetLocalAE(string strAE) {
  cout << "MyDCMMan::SetLocalAE start: " << strAE << endl;
  GetMe(true)->SetLocalAE(strAE);
  cout << "MyDCMMan::SetLocalAE finish" << endl;
}

void MyDCMMan::SetLocalPort(int port) {
  cout << "MyDCMMan::SetLocalPort start: " << port << endl;
  GetMe(true)->SetLocalPort(port);
  cout << "MyDCMMan::SetLocalPort finish" << endl;
}

void MyDCMMan::SetSRVerifierName(string name) {
  cout << "MyDCMMan::SetSRVerifierName start: " << name << endl;
  GetMe(true)->SetSRVerifierName(name);
  cout << "MyDCMMan::SetSRVerifierName finish" << endl;
}

void MyDCMMan::SetSRVerifyFlag(bool flag) {
  cout << "MyDCMMan::SetSRVerifyFlag start: " << flag << endl;
  GetMe(true)->SetSRVerifyFlag(flag);
  cout << "MyDCMMan::SetSRVerifyFlag finish" << endl;
}

void MyDCMMan::SetSRVerifyOrganization(string organization) {
  cout << "MyDCMMan::SetSRVerifyOrganization start: " << organization << endl;
  GetMe(true)->SetSRVerifyOrganization(organization);
  cout << "MyDCMMan::SetSRVerifyOrganization finish" << endl;
}

bool MyDCMMan::StartMPPS(DCMMPPSELEMENT mppsElement) {
  cout << "MyDCMMan::StartMPPS start" << endl;
  bool ret = GetMe(true)->StartMPPS(mppsElement);
  cout << "MyDCMMan::StartMPPS finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkDefaultWorklist() {
  cout << "MyDCMMan::TestLinkDefaultWorklist start" << endl;
  bool ret = GetMe(true)->TestLinkDefaultWorklist();
  cout << "MyDCMMan::TestLinkDefaultWorklist finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkMPPS(string device) {
  cout << "MyDCMMan::TestLinkMPPS start: " << device << endl;
  bool ret = GetMe(true)->TestLinkMPPS(device);
  cout << "MyDCMMan::TestLinkMPPS finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkQueryRetrieve(string device) {
  cout << "MyDCMMan::TestLinkQueryRetrieve start: " << device << endl;
  bool ret = GetMe(true)->TestLinkQueryRetrieve(device);
  cout << "MyDCMMan::TestLinkQueryRetrieve finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkStorage(string device) {
  cout << "MyDCMMan::TestLinkStorage start: " << device << endl;
  bool ret = GetMe(true)->TestLinkStorage(device);
  cout << "MyDCMMan::TestLinkStorage finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkStorageCommit(string device) {
  cout << "MyDCMMan::TestLinkStorageCommit start: " << device << endl;
  bool ret = GetMe(true)->TestLinkStorageCommit(device);
  cout << "MyDCMMan::TestLinkStorageCommit finish" << endl;

  return ret;
}

bool MyDCMMan::TestLinkWorklist(string device) {
  cout << "MyDCMMan::TestLinkWorklist start: " << device << endl;
  bool ret = GetMe(true)->TestLinkWorklist(device);
  cout << "MyDCMMan::TestLinkWorklist finish" << endl;

  return ret;
}
