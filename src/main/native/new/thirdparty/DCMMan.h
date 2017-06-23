#ifndef __DCM_MAN_H__
#define __DCM_MAN_H__

#include "thirdparty/DCMDef.h"

#include <string>
#include <vector>

using namespace std;

class CDCMMan {
public:
  static CDCMMan* GetMe();
  static void Destroy();
  static void Create(string dcmDatabaseDir);

  bool Ping(char* ip);

  // these test link functons is general for all service.
  bool TestLinkStorage(string device);
  bool TestLinkWorklist(string device);
  bool TestLinkMPPS(string device);
  bool TestLinkStorageCommit(string device);
  bool TestLinkQueryRetrieve(string device);

  // below test link functions is for default service.
  bool TestLinkDefaultStorage();
  bool TestLinkDefaultWorklist();
  bool TestLinkDefaultMPPS();
  bool TestLinkDefaultStorageCommit();
  bool TestLinkDefaultQueryRetrieve();

  void EditStudyInfo(DCMSTUDYELEMENT element);
  bool EndStudy();
  void AddImage(DCMIMAGEELEMENT element);
  void EditSR(DCMSRELEMENT element);
  void DeleteImageBeforeEndStudy(string filename);
  bool DeleteImageAfterEndStudy(unsigned int studyNo, string filename);
  bool DeleteStudy(unsigned int studyNo);

  // onStorageCommitment Enable send storage commitment while send image and sr.
  // enableStorageSR Enalbe send sr while send image.
  // enableStorageMultiFrame If true that allow send multiply frame image(as cine).
  // maxFrameNumber When enableStorageMultiFrame is true, this parameter is valid, otherwise this parameter will omit. This parameter limit max frame number of mulitframe image that will send.
  bool SendStudy(unsigned int studyNo, string srcDirImage, string destDirImage, bool onStorageCommitment, bool enableStorageSR, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);
  bool SendImageBeforeEndStudy(string imageFilename, string srcDirImage, string destDirImage, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);
  bool SendImageAfterEndStudy(unsigned int studyNo, string imageFilename, string srcDirImage, string destDirImage, bool enableStorageMultiFrame, int maxFrameNumber, PROGRESSSTATUS psCallback);
  bool GetStudyBackupStatus(unsigned int studyNo);

  // If set verify flag to true, then the verifier name and verify organization must not be empty string.
  void SetSRVerifyFlag(bool flag);
  void SetSRVerifierName(string name);
  void SetSRVerifyOrganization(string organization);

  void SetLocalAE(string strAE);
  void SetLocalPort(int port);
  string GetLocalAE();
  int GetLocalPort();

  bool AddServer(string device, string ip);
  bool DeleteServer(string device);
  bool GetAllServer(GetSingleServerAttribute callback, void* pData);
  bool AddStorageService(string device, string serviceName, string aeTitle, int port);
  bool AddWorklistService(string device, string serviceName, string aeTitle, int port);
  bool AddMPPSService(string device, string serviceName, string aeTitle, int port);
  bool AddStorageCommitService(string device, string serviceName, string aeTitle, int port);
  bool AddQueryRetrieveService(string device, string serviceName, string aeTitle, int port);
  bool DeleteStorageService(string device);
  bool DeleteWorklistService(string device);
  bool DeleteMPPSService(string device);
  bool DeleteStorageCommitService(string device);
  bool DeleteQueryRetrieveService(string device);
  bool SetDefaultStorageService(string device);
  bool SetDefaultWorklistService(string device);
  bool SetDefaultMPPSService(string device);
  bool SetDefaultStorageCommitService(string device);
  bool SetDefaultQueryRetrieveService(string device);
  string GetDefaultStorageServiceDevice();
  string GetDefaultWorklistServiceDevice();
  string GetDefaultMPPSServiceDevice();
  string GetDefaultStorageCommitServiceDevice();
  string GetDefaultQueryRetrieveServiceDevice();
  bool GetAllStorageService(GetSingleServiceAttribute callback, void* pData);
  bool GetAllWorklistService(GetSingleServiceAttribute callback, void* pData);
  bool GetAllMPPSService(GetSingleServiceAttribute callback, void* pData);
  bool GetAllStorageCommitService(GetSingleServiceAttribute callback, void* pData);
  bool GetAllQueryRetrieveService(GetSingleServiceAttribute callback, void* pData);
  vector<string> GetAllDevice();

  // startDate format:YYYYMMDD
  // endDate format:YYYYMMDD
  vector<DCMWORKLISTELEMENT> QueryWorklist(string patientID, string patientName, string accessionNumber, string requestedProcedureID, string startDate, string endDate);
  vector<DCMWORKLISTELEMENT> AutoQueryWorklist(string startDate, string endDate);

  // return true: This study from worklist is existed, false: not existed. The study from worklist must be only one, don't repeat.
  bool IsExistedWorklistStudy(DCMWORKLISTELEMENT wlElement);

  // Only call StartMPPS after call EditStudyInfo, otherwise this function will return false.
  // Only call this function one time each study.
  bool StartMPPS(DCMMPPSELEMENT mppsElement);

  // Only call EndMPPS after call StartMPPS, otherwise this function will return false.
  // Only call this function one time each study.
  // @param[in] studyEndDate format:YYYYMMDD
  // @param[in] studyEndTime format:HHMMSS
  bool EndMPPS(string studyEndDate, string studyEndTime);

  // Only call CancelMPPS after call StartMPPS, otherwise this function will return false.
  // Only call this function one time each study.
  // studyEndDate format:YYYYMMDD
  // studyEndTime format:HHMMSS
  bool CancelMPPS(string studyEndDate, string studyEndTime);

  // startStudyDate format:YYYYMMDD
  // endStudyDate format:YYYYMMDD
  // return DCMSUCCESS: Success to query.
  // return DCMNODEFALUTSERVER: Don't preset default Query/Retrieve server and service.
  // return DCMCONNECTIONFAILURE: Fail to connect remote server, fail to connect network.
  // return DCMINVALIDVALUE: Inputting parameter is invalid value.
  // return DCMNONSUPPORTSTUDYROOT: Don't support studyroot query.
  EDCMReturnStatus Query(string patientName, string patientID, string accessionNumber, string startStudyDate, string endStudyDate, vector<DCMQRQUERYRES> &vQueryRes);

  // return DCMSUCCESS: Success to retrieve.
  // return DCMNODEFALUTSERVER: Don't preset default Query/Retrieve server and service.
  // return DCMCONNECTIONFAILURE: Fail to connect remote server, fail to connect network.
  // return DCMINVALIDVALUE: Inputting parameter is invalid value.
  // return DCMINVALIDQRDESTSERVE: The retrieve destination server is invalid. Maybe the remote server setting is error.
  // return DCMSTUDYEXISTED: The study record is existed in local, will not retrieve no longer.
  // return DCMRETRIEVEFAILURE: Fail to retrieve.
  EDCMReturnStatus Retrieve(unsigned int itemIndex, string destDevice, GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback);
  vector<string> GetAllRetrieveDestinationServerDevice();

  // return DCMSUCCESS: Success to export study.
  // return DCMINVALIDSTORAGEDEVICE: Inputting storage media is invalid.
  // return DCMSTUDYEXISTED: The study record is already existed in DICOMDIR in storage media, will not export no longer.
  // return DCMNOENOUGHSPACE: The storage media have not enough space.
  // return DCMEXPORTFAILURE: Fail to export.
  EDCMReturnStatus ExportStudy(unsigned int studyNo, string srcDirImage, string destDirStorageMedia, PROGRESSSTATUS psCallback);

  // return DCMSUCCESS: Success to browse DICOMDIR.
  // return DCMINVALIDSTORAGEDEVICE: Inputting storage media is invalid.
  // return DCMNONEXISTDICOMDIR: Non DICOMDIR is existed in storage media.
  EDCMReturnStatus BrowseDICOMDIR(string destDirStorageMedia, vector<DCMDICOMDIRSTUDYLEVEL>& vecStudyLevel);

  // return DCMSUCCESS: Success to import study.
  // return DCMINVALIDVALUE: Inputting parameter is invalid value.
  // return DCMINVALIDSTORAGEDEVICE: Inputting storage media is invalid.
  // return DCMSTUDYEXISTED: The study record is already existed in local, will not import no longer.
  // return DCMNONENTIREDICOMDIR: File(s) lost in DICOMDIR in storage media.
  // return DCMIMPORTFAILURE: Fail to import.
  EDCMReturnStatus ImportStudy(int studyIndex, string destDirStorageMedia, GetDCMStudyElement callbackGetStudy, GetDCMImageElement callbackGetImage, GetDCMSRElement callbackGetSR, PROGRESSSTATUS psCallback);

protected:
  CDCMMan(string dcmDatabaseDir);
  virtual ~CDCMMan();
  static CDCMMan* m_pThis;

private:
  CDCMMan(const CDCMMan& dsc);
  CDCMMan& operator =(const CDCMMan& dsc);
};

#endif
