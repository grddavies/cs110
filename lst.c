#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char kPermissionChars[] = {'r', 'w', 'x'};
static const mode_t kPermissionFlags[] = {
    S_IRUSR, S_IWUSR, S_IXUSR, // User
    S_IRGRP, S_IWGRP, S_IXGRP, // Group
    S_IROTH, S_IWOTH, S_IXOTH, // Other
};

static char GetFiletypeChar(const mode_t mode) {
  if (S_ISDIR(mode)) {
    return 'd';
  } else if (S_ISREG(mode)) {
    return '.';
  }
  return '-';
}

static void PrintPerms(const char *path, size_t basePathLen) {
  struct stat st_buff;
  lstat(path, &st_buff);
  size_t n = sizeof(kPermissionFlags) / sizeof(mode_t);
  char permissions[n];
  memset(permissions, '-', sizeof(permissions));
  permissions[n] = '\0';
  char ft_char = GetFiletypeChar(st_buff.st_mode);
  for (size_t i = 0; i < n; i++) {
    if (st_buff.st_mode & kPermissionFlags[i]) {
      permissions[i] = kPermissionChars[i % 3];
    }
  }
  const struct passwd *user = getpwuid(st_buff.st_uid);
  const struct group *grp = getgrgid(st_buff.st_gid);
  printf("%c%s %3d %s %s %6lld %s\n", ft_char, permissions, st_buff.st_nlink,
         user->pw_name, grp->gr_name, st_buff.st_size, path + basePathLen);
}

static void ListDir(char *path, bool all, bool perms) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    fprintf(stderr, "Could not read directory '%s'\n", path);
    // its a directory but permission to open was denied
    return;
  }
  size_t length = strlen(path);
  // Append a / to the path
  strcpy(path + length++, "/");
  size_t basePathLen = length;
  while (true) {
    struct dirent *entry = readdir(dir);
    if (entry == NULL) {
      // We've iterated over every entry in the dir
      break;
    }
    strcpy(path + length, entry->d_name);
    if (!all && path[length] == '.') {
      // File is hidden
      continue;
    }
    if (perms) {
      PrintPerms(path, basePathLen);
    } else {
      printf("%s\n", entry->d_name);
    }
  }
  closedir(dir);
}

// List all entries in a directory
int main(int argc, char *argv[]) {
  bool l = false;
  bool a = false;
  char *directory = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "la")) != -1) {
    switch (opt) {
    case 'l':
      l = true;
      break;
    case 'a':
      a = true;
      break;
    default:
      fprintf(stderr, "Usage: %s [-l] [-a] directory\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind < argc) {
    directory = argv[optind];
  } else {
    fprintf(stderr, "Usage: %s [-l] [-a] directory\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct stat st_buff;
  lstat(directory, &st_buff);

  if (S_ISDIR(st_buff.st_mode)) {
    ListDir(directory, a, l);
  } else {
    if (l) {
      PrintPerms(directory, 0);
    } else {
      printf("%s\n", argv[1]);
    }
  }
  return 0;
}
