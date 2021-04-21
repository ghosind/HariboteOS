  [BITS 32]

  GLOBAL asm_cons_putchar

  EXTERN cons_putchar

asm_cons_putchar:
  PUSH    1
  AND     EAX, 0xff
  PUSH    EAX
  PUSH    DWORD [0x0fec]
  CALL    cons_putchar
  ADD     ESP, 12
  RETF
