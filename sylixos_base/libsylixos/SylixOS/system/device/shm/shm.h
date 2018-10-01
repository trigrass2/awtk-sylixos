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
** 文   件   名: shm.c
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2012 年 12 月 27 日
**
** 描        述: 共享内存设备.
*********************************************************************************************************/

#ifndef __SHM_H
#define __SHM_H

/*********************************************************************************************************
  加入裁剪支持
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0 && LW_CFG_SHM_DEVICE_EN > 0 && LW_CFG_VMM_EN > 0 

LW_API INT      API_ShmDrvInstall(VOID);
LW_API INT      API_ShmDevCreate(VOID);

#define shmDrv          API_ShmDrvInstall
#define shmDevCreate    API_ShmDevCreate

#endif                                                                  /*  LW_CFG_DEVICE_EN            */
                                                                        /*  LW_CFG_SHM_DEVICE_EN        */
                                                                        /*  LW_CFG_VMM_EN               */
#endif                                                                  /*  __SHM_H                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/
