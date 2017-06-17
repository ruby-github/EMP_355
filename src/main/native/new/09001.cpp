#include "imageProc/FpgaReceive.h"
#include "Init.h"
#include "sysMan/SysLog.h"
#include "utils/FakeXUtils.h"
#include "utils/Utils.h"

#include <pthread.h>
#include <signal.h>
#include <UIS4DReceiveInterface.h>

bool g_printProcessFps = false;

extern int g_shm_id0;
extern int g_sem_id0;
extern unsigned char* g_pshm_data0;
extern CUIS4DReceiveInterface g_uis4d_ri;

pthread_t pidDsc;

#include "display/gui_func.h"

// ---------------------------------------------------------

void* ThreadDsc(void* argv) {
  cout << "Hello! I am thread dsc." << endl;

  FpgaReceive fr;
  fr.UsbServiceRoutine(argv);

  return NULL;
}

void IngPipe(int signo) {
  int id = getpid();
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char* argv[]) {
  if (argc > 1) {
    if (string(argv[1]) == "PrintProcessFps") {
      g_printProcessFps = true;

      cout << "================== " << argv[1] << " ==================" << endl;
    }
  }

  signal(SIGSEGV, SIG_IGN);
  signal(SIGPIPE, &IngPipe);

  Init init;
  init.WriteLogHead();

  SysLog* sysLog = SysLog::Instance();
  (*sysLog) << "main: system init!" << endl;
  init.SystemInit(argc, argv);

  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }

  gdk_threads_init();
  gtk_init(&argc, &argv);

  Utils::SetTheme(RC_PATH);

  ChangeKeymap();
  init_colors(); ///

  (*sysLog) << "main: run gui, create main view!" << endl;
  init.CreatMainWindow();

  init.ParaInit();

  (*sysLog) << "main: probe init!" << endl;
  init.ProbeCheck();

  (*sysLog) << "main: start up dsc thread!" << endl;

  if(pthread_create(&pidDsc, NULL, ThreadDsc, NULL) != 0) {
    cerr << "main: fail to create dsc thread!" << endl;
  }

  g_uis4d_ri.IPC4DInit();
  g_shm_id0 = g_uis4d_ri.GetSHMID(0);
  g_pshm_data0 = g_uis4d_ri.GetSHMData(0);
  g_sem_id0 = g_uis4d_ri.GetSEMID(0);

  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  (*sysLog) << "main: exit main, and do something clear!" << endl;

  g_uis4d_ri.IPC4DDestroy();

  init.ExitSystem();
}
