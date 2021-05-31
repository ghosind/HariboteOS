void api_putstr(char *s);
void api_end(void);

int main() {
  api_putstr("hello world\n");
  api_end();

  return 0;
}
