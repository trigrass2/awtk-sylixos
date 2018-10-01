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
** 文   件   名: i8237a.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2016 年 08 月 26 日
**
** 描        述: Intel 8237A DMA 驱动支持.
*********************************************************************************************************/

#ifndef __I8237A_H
#define __I8237A_H

#include "SylixOS.h"

/*********************************************************************************************************
  i8254 control
*********************************************************************************************************/

typedef struct i8237a_ctl I8237A_CTL;

struct i8237a_ctl {
    /*
     *  user MUST set following members before calling this module api.
     */
    addr_t  iobase_dma1;                                                /* 8237A DMA1 I/O base address  */
                                                                        /* eg. IO-BASE + 0x00           */
    addr_t  iobase_dma2;                                                /* 8237A DMA2 I/O base address  */
                                                                        /* eg. IO-BASE + 0xc0           */
};

/*********************************************************************************************************
  i8237a driver functions
*********************************************************************************************************/

PLW_DMA_FUNCS  i8237aGetFuncs(I8237A_CTL *pctl);

#endif                                                          /*  __I8237A_H                          */
/*********************************************************************************************************
  END
*********************************************************************************************************/
