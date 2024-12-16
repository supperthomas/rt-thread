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
    vprotobuf

Description:
    serializing structured data

Revision History:
    Date                  Author                   Description
    2018-08-16            ming.m                   create module
------------------------------------------------------------------------------*/
#ifndef __VPROTOBUF_H__
#define __VPROTOBUF_H__

/// serializing structured data
#include <stdarg.h>

#include "vstdlib.h"


/// pack value items to bytes sequence
///
/// @cnt    number of value groups,
/// @...    item size list
///               int size;
///               void *val;
///
/// @return pack size = sum(value[n].size)
///          <0 for error
int protobuf_calc_sz(int cnt, ...);


/// pack value items to bytes sequence
///
/// @buff   target buffer
/// @sz     size of buff
/// @cnt    number of value group,
/// @...    value groups, each group of value is like
///               int size;
///               void *val;
///
/// @return pack size = sum(value[n].size)
///          <0 for error
int protobuf_pack(UINT8 *buff, int sz, int cnt, ...);

/// unpack value items from bytes sequence
///
/// @buff   source buffer
/// @sz     size of buff
/// @cnt    number of value group,
/// @...    value groups, each group of value is like
///               int size;
///               void *val;
///
/// @return pack size = sum(value[n].size)
///          <0 for error
int protobuf_unpack(UINT8 *buff, int sz, int cnt, ...);

#endif //#ifndef __VPROTOBUF_H__
