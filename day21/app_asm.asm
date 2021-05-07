  [BITS 32]

  GLOBAL start_app

start_app:                ; void start_app(int eip, int cs, int esp, int ds);
  PUSHAD
  MOV     EAX, [ESP+36]   ; EIP
  MOV     ECX, [ESP+40]   ; CS
  MOV     EDX, [ESP+44]   ; ESP
  MOV     EBX, [ESP+48]   ; DS/SS
  MOV     [0x0fe4], ESP

  CLI

  MOV     ES, BX
  MOV     SS, BX
  MOV     DS, BX
  MOV     FS, BX
  MOV     GS, BX
  MOV     ESP, EDX

  STI

  PUSH    ECX
  PUSH    EAX

  CALL    FAR [ESP]

  MOV     EAX, 1*8
  CLI
  MOV     ES, AX
  MOV     SS, AX
  MOV     DS, AX
  MOV     FS, AX
  MOV     GS, AX
  MOV     ESP, [0x0fe4]

  STI

  POPAD
  RET
