#ifndef __INI_FILE_H__
#define __INI_FILE_H__

#include <glib.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class IniFile {
public:
  IniFile(const string filename);
  ~IniFile();

  string ReadString(const string group, const string key);
  void WriteString(const string group, const string key, const string value);

  int ReadInt(const string group, const string key);
  void WriteInt(const string group, const string key, const int value);

  double ReadDouble(const string group, const string key);
  void WriteDouble(const string group, const string key, const double value);

  bool ReadBool(const string group, const string key);
  void WriteBool(const string group, const string key, const bool value);

  vector<double> ReadDoubleList(const string group, const string key);
  void WriteDoubleList(const string group, const string key, const double value[], const int length);

  void RemoveString(const string group, const string key);
  void RemoveGroup(const string group);

  vector<string> GetGroupName();
  vector<string> GetGroupKey(const string group);

  bool SyncConfigFile();

private:
  void free_error(GError* error);

private:
  string m_filename;
  GKeyFile* m_keyfile;
};

#endif
