#include "def.h"

/*this definition is copied (and commented) from android/external/strace*/
_Unwind_Word _Unwind_GetIP(struct _Unwind_Context *__context, int __index) {
 // _Unwind_Word __ip = _Unwind_GetGR(__context, 15);
 // return __ip & ~(_Unwind_Word)(0x1); /* Remove thumb mode bit. */
 printf("im in user defined unwind ");
 return 0;
}

_Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn abd, void * abc){
	return 0;
}

/* ---------------------------- free --------------------------- */
/*
void dlfree(void* mem) {
  
}
void* dlmalloc(size_t bytes) {
  return NULL;
}*/
