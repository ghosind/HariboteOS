  [BITS 32]

  GLOBAL asm_hrb_api
  GLOBAL api_putchar

  EXTERN hrb_api

asm_hrb_api:
  PUSH    DS
  PUSH    ES
  PUSHAD
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -40
  MOV     [ECX+32], ESP
  MOV     [ECX+36], SS
  
  MOV     EDX, [ESP]
  MOV     EBX, [ESP+4]
  MOV     [ECX], EDX
  MOV     [ECX+4], EBX
  MOV     EDX, [ESP+8]
  MOV     EBX, [ESP+12]
  MOV     [ECX+4], EDX
  MOV     [ECX+12], EBX
  MOV     EDX, [ESP+16]
  MOV     EBX, [ESP+20]
  MOV     [ECX+16], EDX
  MOV     [ECX+20], EBX
  MOV     EDX, [ESP+24]
  MOV     EBX, [ESP+28]
  MOV     [ECX+24], EDX
  MOV     [ECX+28], EBX

  MOV     ES, AX
  MOV     SS, AX
  MOV     ESP, ECX
  STI

  CALL    hrb_api

  MOV     ECX, [ESP+32]
  MOV     EAX, [ESP+36]
  
  CLI

  MOV     SS, AX
  MOV     ESP, ECX

  POPAD
  POP     ES
  POP     DS
  IRETD
