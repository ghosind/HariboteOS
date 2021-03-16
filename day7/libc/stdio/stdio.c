#include <stdarg.h>
#include <stdio.h>

struct SprintBuf {
  char *buf;
  int count;
};

void printnum(void (*fputch)(char, void *), void *data, unsigned long num,
              int base, int width, int pad, int uppercase) {
  if (num >= base) {
    printnum(fputch, data, num / base, base, width - 1, pad, uppercase);
  } else {
    while (--width > 0) {
      fputch(pad, data);
    }
  }

  if (uppercase) {
    fputch("0123456789ABCDEF"[num % base], data);
  } else {
    fputch("0123456789abcdef"[num % base], data);
  }
}

void vprintfmt(void (*fputch)(char, void *), void *data, const char *fmt,
               va_list ap) {
  int ch, pad, width;
  unsigned long num;
  char *str;

  while (1) {
    while ((ch = *fmt++) != '%') {
      fputch(ch, data);
      if (ch == '\0') {
        return;
      }
    }

    num = 0;
    pad = 0;
    width = 0;
  reswitch:
    switch (ch = *fmt++) {
    case '0':
      pad = '0';
      goto reswitch;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      for (width = 0;; ++fmt) {
        width = width * 10 + ch - '0';
        ch = *fmt;
        if (ch < '0' || ch > '9')
          break;
      }
      goto reswitch;

    case 'c':
      fputch(va_arg(ap, int), data);
      break;

    case 'd':
      num = va_arg(ap, int);
      if ((long)num < 0) {
        fputch('-', data);
        num = -(long)num;
      }
      printnum(fputch, data, num, 10, width, pad, 0);
      break;

    case 'p':
      fputch('0', data);
      fputch('x', data);
      num = (unsigned long)va_arg(ap, void *);
      printnum(fputch, data, num, 16, width, pad, 0);
      break;

    case 's':
      str = va_arg(ap, char *);
      if (str == NULL) {
        str = "<null>";
      }
      while (*str != '\0') {
        fputch(*str, data);
        str++;
      }
      break;

    case 'x':
      num = va_arg(ap, unsigned int);
      printnum(fputch, data, num, 16, width, pad, 0);
      break;

    case 'X':
      num = va_arg(ap, unsigned int);
      printnum(fputch, data, num, 16, width, pad, 1);
      break;

    case '%':
      fputch('%', data);
      break;

    default:
      break;
    }
  }
}

void sprint_putch(char c, struct SprintBuf *data) {
  *data->buf++ = c;
  data->count++;
}

int vsprintf(char *s, const char *format, va_list ap) {
  struct SprintBuf buf = {s, 0};

  vprintfmt((void *)sprint_putch, &buf, format, ap);

  return buf.count;
}

int sprintf(char *s, const char *format, ...) {
  va_list ap;
  int ret;

  va_start(ap, format);
  ret = vsprintf(s, format, ap);
  va_end(ap);

  return ret;
}
