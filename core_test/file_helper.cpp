/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*             Copyright (C)2014, WWIV Software Services                  */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include "file_helper.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef GetFullPathName
#else
#include <sys/stat.h>
#endif

#include "core/wwivport.h"
#include "core/strings.h"

using std::string;

FileHelper::FileHelper() {
  const ::testing::TestInfo* const test_info =
      ::testing::UnitTest::GetInstance()->current_test_info();
  const string dir = wwiv::strings::StrCat(
    test_info->test_case_name(), "_", test_info->name());
  tmp_ = FileHelper::CreateTempDir(dir);
}

const std::string FileHelper::DirName(const std::string& name) const {
    return wwiv::strings::StringPrintf("%s%c%s%c", tmp_.c_str(), 
        WWIV_FILE_SEPERATOR_CHAR, name.c_str(), WWIV_FILE_SEPERATOR_CHAR);
}

bool FileHelper::Mkdir(const std::string& name) const {
    const std::string path = DirName(name); 
#ifdef _WIN32
    return CreateDirectory(path.c_str(), NULL) ? true:false;
#else
    return (mkdir(path.c_str()) == 0);
#endif
}

// static
std::string FileHelper::CreateTempDir(const string base) {
#ifdef _WIN32
    char local_dir_template[_MAX_PATH];
    char temp_path[_MAX_PATH];
    GetTempPath(_MAX_PATH, temp_path);
    time_t now = time(nullptr);
    sprintf(local_dir_template, "%s%s.%lx", temp_path, base.c_str(), now);
    if (!CreateDirectory(local_dir_template, NULL)) {
        return string();
    }
    return std::string(local_dir_template);
#else
    char local_dir_template[MAX_PATH];
    static const char kTemplate[] = "/tmp/fileXXXXXX";
    strcpy(local_dir_template, kTemplate);
    char *result = mkdtemp(local_dir_template);
    return std::string(result);
#endif
}

FILE* FileHelper::OpenTempFile(const string& orig_name, string* path) {
    std::string tmp = TempDir();
    string name = orig_name;
#ifdef _WIN32
    std::replace(name.begin(), name.end(), '/', WWIV_FILE_SEPERATOR_CHAR);
#endif  // _WIN32
    path->assign(wwiv::strings::StringPrintf("%s%c%s", tmp.c_str(), 
        WWIV_FILE_SEPERATOR_CHAR, name.c_str()));
    FILE* file = fopen(path->c_str(), "wt");
    assert(file);
    return file;
}

std::string FileHelper::CreateTempFile(const std::string& orig_name, const std::string& contents) {
  std::string path;
  FILE* file = OpenTempFile(orig_name, &path);
  assert(file);
  fputs(contents.c_str(), file);
  fclose(file);
  return path;
}

const std::string FileHelper::ReadFile(const std::string name) const {
  std::FILE *fp = fopen(name.c_str(), "rt");
  if (!fp) {
    throw (errno);
  }
  string contents;
  fseek(fp, 0, SEEK_END);
  contents.resize(ftell(fp));
  rewind(fp);
  fread(&contents[0], 1, contents.size(), fp);
  fclose(fp);
  return contents;
}
