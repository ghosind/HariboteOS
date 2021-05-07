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
  MOV     AX, SS
  CMP     AX, 1*8
  JNE     .from_app
  MOV     EAX, ESP
  PUSH    SS
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler0d
  ADD     ESP, 8
  POPAD
  POP     DS
  POP     ES
  ADD     ESP, 4
  IRETD

.from_app:
  CLI
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -8
  MOV     [ECX+4], SS
  MOV     [ECX], ESP
  MOV     SS, AX
  MOV     ES, AX
  MOV     ESP, ECX
  STI
  CALL    int_handler0d
  CLI
  CMP     EAX, 0
  JNE     .kill
  POP     ECX
  POP     EAX
  MOV     SS, AX
  MOV     ESP, ECX
  POPAD
  POP     DS
  POP     ES
  ADD     ESP, 4
  IRETD

.kill:
  MOV     EAX, 1*8
  MOV     ES, AX
  MOV     SS, AX
  MOV     DS, AX
  MOV     FS, AX
  MOV     GS, AX
  MOV     ESP, [0x0fe4]
  STI
  POPAD
  RET

asm_int_handler20:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     AX, SS
  CMP     AX, 1*8
  JNE     .from_app
  MOV     EAX, ESP
  PUSH    SS
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler20
  ADD     ESP, 8
  POPAD
  POP     DS
  POP     ES
  IRETD

.from_app:
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -8
  MOV     [ECX+4], SS
  MOV     [ECX], ESP
  MOV     SS, AX
  MOV     ES, AX
  MOV     ESP, ECX
  CALL    int_handler20
  POP     ECX
  POP     EAX
  MOV     SS, AX
  MOV     ESP, ECX
  POPAD
  POP     DS
  POP     ES
  IRETD

asm_int_handler21:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     AX, SS
  CMP     AX, 1*8
  JNE     .from_app
  MOV     EAX, ESP
  PUSH    SS
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler21
  ADD     ESP, 8
  POPAD
  POP     DS
  POP     ES
  IRETD

.from_app:
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -8
  MOV     [ECX+4], SS
  MOV     [ECX], ESP
  MOV     SS, AX
  MOV     ES, AX
  MOV     ESP, ECX
  CALL    int_handler21
  POP     ECX
  POP     EAX
  MOV     SS, AX
  MOV     ESP, ECX
  POPAD
  POP     DS
  POP     ES
  IRETD

asm_int_handler27:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     AX, SS
  CMP     AX, 1*8
  JNE     .from_app
  MOV     EAX, ESP
  PUSH    SS
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler27
  ADD     ESP, 8
  POPAD
  POP     DS
  POP     ES
  IRETD

.from_app:
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -8
  MOV     [ECX+4], SS
  MOV     [ECX], ESP
  MOV     SS, AX
  MOV     ES, AX
  MOV     ESP, ECX
  CALL    int_handler27
  POP     ECX
  POP     EAX
  MOV     SS, AX
  MOV     ESP, ECX
  POPAD
  POP     DS
  POP     ES
  IRETD

asm_int_handler2c:
  PUSH    ES
  PUSH    DS
  PUSHAD
  MOV     AX, SS
  CMP     AX, 1*8
  JNE     .from_app
  MOV     EAX, ESP
  PUSH    SS
  PUSH    EAX
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    int_handler2c
  ADD     ESP, 8
  POPAD
  POP     DS
  POP     ES
  IRETD

.from_app:
  MOV     EAX, 1*8
  MOV     DS, AX
  MOV     ECX, [0x0fe4]
  ADD     ECX, -8
  MOV     [ECX+4], SS
  MOV     [ECX], ESP
  MOV     SS, AX
  MOV     ES, AX
  MOV     ESP, ECX
  CALL    int_handler2c
  POP     ECX
  POP     EAX
  MOV     SS, AX
  MOV     ESP, ECX
  POPAD
  POP     DS
  POP     ES
  IRETD
