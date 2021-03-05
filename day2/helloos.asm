; hello-os

  ORG   0x7c00            ; 指明程序的装载地址

; 用于标准FAT12格式的软盘
  JMP   entry             ; 跳转指令
  NOP                     ; NOP指令（0x90）
  DB    "HELLOIPL"        ; OEM标识符（8字节）
  DW    512               ; 每个扇区（sector）的字节数（必须为512字节）
  DB    1                 ; 每个簇（cluster）的扇区数（必须为1个扇区）
  DW    1                 ; FAT的预留扇区数（包含boot扇区）
  DB    2                 ; FAT表的数量，通常为2
  DW    224               ; 根目录文件的最大值（一般设为224项）
  DW    2880              ; 磁盘的扇区总数，若为0则代表超过65535个扇区，需要使用20行记录
  DB    0xf0              ; 磁盘的种类（本项目中设为0xf0代表1.44MB的软盘）
  DW    9                 ; 每个FAT的长度（必须为9扇区）
  DW    18                ; 1个磁道（track）拥有的扇区数（必须是18）
  DW    2                 ; 磁头数（必须为2）
  DD    0                 ; 隐藏的扇区数
  DD    2880              ; 大容量扇区总数，若14行记录的值为0则使用本行记录扇区数
  DB    0                 ; 中断0x13的设备号
  DB    0                 ; Windows NT标识符
  DB    0x29              ; 扩展引导标识
  DD    0xffffffff        ; 卷序列号
  DB    "HELLO-OS   "     ; 卷标（11字节）
  DB    "FAT12   "        ; 文件系统类型（8字节）
  RESB  18                ; 空18字节

; 程序核心

entry:
  MOV   AX, 0             ; 初始化寄存器
  MOV   SS, AX
  MOV   SP, 0x7c00
  MOV   DS, AX
  MOV   ES, AX

  MOV   SI, msg
putloop:
  MOV   AL, [SI]
  ADD   SI, 1             ; SI加1
  CMP   AL, 0

  JE    fin
  MOV   AH, 0x0e          ; 显示一个文字
  MOV   BX, 15            ; 指定字符颜色
  INT   0x10              ; 调用显卡BIOS
  JMP   putloop

fin:
  HLT                     ; CPU停止，等待指令
  JMP   fin               ; 无限循环

msg:
  DB    0x0a, 0x0a        ; 两个换行
  DB    "hello, world"
  DB    0x0a              ; 换行
  DB    0

  RESB  0x1fe - ($ - $$)  ; 填写0x00，直到0x001fe
  DB    0x55, 0xaa
