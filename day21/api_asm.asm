  [BITS 32]

  GLOBAL asm_hrb_api

  EXTERN hrb_api

asm_hrb_api:
  STI
  PUSH    DS
  PUSH    ES
  PUSHAD
  PUSHAD
  MOV     AX, SS
  MOV     DS, AX
  MOV     ES, AX
  CALL    hrb_api
  CMP     EAX, 0
  JNE     .end_app
  ADD     ESP, 32
  POPAD
  POP     ES
  POP     DS
  IRETD

.end_app:
  MOV     ESP, [EAX]
  POPAD
  RET
