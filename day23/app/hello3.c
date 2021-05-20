void api_putchar(int c);
void api_end(void);

int main(void) {
  api_putchar('h');
  api_putchar('e');
  api_putchar('l');
  api_putchar('l');
  api_putchar('o');

  api_end();

  return 0;
}
