/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: sparc_gdb.h
**
** 创   建   人: Xu.Guizhou (徐贵洲)
**
** 文件创建日期: 2017 年 05 月 15 日
**
** 描        述: SPARC 体系构架 GDB 调试接口.
*********************************************************************************************************/

#ifndef __ARCH_SPARC_GDB_H
#define __ARCH_SPARC_GDB_H

/*********************************************************************************************************
  最大寄存器数
*********************************************************************************************************/
#define GDB_MAX_REG_CNT     86

/*********************************************************************************************************
  寄存器集合结构
*********************************************************************************************************/
typedef struct {
    INT         GDBR_iRegCnt;                                           /*  寄存器数量                  */
    struct {
        ULONG   GDBRA_ulValue;                                          /*  寄存器值                    */
    } regArr[GDB_MAX_REG_CNT];                                          /*  寄存器数组                  */
} GDB_REG_SET;

/*********************************************************************************************************
  Xfer:features:read:target.xml 与 Xfer:features:read:arch-core.xml 回应包
*********************************************************************************************************/
CPCHAR  archGdbTargetXml(VOID);

CPCHAR  archGdbCoreXml(VOID);

/*********************************************************************************************************
  gdb 需要的和体系结构相关的功能
*********************************************************************************************************/
INT     archGdbRegsGet(PVOID               pvDtrace,
                       LW_OBJECT_HANDLE    ulThread,
                       GDB_REG_SET        *pregset);                    /*  获取系统寄存器信息          */

INT     archGdbRegsSet(PVOID               pvDtrace,
                       LW_OBJECT_HANDLE    ulThread,
                       GDB_REG_SET        *pregset);                    /*  设置系统寄存器信息          */

INT     archGdbRegSetPc(PVOID              pvDtrace,
                        LW_OBJECT_HANDLE   ulThread,
                        ULONG              uiPc);                       /*  设置 PC 寄存器              */

ULONG   archGdbRegGetPc(GDB_REG_SET *pRegs);                            /*  获取 PC 寄存器值            */

ULONG   archGdbGetNextPc(PVOID             pvDtrace,
                         LW_OBJECT_HANDLE  ulThread,
                         GDB_REG_SET      *pRegs);                      /*  获取下一个 PC 值，含分支预测*/

#endif                                                                  /*  __ARCH_SPARC_GDB_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/
