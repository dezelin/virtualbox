/* $Id$ */
/** @file
 * IEM Testcase - Check the "Microcode".
 */

/*
 * Copyright (C) 2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <iprt/assert.h>
#include <iprt/rand.h>
#include <iprt/test.h>

#include <VBox/types.h>
#include <VBox/err.h>
#include "../include/IEMInternal.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
bool volatile       g_fRandom;
uint8_t volatile    g_bRandom;


/** For hacks.  */
#define TST_IEM_CHECK_MC

#define CHK_TYPE(a_ExpectedType, a_Param) \
    do { a_ExpectedType *pCheckType = &(a_Param); } while (0)
#define CHK_PTYPE(a_ExpectedType, a_Param) \
    do { a_ExpectedType pCheckType = (a_Param); } while (0)


/** @name Other stubs.
 * @{   */

typedef VBOXSTRICTRC (* PFNIEMOP)(PIEMCPU pIemCpu);
#define FNIEMOP_DEF(a_Name) \
    static VBOXSTRICTRC a_Name(PIEMCPU pIemCpu) RT_NO_THROW
#define FNIEMOP_DEF_1(a_Name, a_Type0, a_Name0) \
    static VBOXSTRICTRC a_Name(PIEMCPU pIemCpu, a_Type0 a_Name0) RT_NO_THROW
#define FNIEMOP_DEF_2(a_Name, a_Type0, a_Name0, a_Type1, a_Name1) \
    static VBOXSTRICTRC a_Name(PIEMCPU pIemCpu, a_Type0 a_Name0, a_Type1 a_Name1) RT_NO_THROW

#define IEM_NOT_REACHED_DEFAULT_CASE_RET()                  default: return VERR_INTERNAL_ERROR_4

#define IEM_OPCODE_GET_NEXT_BYTE(a_pIemCpu, a_pu8)          do { *(a_pu8)  = g_bRandom; CHK_PTYPE(uint8_t  *, a_pu8);  } while (0)
#define IEM_OPCODE_GET_NEXT_U16(a_pIemCpu, a_pu16)          do { *(a_pu16) = g_bRandom; CHK_PTYPE(uint16_t *, a_pu16); } while (0)
#define IEM_OPCODE_GET_NEXT_U32(a_pIemCpu, a_pu32)          do { *(a_pu32) = g_bRandom; CHK_PTYPE(uint32_t *, a_pu32); } while (0)
#define IEM_OPCODE_GET_NEXT_S32_SX_U64(a_pIemCpu, a_pu64)   do { *(a_pu64) = g_bRandom; CHK_PTYPE(uint64_t *, a_pu64); } while (0)
#define IEM_OPCODE_GET_NEXT_U64(a_pIemCpu, a_pu64)          do { *(a_pu64) = g_bRandom; CHK_PTYPE(uint64_t *, a_pu64); } while (0)
#define IEM_OPCODE_GET_NEXT_S8(a_pIemCpu,  a_pi8)           do { *(a_pi8)  = g_bRandom; CHK_PTYPE(int8_t   *, a_pi8);  } while (0)
#define IEM_OPCODE_GET_NEXT_S16(a_pi16)                     do { *(a_pi16) = g_bRandom; CHK_PTYPE(int16_t  *, a_pi16); } while (0)
#define IEM_OPCODE_GET_NEXT_S32(a_pi32)                     do { *(a_pi32) = g_bRandom; CHK_PTYPE(int32_t  *, a_pi32); } while (0)
#define IEMOP_HLP_NO_LOCK_PREFIX()                          do { } while (0)
#define IEMOP_HLP_NO_64BIT()                                do { } while (0)
#define IEMOP_HLP_DEFAULT_64BIT_OP_SIZE()                   do { } while (0)
#define IEMOP_RAISE_INVALID_OPCODE()                        VERR_TRPM_ACTIVE_TRAP
#define IEMOP_RAISE_INVALID_LOCK_PREFIX()                   VERR_TRPM_ACTIVE_TRAP
#define IEMOP_MNEMONIC(a_szMnemonic)                        do { } while (0)
#define IEMOP_MNEMONIC2(a_szMnemonic, a_szOps)              do { } while (0)
#define FNIEMOP_STUB(a_Name) \
    FNIEMOP_DEF(a_Name) { return VERR_NOT_IMPLEMENTED; } \
    typedef int ignore_semicolon
#define FNIEMOP_STUB_1(a_Name, a_Type0, a_Name0) \
    FNIEMOP_DEF_1(a_Name, a_Type0, a_Name0) { return VERR_NOT_IMPLEMENTED; } \
    typedef int ignore_semicolon


#define FNIEMOP_CALL(a_pfn)                                 (a_pfn)(pIemCpu)
#define FNIEMOP_CALL_1(a_pfn, a0)                           (a_pfn)(pIemCpu, a0)
#define FNIEMOP_CALL_2(a_pfn, a0, a1)                       (a_pfn)(pIemCpu, a0, a1)

#define IEM_IS_REAL_OR_V86_MODE(a_pIemCpu)                  (g_fRandom)
#define IEM_IS_LONG_MODE(a_pIemCpu)                         (g_fRandom)
#define IEM_IS_REAL_MODE(a_pIemCpu)                         (g_fRandom)
#define IEM_IS_AMD_CPUID_FEATURE_PRESENT_ECX(a_fEcx)        (g_fRandom)
#define IEM_IS_INTEL_CPUID_FEATURE_PRESENT_EDX(a_fEdx)      (g_fRandom)
//#define IEM_IS_CANONICAL(a_u64Addr)                         (g_fRandom)

#define iemRecalEffOpSize(a_pIemCpu)                        do { } while (0)

IEMOPBINSIZES g_iemAImpl_add;
IEMOPBINSIZES g_iemAImpl_adc;
IEMOPBINSIZES g_iemAImpl_sub;
IEMOPBINSIZES g_iemAImpl_sbb;
IEMOPBINSIZES g_iemAImpl_or;
IEMOPBINSIZES g_iemAImpl_xor;
IEMOPBINSIZES g_iemAImpl_and;
IEMOPBINSIZES g_iemAImpl_cmp;
IEMOPBINSIZES g_iemAImpl_test;
IEMOPBINSIZES g_iemAImpl_bt;
IEMOPBINSIZES g_iemAImpl_btc;
IEMOPBINSIZES g_iemAImpl_btr;
IEMOPBINSIZES g_iemAImpl_bts;
IEMOPBINSIZES g_iemAImpl_bsf;
IEMOPBINSIZES g_iemAImpl_bsr;
IEMOPBINSIZES g_iemAImpl_imul_two;
PCIEMOPBINSIZES g_apIemImplGrp1[8];
IEMOPUNARYSIZES g_iemAImpl_inc;
IEMOPUNARYSIZES g_iemAImpl_dec;
IEMOPUNARYSIZES g_iemAImpl_neg;
IEMOPUNARYSIZES g_iemAImpl_not;
IEMOPSHIFTSIZES g_iemAImpl_rol;
IEMOPSHIFTSIZES g_iemAImpl_ror;
IEMOPSHIFTSIZES g_iemAImpl_rcl;
IEMOPSHIFTSIZES g_iemAImpl_rcr;
IEMOPSHIFTSIZES g_iemAImpl_shl;
IEMOPSHIFTSIZES g_iemAImpl_shr;
IEMOPSHIFTSIZES g_iemAImpl_sar;
IEMOPMULDIVSIZES g_iemAImpl_mul;
IEMOPMULDIVSIZES g_iemAImpl_imul;
IEMOPMULDIVSIZES g_iemAImpl_div;
IEMOPMULDIVSIZES g_iemAImpl_idiv;
IEMOPSHIFTDBLSIZES g_iemAImpl_shld;
IEMOPSHIFTDBLSIZES g_iemAImpl_shrd;

/** @}  */


/** @name Microcode test stubs
 * @{  */

#define IEM_MC_BEGIN(cArgs, cLocals)                    {
#define IEM_MC_END()                                    }
#define IEM_MC_PAUSE()                                  do {} while (0)
#define IEM_MC_CONTINUE()                               do {} while (0)
#define IEM_MC_RETURN_ON_FAILURE(a_Expr)                do {} while (0)
#define IEM_MC_ADVANCE_RIP()                            do {} while (0)
#define IEM_MC_REL_JMP_S8(a_i8)                         CHK_TYPE(int8_t, a_i8)
#define IEM_MC_REL_JMP_S16(a_i16)                       CHK_TYPE(int16_t, a_i16)
#define IEM_MC_REL_JMP_S32(a_i32)                       CHK_TYPE(int32_t, a_i32)
#define IEM_MC_SET_RIP_U16(a_u16NewIP)                  do {} while (0)
#define IEM_MC_SET_RIP_U32(a_u32NewIP)                  do {} while (0)
#define IEM_MC_SET_RIP_U64(a_u64NewIP)                  do {} while (0)
#define IEM_MC_RAISE_DIVIDE_ERROR()                     return VERR_TRPM_ACTIVE_TRAP
#define IEM_MC_MAYBE_RAISE_DEVICE_NOT_AVAILABLE()       do {} while (0)
#define IEM_MC_MAYBE_RAISE_FPU_XCPT()                   do {} while (0)
#define IEM_MC_RAISE_GP0_IF_CPL_NOT_ZERO()              do {} while (0)

#define IEM_MC_LOCAL(a_Type, a_Name)                    a_Type a_Name
#define IEM_MC_LOCAL_CONST(a_Type, a_Name, a_Value)     a_Type const a_Name = (a_Value)
#define IEM_MC_REF_LOCAL(a_pRefArg, a_Local)            (a_pRefArg) = &(a_Local)
#define IEM_MC_ARG(a_Type, a_Name, a_iArg)              a_Type a_Name
#define IEM_MC_ARG_CONST(a_Type, a_Name, a_Value, a_iArg)   a_Type const a_Name = (a_Value)
#define IEM_MC_ARG_LOCAL_EFLAGS(a_pName, a_Name, a_iArg) \
    uint32_t a_Name; \
    uint32_t *a_pName = &a_Name
#define IEM_MC_COMMIT_EFLAGS(a_EFlags)                  do {} while (0)
#define IEM_MC_ASSIGN(a_VarOrArg, a_CVariableOrConst)   (a_VarOrArg) = (0)
#define IEM_MC_FETCH_GREG_U8(a_u8Dst, a_iGReg)          (a_u8Dst)  = 0
#define IEM_MC_FETCH_GREG_U8_ZX_U16(a_u16Dst, a_iGReg)  (a_u16Dst) = 0
#define IEM_MC_FETCH_GREG_U8_ZX_U32(a_u32Dst, a_iGReg)  (a_u32Dst) = 0
#define IEM_MC_FETCH_GREG_U8_ZX_U64(a_u64Dst, a_iGReg)  (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U8_SX_U16(a_u16Dst, a_iGReg)  (a_u16Dst) = 0
#define IEM_MC_FETCH_GREG_U8_SX_U32(a_u32Dst, a_iGReg)  (a_u32Dst) = 0
#define IEM_MC_FETCH_GREG_U8_SX_U64(a_u64Dst, a_iGReg)  (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U16(a_u16Dst, a_iGReg)        (a_u16Dst) = 0
#define IEM_MC_FETCH_GREG_U16_ZX_U32(a_u32Dst, a_iGReg) (a_u32Dst) = 0
#define IEM_MC_FETCH_GREG_U16_ZX_U64(a_u64Dst, a_iGReg) (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U16_SX_U32(a_u32Dst, a_iGReg) (a_u32Dst) = 0
#define IEM_MC_FETCH_GREG_U16_SX_U64(a_u64Dst, a_iGReg) (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U32(a_u32Dst, a_iGReg)        (a_u32Dst) = 0
#define IEM_MC_FETCH_GREG_U32_ZX_U64(a_u64Dst, a_iGReg) (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U32_SX_U64(a_u64Dst, a_iGReg) (a_u64Dst) = 0
#define IEM_MC_FETCH_GREG_U64(a_u64Dst, a_iGReg)        (a_u64Dst) = 0
#define IEM_MC_FETCH_SREG_U16(a_u16Dst, a_iSReg)        (a_u16Dst) = 0
#define IEM_MC_FETCH_SREG_ZX_U32(a_u32Dst, a_iSReg)     (a_u32Dst) = 0
#define IEM_MC_FETCH_SREG_ZX_U64(a_u64Dst, a_iSReg)     (a_u64Dst) = 0
#define IEM_MC_FETCH_CR0_U16(a_u16Dst)                  (a_u16Dst) = 0
#define IEM_MC_FETCH_CR0_U32(a_u32Dst)                  (a_u32Dst) = 0
#define IEM_MC_FETCH_CR0_U64(a_u64Dst)                  (a_u64Dst) = 0
#define IEM_MC_FETCH_EFLAGS(a_EFlags)                   (a_EFlags) = 0
#define IEM_MC_FETCH_FSW(a_u16Fsw)                      (a_u16Fsw) = 0
#define IEM_MC_STORE_GREG_U8(a_iGReg, a_u8Value)        do {} while (0)
#define IEM_MC_STORE_GREG_U16(a_iGReg, a_u16Value)      do {} while (0)
#define IEM_MC_STORE_GREG_U32(a_iGReg, a_u32Value)      do {} while (0)
#define IEM_MC_STORE_GREG_U64(a_iGReg, a_u64Value)      do {} while (0)
#define IEM_MC_CLEAR_HIGH_GREG_U64(a_iGReg)             do {} while (0)
#define IEM_MC_REF_GREG_U8(a_pu8Dst, a_iGReg)           (a_pu8Dst)  = (uint8_t  *)((uintptr_t)0)
#define IEM_MC_REF_GREG_U16(a_pu16Dst, a_iGReg)         (a_pu16Dst) = (uint16_t *)((uintptr_t)0)
#define IEM_MC_REF_GREG_U32(a_pu32Dst, a_iGReg)         (a_pu32Dst) = (uint32_t *)((uintptr_t)0)
#define IEM_MC_REF_GREG_U64(a_pu64Dst, a_iGReg)         (a_pu64Dst) = (uint64_t *)((uintptr_t)0)
#define IEM_MC_REF_EFLAGS(a_pEFlags)                    (a_pEFlags) = (uint32_t *)((uintptr_t)0)
#define IEM_MC_ADD_GREG_U8(a_iGReg, a_u16Value)         do {} while (0)
#define IEM_MC_ADD_GREG_U16(a_iGReg, a_u16Value)        do {} while (0)
#define IEM_MC_ADD_GREG_U32(a_iGReg, a_u32Value)        do {} while (0)
#define IEM_MC_ADD_GREG_U64(a_iGReg, a_u64Value)        do {} while (0)
#define IEM_MC_SUB_GREG_U8(a_iGReg,  a_u8Value)         do {} while (0)
#define IEM_MC_SUB_GREG_U16(a_iGReg, a_u16Value)        do {} while (0)
#define IEM_MC_SUB_GREG_U32(a_iGReg, a_u32Value)        do {} while (0)
#define IEM_MC_SUB_GREG_U64(a_iGReg, a_u64Value)        do {} while (0)

#define IEM_MC_ADD_GREG_U8_TO_LOCAL(a_u16Value, a_iGReg)   (a_u8Value)  += 1
#define IEM_MC_ADD_GREG_U16_TO_LOCAL(a_u16Value, a_iGReg)  (a_u16Value) += 1
#define IEM_MC_ADD_GREG_U32_TO_LOCAL(a_u32Value, a_iGReg)  (a_u32Value) += 1
#define IEM_MC_ADD_GREG_U64_TO_LOCAL(a_u64Value, a_iGReg)  (a_u64Value) += 1
#define IEM_MC_ADD_LOCAL_S16_TO_EFF_ADDR(a_EffAddr, a_i16) do { (a_EffAddr) += (a_i16); } while (0)
#define IEM_MC_ADD_LOCAL_S32_TO_EFF_ADDR(a_EffAddr, a_i32) do { (a_EffAddr) += (a_i32); } while (0)
#define IEM_MC_ADD_LOCAL_S64_TO_EFF_ADDR(a_EffAddr, a_i64) do { (a_EffAddr) += (a_i64); } while (0)
#define IEM_MC_AND_LOCAL_U16(a_u16Local, a_u16Mask)     do { (a_u16Local) &= (a_u16Mask); } while (0)
#define IEM_MC_AND_LOCAL_U32(a_u32Local, a_u32Mask)     do { (a_u32Local) &= (a_u32Mask); } while (0)
#define IEM_MC_AND_LOCAL_U64(a_u64Local, a_u64Mask)     do { (a_u64Local) &= (a_u64Mask); } while (0)
#define IEM_MC_AND_ARG_U16(a_u16Arg, a_u16Mask)         do { (a_u16Arg) &= (a_u16Mask); } while (0)
#define IEM_MC_AND_ARG_U32(a_u32Arg, a_u32Mask)         do { (a_u32Arg) &= (a_u32Mask); } while (0)
#define IEM_MC_AND_ARG_U64(a_u64Arg, a_u64Mask)         do { (a_u64Arg) &= (a_u64Mask); } while (0)
#define IEM_MC_SAR_LOCAL_S16(a_i16Local, a_cShift)      do { (a_i16Local) >>= (a_cShift);  } while (0)
#define IEM_MC_SAR_LOCAL_S32(a_i32Local, a_cShift)      do { (a_i32Local) >>= (a_cShift);  } while (0)
#define IEM_MC_SAR_LOCAL_S64(a_i64Local, a_cShift)      do { (a_i64Local) >>= (a_cShift);  } while (0)
#define IEM_MC_SHL_LOCAL_S16(a_i16Local, a_cShift)      do { (a_i16Local) <<= (a_cShift);  } while (0)
#define IEM_MC_SHL_LOCAL_S32(a_i32Local, a_cShift)      do { (a_i32Local) <<= (a_cShift);  } while (0)
#define IEM_MC_SHL_LOCAL_S64(a_i64Local, a_cShift)      do { (a_i64Local) <<= (a_cShift);  } while (0)
#define IEM_MC_SET_EFL_BIT(a_fBit)                      do { (pIemCpu)->CTX_SUFF(pCtx)->eflags.u |= (a_fBit); } while (0)
#define IEM_MC_CLEAR_EFL_BIT(a_fBit)                    do { (pIemCpu)->CTX_SUFF(pCtx)->eflags.u &= ~(a_fBit); } while (0)

#define IEM_MC_FETCH_MEM_U8(a_u8Dst, a_iSeg, a_GCPtrMem)                do {} while (0)
#define IEM_MC_FETCH_MEM_U16(a_u16Dst, a_iSeg, a_GCPtrMem)              do {} while (0)
#define IEM_MC_FETCH_MEM_U32(a_u32Dst, a_iSeg, a_GCPtrMem)              do {} while (0)
#define IEM_MC_FETCH_MEM_S32_SX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U64(a_u64Dst, a_iSeg, a_GCPtrMem)              do {} while (0)
#define IEM_MC_FETCH_MEM_U8_ZX_U16(a_u16Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U8_ZX_U32(a_u32Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U8_ZX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U16_ZX_U32(a_u32Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U16_ZX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U32_ZX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U8_SX_U16(a_u16Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U8_SX_U32(a_u32Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U8_SX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)        do {} while (0)
#define IEM_MC_FETCH_MEM_U16_SX_U32(a_u32Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U16_SX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_FETCH_MEM_U32_SX_U64(a_u64Dst, a_iSeg, a_GCPtrMem)       do {} while (0)
#define IEM_MC_STORE_MEM_U8(a_iSeg, a_GCPtrMem, a_u8Value)              do {} while (0)
#define IEM_MC_STORE_MEM_U16(a_iSeg, a_GCPtrMem, a_u16Value)            do {} while (0)
#define IEM_MC_STORE_MEM_U32(a_iSeg, a_GCPtrMem, a_u32Value)            do {} while (0)
#define IEM_MC_STORE_MEM_U64(a_iSeg, a_GCPtrMem, a_u64Value)            do {} while (0)

#define IEM_MC_PUSH_U16(a_u16Value)                                     do {} while (0)
#define IEM_MC_PUSH_U32(a_u32Value)                                     do {} while (0)
#define IEM_MC_PUSH_U64(a_u64Value)                                     do {} while (0)
#define IEM_MC_POP_U16(a_pu16Value)                                     do {} while (0)
#define IEM_MC_POP_U32(a_pu32Value)                                     do {} while (0)
#define IEM_MC_POP_U64(a_pu64Value)                                     do {} while (0)
#define IEM_MC_MEM_MAP(a_pMem, a_fAccess, a_iSeg, a_GCPtrMem, a_iArg)   do {} while (0)
#define IEM_MC_MEM_MAP_EX(a_pvMem, a_fAccess, a_cbMem, a_iSeg, a_GCPtrMem, a_iArg)  do {} while (0)
#define IEM_MC_MEM_COMMIT_AND_UNMAP(a_pvMem, a_fAccess)                 do {} while (0)
#define IEM_MC_CALC_RM_EFF_ADDR(a_GCPtrEff, bRm)                        do { (a_GCPtrEff) = 0; } while (0)
#define IEM_MC_CALL_VOID_AIMPL_2(a_pfn, a0, a1)                         do {} while (0)
#define IEM_MC_CALL_VOID_AIMPL_3(a_pfn, a0, a1, a2)                     do {} while (0)
#define IEM_MC_CALL_VOID_AIMPL_4(a_pfn, a0, a1, a2, a3)                 do {} while (0)
#define IEM_MC_CALL_AIMPL_4(a_rc, a_pfn, a0, a1, a2, a3)                do {} while (0)
#define IEM_MC_CALL_CIMPL_0(a_pfnCImpl)                                 return VINF_SUCCESS
#define IEM_MC_CALL_CIMPL_1(a_pfnCImpl, a0)                             return VINF_SUCCESS
#define IEM_MC_CALL_CIMPL_2(a_pfnCImpl, a0, a1)                         return VINF_SUCCESS
#define IEM_MC_CALL_CIMPL_3(a_pfnCImpl, a0, a1, a2)                     return VINF_SUCCESS
#define IEM_MC_CALL_CIMPL_5(a_pfnCImpl, a0, a1, a2, a3, a4)             return VINF_SUCCESS
#define IEM_MC_DEFER_TO_CIMPL_0(a_pfnCImpl)                             (VINF_SUCCESS)
#define IEM_MC_DEFER_TO_CIMPL_1(a_pfnCImpl, a0)                         (VINF_SUCCESS)
#define IEM_MC_DEFER_TO_CIMPL_2(a_pfnCImpl, a0, a1)                     (VINF_SUCCESS)
#define IEM_MC_DEFER_TO_CIMPL_3(a_pfnCImpl, a0, a1, a2)                 (VINF_SUCCESS)

#define IEM_MC_IF_EFL_BIT_SET(a_fBit)                                   if (g_fRandom) {
#define IEM_MC_IF_EFL_BIT_NOT_SET(a_fBit)                               if (g_fRandom) {
#define IEM_MC_IF_EFL_ANY_BITS_SET(a_fBits)                             if (g_fRandom) {
#define IEM_MC_IF_EFL_NO_BITS_SET(a_fBits)                              if (g_fRandom) {
#define IEM_MC_IF_EFL_BITS_NE(a_fBit1, a_fBit2)                         if (g_fRandom) {
#define IEM_MC_IF_EFL_BITS_EQ(a_fBit1, a_fBit2)                         if (g_fRandom) {
#define IEM_MC_IF_EFL_BIT_SET_OR_BITS_NE(a_fBit, a_fBit1, a_fBit2)      if (g_fRandom) {
#define IEM_MC_IF_EFL_BIT_NOT_SET_AND_BITS_EQ(a_fBit, a_fBit1, a_fBit2) if (g_fRandom) {
#define IEM_MC_IF_CX_IS_NZ()                                            if (g_fRandom) {
#define IEM_MC_IF_ECX_IS_NZ()                                           if (g_fRandom) {
#define IEM_MC_IF_RCX_IS_NZ()                                           if (g_fRandom) {
#define IEM_MC_IF_CX_IS_NZ_AND_EFL_BIT_SET(a_fBit)                      if (g_fRandom) {
#define IEM_MC_IF_ECX_IS_NZ_AND_EFL_BIT_SET(a_fBit)                     if (g_fRandom) {
#define IEM_MC_IF_RCX_IS_NZ_AND_EFL_BIT_SET(a_fBit)                     if (g_fRandom) {
#define IEM_MC_IF_CX_IS_NZ_AND_EFL_BIT_NOT_SET(a_fBit)                  if (g_fRandom) {
#define IEM_MC_IF_ECX_IS_NZ_AND_EFL_BIT_NOT_SET(a_fBit)                 if (g_fRandom) {
#define IEM_MC_IF_RCX_IS_NZ_AND_EFL_BIT_NOT_SET(a_fBit)                 if (g_fRandom) {
#define IEM_MC_IF_LOCAL_IS_Z(a_Local)                                   if ((a_Local) == 0) {
#define IEM_MC_IF_GREG_BIT_SET(a_iGReg, a_iBitNo)                       if (g_fRandom) {
#define IEM_MC_ELSE()                                                   } else {
#define IEM_MC_ENDIF()                                                  } do {} while (0)

/** @}  */

#include "../VMMAll/IEMAllInstructions.cpp.h"



/**
 * Formalities...
 */
int main()
{
    RTTEST hTest;
    RTEXITCODE rcExit = RTTestInitAndCreate("tstIEMCheckMc", &hTest);
    if (rcExit == RTEXITCODE_SUCCESS)
    {
        RTTestBanner(hTest);
        RTTestPrintf(hTest, RTTESTLVL_ALWAYS, "(this is only a compile test.)");
        rcExit = RTTestSummaryAndDestroy(hTest);
    }
    return rcExit;
}