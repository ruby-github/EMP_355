#ifndef __MY_DCM_MAN_H__
#define __MY_DCM_MAN_H__

#include "thirdparty/DCMMan.h"

class MyDCMMan {
public:
  static void Create(string dcm_database_dir);
  static void Destroy();

  static void AddImage(DCMIMAGEELEMENT element);
  static bool AddMPPSService(string device, string serviceName, string aeTitle, int port);
  static bool AddQueryRetrieveService(string device, string serviceName, string aeTitle, int port);
  static bool AddServer(string device, string ip);
  static bool AddStorageCommitService(string device, string serviceName, string aeTitle, int port);
  static bool AddStorageService(string device, string serviceName, string aeTitle, int port);
  static bool AddWorklistService(string device, string serviceName, string aeTitle, int port);

  static EDCMReturnStatus BrowseDICOMDIR(string destDirStorageMedia, vector<DCMDICOMDIRSTUDYLEVEL> &vecStudyLevel);

  static void DeleteImageBeforeEndStudy(string filename);
  static bool DeleteMPPSService(string device);
  static bool DeleteQueryRetrieveService(string device);
  static bool DeleteServer(string device);
  static bool DeleteStorageCommitService(string device);
  static bool DeleteStorageService(string device);
  static bool DeleteStudy(unsigned int studyNo);
  static bool DeleteWorklistService(string device);

  static void EditSR(DCMSRELEMENT element);
  static void EditStudyInfo(DCMSTUDYELEMENT element);
  static bool EndMPPS(string studyEndDate, string studyEndTime);
  static bool EndStudy();

  static EDCMReturnStatus ExportStudy(unsigned int studyNo, string srcDirImage, string destDirStorageMedia, PROGRESSSTATUS psCallback);

  static vector<string> GetAllDevice();
  static bool GetAllMPPSService(GetSingleServiceAttribute callback, void *pData);
  static bool GetAllQueryRetrieveService(GetSingleServiceAttribute callback, void *pData);
  static vector<string> GetAllRetrieveDestinationServerDevice();
  static bool GetAllServer(GetSingleServerAttribute callback, void *pData);
  static bool GetAllStorageCommitService(GetSingleServiceAttribute callback, void *pData);
  static bool GetAllStorageService(GetSingleServiceAttribute callback, void *pData);
  static bool GetAllWorklistService(GetSingleServiceAttribute callback, void *pData);

  static string GetDefaultMPPSServiceDevice();
  static string GetDefaultQueryRetrieveServiceDevice();
  static string GetDefaultStorageCommitServiceDevice();
  static string GetDefaultStorageServiceDevice();
  static string GetDefaultWorklistServiceDevice();
  static string GetLocalAE();
  static int GetLocalPort();
  static bool GetStudyBackupStatus(unsigned int studyNo);

  static EDCMReturnStatus ImportStudy(int studyIndex, string destDirStorageMedia,
    GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback);
  static bool IsExistedWorklistStudy(DCMWORKLISTELEMENT wlElement);
  static bool Ping(string ip);
  static EDCMReturnStatus Query(string patientName, string patientID,
    string accessionNumber, string startStudyDate, string endStudyDate, vector<DCMQRQUERYRES> &vQueryRes);
  static vector<DCMWORKLISTELEMENT> QueryWorklist(string patientID,
    string patientName, string accessionNumber, string requestedProcedureID, string startDate, string endDate);
  static EDCMReturnStatus Retrieve(unsigned int itemIndex, string destDevice,
    GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback);
  static bool SendStudy(unsigned int studyNo, string srcDirImage, string destDirImage,
    bool onStorageCommitment, bool enableStorageSR, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);

  static bool SetDefaultMPPSService(string device);
  static bool SetDefaultQueryRetrieveService(string device);
  static bool SetDefaultStorageCommitService(string device);
  static bool SetDefaultStorageService(string device);
  static bool SetDefaultWorklistService(string device);
  static void SetLocalAE(string strAE);
  static void SetLocalPort(int port);
  static void SetSRVerifierName(string name);
  static void SetSRVerifyFlag(bool flag);
  static void SetSRVerifyOrganization(string organization);

  static bool StartMPPS(DCMMPPSELEMENT mppsElement);

  static bool TestLinkDefaultWorklist();
  static bool TestLinkMPPS(string device);
  static bool TestLinkQueryRetrieve(string device);
  static bool TestLinkStorage(string device);
  static bool TestLinkStorageCommit(string device);
  static bool TestLinkWorklist(string device);

  /*
  static bool TestLinkDefaultStorage();
  static bool TestLinkDefaultMPPS();
  static bool TestLinkDefaultStorageCommit();
  static bool TestLinkDefaultQueryRetrieve();
  static bool DeleteImageAfterEndStudy(unsigned int studyNo, string filename);
  static bool SendImageBeforeEndStudy(string imageFilename, string srcDirImage, string destDirImage, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);
  static bool SendImageAfterEndStudy(unsigned int studyNo, string imageFilename, string srcDirImage, string destDirImage, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);
  static vector<DCMWORKLISTELEMENT> AutoQueryWorklist(string startDate, string endDate);
  static bool CancelMPPS(string studyEndDate, string studyEndTime);
  */
};

#endif
