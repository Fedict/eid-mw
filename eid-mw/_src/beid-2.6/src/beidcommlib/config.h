#ifndef EIDLIBCOMM_CONFIG_H
#define EIDLIBCOMM_CONFIG_H

typedef unsigned char byte;		// put in global namespace to avoid ambiguity with other byte typedefs

#define NAMESPACE_BEGIN(x) namespace x {
#define NAMESPACE_END }

#ifdef _MSC_VER
	// 4231: nonstandard extension used : 'extern' before template explicit instantiation
	// 4250: dominance
	// 4251: member needs to have dll-interface
	// 4275: base needs to have dll-interface
	// 4660: explicitly instantiating a class that's already implicitly instantiated
	// 4661: no suitable definition provided for explicit template instantiation request
	// 4786: identifer was truncated in debug information
	// 4355: 'this' : used in base member initializer list
#pragma warning(disable: 4231 4250 4251 4275 4660 4661 4786 4355)
#endif

#include <string>
#include <vector>
#include <map>
#include <deque>
#include <sstream>

#include <qmutex.h>
#include <qcstring.h> 
#include <qdatastream.h> 
#include <quuid.h>
#include <qthread.h>
#include <qsocketdevice.h>

#define BEID_STOP_THREAD_TIME   5000
#define BEID_MAX_MESSAGE_SIZE 64000

#endif // EIDLIBCOMM_CONFIG_H
