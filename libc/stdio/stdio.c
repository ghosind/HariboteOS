#include <stdarg.h>
#include <stdio.h>

static char *vsprint_num(char *s, unsigned long num, int base, int width, int pad, int sign, int uppercase) {
  static char buf[128];
  static char *letters = "0123456789abcdefABCDEF";
  int i = 0, pos;

  while (num > 0) {
    pos = num % base;
    num /= base;
    if (uppercase && pos >= 10) {
      pos += 6;
    }
    buf[i++] = *(letters+pos);
  }
  if (sign) {
    buf[i++] = '-';
  }
  while (i < width) {
    buf[i++] = pad;
  }
  i--;
  while (i >= 0) {
    *s++ = buf[i--];
  }
  return s;
}

static char *vsprint_fmt(char *s, const char *fmt, va_list ap) {
  int state = 0;
  int width, pad, sign;
  unsigned long num;
  char *str;
  for (; *fmt; fmt++) {
    if (!state) {
      if (*fmt == '%') {
        state = 1;
        width = 0;
        sign = 0;
        pad = ' ';
      } else {
        *s++ = *fmt;
      }
    } else {
next:
      switch (*fmt) {
      case '0':
        if (width == 0) {
          pad = '0';
          fmt++;
          goto next;
        }
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        width = width * 10 + *fmt - '0';
        fmt++;
        goto next;
      case 'c':
        *s++ = va_arg(ap, int);
        break;
      case 'd':
        num = va_arg(ap, int);
        if ((long)num < 0) {
          sign = 1;
          num = -(long)num;
        }
        s = vsprint_num(s, num, 10, width, pad, sign, 0);
        break;
      case 'p':
        *s++ = '0';
        *s++ = 'x';
        num = (unsigned long)va_arg(ap, void *);
        s = vsprint_num(s, num, 16, width, pad, sign, 1);
        break;
      case 's':
        str = va_arg(ap, char *);
        if (str == NULL) {
          str = "<null>";
        }
        while (*str != '\0') {
          *s++ = *str++;
        }
        break;
      case 'x':
        num = (unsigned long)va_arg(ap, void *);
        s = vsprint_num(s, num, 16, width, pad, sign, 0);
        break;
      case 'X':
        num = (unsigned long)va_arg(ap, void *);
        s = vsprint_num(s, num, 16, width, pad, sign, 1);
        break;
      case '%':
        *s++ = '%';
        break;
      }
      state = 0;
    }
  }

  *s = '\0';

  return s;
}

int vsprintf(char *s, const char *format, va_list ap) {
  char *ret = vsprint_fmt(s, format, ap);

  return ret - s;
}

int sprintf(char *s, const char *format, ...) {
  va_list ap;
  int ret;

  va_start(ap, format);
  ret = vsprintf(s, format, ap);
  va_end(ap);

  return ret;
}
