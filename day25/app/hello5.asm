  [BITS 32]

  GLOBAL main

  [SECTION .text]

main:
  MOV   EDX, 2
  MOV   EBX, msg
  INT   0x40
  MOV   EDX, 4
  INT   0x40

  ; .data段执行失败？
  [SECTION .data]
msg:
  DB    "hello, world", 0x0a, 0
