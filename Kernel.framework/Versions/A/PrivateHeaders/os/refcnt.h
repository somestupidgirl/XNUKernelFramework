/*
 * Copyright (c) 2017 Apple Inc. All rights reserved.
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

#ifndef _OS_REFCNT_H_
#define _OS_REFCNT_H_

/*
 * os_refcnt reference counting API
 *
 * Two flavors are provided: atomic and locked. Atomic internally uses C11 atomic
 * operations and requires no external synchronization, whereas the locked flavor
 * assumes the refcnt object is locked by the caller. It is NOT safe to
 * mix-and-match locked and atomic calls.
 *
 * 'refgrp's are a way to (hierarchically) group like refcount objects for
 * debugging purposes. The group keeps track of the total number and aggregate
 * reference count of member refcounts, and the "rlog=" boot-arg is used to enable
 * refcount logging by group name. Named groups can be created explicitly with
 * os_refgrp_decl(), or implicitly by passing NULL for the refgrp when
 * initializing a refcnt object. In the latter case, the group name is the same as
 * the function enclosing the init call. Groups are only available on DEV or DEBUG
 * builds, and are otherwise compiled out.
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <os/base.h>

struct os_refcnt;
struct os_refgrp;
typedef struct os_refcnt os_refcnt_t;

/* type of the internal counter */
typedef uint32_t os_ref_count_t;
typedef _Atomic(os_ref_count_t) os_ref_atomic_t;

/*
 * OS_REF_INITIALIZER
 * OS_REF_ATOMIC_INITIALIZER
 *
 * Static initializers that create refcnt objects with safe initial values for use
 * between declaration and initialization (os_ref*_init()). Equivalent to zeroing.
 */


#ifndef OS_REFCNT_DEBUG
# if DEVELOPMENT || DEBUG
#  define OS_REFCNT_DEBUG 1
# else
#  define OS_REFCNT_DEBUG 0
# endif
#endif

#if __has_attribute(diagnose_if)
# define os_error_if(cond, msg) __attribute__((diagnose_if((cond), (msg), "error")))
#else
# define os_error_if(...)
#endif

__BEGIN_DECLS

/*
 * os_ref_init: initialize an os_refcnt with a count of 1
 * os_ref_init_count: initialize an os_refcnt with a specific count >= 1
 */
#define os_ref_init(rc, grp) os_ref_init_count((rc), (grp), 1)
static void os_ref_init_count(struct os_refcnt *, struct os_refgrp *, os_ref_count_t count)
os_error_if(count == 0, "Reference count must be non-zero initialized");

/*
 * os_refgrp_decl(qual, var, name, parent): declare a refgroup object 'var' with
 *   given name string and parent group.
 */

/*
 *
 * os_ref_retain: acquire a reference (increment reference count by 1) atomically.
 *
 * os_ref_release: release a reference (decrement reference count) atomically and
 *		return the new count. Memory is synchronized such that the dealloc block
 *		(i.e. code handling the final release() == 0 call) sees up-to-date memory
 *		with respect to all prior release()s on the same refcnt object. This
 *		memory ordering is sufficient for most use cases.
 *
 * os_ref_release_relaxed: same as release() but with weaker relaxed memory ordering.
 *		This can be used when the dealloc block is already synchronized with other
 *		accesses to the object (for example, with a lock).
 *
 * os_ref_release_live: release a reference that is guaranteed not to be the last one.
 */
static void os_ref_retain(struct os_refcnt *);
static os_ref_count_t os_ref_release(struct os_refcnt *) OS_WARN_RESULT;
static os_ref_count_t os_ref_release_relaxed(struct os_refcnt *) OS_WARN_RESULT;
static void os_ref_release_live(struct os_refcnt *);

/*
 * os_ref_retain_try: a variant of atomic retain that fails for objects with a
 *		zero reference count. The caller must therefore ensure that the object
 *		remains alive for any possible retain_try() caller, usually by using a
 *		lock protecting both the retain and dealloc paths. This variant is useful
 *		for objects stored in a collection, because no lock is required on the
 *		release() side until the object is deallocated.
 */
static bool os_ref_retain_try(struct os_refcnt *) OS_WARN_RESULT;

/*
 * os_ref_retain_locked: acquire a reference on an object protected by a held
 *		lock. The caller must ensure mutual exclusivity of retain_locked() and
 *		release_locked() calls on the same object.
 *
 * os_ref_release_locked: release a reference on an object protected by a held
 *		lock.
 */
static void os_ref_retain_locked(struct os_refcnt *);
static os_ref_count_t os_ref_release_locked(struct os_refcnt *) OS_WARN_RESULT;

/*
 * os_ref_get_count: return the current reference count. This is unsafe for
 *		synchronization.
 */
static os_ref_count_t os_ref_get_count(struct os_refcnt *rc);



__END_DECLS

#include <os/refcnt_internal.h>
#endif
