#ifndef __INIT_H__
#define __INIT_H__

class Init {
public:
  Init();
  ~Init();

public:
  void SystemInit(int argc, char* argv[]);
  void ParaInit();
  void ProbeCheck();
  void WriteLogHead();
  void CreatMainWindow();
  void ExitSystem();
};

#endif
