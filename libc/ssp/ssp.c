/*
 * A Stack Protector placeholder from OSDev.org
 *
 * For more detail about stack protector, please see
 * https://wiki.osdev.org/Stack_Smashing_Protector.
 */

#include <stdint.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void){
  // do nothing here, just a placeholder for stack protector.
}
