#include <helpers/maapi_defs.h>
#include <jni.h>

long long jniIOCtl(JNIEnv* mJNIEnv, jobject mJThis, int function, int a, int b, int c MA_IOCTL_ELLIPSIS);
void testIOCtls(JNIEnv* mJNIEnv, jobject mJThis);

namespace jni {
	void Panic(int i, const char* msg);
	void* GetValidatedMemRange(int address, int size);
	void ValidateMemRange(const void* ptr, int size);
	int ValidatedStrLen(const char* ptr);
	int GetValidatedStackValue(int offset);
	const char* GetValidatedStr(int address);
	const wchar_t* GetValidatedWStr(int address);
	jstring wcharToJchar(JNIEnv* env, const wchar_t* src);
}

#undef ARG_NO_4
#undef ARG_NO_5
#undef ARG_NO_6

#ifdef MOSYNC_NATIVE
#define ARG_NO_4 __d
#define ARG_NO_5 __e
#define ARG_NO_6 __f
#define MA_IOCTL_ELLIPSIS , int __d = 0, int __e = 0, int __f = 0
#define MA_IOCTL_ARGS , __d, __e, __f
#else
#define ARG_NO_4 SYSCALL_THIS->GetValidatedStackValue(0)
#define ARG_NO_5 SYSCALL_THIS->GetValidatedStackValue(4)
#define ARG_NO_6 SYSCALL_THIS->GetValidatedStackValue(8)
#define MA_IOCTL_ARGS
#endif
