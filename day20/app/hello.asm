  [BITS 32]

  MOV   ECX, msg
  MOV   EDX, 1

put_loop:
  MOV   AL, [CS:ECX]
  CMP   AL, 0
  JE    fin
  INT   0x40
  ADD   ECX, 1
  JMP   put_loop

fin:
  RETF

msg:
  DB    "hello", 0
