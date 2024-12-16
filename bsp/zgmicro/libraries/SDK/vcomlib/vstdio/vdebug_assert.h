#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDEBUG_ASSERT_H_
#define _VDEBUG_ASSERT_H_
void vdbg_assert(const char *module_name,unsigned int line_num,const char *expr,unsigned int ret_addr);
#endif //#ifndef _VDEBUG_ASSERT_H_


