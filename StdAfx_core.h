
//
// StdAfx_core.h
//
// Top level include for Outlaws platform abstraction layer

#ifndef STDAFX_CORE_H
#define STDAFX_CORE_H

#include "PlatformIncludes.h"
#include "Outlaws.h"

#include <cstdio>
#include <cstdlib>

#include <memory>
#include <thread>
#include <mutex>

#include "Str.h"

using std::deque;
using std::string;
using std::pair;
using std::make_pair;
using std::unique_ptr;
using std::shared_ptr;
using namespace std::rel_ops;
using std::vector;
using std::swap;

inline void vReportMessagef(const char *format, va_list vl)  __printflike(1, 0);

inline void vReportMessagef(const char *format, va_list vl)
{
    string buf = str_vformat(format, vl);
    OL_ReportMessage(buf.c_str());
}

inline void ReportMessagef(const char *format, ...)  __printflike(1, 2);

inline void ReportMessagef(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    vReportMessagef(format, vl);
    va_end(vl);
}


#define TYPE_NAME(X) (typeid(X).name())

#define arraySize(X) (sizeof(X) / sizeof(X[0]))

#define SIZEOF_PTR(X) ((X) ? sizeof(*(X)) : 0)
#define SIZEOF_REC(X) ((X) ? (X)->getSizeof() : 0)
#define SIZEOF_IREC(X) ((X).getSizeof() - sizeof(X))
#define SIZEOF_VEC(X) (sizeof((X)[0]) * (X).capacity())
#define SIZEOF_PVEC(X) ((X) ? SIZEOF_VEC(*X) : 0)
#define SIZEOF_ARR(X, Y) ((X) ? (sizeof(X[0]) * (Y)) : 0)

// assertions are enabled even in release builds
#define ASSERT(X) (__builtin_expect(!(X), 0) ? OLG_OnAssertFailed(__FILE__, __LINE__, __func__, #X, ""), 1 : 0)
#define ASSERTF(X, Y, ...) (__builtin_expect(!(X), 0) ? OLG_OnAssertFailed(__FILE__, __LINE__, __func__, #X, (Y), __VA_ARGS__), 1 : 0)


#if !defined(DEBUG) && !defined(DEVELOP)
#  define DPRINTVAR(X) do{(void)sizeof(X);}while(0)
#  define WARN(X) do{(void) sizeof(X);}while(0)
#  define NDEBUG 1
#else
#  define WARN(X) ReportMessagef X
#  define DPRINTVAR(X) DebugPrintVar(#X, (X))
#endif

// special assertion only enabled in debug builds
#if !defined(DEBUG)
#  define DASSERT(X) do{(void) sizeof(X);}while(0)
#else
#  define DASSERT(X) ASSERT(X)
#endif


// base class for deferred deletion by main thread
struct IDeletable {

    virtual ~IDeletable() {}
}; 

#include "Geometry.h"
#include "Rand.h"
#include "stl_ext.h"
#include "SpacialHash.h"
#include "SerialCore.h"
#include "Event.h"

inline void DebugPrintVar(const char* str, int v) { ReportMessagef("%s = %d", str, v); }
inline void DebugPrintVar(const char* str, float v) { ReportMessagef("%s = %g", str, v); }
inline void DebugPrintVar(const char* str, float2 v) { ReportMessagef("%s = (%g, %g)", str, v.x, v.y); }
inline void DebugPrintVar(const char* str, float3 v) { ReportMessagef("%s = (%g, %g, %g)", str, v.x, v.y, v.z); }


#endif // STDAFX_CORE_H