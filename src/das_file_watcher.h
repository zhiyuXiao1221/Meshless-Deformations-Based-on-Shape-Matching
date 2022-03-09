#ifndef DAS_FILE_WATCHER_H
#define DAS_FILE_WATCHER_H
#include <stdint.h>
#include <string>

struct FileWatchInfo {
  std::string FileName;
  uint64_t LastModificationTime;
};

bool dasfw_start_watching(const char *filename, FileWatchInfo *fileWatchInfo);
bool dasfw_did_change(FileWatchInfo *watchInfo);

#endif

#ifdef DAS_FILE_WATCHER_IMPLEMENTATION
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stat
#endif

bool dasfw_start_watching(const std::string& filename, FileWatchInfo *fileWatchInfo) {
  struct stat platFileInfo;
  int err = stat(filename.c_str(), &platFileInfo);

  if (err != 0) {
    return false;
  }

  fileWatchInfo->FileName = filename;
  fileWatchInfo->LastModificationTime = (uint64_t)platFileInfo.st_mtime;
  return true;
}

bool dasfw_did_change(FileWatchInfo *fileWatchInfo) {
  struct stat platFileInfo;
  int err = stat(fileWatchInfo->FileName.c_str(), &platFileInfo);

  if (err != 0) {
    return false;
  }

  bool didChange = fileWatchInfo->LastModificationTime < (uint64_t)platFileInfo.st_mtime;
  fileWatchInfo->LastModificationTime = (uint64_t)platFileInfo.st_mtime;
  return didChange;
}

#ifdef _WIN32
#undef stat
#endif

#endif

