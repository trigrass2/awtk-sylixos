/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: sparcVfpNone.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2017 �� 09 �� 26 ��
**
** ��        ��: SPARC ��ϵ�ܹ��� VFP ֧��.
*********************************************************************************************************/

#ifndef __ARCH_SPARC_VFP_NONE_H
#define __ARCH_SPARC_VFP_NONE_H

PSPARC_FPU_OP  sparcVfpNonePrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
VOID           sparcVfpNoneSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);

#endif                                                                  /*  __ARCH_SPARC_VFP_NONE_H     */
/*********************************************************************************************************
  END
*********************************************************************************************************/