#include "utils/IniFile.h"
#include "utils/Logger.h"

#include <cstddef>

IniFile::IniFile(const string filename) {
  m_filename = filename;
  m_keyfile = NULL;

  if (!g_file_test(m_filename.c_str(), G_FILE_TEST_EXISTS)) {
    g_build_filename(m_filename.c_str(), NULL);
  }

  GError* error = NULL;
  gchar* contents = NULL;
  gsize length = 0;

  if (g_file_get_contents(m_filename.c_str(), &contents, &length, &error)) {
    m_keyfile = g_key_file_new();

    if (!g_key_file_load_from_data(m_keyfile, contents, -1, G_KEY_FILE_NONE, &error)) {
      free_error(error);

      g_key_file_free(m_keyfile);
      m_keyfile = NULL;
    }

    g_free(contents);
  } else {
    free_error(error);
  }
}

IniFile::~IniFile() {
  g_key_file_free(m_keyfile);
  m_keyfile = NULL;
}

string IniFile::ReadString(const string group, const string key) {
  GError* error = NULL;

  gchar* val = g_key_file_get_string(m_keyfile, group.c_str(), key.c_str(), &error);
  free_error(error);

  string value = "";

  if (val != NULL) {
    value = val;
  }

  return value;
}

void IniFile::WriteString(const string group, const string key, const string value) {
  g_key_file_set_string(m_keyfile, group.c_str(), key.c_str(), value.c_str());
}

int IniFile::ReadInt(const string group, const string key) {
  GError* error = NULL;

  int value = g_key_file_get_integer(m_keyfile, group.c_str(), key.c_str(), &error);
  free_error(error);

  return value;
}

void IniFile::WriteInt(const string group, const string key, const int value) {
  g_key_file_set_integer(m_keyfile, group.c_str(), key.c_str(), value);
}

double IniFile::ReadDouble(const string group, const string key) {
  GError* error = NULL;

  double value = g_key_file_get_double(m_keyfile, group.c_str(), key.c_str(), &error);
  free_error(error);

  return value;
}

void IniFile::WriteDouble(const string group, const string key, const double value) {
  g_key_file_set_double(m_keyfile, group.c_str(), key.c_str(), value);
}

bool IniFile::ReadBool(const string group, const string key) {
  GError* error = NULL;

  bool value = g_key_file_get_boolean(m_keyfile, group.c_str(), key.c_str(), &error);
  free_error(error);

  return value;
}

void IniFile::WriteBool(const string group, const string key, const bool value) {
  g_key_file_set_boolean(m_keyfile, group.c_str(), key.c_str(), value);
}

vector<double> IniFile::ReadDoubleList(const string group, const string key) {
  GError* error = NULL;
  gsize length = 0;

  double* list = g_key_file_get_double_list(m_keyfile, group.c_str(), key.c_str(), &length, &error);
  free_error(error);

  vector<double> vec;

  for (gsize i = 0; i < length; i++) {
    vec.push_back(list[i]);
  }

  return vec;
}

void IniFile::WriteDoubleList(const string group, const string key, double value[], const int length) {
  g_key_file_set_double_list(m_keyfile, group.c_str(), key.c_str(), value, length);
}

void IniFile::RemoveString(const string group, const string key) {
  GError* error = NULL;

  g_key_file_remove_key(m_keyfile, group.c_str(), key.c_str(), &error);
  free_error(error);
}

void IniFile::RemoveGroup(const string group) {
  GError* error = NULL;

  g_key_file_remove_group(m_keyfile, group.c_str(), &error);
  free_error(error);
}

vector<string> IniFile::GetGroupName() {
  gsize length = 0;

  gchar** list = g_key_file_get_groups(m_keyfile, &length);

  vector<string> vec;

  for (gsize i = 0; i < length; i++) {
    vec.push_back(list[i]);
  }

  g_strfreev(list);

  return vec;
}

vector<string> IniFile::GetGroupKey(const string group) {
  GError* error = NULL;
  gsize length = 0;

  gchar** list = g_key_file_get_keys(m_keyfile, group.c_str(), &length, &error);
  free_error(error);

  vector<string> vec;

  for (gsize i = 0; i < length; i++) {
    vec.push_back(list[i]);
  }

  g_strfreev(list);

  return vec;
}

bool IniFile::SyncConfigFile() {
  GError* error = NULL;
  gsize length = 0;

  gchar *contents = g_key_file_to_data(m_keyfile, &length, NULL);

  if (!g_file_set_contents(m_filename.c_str(), contents, length, &error)) {
    free_error(error);
    g_free(contents);

    return false;
  }

  g_free(contents);

  return true;
}

// ---------------------------------------------------------

void IniFile::free_error(GError* error) {
  if (error != NULL) {
    Logger::error(error->message);

    g_error_free(error);
  }
}
