#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "input.h"
#include "vmouse.h"

static volatile bool running = true;

void handle_sigint(int sig) {
  (void)sig;
  running = false;
}

int main(void) {
  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);

  if (input_open() != 0 || vmouse_setup() != 0) {
    fprintf(stderr, "Initialization failed\n");
    return 1;
  }

  printf("[+] Daemon started. Press CAPS LOCK to toggle\n");
  printf("[+] Press CTRL+C to exit\n");
  printf("[+] Process ID: %d\n", getpid());

  while (running) {
    input_poll();

    if (input_should_trigger()) {
      //   printf("\r[+] Trigger pressed, sending click...");
      vmouse_click();
      usleep(50000);
    } else {
      usleep(5000);
    }
  }

  printf("\n[!] Terminating...\n");
  input_close();
  vmouse_cleanup();
  return 0;
}
