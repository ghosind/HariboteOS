  [BITS 32]

  GLOBAL asm_int_handler21, asm_int_handler27, asm_int_handler2c

  EXTERN int_handler21, int_handler27, int_handler2c

asm_int_handler21:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     EAX, ESP
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler21
  POP     EAX
  POPAD
  POP     DS
  POP     ES
  IRETD

asm_int_handler27:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     EAX, ESP
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler27
  POP     EAX
  POPAD
  POP     DS
  POP     ES
  IRETD

asm_int_handler2c:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     EAX, ESP
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler2c
  POP     EAX
  POPAD
  POP     DS
  POP     ES
  IRETD
