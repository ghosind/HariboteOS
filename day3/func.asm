  ; [FORMAT "WCOFF"]
  [BITS 32]

  global _io_hlt      ; 程序中包含函数名

  [SECTION .text]
_io_hlt:              ; void io_hlt(void);
  HLT
  RET
