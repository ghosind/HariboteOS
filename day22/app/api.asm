  [BITS 32]

  GLOBAL api_putchar, api_end, api_putstr

api_putchar:
  MOV   EDX, 1
  MOV   AL, [ESP+4]
  INT   0x40
  RET

api_end:
  MOV   EDX, 4
  INT   0x40

api_putstr:             ; void api_putstr(char *s);
  PUSH    EBX
  MOV     EDX, 2
  MOV     EBX, [ESP+8]
  INT     0x40
  POP     EBX
  RET
