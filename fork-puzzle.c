#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Tiny program to demonstrate non-deterministic nature of process forking
int main(void) {
  const char *letters = "abcd";
  int lettersLength = strlen(letters);
  for (size_t i = 0; i < lettersLength; i++) {
    printf("%c\n", letters[i]);
    pid_t pid = fork();
    // If we cannot spawn a process, halt
    assert(pid >= 0);
  };
  return 0;
}
