#include <iostream>
#include <signal.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "sysMan/SysLog.h"
#include "Init.h"
#include "periDevice/IoCtrl.h"
#include "imageProc/FpgaReceive.h"
#include "probe/ExamItem.h"
#include "probe/ProbeMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/UpgradeMan.h"
#include "display/gui_func.h"
#include <UIS4DReceiveInterface.h>
#include "imageControl/FpgaGeneral.h"
#include "sysMan/ViewSystem.h"
#include "periDevice/ManRegister.h"

#include "utils/FakeXUtils.h"

using namespace std;

pthread_t pidDsc;

extern int g_shm_id0;
extern int g_shm_id1;
extern int g_sem_id0;
extern int g_sem_id1;
extern unsigned char* g_pshm_data0;
extern unsigned char* g_pshm_data1;

extern CUIS4DReceiveInterface g_uis4d_ri;

bool g_printProcessFps = false;

void* ThreadDsc(void* argv) {
  cout << "Hello! I am thread dsc\n";

  FpgaReceive fr;
  fr.UsbServiceRoutine(argv);

  return NULL;
}

void dump(int signo) {
  char buf[1024];
  char cmd[1024];
  FILE *fh;

  snprintf(buf, sizeof(buf), "/proc/%d/cmdline", getpid());
  if(!(fh = fopen(buf, "r"))) {
    exit(0);
  }

  if(!fgets(buf, sizeof(buf), fh)) {
    exit(0);
  }

  fclose(fh);

  if(buf[strlen(buf) - 1] == '\n') {
    buf[strlen(buf) - 1] = '\0';
  }

  snprintf(cmd, sizeof(cmd), "gdb %s %d", buf, getpid());
  _system_(cmd);

  exit(0);
}

#include "periDevice/Printmain.h"

void IngPipe(int signo) {
  int id = getpid();
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char* argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "PrintProcessFps") == 0) {
      g_printProcessFps = true;
      printf("==================printf:%s\n", argv[1]);
    }
  }

  signal(SIGSEGV, SIG_IGN);
  signal(SIGPIPE, &IngPipe);

  Init init;
  init.WriteLogHead();

  SysLog* sysLog = SysLog::Instance();
  (*sysLog) << "main: Sytem init!" << endl;
  init.SystemInit(argc, argv);

  if(!g_thread_supported()) {
    g_thread_init(NULL);
  }

  gdk_threads_init();
  gtk_init(&argc, &argv);

  SetTheme(RC_PATH);

  ChangeKeymap();
  init_colors();

  (*sysLog) << "main: Run gui, create main view!" << endl;
  init.CreatMainWindow();

  init.ParaInit();

  (*sysLog) << "main: Probe init!" << endl;
  init.ProbeCheck();

  (*sysLog) << "main: Start up DSC thread!" << endl;

  if(pthread_create(&pidDsc, NULL, ThreadDsc, NULL) != 0) {
    cerr << "main: Fail to create DSC thread!" << endl;
  }

  g_uis4d_ri.IPC4DInit();
  g_shm_id0 = g_uis4d_ri.GetSHMID(0);
  g_pshm_data0 = g_uis4d_ri.GetSHMData(0);
  g_sem_id0 = g_uis4d_ri.GetSEMID(0);

  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  (*sysLog) << "main: Exit main, and do something clear!" << endl;

  g_uis4d_ri.IPC4DDestroy();

  init.ExitSystem();
}
