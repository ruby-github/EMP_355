#include "utils/CalcTime.h"
#include "utils/IniFile.h"

#include "probe/ProbeMan.h"
#include "Def.h"
#include "probe/ProbeList.h"
#include "imageControl/FpgaGeneral.h"

using namespace std;

ProbeMan* ProbeMan::m_ptrInstance = NULL;

const int ProbeMan::CMD_READ_PARA[MAX_SOCKET] = {0x2, 0x2, 0x2};
const int ProbeMan::SOCKET_ADDR[MAX_SOCKET] = {0x0, 0x2, 0x1};

extern bool g_init;

ProbeMan::ProbeMan() {
    ProbeSocket::ProbePara para;
    int i;
    int size;

    para.exist = TRUE;

    para.model[0] = '3';
    para.model[1] = '5';
    para.model[2] = 'C';
    para.model[3] = '5';
    para.model[4] = '0';
    para.model[5] = 'L';

    para.model[6] = 0;
    para.type = 'C';

    para.width = 7200;
    para.array = 128;
    para.lines = 256;
    para.r = 50;

    para.widthPhase = 0;
    para.depth = 300;
    para.freqRange[0] = 40;
    para.freqRange[1] = 120;
    para.freqDop[0] = 54;
    para.freqDop[1] = 66;
    para.vecFreqRange.clear();
    size = 7;
    for (i = 0; i < size; i ++) {
        para.vecFreqRange.push_back(ProbeSocket::FREQ2D[0][i]);
    }
    for (i = 0; i < ProbeSocket::MAX_HARMONIC_FREQ; i++) {
        para.thiFreq[i] = ProbeSocket::FREQ_THI[0][i];
    }
    para.exist = TRUE;

    ///> init all sockets
    ProbeSocket socket0(SOCKET_ADDR[0], CMD_READ_PARA[0]);
    //socket0.SetProbePara(para);
    m_vecSockets.push_back(socket0);

    if (MAX_SOCKET > 1) {
        ProbeSocket socket1(SOCKET_ADDR[1], CMD_READ_PARA[1]);
        //socket1.SetProbePara(para);
        m_vecSockets.push_back(socket1);
    }

    if (MAX_SOCKET > 2) {
        ProbeSocket socket2(SOCKET_ADDR[2], CMD_READ_PARA[2]);
        //socket2.SetProbePara(para);
        m_vecSockets.push_back(socket2);
    }

    if (MAX_SOCKET > 3) {
        ProbeSocket socket3(SOCKET_ADDR[3], CMD_READ_PARA[3]);
        //socket3.SetProbePara(para);
        m_vecSockets.push_back(socket3);
    }

    ///> create default socket, used when no probe is exist in all sockets
    //35C50K

    //四个座都没有读取到探头时，默认读取的探头座.此时读取不到探头参数，因为cmd的数据设置为0xff了
    ProbeSocket ps(SOCKET_ADDR[2], 0Xff); //default socket is socket0, and can not read probe para(read command set to 0xff)
    //默认设置的该探头座上的参数
    ps.SetProbePara(para);
    m_vecSockets.push_back(ps);

    m_defaultSocket = MAX_SOCKET;
}

ProbeMan::~ProbeMan() {
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ProbeMan* ProbeMan::GetInstance() {
    if (m_ptrInstance == NULL) {
        m_ptrInstance = new ProbeMan;
    }

    return (m_ptrInstance);
}

void ProbeMan::ActiveHV(bool on) {
    PRINTF("enter the probeMan high value = %d\n",on);
    m_vecSockets[m_curSocket].ActiveHV(on);
}

void ProbeMan::GetCurProbe(ProbeSocket::ProbePara &para) {
    m_vecSockets[m_curSocket].GetProbePara(para);
}

void ProbeMan::GetDefaultProbe(ProbeSocket::ProbePara &para) {
    m_vecSockets[MAX_SOCKET].GetProbePara(para);
}

void ProbeMan::GetAllProbe(ProbeSocket::ProbePara para[MAX_SOCKET]) {
    char defaultModel[256];
    if (g_init) {
        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        strcpy(defaultModel, ReadDefaultProbe(&ini).c_str());
    }

    m_vecSockets[2].ActiveSocket();
    usleep(430000);//

    int i;
    for (i = 0; i < MAX_SOCKET; i ++) {

        m_vecSockets[i].ActiveSocket();
        usleep(430000);//

        m_vecSockets[i].ReadProbePara();
        m_vecSockets[i].GetProbePara(para[i]);
        if (g_init) {
            if (strcmp(defaultModel, para[i].model) == 0)
                m_defaultSocket = i;
        }
    }

    if ((para[0].type==para[1].type)&&(para[1].type==para[2].type)) {     //the same as one probe addr
        para[1].exist=false;
        para[2].exist=false;
    }
}

void ProbeMan::GetOneProbe(ProbeSocket::ProbePara para[MAX_SOCKET], int socket) {
    m_vecSockets[socket].ActiveSocket();
    usleep(900000); //900ms
    m_vecSockets[socket].ReadProbePara();
    m_vecSockets[socket].GetProbePara(para[socket]);
}

int ProbeMan::SetProbeSocket(int socket) {
    ///> select socket
    if (socket <= MAX_SOCKET) {
        m_curSocket = socket;
    } else {
        PRINTF("ProbeMan: socket is out of range\n");
        return FALSE;
    }

    m_vecSockets[m_curSocket].ActiveSocket();
    return TRUE;
}

void ProbeMan::WriteProbeManual() {
    unsigned char* buf = NULL;

    buf = Probe35C50K;

    m_vecSockets[0].ActiveSocket();
    usleep(100000);

    int i;
    for (i = 0; i < MAX_SOCKET; i ++) {
        m_vecSockets[i].WriteProbePara(buf, PROBE_INFO_SIZE);
        usleep(100000);
    }
    PRINTF("end Write Probe!\n");
}

void ProbeMan::WriteProbe(int type) {
    unsigned char* buf = NULL;

    switch((EProbeList355)type) {
    case P35C50L:
        buf = Probe35C50L;
        break;

    case P35C20I:
        buf = Probe35C20I;
        break;

    case P70L40J:
        buf = Probe70L40J;
        break;

    case P75L40J:
        buf = Probe75L40J;
        break;

    case P70L60J:
        buf = Probe70L60J;
        break;

    case P90L40J_1:
        buf = Probe90L40J_1;
        break;

    case P65C10L:
        buf = Probe65C10L;
        break;

    case P65C15E_80:
        buf = Probe65C15E_80;
        break;

    case P65C15E_128:
        buf = Probe65C15E_128;
        break;

    case P30P16B:
        buf = Probe30P16B;
        break;

    case P10L25K_1:
        buf = Probe10L25K_1;
        break;

    case P65C10I:
        buf = Probe65C10I;
        break;
    default:
        buf = Probe35C50L;
        break;
    }

    PRINTF("write probe: buf = %s, size = %d\n", buf, PROBE_INFO_SIZE);
    m_vecSockets[0].ActiveSocket();
    usleep(100000);

    int i;
    for (i = 0; i < MAX_SOCKET; i ++) {
        m_vecSockets[i].WriteProbePara(buf, PROBE_INFO_SIZE);
        usleep(100000);
    }
}

void ProbeMan::WriteDefaultProbe(const char* probeModel, IniFile* ptrIni) {
    // write default probe from file
    ptrIni->WriteString("ProbeModel", "ProbeModel", probeModel);
    ptrIni->SyncConfigFile();
}

string ProbeMan::ReadDefaultProbe(IniFile* ptrIni) {
    // read default probe from file
    PRINTF("probemodel:%s\n", ptrIni->ReadString("ProbeModel", "ProbeModel").c_str());
    return (ptrIni->ReadString("ProbeModel", "ProbeModel"));
}

string ProbeMan::VerifyProbeName(string fromType) {
    // "35C50L",  "70L40J", "65C10L", "35C20I", "65C15E", "30P16B", "10L25K"
    // "C35",     "L75",    "C65",    "C35-20", "C65-15", "P30",    "L10"

    string probe_name = string(fromType);

    if (probe_name == "35C50L") {
        probe_name = "C35";
    } else if (probe_name == "70L40J") {
        probe_name = "L75";
    } else if (probe_name == "65C10L") {
        probe_name = "C65";
    } else if (probe_name == "35C20I") {
        probe_name = "C35-20";
    } else if (probe_name == "65C15E") {
        probe_name = "C65-15";
    } else if (probe_name == "30P16B") {
        probe_name = "P30";
    } else if (probe_name == "10L25K") {
        probe_name = "L10";
    } else {
    }

    return probe_name;
}
