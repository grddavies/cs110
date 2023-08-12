#include <fcntl.h>   // open
#include <stdbool.h> // bool type
#include <unistd.h>  // read/write/close

// O_TRUNC overwrites existing file content
#define DEFAULT_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)
#define DEFAULT_PERMISSIONS 0644

static void writeBuffToFile(int fd, const char buffer[], size_t len) {
  size_t bitsWritten = 0;
  do {
    bitsWritten += write(fd, buffer + bitsWritten, len - bitsWritten);
  } while (bitsWritten < len);
}

// tee
// Copy stdin to stdout and any number of specified files
int main(int argc, char *argv[]) {
  // Create same number of files as were passed as args
  int fds[argc];
  // Set first output as stdout
  fds[0] = STDOUT_FILENO;
  // Read next outfiles from argv
  for (int i = 1; i < argc; i++) {
    fds[i] = open(argv[i], DEFAULT_FLAGS, DEFAULT_PERMISSIONS);
  }

  char buffer[2048];
  while (true) {
    ssize_t bitsRead = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bitsRead == 0) {
      break;
    }

    for (int i = 0; i < argc; i++) {
      writeBuffToFile(fds[i], buffer, bitsRead);
    }
  }

  for (int i = 0; i < argc; i++) {
    close(fds[i]);
  }

  return 0;
}
