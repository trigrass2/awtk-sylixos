;/*********************************************************************************************************
;**
;**                                    �й�������Դ��֯
;**
;**                                   Ƕ��ʽʵʱ����ϵͳ
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------�ļ���Ϣ--------------------------------------------------------------------------------
;**
;** ��   ��   ��: mipsExcAsm.S
;**
;** ��   ��   ��: Jiao.JinXing (������)
;**
;** �ļ���������: 2015 �� 09 �� 01 ��
;**
;** ��        ��: MIPS ��ϵ�ܹ��쳣����.
;*********************************************************************************************************/

#ifndef ASSEMBLY
#define ASSEMBLY 1
#endif

#include <arch/assembler.h>
#include "mipsContextAsm.h"

    FILE_BEGIN()

    IMPORT_LABEL(API_ThreadTcbInter)
    IMPORT_LABEL(API_InterStackBaseGet)
    IMPORT_LABEL(API_InterEnter)
    IMPORT_LABEL(API_InterExit)

    IMPORT_LABEL(bspIntHandle)
    IMPORT_LABEL(archCacheErrorHandle)
    IMPORT_LABEL(archExceptionHandle)

    EXPORT_LABEL(archInterruptEntry)
    EXPORT_LABEL(archCacheErrorEntry)
    EXPORT_LABEL(archExceptionEntry)

;/*********************************************************************************************************
;  ������ʱ�����쳣������
;  ����� RDHWR ָ��, ����ͨ�� RDHWR ָ���� CPU NUM,
;  ����ÿ���������߼��˵���ʱ�쳣���������໥������, �ʲ��ü�������, �����ʵʱ��
;*********************************************************************************************************/

#define ARCH_TEMP_CTX_SIZE      4096
#define XLOCK                   4000

#if LW_CFG_MIPS_HAS_RDHWR_INSTR > 0
MACRO_DEF(QUAD_VALUE from, to)
    .quad       ((\from) * ARCH_TEMP_CTX_SIZE + archExcTempCtxMp)
    .if         \to-\from
    QUAD_VALUE  "(\from+1)",\to
    .endif
    MACRO_END()

    SECTION(.data)
    .balign     ARCH_STK_ALIGN_SIZE
LINE_LABEL(archExcTempCtx)
    QUAD_VALUE  0 , (LW_CFG_MAX_PROCESSORS - 1)

    SECTION(.bss)
    .balign     ARCH_STK_ALIGN_SIZE
LINE_LABEL(archExcTempCtxMp)
    .space      (ARCH_TEMP_CTX_SIZE * LW_CFG_MAX_PROCESSORS)
#else
    SECTION(.bss)
    .balign     ARCH_STK_ALIGN_SIZE
LINE_LABEL(archExcTempCtx)
    .space      ARCH_TEMP_CTX_SIZE
#endif

;/*********************************************************************************************************
;  �쳣��ں�
;*********************************************************************************************************/

MACRO_DEF(EXCEPTION_ENTRY handle)
    .set    push
    .set    noat

    ;/*
    ; * ��Ϊ��Щ�����ں˴��룬���� TLB ת��������ִ��ʱ���ᷢ�� TLB �����쳣
    ; */

    ;/*
    ; * archExcTempCtx ���ں��ڴ棬���� TLB ת�������Է���ʱ���ᷢ�� TLB �����쳣
    ; */
1:
    PTR_LA      K0 , archExcTempCtx
#if LW_CFG_MIPS_HAS_RDHWR_INSTR > 0
    ;/*
    ; * ʹ�øú˵� archExcTempCtx
    ; */
    .set        push
    .set        mips64r2
    RDHWR       K1 , HWR_CPUNUM
    .set        pop
    LONG_SLL    K1 , K1 , LONGLOG
    REG_ADDU    K1 , K1 , K0
    REG_L       K0 , 0(K1)
#else
    ;/*
    ; * �� archExcTempCtx ���м���
    ; */
    LL          K1 , XLOCK(K0)
    BNEZ        K1 , 1b
    NOP

    LI          K1 , 1
    SC          K1 , XLOCK(K0)
    BEQZ        K1 , 1b
    SYNC
#endif

    ;/*
    ; * ����Ĵ����� archExcTempCtx
    ; */
    REG_S       $0  , XGREG(0)(K0)
    REG_S       $1  , XGREG(1)(K0)
    REG_S       $2  , XGREG(2)(K0)
    REG_S       $3  , XGREG(3)(K0)
    REG_S       $4  , XGREG(4)(K0)
    REG_S       $5  , XGREG(5)(K0)
    REG_S       $6  , XGREG(6)(K0)
    REG_S       $7  , XGREG(7)(K0)
    REG_S       $8  , XGREG(8)(K0)
    REG_S       $9  , XGREG(9)(K0)
    REG_S       $10 , XGREG(10)(K0)
    REG_S       $11 , XGREG(11)(K0)
    REG_S       $12 , XGREG(12)(K0)
    REG_S       $13 , XGREG(13)(K0)
    REG_S       $14 , XGREG(14)(K0)
    REG_S       $15 , XGREG(15)(K0)
    REG_S       $16 , XGREG(16)(K0)
    REG_S       $17 , XGREG(17)(K0)
    REG_S       $18 , XGREG(18)(K0)
    REG_S       $19 , XGREG(19)(K0)
    REG_S       $20 , XGREG(20)(K0)
    REG_S       $21 , XGREG(21)(K0)
    REG_S       $22 , XGREG(22)(K0)
    REG_S       $23 , XGREG(23)(K0)
    REG_S       $24 , XGREG(24)(K0)
    REG_S       $25 , XGREG(25)(K0)
    ;/*
    ; * $26 $27 �� K0 K1
    ; */
    REG_S       $28 , XGREG(28)(K0)
    REG_S       $29 , XGREG(29)(K0)
    REG_S       $30 , XGREG(30)(K0)
    REG_S       $31 , XGREG(31)(K0)

    MFC0_LONG_EHB(T1, CP0_EPC)
    REG_S       T1 , XEPC(K0)                                           ;/*  ���� EPC �Ĵ���             */

    MFC0_EHB(T2, CP0_STATUS)                                            ;/*  ���� STATUS �Ĵ���          */
    LI          T0 , ~(ST0_EXL | ST0_ERL)
    AND         T2 , T2, T0                                             ;/*  T2 �����쳣ǰ�� SR �Ĵ���   */
    REG_S       T2 , XSR(K0)

    MFC0_LONG_EHB(T1, CP0_BADVADDR)                                     ;/*  ���� BADVADDR �Ĵ���        */
    REG_S       T1 , XBADVADDR(K0)

    MFC0_EHB(T1, CP0_CAUSE)                                             ;/*  ���� CAUSE �Ĵ���           */
    REG_S       T1 , XCAUSE(K0)

    MFLO        T1                                                      ;/*  ���� LO �Ĵ���              */
    REG_S       T1 , XLO(K0)

    MFHI        T1                                                      ;/*  ���� HI �Ĵ���              */
    REG_S       T1 , XHI(K0)

    ;/*
    ; * �ص��ں�ģʽ�����ж�
    ; */
    LI          T0 , ~(ST0_IE | ST0_KSU)
    AND         T2 , T2, T0
    MTC0_EHB(T2, CP0_STATUS)

    ;/*
    ; * �Ѿ���� EXL λ��TLB ����������Լ����쳣�������
    ; */

    ;/*
    ; * �������� GP �Ĵ���
    ; */

    PTR_LA      GP , _gp

    ;/*
    ; * ���µ��Ӻ������ò����ƻ� S0 �Ĵ���
    ; */
    MOV         S0 , K0
    REG_ADDU    SP , K0 , (ARCH_TEMP_CTX_SIZE - ARCH_STK_ALIGN_SIZE)

    ;/*
    ; * API_InterEnter(A0)
    ; */
    REG_SUBU    SP , ARCH_STK_VAR_SIZE
    MOV         A0 , S0
    JAL         API_InterEnter
    NOP
    REG_ADDU    SP , ARCH_STK_VAR_SIZE

    LI          V1 , 1
    BNE         V0 , V1 , 3f
    NOP

    ;/*
    ; * ��һ�ν����ж�: ��õ�ǰ CPU �ж϶�ջջ��, ������ SP
    ; */
    REG_SUBU    SP , ARCH_STK_VAR_SIZE
    JAL         API_InterStackBaseGet
    NOP
    MOV         SP , V0

    ;/*
    ; * ��õ�ǰ���� TCB �� ARCH_REG_CTX ��ַ
    ; */
    REG_SUBU    SP , ARCH_STK_VAR_SIZE
    JAL         API_ThreadTcbInter
    NOP
    REG_ADDU    SP , ARCH_STK_VAR_SIZE
    MOV         A0 , V0

2:
    ;/*
    ; * ���� archExcTempCtx
    ; */
#if LW_CFG_MIPS_HAS_RDHWR_INSTR == 0
    SYNC
    REG_S       ZERO , XLOCK(S0)
#endif

    ;/*
    ; * handle(�Ĵ���������)
    ; */
    REG_SUBU    SP , ARCH_STK_VAR_SIZE
    JAL         \handle
    NOP

    ;/*
    ; * API_InterExit()
    ; * ���û�з����ж�Ƕ��, �� API_InterExit ����� archIntCtxLoad ����
    ; */
    JAL         API_InterExit
    NOP

    ;/*
    ; * ��������, ˵���������ж�Ƕ��
    ; */
    REG_ADDU    SP , ARCH_STK_VAR_SIZE

    MOV         T9 , SP

    RESTORE_REGS                                                        ;/*  �ָ����мĴ���              */

3:
    ;/*
    ; * ���ǵ�һ�ν����ж�
    ; */
    REG_L       SP , XGREG(29)(S0)
    REG_SUBU    SP , ARCH_REG_CTX_SIZE
    B           2b
    MOV         A0 , SP
    .set    pop
    MACRO_END()

;/*********************************************************************************************************
;  �ж����
;*********************************************************************************************************/

FUNC_DEF(archInterruptEntry)
    EXCEPTION_ENTRY     bspIntHandle
    FUNC_END(archInterruptEntry)

;/*********************************************************************************************************
;  CACHE �����쳣���
;*********************************************************************************************************/

FUNC_DEF(archCacheErrorEntry)
    EXCEPTION_ENTRY     archCacheErrorHandle
    FUNC_END(archCacheErrorEntry)

;/*********************************************************************************************************
;  ͨ���쳣���
;*********************************************************************************************************/

FUNC_DEF(archExceptionEntry)
    EXCEPTION_ENTRY     archExceptionHandle
    FUNC_END(archExceptionEntry)

    FILE_END()

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/