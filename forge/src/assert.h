#pragma once

#include <cstdio>

namespace forge {

#ifndef NDEBUG
	#define FG_ASSERT(expr, msg) { if (!(expr)) { printf(msg); __debugbreak(); } }
#else
	#define FG_ASSERT(expr, msg) 
#endif

}