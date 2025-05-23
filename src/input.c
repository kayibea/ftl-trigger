#include "input.h"

#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int kb_fd = -1;
static int mouse_fd = -1;

static bool active = false;
static bool trigger_down = false;
static bool modifier_down = false;

bool input_should_trigger(void) {
  if (!active) return false;
  return (modifier_down && trigger_down);
}

int input_open(void) {
  kb_fd = open("/dev/input/event7", O_RDONLY | O_NONBLOCK);
  mouse_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);

  if (kb_fd < 0) perror("Failed to open keyboard device");
  if (mouse_fd < 0) perror("Failed to open mouse device");

  return (kb_fd < 0 || mouse_fd < 0) ? -1 : 0;
}

void input_close(void) {
  if (kb_fd >= 0) close(kb_fd);
  if (mouse_fd >= 0) close(mouse_fd);
}

void input_poll(void) {
  struct input_event ev;
  ssize_t n;

  struct pollfd fds[2];
  fds[0].fd = kb_fd;
  fds[0].events = POLLIN;
  fds[1].fd = mouse_fd;
  fds[1].events = POLLIN;

  int ret = poll(fds, 2, 10);
  if (ret <= 0) return;

  if (fds[0].revents & POLLIN) {
    while ((n = read(kb_fd, &ev, sizeof(ev))) == sizeof(ev)) {
      if (ev.type == EV_KEY && ev.code == KEY_CAPSLOCK && ev.value == 1) {
        active = !active;
        printf("[*] Macro %s\n", active ? "ON" : "OFF");
      }
    }
  }

  if (!(fds[1].revents & POLLIN)) return;

  while ((n = read(mouse_fd, &ev, sizeof(ev))) == sizeof(ev)) {
    if (ev.type == EV_KEY && ev.code == BTN_RIGHT) {
      modifier_down = (ev.value == 1);
      //   printf("[*] Mouse BTN_RIGHT %s\n", modifier_down ? "DOWN" : "UP");
    } else if (ev.type == EV_KEY && ev.code == BTN_SIDE) {
      trigger_down = (ev.value == 1);
      //   printf("[*] Mouse BTN_SIDE %s\n", trigger_down ? "DOWN" : "UP");
    }
  }
}
