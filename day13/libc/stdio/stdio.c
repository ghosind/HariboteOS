#include <stdarg.h>
#include <stdio.h>

struct SprintBuf {
  char *buf;
  int count;
};

struct PrintNumAttr {
  int base;
  int width;
  int pad;
  int uppercase;
  int negative;
};

void printnum(void (*fputch)(char, void *), void *data, unsigned long num,
              struct PrintNumAttr attr) {
  if (num >= attr.base) {
    attr.width -= 1;
    printnum(fputch, data, num / attr.base, attr);
  } else {
    while (--attr.width > attr.negative) {
      fputch(attr.pad, data);
    }

    if (attr.negative) {
      fputch('-', data);
    }
  }

  if (attr.uppercase) {
    fputch("0123456789ABCDEF"[num % attr.base], data);
  } else {
    fputch("0123456789abcdef"[num % attr.base], data);
  }
}

void vprintfmt(void (*fputch)(char, void *), void *data, const char *fmt,
               va_list ap) {
  int ch;
  unsigned long num;
  char *str;
  struct PrintNumAttr attr;

  while (1) {
    while ((ch = *fmt++) != '%') {
      fputch(ch, data);
      if (ch == '\0') {
        return;
      }
    }

    num = 0;
    attr.base = 0;
    attr.pad = ' ';
    attr.width = 0;
    attr.negative = 0;
    attr.uppercase = 0;
  reswitch:
    switch (ch = *fmt++) {
    case '0':
      attr.pad = '0';
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
      for (attr.width = 0;; ++fmt) {
        attr.width = attr.width * 10 + ch - '0';
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
        attr.negative = 1;
        num = -(long)num;
      }
      attr.base = 10;
      printnum(fputch, data, num, attr);
      break;

    case 'p':
      fputch('0', data);
      fputch('x', data);
      num = (unsigned long)va_arg(ap, void *);
      attr.base = 16;
      printnum(fputch, data, num, attr);
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
      attr.base = 16;
      printnum(fputch, data, num, attr);
      break;

    case 'X':
      num = va_arg(ap, unsigned int);
      attr.base = 16;
      attr.uppercase = 1;
      printnum(fputch, data, num, attr);
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
