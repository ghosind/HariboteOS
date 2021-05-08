  [BITS 32]

  ; io_out8(PIT_CTRL, 0x34);
  MOV   AL, 0x34
  OUT   0x43, AL
  ; io_out8(PIT_CNT0, 0x40);
  MOV   AL, 0xff
  OUT   0x40, AL
  ; io_out8(PIT_CNT0, 0x40);
  MOV   AL, 0xff
  OUT   0x40, AL

  MOV   EDX, 4
  INT   0x40
