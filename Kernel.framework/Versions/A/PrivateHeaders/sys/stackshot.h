/*
 * Copyright (c) 2014 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */

#ifndef _SYS_STACKSHOT_H
#define _SYS_STACKSHOT_H

#include <stdint.h>

#define STACKSHOT_CONFIG_TYPE 1

typedef struct stackshot_config {
	/* Input options */
	int             sc_pid;                 /* PID to trace, or -1 for the entire system */
	uint64_t        sc_flags;               /* Stackshot flags */
	uint64_t        sc_delta_timestamp;     /* Retrieve a delta stackshot of system state that has changed since this time */

	/* Stackshot results */
	uint64_t        sc_buffer;              /* Pointer to stackshot buffer */
	uint32_t        sc_size;                /* Length of the stackshot buffer */

	/* Internals */
	uint64_t        sc_out_buffer_addr;     /* Location where the kernel should copy the address of the newly mapped buffer in user space */
	uint64_t        sc_out_size_addr;       /* Location where the kernel should copy the size of the stackshot buffer */
	uint64_t                sc_pagetable_mask;      /* Mask of page table levels to dump, must pass STACKSHOT_PAGE_TABLES */
} stackshot_config_t;


#endif /* _SYS_STACKSHOT_H */
