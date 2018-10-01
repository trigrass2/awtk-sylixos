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
** 文   件   名: mipsAssert.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 09 月 01 日
**
** 描        述: MIPS 体系架构断言.
*********************************************************************************************************/
#define  __SYLIXOS_STDIO
#define  __SYLIXOS_PANIC
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
** 函数名称: archAssert
** 功能描述: 系统断言.
** 输　入  : iCond          条件
**           pcFunc         调用函数
**           pcFile         所在文件
**           iLine          文件行号
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  archAssert (INT  iCond, CPCHAR  pcFunc, CPCHAR  pcFile, INT  iLine)
{
    if (!iCond) {
#if LW_CFG_DEVICE_EN > 0
        fprintf(stderr, "function: %s() in file: %s line: %d serious error!\n",
                pcFunc, pcFile, iLine);
        panic("function: %s() in file: %s line: %d serious error!\n",
              pcFunc, pcFile, iLine);
#else
        panic("");
#endif
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
