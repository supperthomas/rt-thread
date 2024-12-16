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
		2019-11-22            ming.m                   add size calc
------------------------------------------------------------------------------*/

#include <stdarg.h>

#include "vstdlib.h"

#define protobuf_memcpy(dst, src, sz) VCOMMON_MemCpy(dst,src, sz)


/// pack value items to bytes sequence
///
/// @cnt    number of value groups,
/// @...    item size list
///               int size;
///               void *val;
///
/// @return pack size = sum(value[n].size)
///          <0 for error
int protobuf_calc_sz(int cnt, ...) {
    int value_size;
    int buff_size = 0;
    va_list var;

    va_start(var, cnt);

    while(cnt--) {

        // get the value
        value_size = va_arg(var, int);
        buff_size += value_size;
    }
    va_end(var);
    
    return (int)buff_size;
}

/// pack value items to bytes sequence
///
/// @buff   target buffer
/// @sz     size of buff
/// @cnt    number of value group,
/// @...    value groups, each group of value is like
///               int size, void *val;
///
/// @return  pack size = sum(value[n].size)
///          <0 for error
int protobuf_pack(UINT8 *buff, int sz, int cnt, ...){
    int build_sz = 0;
    int  value_size;
    void *value;
    va_list var;

    va_start(var, cnt);

    while(cnt--) {
        // check if buffer full
        if(sz < build_sz) goto err_quit;

        // get the value
        value_size = va_arg(var, int);
        value  =  va_arg(var, void *);

        // check if buffer will overflow
        if(sz < (build_sz + value_size)) goto err_quit;

        // VCOMMON_MemCpy()
        protobuf_memcpy(buff+build_sz, value, value_size);
        build_sz += value_size;
    }
    va_end(var);
    return (int)build_sz;

err_quit:
    va_end(var);
    return -1;
}

/// unpack value items from bytes sequence
///
/// @buff   source buffer
/// @sz     size of buff
/// @cnt    number of value group,
/// @...    value groups, each group of value is like
///               int size, void *val;
///
/// @return  pack size = sum(value[n].size)
///          <0 for error
int protobuf_unpack(UINT8 *buff, int sz, int cnt, ...) {
    int build_sz = 0;
    int value_size;
    void *value;
    va_list var;

    va_start(var, cnt);
    while(cnt--) {
        // check if buffer full
        if(sz <= build_sz) goto err_quit;

        // get the value
        value_size = va_arg(var, int);
        value  =  va_arg(var, void *);

        // check if not enough data in buffer
        if(sz < (build_sz + value_size)) goto err_quit;

        // VCOMMON_MemCpy()
        protobuf_memcpy(value, buff+build_sz, value_size);
        build_sz += value_size;
    }
    va_end(var);
    return (int)build_sz;

err_quit:
    va_end(var);
    return -1;
}
