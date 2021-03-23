; haribote-os

BOTPAK  EQU   0x00280000  ; 加载bootpack
DSKCAC  EQU   0x00100000  ; 磁盘缓存的位置
DSKCAC0 EQU   0x00008000  ; 实模式磁盘缓存的位置

; 有关BOOT_INFO
CYLS    EQU   0x0ff0      ; 设置启动区
LEDS    EQU   0x0ff1
VMODE   EQU   0x0ff2      ; 关于颜色数目的信息，颜色的位数
SCRNX   EQU   0x0ff4      ; 分辨率X
SCRNY   EQU   0x0ff6      ; 分辨率Y
VRAM    EQU   0x0ff8      ; 图像缓冲区的起始位置

  ; 使用linker script指定起始地址
  ORG   0xc200            ; 程序被加载的内存地址

  [BITS 16]
entry:
; 设置屏幕模式
  MOV   AL, 0x13          ; VGA显卡，320x200x8 bit
  MOV   AH, 0x00
  INT   0x10

  MOV   BYTE [VMODE], 8   ; 屏幕的模式
  MOV   WORD [SCRNX], 320
  MOV   WORD [SCRNY], 200
  MOV   DWORD [VRAM], 0x000a0000

; 用BIOS取得键盘上各种LED指示灯的状态
  MOV   AH, 0x02
  INT   0x16              ; 键盘BIOS
  MOV   [LEDS], AL

; 防止PIC接受所有中断
;   根据AT兼容机的规范初始化PIC
;   如果没有在CLI指令前执行可能会挂起
;   并继续初始化PIC
  MOV   AL, 0xff
  OUT   0x21, AL
  NOP                     ; 有些机器不能连续执行NOP指令
  OUT   0xa1, AL

  CLI

; 设置A20GATE使CPU支持1M以上的内存
  CALL  waitkbdout
  MOV   AL, 0xd1
  OUT   0x64, AL
  CALL  waitkbdout
  MOV   AL, 0xdf          ; 开启A20
  OUT   0x60, AL
  CALL  waitkbdout

; 切换到保护模式
  LGDT  [GDTR0]           ; 设置临时GDT
  MOV   EAX, CR0
  AND   EAX, 0x7fffffff   ; 禁用分页
  OR    EAX, 0x00000001   ; 开启保护模式
  MOV   CR0, EAX
  JMP   pipelineflush

pipelineflush:
  MOV   AX, 1*8           ; 写32bit段
  MOV   DS, AX
  MOV   ES, AX
  MOV   FS, AX
  MOV   GS, AX
  MOV   SS, AX

; bootpack传递
  MOV   ESI, bootpack     ; 源
  MOV   EDI, BOTPAK       ; 目标
  MOV   ECX, 512*1024/4
  CALL  memcpy

; 传输磁盘数据

; 从引导区开始

  MOV   ESI, 0x7c00       ; 源
  MOV   EDI, DSKCAC       ; 目标
  MOV   ECX, 512/4
  CALL  memcpy

; 剩余的全部
  MOV   ESI, DSKCAC0+512  ; 源
  MOV   EDI, DSKCAC+512   ; 目标
  MOV   ECX, 0
  MOV   CL, BYTE [CYLS]
  IMUL  ECX, 512*18*2/4   ; 除以4得到字节数
  SUB   ECX, 512/4        ; IPL偏移量
  CALL  memcpy

skip:
  MOV   ESP, 0xffff
  JMP   DWORD 2*8:0x00000000

waitkbdout:
  IN    AL, 0x64
  AND   AL, 0x02
  JNZ   waitkbdout        ; AND结果不为0跳转至waitkbdout
  RET

memcpy:
  MOV   EAX, [ESI]
  ADD   ESI, 4
  MOV   [EDI], EAX
  ADD   EDI, 4
  SUB   ECX, 1
  JNZ   memcpy            ; 结果不为0跳转至memcpy
  RET
; memcpy地址前缀大小

  ALIGN 16
GDT0:
  RESB  8                 ; 初始值
  DW    0xffff, 0x0000, 0x9200, 0x00cf  ; 可写的32位段寄存器
  DW    0xffff, 0x0000, 0x9a28, 0x0047  ; 可执行的文件的32位寄存器

  DW    0

GDTR0:
  DW    8*3-1
  DD    GDT0

  ALIGN 16
bootpack:
