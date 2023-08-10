#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static const int kWrongArgCount = 1;
size_t kMaxPath = 1024;

static void listMatches(char path[], size_t length, const char *name) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    fprintf(stderr, "Could not read directory '%s'\n", path);
    // its a directory but permission to open was denied
    return;
  }
  // Append a / to the path
  strcpy(path + length++, "/");
  while (true) {
    struct dirent *entry = readdir(dir);
    if (entry == NULL) {
      // We've iterated over every entry in the dir
      break;
    }
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      // Skip current and parent dir
      continue;
    }
    if (length + strlen(entry->d_name) > kMaxPath) {
      // Dont overflow the path buffer
      continue;
    }
    // Append the entry to the path
    strcpy(path + length, entry->d_name);
    struct stat st_buff;
    // We do not treat links as the same as the regular files they point to
    // Following links could cause an infinite loop
    lstat(path, &st_buff);
    if (S_ISREG(st_buff.st_mode)) {
      if (strcmp(entry->d_name, name) == 0) {
        printf("%s\n", path);
      }
    } else if (S_ISDIR(st_buff.st_mode)) {
      // Recursively search directories
      listMatches(path, length + strlen(entry->d_name), name);
    }
  }
  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage:\n%s <base-dir> <search-pattern>\n", argv[0]);
    return kWrongArgCount;
  }
  const char *directory = argv[1];
  struct stat st_buff;
  lstat(directory, &st_buff);
  if (!S_ISDIR(st_buff.st_mode)) {
    fprintf(stderr, "'%s' is not a directory\n", argv[1]);
    return ENOTDIR;
  }
  size_t length = strlen(directory);
  if (length > kMaxPath)
    return 2;
  const char *pattern = argv[2];
  char path[kMaxPath + 1];
  // Cannot buffer overflow as we have checked directory strlen
  strcpy(path, directory);
  listMatches(path, length, pattern);
  return 0;
}
