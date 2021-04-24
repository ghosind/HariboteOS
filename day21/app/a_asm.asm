  [BITS 32]

  GLOBAL api_putchar

api_putchar:
  MOV   EDX, 1
  MOV   AL, [ESP+4]
  INT   0x40
  RET
