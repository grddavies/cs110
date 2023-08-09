#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const int kWrongArgCount = 1;
static const int kSourceFileNonExistent = 2;
static const int kDestFileOpenFailure = 4;
static const int kReadFailure = 8;
// Permissions: "rw-r--r--"
static const int kDefaultPermissions = 0644;

// Simple copy file function
int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage:\n%s <source-file> <destination-file>\n", argv[0]);
    return kWrongArgCount;
  }

  int fdin = open(argv[1], O_RDONLY);
  if (fdin == -1) {
    fprintf(stderr, "Error opening source file '%s':  %s\n", argv[1],
            strerror(kSourceFileNonExistent));
  }

  int fdout = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, kDefaultPermissions);
  if (fdin == -1) {
    fprintf(stderr, "Error opening destination file '%s':  %s\n", argv[2],
            strerror(kDestFileOpenFailure));
  }

  while (true) {
    char buffer[1024];
    ssize_t bytesRead = read(fdin, buffer, sizeof(buffer));
    if (bytesRead == -1) {
      fprintf(stderr, "Error reading source file '%s':  %s\n", argv[1],
              strerror(kReadFailure));
    }
    if (bytesRead == 0) {
      break;
    }
    size_t bytesWritten = 0;
    while (bytesWritten < bytesRead) {
      bytesWritten +=
          write(fdout, buffer + bytesWritten, bytesRead - bytesWritten);
    }
  }
  close(fdin);
  close(fdout);
  return 0;
}
