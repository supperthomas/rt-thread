/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2009, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
	FPB

Description:
 	Flash patching relative function definitions.

Revision History:
 	Date                  Author                   Description
 	2009.10.21           chenjingen              initial version
------------------------------------------------------------------------------*/
#ifndef _VFPB_H_
#define _VFPB_H_
	
#ifdef __cplusplus 
extern "C" {
#endif

#define FPB_SLOTS_EXTENED   24 /* patch slots extended from 8 to 32. */

#define FPB_CTRL_WRITE_KEY 	((UINT32)0x1 << 1)  /* Key for FP_CTRL write access */

typedef enum tag_VFPB_COMP {
	FPB_INST_COMP0 = 0,
	FPB_INST_COMP1,
	FPB_INST_COMP2,
	FPB_INST_COMP3,
	FPB_INST_COMP4,
	FPB_INST_COMP5,
	FPB_LITERAL_COMP0,
	FPB_LITERAL_COMP1
#if (FPB_SLOTS_EXTENED >= 24)
	,
	FPB_INST_COMP6,
	FPB_INST_COMP7,
	FPB_INST_COMP8,
	FPB_INST_COMP9,
	FPB_INST_COMP10,
	FPB_INST_COMP11,
	FPB_INST_COMP12,
	FPB_INST_COMP13,
	FPB_INST_COMP14,
	FPB_INST_COMP15,
	FPB_INST_COMP16,
	FPB_INST_COMP17,
	FPB_INST_COMP18,
	FPB_INST_COMP19,
	FPB_INST_COMP20,
	FPB_INST_COMP21,
	FPB_INST_COMP22,
	FPB_INST_COMP23,
	FPB_INST_COMP24,
	FPB_INST_COMP25,
	FPB_INST_COMP26,
	FPB_INST_COMP27,
	FPB_INST_COMP28,
	FPB_INST_COMP29
#endif
}VFPB_COMP;

#define FPB_INST_COMP0 		0
#define FPB_INST_COMP1 		1
#define FPB_INST_COMP2 		2
#define FPB_INST_COMP3 		3
#define FPB_INST_COMP4 		4
#define FPB_INST_COMP5 		5
#define FPB_LITERAL_COMP0   6
#define FPB_LITERAL_COMP1   7
#if (FPB_SLOTS_EXTENED >= 24)
#define FPB_INST_COMP6 		8
#define FPB_INST_COMP7 		9
#define FPB_INST_COMP8 		10
#define FPB_INST_COMP9 		11
#define FPB_INST_COMP10		12
#define FPB_INST_COMP11		13
#define FPB_INST_COMP12 	14
#define FPB_INST_COMP13 	15
#define FPB_INST_COMP14		16
#define FPB_INST_COMP15		17
#define FPB_INST_COMP16		18
#define FPB_INST_COMP17		19
#define FPB_INST_COMP18 	20
#define FPB_INST_COMP19 	21
#define FPB_INST_COMP20		22
#define FPB_INST_COMP21		23
#define FPB_INST_COMP22		24
#define FPB_INST_COMP23		25
#define FPB_INST_COMP24		26
#define FPB_INST_COMP25		27
#define FPB_INST_COMP26		28
#define FPB_INST_COMP27		29
#define FPB_INST_COMP28		30
#define FPB_INST_COMP29		31
#endif

#if (FPB_SLOTS_EXTENED == 24)
#define IS_VALID_REMAP_ADDR(addr) (((UINT32)addr & (UINT32)0x7F) == 0)
#else
#define IS_VALID_REMAP_ADDR(addr) (((UINT32)addr & (UINT32)0x1F) == 0)
#endif

#define IS_VALID_COMP_ADDR(addr) ((((UINT32)addr & 0xC0000000) == 0) && (((UINT32)addr & (UINT32)0x3) == 0))

void VFPB_EnableFlashPatch(void);
void VFPB_DisableFlashPatch(void);
void VFPB_SetRemapBase(UINT32 BaseAddr);
void VFPB_EnableComparator(UINT32 CompAddr, VFPB_COMP CompX);
void VFPB_DisableComparator(VFPB_COMP CompX);

#ifdef RELEASE_BUILD
#if (FPB_SLOTS_EXTENED != 24)
#error "FPB_SLOTS_EXTENED must be set as 24 in vnvic_fpb.h for 9628 release building."
#endif
#endif

#ifdef __cplusplus
}
#endif 

#endif  /* _VFPB_H_ */

