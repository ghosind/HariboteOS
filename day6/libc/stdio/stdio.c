#include <stdarg.h>
#include <stdio.h>

struct SprintBuf {
  char *buf;
  int count;
};

void printnum(void (*fputch)(char, void *), void *data, unsigned long num,
              int base) {
  if (num >= base) {
    printnum(fputch, data, num / base, base);
  }

  fputch("0123456789abcdef"[num % base], data);
}

void vprintfmt(void (*fputch)(char, void *), void *data, const char *fmt,
               va_list ap) {
  int ch;
  unsigned long long num;
  char *str;

  while (1) {
    while ((ch = *fmt++) != '%') {
      fputch(ch, data);
      if (ch == '\0') {
        return;
      }
    }

    num = 0;
    switch (ch = *fmt++) {
    case 'c':
      fputch(va_arg(ap, int), data);
      break;

    case 'd':
      num = va_arg(ap, int);
      if ((long long)num < 0) {
        fputch('-', data);
        num = -(long long)num;
      }
      printnum(fputch, data, num, 10);
      break;

    case 'p':
      fputch('0', data);
      fputch('x', data);
      num = (unsigned long)va_arg(ap, void *);
      printnum(fputch, data, num, 16);
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
