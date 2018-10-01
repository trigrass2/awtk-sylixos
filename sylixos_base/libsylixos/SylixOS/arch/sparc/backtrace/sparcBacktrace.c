/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: sparcBacktrace.c
**
** 创   建   人: Xu.Guizhou (徐贵洲)
**
** 文件创建日期: 2017 年 05 月 15 日
**
** 描        述: SPARC 体系构架堆栈回溯 (来源于 glibc).
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  Only GCC support now.
*********************************************************************************************************/
#ifdef   __GNUC__
#include "sparcBacktrace.h"
/*********************************************************************************************************
  Private macros for guiding the backtrace implementation, sparc32 version.
*********************************************************************************************************/
#define BACKTRACE_STACK_BIAS    0
/*********************************************************************************************************
** 函数名称: backtrace
** 功能描述: 获得当前任务调用栈
** 输　入  : array     获取数组
**           size      数组大小
** 输　出  : 获取的数目
** 全局变量: 
** 调用模块: 
                                           API 函数
*********************************************************************************************************/
LW_API 
int  backtrace (void **array, int size)
{
    struct layout *current;
    unsigned long fp, i7;
    int count;

    asm volatile ("mov %%fp, %0" : "=r"(fp));
    asm volatile ("mov %%i7, %0" : "=r"(i7));
    current = (struct layout *)(fp + BACKTRACE_STACK_BIAS);

    array[0] = (void *)i7;

    if (size == 1) {
        return  (1);
    }

    SPARC_FLUSH_REG_WINDOWS();
    for (count = 1; count < size; count++) {
        array[count] = current->return_address;
        if (!current->next) {
            break;
        }
        current = (struct layout *)(current->next + BACKTRACE_STACK_BIAS);
    }

    return  (count);
}

#endif                                                                  /*  __GNUC__                    */
/*********************************************************************************************************
  END
*********************************************************************************************************/
