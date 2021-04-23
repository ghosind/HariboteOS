  [BITS 32]

  GLOBAL asm_hrb_api
  GLOBAL api_putchar

  EXTERN hrb_api

asm_hrb_api:
  STI
  PUSHAD
  
  PUSHAD
  CALL    hrb_api
  ADD     ESP, 32
  POPAD
  IRETD

api_putchar:            ; void api_putchar(int c);
  MOV     EDX, 1
  MOV     AL, [ESP+4]
  INT     0x40
  RET
