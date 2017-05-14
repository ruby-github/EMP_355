#include "sysMan/SysLog.h"

#include <ctime>

using namespace std;

#define FILENAME "./res/log/sys.log"

SysLog* SysLog::m_instance = NULL;

// ---------------------------------------------------------

class SysLog* SysLog::Instance() {
  if (m_instance == NULL) {
    m_instance = new SysLog();
  }

  return m_instance;
}

SysLog::SysLog() {
  Open();
}

SysLog::~SysLog() {
  if (is_open()) {
    close();
  }

  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

// ---------------------------------------------------------

void SysLog::Open() {
  time_t t = time(NULL);
  struct tm* timeinfo = localtime(&t);

  char buffer[80] = {0};
  strftime (buffer, 80, "%F %T", timeinfo);

  ofstream ofs;
  ofs.open(FILENAME, ofstream::out | ofstream::trunc);

  ofs << "***** Log Name: 09001" << endl;
  ofs << "***** Log Time: " << buffer << endl;

  ofs.close();

  open(FILENAME, ofstream::app);
}
