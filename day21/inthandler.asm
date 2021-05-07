  [BITS 32]

  GLOBAL asm_int_handler0d, asm_int_handler20, asm_int_handler21
  GLOBAL asm_int_handler27, asm_int_handler2c

  EXTERN int_handler0d, int_handler20, int_handler21, int_handler27
  EXTERN int_handler2c

asm_int_handler0d:
  STI
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     EAX, ESP
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler0d
  CMP     EAX, 0
  JNE     .end_app
  POP     EAX
  POPAD
  POP     DS
  POP     ES
  ADD     ESP, 4
  IRETD

.end_app:
  MOV     ESP, [EAX]
  POPAD
  RET

asm_int_handler20:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     EAX, ESP
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler20
  POP     EAX
  POPAD
  POP     DS
  POP     ES
  IRETD

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
