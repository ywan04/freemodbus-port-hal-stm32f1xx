/*
 * port.c
 *
 *  Created on: Jan 17, 2024
 *      Author: ivan
 */

#include "port.h"

static uint32_t lock_nesting_count = 0;

void critical_enter(void)
{
    __disable_irq();
    ++lock_nesting_count;
}

void critical_exit(void)
{
    /* Unlock interrupts only when we are exiting the outermost nested call. */
    --lock_nesting_count;
    if (lock_nesting_count == 0) {
        __enable_irq();
    }
}
