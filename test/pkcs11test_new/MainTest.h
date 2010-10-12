/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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
#include "basetest.h"

/****************************************************************************
 * Test Functions prototypes
 ***************************************************************************/
testRet test_finalize_initialize(void);
testRet test_finalize_initialize_st(void); 
testRet test_initialize_ownmutex(void);
testRet test_initialize_preserved(void); 
testRet test_finalize_preserved(void); 
testRet test_getinfo(void);
testRet test_getslotlist(void);
testRet test_getslotlist_multiple_slots(void);
testRet test_open_close_session(void);
testRet test_open_close_session_info(void);
testRet test_open_close_session_bad_params(void); 
testRet test_open_close_session_limits(void); 
testRet test_waitforslotevent_noblock(void);
testRet test_waitforslotevent_userinteraction(void);
testRet test_waitforslotevent_whilefinalize(void);
testRet test_getmechanisms(void);
testRet test_getprivatekeys(void);
testRet test_sign(void);
testRet test_verify_signature(void);
testRet test_getallobjects(void);
