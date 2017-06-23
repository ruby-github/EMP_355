#include "thirdparty/MyDCMMan.h"

#include <exception>
#include <iostream>

using namespace std;

void MyDCMMan::Create(string dcm_database_dir) {
  try {
    CDCMMan::Create(dcm_database_dir);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::Create: " << dcm_database_dir << endl;
  }
}

void MyDCMMan::Destroy() {
  try {
    CDCMMan::Destroy();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::Destroy" << endl;
  }
}

void MyDCMMan::AddImage(DCMIMAGEELEMENT element) {
  try {
    CDCMMan::GetMe()->AddImage(element);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddImage" << endl;
  }
}

bool MyDCMMan::AddMPPSService(string device, string serviceName, string aeTitle, int port) {
  try {
    return CDCMMan::GetMe()->AddMPPSService(device, serviceName, aeTitle, port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddMPPSService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

    return false;
  }
}

bool MyDCMMan::AddQueryRetrieveService(string device, string serviceName, string aeTitle, int port) {
  try {
    return CDCMMan::GetMe()->AddQueryRetrieveService(device, serviceName, aeTitle, port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddQueryRetrieveService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

    return false;
  }
}

bool MyDCMMan::AddServer(string device, string ip) {
  try {
    return CDCMMan::GetMe()->AddServer(device, ip);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddServer: " << device << ", " << ip << endl;

    return false;
  }
}

bool MyDCMMan::AddStorageCommitService(string device, string serviceName, string aeTitle, int port) {
  try {
    return CDCMMan::GetMe()->AddStorageCommitService(device, serviceName, aeTitle, port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddStorageCommitService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

    return false;
  }
}

bool MyDCMMan::AddStorageService(string device, string serviceName, string aeTitle, int port) {
  try {
    return CDCMMan::GetMe()->AddStorageService(device, serviceName, aeTitle, port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddStorageService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

    return false;
  }
}

bool MyDCMMan::AddWorklistService(string device, string serviceName, string aeTitle, int port) {
  try {
    return CDCMMan::GetMe()->AddWorklistService(device, serviceName, aeTitle, port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::AddWorklistService: " << device << ", " << serviceName << ", " << aeTitle << ", " << port << endl;

    return false;
  }
}

EDCMReturnStatus MyDCMMan::BrowseDICOMDIR(string destDirStorageMedia, vector<DCMDICOMDIRSTUDYLEVEL>& vecStudyLevel) {
  try {
    return CDCMMan::GetMe()->BrowseDICOMDIR(destDirStorageMedia, vecStudyLevel);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::BrowseDICOMDIR: " << destDirStorageMedia << endl;

    return DCMNONEXISTDICOMDIR;
  }
}

void MyDCMMan::DeleteImageBeforeEndStudy(string filename) {
  try {
    CDCMMan::GetMe()->DeleteImageBeforeEndStudy(filename);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteImageBeforeEndStudy: " << filename << endl;
  }
}

bool MyDCMMan::DeleteMPPSService(string device) {
  try {
    return CDCMMan::GetMe()->DeleteMPPSService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteMPPSService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::DeleteQueryRetrieveService(string device) {
  try {
    return CDCMMan::GetMe()->DeleteQueryRetrieveService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteQueryRetrieveService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::DeleteServer(string device) {
  try {
    return CDCMMan::GetMe()->DeleteServer(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteServer: " << device << endl;

    return false;
  }
}

bool MyDCMMan::DeleteStorageCommitService(string device) {
  try {
    return CDCMMan::GetMe()->DeleteStorageCommitService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteStorageCommitService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::DeleteStorageService(string device) {
  try {
    return CDCMMan::GetMe()->DeleteStorageService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteStorageService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::DeleteStudy(unsigned int studyNo) {
  try {
    return CDCMMan::GetMe()->DeleteStudy(studyNo);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteStudy: " << studyNo << endl;

    return false;
  }
}

bool MyDCMMan::DeleteWorklistService(string device) {
  try {
    return CDCMMan::GetMe()->DeleteWorklistService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::DeleteWorklistService: " << device << endl;

    return false;
  }
}

void MyDCMMan::EditSR(DCMSRELEMENT element) {
  try {
    CDCMMan::GetMe()->EditSR(element);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::EditSR" << endl;
  }
}

void MyDCMMan::EditStudyInfo(DCMSTUDYELEMENT element) {
  try {
    CDCMMan::GetMe()->EditStudyInfo(element);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::EditStudyInfo" << endl;
  }
}

bool MyDCMMan::EndMPPS(string studyEndDate, string studyEndTime) {
  try {
    return CDCMMan::GetMe()->EndMPPS(studyEndDate, studyEndTime);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::EndMPPS: " << studyEndDate << ", " << studyEndTime << endl;

    return false;
  }
}

bool MyDCMMan::EndStudy() {
  try {
    return CDCMMan::GetMe()->EndStudy();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::EndStudy" << endl;

    return false;
  }
}

EDCMReturnStatus MyDCMMan::ExportStudy(unsigned int studyNo, string srcDirImage, string destDirStorageMedia, PROGRESSSTATUS psCallback) {
  try {
    return CDCMMan::GetMe()->ExportStudy(studyNo, srcDirImage, destDirStorageMedia, psCallback);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::ExportStudy: " << studyNo << ", " << srcDirImage << ", " << destDirStorageMedia << endl;

    return DCMEXPORTFAILURE;
  }
}

vector<string> MyDCMMan::GetAllDevice() {
  try {
    return CDCMMan::GetMe()->GetAllDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllDevice" << endl;

    vector<string> vec;

    return vec;
  }
}

bool MyDCMMan::GetAllMPPSService(GetSingleServiceAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllMPPSService(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllMPPSService" << endl;

    return false;
  }
}

bool MyDCMMan::GetAllQueryRetrieveService(GetSingleServiceAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllQueryRetrieveService(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllQueryRetrieveService" << endl;

    return false;
  }
}

vector<string> MyDCMMan::GetAllRetrieveDestinationServerDevice() {
  try {
    return CDCMMan::GetMe()->GetAllRetrieveDestinationServerDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllRetrieveDestinationServerDevice" << endl;

    vector<string> vec;

    return vec;
  }
}

bool MyDCMMan::GetAllServer(GetSingleServerAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllServer(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllServer" << endl;

    return false;
  }
}

bool MyDCMMan::GetAllStorageCommitService(GetSingleServiceAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllStorageCommitService(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllStorageCommitService" << endl;

    return false;
  }
}

bool MyDCMMan::GetAllStorageService(GetSingleServiceAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllStorageService(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllStorageService" << endl;

    return false;
  }
}

bool MyDCMMan::GetAllWorklistService(GetSingleServiceAttribute callback, void* pData) {
  try {
    return CDCMMan::GetMe()->GetAllWorklistService(callback, pData);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetAllWorklistService" << endl;

    return false;
  }
}

string MyDCMMan::GetDefaultMPPSServiceDevice() {
  try {
    return CDCMMan::GetMe()->GetDefaultMPPSServiceDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetDefaultMPPSServiceDevice" << endl;

    return "";
  }
}

string MyDCMMan::GetDefaultQueryRetrieveServiceDevice() {
  try {
    return CDCMMan::GetMe()->GetDefaultQueryRetrieveServiceDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetDefaultQueryRetrieveServiceDevice" << endl;

    return "";
  }
}

string MyDCMMan::GetDefaultStorageCommitServiceDevice() {
  try {
    return CDCMMan::GetMe()->GetDefaultStorageCommitServiceDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetDefaultStorageCommitServiceDevice" << endl;

    return "";
  }
}

string MyDCMMan::GetDefaultStorageServiceDevice() {
  try {
    return CDCMMan::GetMe()->GetDefaultStorageServiceDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetDefaultStorageServiceDevice" << endl;

    return "";
  }
}

string MyDCMMan::GetDefaultWorklistServiceDevice() {
  try {
    return CDCMMan::GetMe()->GetDefaultWorklistServiceDevice();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetDefaultWorklistServiceDevice" << endl;

    return "";
  }
}

string MyDCMMan::GetLocalAE() {
  try {
    return CDCMMan::GetMe()->GetLocalAE();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetLocalAE" << endl;

    return "";
  }
}

int MyDCMMan::GetLocalPort() {
  try {
    return CDCMMan::GetMe()->GetLocalPort();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetLocalPort" << endl;

    return 0;
  }
}

bool MyDCMMan::GetStudyBackupStatus(unsigned int studyNo) {
  try {
    return CDCMMan::GetMe()->GetStudyBackupStatus(studyNo);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::GetStudyBackupStatus: " << studyNo << endl;

    return false;
  }
}

EDCMReturnStatus MyDCMMan::ImportStudy(int studyIndex, string destDirStorageMedia,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  try {
    return CDCMMan::GetMe()->ImportStudy(studyIndex, destDirStorageMedia, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::ImportStudy: " << studyIndex << ", " << destDirStorageMedia << endl;

    return DCMIMPORTFAILURE;
  }
}

bool MyDCMMan::IsExistedWorklistStudy(DCMWORKLISTELEMENT wlElement) {
  try {
    return CDCMMan::GetMe()->IsExistedWorklistStudy(wlElement);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::IsExistedWorklistStudy" << endl;

    return false;
  }
}

bool MyDCMMan::Ping(string ip) {
  try {
    return CDCMMan::GetMe()->Ping((char*)ip.c_str());
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::Ping: " << ip << endl;

    return false;
  }
}

EDCMReturnStatus MyDCMMan::Query(string patientName, string patientID,
  string accessionNumber, string startStudyDate, string endStudyDate, vector<DCMQRQUERYRES>& vQueryRes) {
  try {
    return CDCMMan::GetMe()->Query(patientName, patientID, accessionNumber, startStudyDate, endStudyDate, vQueryRes);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::Query: " << patientName << ", " << patientID << ", "
      << accessionNumber << ", " << startStudyDate << ", " << endStudyDate << endl;

    return DCMIMPORTFAILURE;
  }
}

vector<DCMWORKLISTELEMENT> MyDCMMan::QueryWorklist(string patientID, string patientName,
  string accessionNumber, string requestedProcedureID, string startDate, string endDate) {
  try {
    return CDCMMan::GetMe()->QueryWorklist(patientID, patientName, accessionNumber, requestedProcedureID, startDate, endDate);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::QueryWorklist: " << patientID << ", " << patientName << ", "
      << accessionNumber << ", " << requestedProcedureID << ", " << startDate << ", " << endDate << endl;

    vector<DCMWORKLISTELEMENT> vec;

    return vec;
  }
}

EDCMReturnStatus MyDCMMan::Retrieve(unsigned int itemIndex, string destDevice,
  GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback) {
  try {
    return CDCMMan::GetMe()->Retrieve(itemIndex, destDevice, callbackGetStudy, callbackGetImage, callbackGetSR, psCallback);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::Retrieve: " << itemIndex << ", " << destDevice << endl;

    return DCMRETRIEVEFAILURE;
  }
}

bool MyDCMMan::SendStudy(unsigned int studyNo, string srcDirImage, string destDirImage,
  bool onStorageCommitment, bool enableStorageSR, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback) {
  try {
    return CDCMMan::GetMe()->SendStudy(studyNo, srcDirImage, destDirImage,
      onStorageCommitment, enableStorageSR, enableStorageMultiFrame, maxFrameNumber, psCallback);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SendStudy: " << studyNo << ", " << srcDirImage << ", " << destDirImage << ", "
      << onStorageCommitment << ", " << enableStorageSR << ", " << enableStorageMultiFrame << ", " << maxFrameNumber << endl;

    return false;
  }
}

bool MyDCMMan::SetDefaultMPPSService(string device) {
  try {
    return CDCMMan::GetMe()->SetDefaultMPPSService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetDefaultMPPSService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::SetDefaultQueryRetrieveService(string device) {
  try {
    return CDCMMan::GetMe()->SetDefaultQueryRetrieveService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetDefaultQueryRetrieveService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::SetDefaultStorageCommitService(string device) {
  try {
    return CDCMMan::GetMe()->SetDefaultStorageCommitService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetDefaultStorageCommitService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::SetDefaultStorageService(string device) {
  try {
    return CDCMMan::GetMe()->SetDefaultStorageService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetDefaultStorageService: " << device << endl;

    return false;
  }
}

bool MyDCMMan::SetDefaultWorklistService(string device) {
  try {
    return CDCMMan::GetMe()->SetDefaultWorklistService(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetDefaultWorklistService: " << device << endl;

    return false;
  }
}

void MyDCMMan::SetLocalAE(string strAE) {
  try {
    CDCMMan::GetMe()->SetLocalAE(strAE);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetLocalAE: " << strAE << endl;
  }
}

void MyDCMMan::SetLocalPort(int port) {
  try {
    CDCMMan::GetMe()->SetLocalPort(port);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetLocalPort: " << port << endl;
  }
}

void MyDCMMan::SetSRVerifierName(string name) {
  try {
    CDCMMan::GetMe()->SetSRVerifierName(name);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetSRVerifierName: " << name << endl;
  }
}

void MyDCMMan::SetSRVerifyFlag(bool flag) {
  try {
    CDCMMan::GetMe()->SetSRVerifyFlag(flag);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetSRVerifyFlag: " << flag << endl;
  }
}

void MyDCMMan::SetSRVerifyOrganization(string organization) {
  try {
    CDCMMan::GetMe()->SetSRVerifyOrganization(organization);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::SetSRVerifyOrganization: " << organization << endl;
  }
}

bool MyDCMMan::StartMPPS(DCMMPPSELEMENT mppsElement) {
  try {
    return CDCMMan::GetMe()->StartMPPS(mppsElement);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::StartMPPS" << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkDefaultWorklist() {
  try {
    return CDCMMan::GetMe()->TestLinkDefaultWorklist();
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkDefaultWorklist" << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkMPPS(string device) {
  try {
    return CDCMMan::GetMe()->TestLinkMPPS(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkMPPS: " << device << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkQueryRetrieve(string device) {
  try {
    return CDCMMan::GetMe()->TestLinkQueryRetrieve(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkQueryRetrieve: " << device << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkStorage(string device) {
  try {
    return CDCMMan::GetMe()->TestLinkStorage(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkStorage: " << device << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkStorageCommit(string device) {
  try {
    return CDCMMan::GetMe()->TestLinkStorageCommit(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkStorageCommit: " << device << endl;

    return false;
  }
}

bool MyDCMMan::TestLinkWorklist(string device) {
  try {
    return CDCMMan::GetMe()->TestLinkWorklist(device);
  } catch (exception e) {
    cerr << "[ERROR] MyDCMMan::TestLinkWorklist: " << device << endl;

    return false;
  }
}
