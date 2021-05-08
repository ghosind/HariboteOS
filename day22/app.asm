  [BITS 32]

  GLOBAL start_app

start_app:                ; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
  PUSHAD
  MOV     EAX, [ESP+36]   ; EIP
  MOV     ECX, [ESP+40]   ; CS
  MOV     EDX, [ESP+44]   ; ESP
  MOV     EBX, [ESP+48]   ; DS/SS
  MOV     EBP, [ESP+52]   ; tss.esp0
  MOV     [EBP], ESP
  MOV     [EBP+4], SS
  MOV     ES, BX
  MOV     DS, BX
  MOV     FS, BX
  MOV     GS, BX

  OR      ECX, 3
  OR      EBX, 3
  PUSH    EBX
  PUSH    EDX
  PUSH    ECX
  PUSH    EAX
  RETF
