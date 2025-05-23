#include "vmouse.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int fd = -1;

static void emit_event(uint16_t type, uint16_t code, int32_t value) {
  struct input_event ev = {
      .type = type,
      .code = code,
      .value = value,
  };

  if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
    fprintf(stderr, "Failed to emit event (type=%u, code=%u): %s\n", type, code,
            strerror(errno));
  }
}

int vmouse_setup(void) {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fd < 0) {
    perror("Failed to open uinput device");
    return -1;
  }

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_EVBIT, EV_SYN);

  struct uinput_setup usetup = {
      .id.bustype = BUS_USB,
      .id.vendor = 0x1,
      .id.product = 0x1,
      .id.version = 1,
  };
  snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "ftl-vmouse");

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);

  sleep(1);
  return 0;
}

void vmouse_click(void) {
  emit_event(EV_KEY, BTN_LEFT, 1);
  emit_event(EV_SYN, SYN_REPORT, 0);

  usleep(10000);

  emit_event(EV_KEY, BTN_LEFT, 0);
  emit_event(EV_SYN, SYN_REPORT, 0);
}

void vmouse_cleanup(void) {
  if (fd >= 0) {
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    fd = -1;
  }
}
