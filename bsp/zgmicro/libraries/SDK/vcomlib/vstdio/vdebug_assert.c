#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdio.h"
#include "vstdcore.h"
#include "vdebug_assert.h"

#if CM_BACKTRACE_ENABLE
#include "cm_backtrace.h"
#endif

#if (VDBG_EN > 0)

static volatile int assert_halt_flag;

void vdbg_assert(const char *module_name,unsigned int line_num,const char *expr,unsigned int ret_addr)
{
  assert_halt_flag = 1;

  DEBUG_CPRINTF("Assertion %s(%d): '%s' failed! Call from: 0x%08X\r\n", \
			module_name, line_num, expr, ret_addr);
  vdirect_printf("Assertion %s(%d): '%s' failed! Call from: 0x%08X\r\n", \
			module_name, line_num, expr, ret_addr);
	/*Call Stack Backtrace function will be added soon here..*/

#if CM_BACKTRACE_ENABLE
    cm_backtrace_assert(cmb_get_psp());
#endif

  __SOFT_BREAKPOINT();
  while(assert_halt_flag);
}

#endif
