; hello-os

; 第一部分：FAT12 Boot记录
; 标准FAT12格式软盘专用代码
  DB    0xeb, 0x4e        ; 跳转指令
  DB    0x90              ; NOP指令
  DB    "HELLOIPL"        ; OEM标识符（8字节）
  DW    512               ; 每个扇区（sector）的字节数（必须为512字节）
  DB    1                 ; 每个簇（cluster）的扇区数（必须为1个扇区）
  DW    1                 ; FAT的预留扇区数（包含boot扇区）
  DB    2                 ; FAT表的数量，通常为2
  DW    224               ; 根目录文件的最大值（一般设为224项）
  DW    2880              ; 磁盘的扇区总数，若为0则代表超过65535个扇区，需要使用19行记录
  DB    0xf0              ; 磁盘的种类（本项目中设为0xf0代表1.44MB的软盘）
  DW    9                 ; 每个FAT的长度（必须为9扇区）
  DW    18                ; 1个磁道（track）拥有的扇区数（必须是18）
  DW    2                 ; 磁头数（必须为2）
  DD    0                 ; 隐藏的扇区数
  DD    2880              ; 大容量扇区总数，若13行记录的值为0则使用本行记录扇区数
  DB    0                 ; 中断0x13的设备号
  DB    0                 ; Windows NT标识符
  DB    0x29              ; 扩展引导标识
  DD    0xffffffff        ; 卷序列号
  DB    "HELLO-OS   "     ; 卷标（11字节）
  DB    "FAT12   "        ; 文件系统类型（8字节）

; 第二部分：引导代码、数据以及其它信息
  RESB  18                ; 空18字节

; 程序主体
  DB    0xb8, 0x00, 0x00, 0x8e, 0xd0, 0xbc, 0x00, 0x7c
  DB    0x8e, 0xd8, 0x8e, 0xc0, 0xbe, 0x74, 0x7c, 0x8a
  DB    0x04, 0x83, 0xc6, 0x01, 0x3c, 0x00, 0x74, 0x09
  DB    0xb4, 0x0e, 0xbb, 0x0f, 0x00, 0xcd, 0x10, 0xeb
  DB    0xee, 0xf4, 0xeb, 0xfd

; 信息显示部分数据
  DB    0x0a, 0x0a        ; 两个换行
  DB    "hello, world"
  DB    0x0a              ; 换行
  DB    0

  RESB  0x1fe - ($ - $$)  ; 填写0x00，直到0x001fe

; 第三部分：boot扇区结束标志，固定为0xaa55（小端表示）
  DB    0x55, 0xaa

; 启动区外的内容
  DB    0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
  RESB  4600
  DB    0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
  RESB  1469432
