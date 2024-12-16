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
    COMMON

Description:
    Common data type & function definitions.

Revision History:
    Date                  Author                   Description
    2009.04.08           chenjingen              initial version
------------------------------------------------------------------------------*/
#include "VersionNo.h"

__attribute__((used)) const VERSION_STR ver_string =
{
    VERSION_TYPE,
    VERSION_HASH,
    VERSION_FILESTR,
    VERSION_TIME
};

