
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2014 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifndef __util_h__
#define __util_h__

#include <stdlib.h>
#include <string.h>
#include "pkcs11log.h"

//#define DUMP_TEMPLATE_MAX     32
#define BEIDP11_NOT_INITIALIZED			0
#define BEIDP11_INITIALIZED				1
#define BEIDP11_DEINITIALIZING			2
#define BEIDP11_INITIALIZING			3

#ifdef __cplusplus
extern "C"
{
#endif

	CK_RV p11_init_lock(CK_C_INITIALIZE_ARGS_PTR args);
    void p11_lock(void);
    void p11_unlock(void);
    void p11_free_lock(void);
	void util_init_lock(void **lock);
	void util_clean_lock(void **lock);
	void util_lock(void *lock);
	void util_unlock(void *lock);
	void strcpy_n(unsigned char *to, const char *from, size_t n,
		      char padding);
	void p11_set_init(unsigned char initialized);
	unsigned char p11_get_init(void);

#ifdef __cplusplus
}
#endif

#endif
