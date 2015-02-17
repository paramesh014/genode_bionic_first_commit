#include <stdint.h>
#include <stdio.h>

/*code copied from android/external/clang/lib/headers/unwind.h */
typedef uintptr_t _Unwind_Word;
struct _Unwind_Context;
typedef enum {
  _URC_NO_REASON = 0,
  _URC_FOREIGN_EXCEPTION_CAUGHT = 1,

  _URC_FATAL_PHASE2_ERROR = 2,
  _URC_FATAL_PHASE1_ERROR = 3,
  _URC_NORMAL_STOP = 4,

  _URC_END_OF_STACK = 5,
  _URC_HANDLER_FOUND = 6,
  _URC_INSTALL_CONTEXT = 7,
  _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn)(struct _Unwind_Context *,
                                                void *);

extern __inline__
_Unwind_Word _Unwind_GetIP(struct _Unwind_Context *__context, int __index);

_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn abd, void * abc);
void* dlmalloc(size_t bytes);
void dlfree(void* mem);
