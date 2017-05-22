#ifndef PROBEMAN_H
#define PROBEMAN_H

#include "utils/IniFile.h"


#include "probe/ProbeSocket.h"

#define MAX_SOCKET 3

class ProbeMan {
public:
    ///> construct
    ~ProbeMan();
    static ProbeMan* GetInstance();


    ///> general member
    void ActiveHV(bool on);
    void GetCurProbe(ProbeSocket::ProbePara &para);
    void GetDefaultProbe(ProbeSocket::ProbePara &para);
    void GetAllProbe(ProbeSocket::ProbePara para[MAX_SOCKET]);
    void GetOneProbe(ProbeSocket::ProbePara para[MAX_SOCKET], int socket);
    int SetProbeSocket(int socket);

    void WriteProbeManual();
    void WriteProbe(int type);
    int GetCurProbeSocket() {
        return m_curSocket;
    }

    bool IsProbeExist() {
        if(m_curSocket < MAX_SOCKET)
            return TRUE;
        else
            return FALSE;
    }
    /**
     * get the default socket when open machine.
     */
    int GetDefaultProbeSocket() {
        return m_defaultSocket;
    }
    void WriteDefaultProbe(const char *probeModel, IniFile* ptrIni);
    string VerifyProbeName(string fromType);

private:
    ProbeMan();
    string ReadDefaultProbe(IniFile* ptrIni);

    static ProbeMan* m_ptrInstance;
    static const int SOCKET_ADDR[MAX_SOCKET];
    static const int CMD_READ_PARA[MAX_SOCKET];

    //static ProbeMan* m_ptrInstance;
    vector<ProbeSocket> m_vecSockets;
    int m_curSocket;
    int m_defaultSocket;
};

#endif
