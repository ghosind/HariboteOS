  ; [FORMAT "WCOFF"]
  [BITS 32]

  GLOBAL io_hlt      ; 程序中包含函数名

  [SECTION .text]
io_hlt:              ; void io_hlt(void);
  HLT
  RET
