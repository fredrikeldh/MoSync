#include "jniIOCtl.h"
#include <generated/JNI_defs.h>
#include <generated/JNI_IX_OPENGL_ES_MA.h>
#include <generated/JNI_IX_WIDGET.h>
#include <generated/JNI_IX_PIM.h>
#include <generated/JNI_IX_CELLID.h>
#include <android/log.h>

#ifdef LOGGING_ENABLED
#define IMPL_maWriteLog(m) m(maWriteLog)
#else
#define IMPL_maWriteLog(m)
#endif

#define ANDROID_IMPLEMENTED_IOCTLS(m)\
	IMPL_maWriteLog(m)\
	m(maSendTextSMS)\
	m(maBtStartDeviceDiscovery)\
	m(maBtGetNewDevice)\
	m(maBtCancelDiscovery)\
	m(maAccept)\
	m(maOpenGLInitFullscreen)\
	m(maOpenGLCloseFullscreen)\
	m(maOpenGLTexImage2D)\
	m(maOpenGLTexSubImage2D)\
	m(maAudioDataCreateFromResource)\
	m(maAudioDataCreateFromURL)\
	m(maAudioDataDestroy)\
	m(maAudioInstanceCreate)\
	m(maAudioInstanceCreateDynamic)\
	m(maAudioSubmitBuffer)\
	m(maAudioGetPendingBufferCount)\
	m(maAudioInstanceDestroy)\
	m(maAudioGetLength)\
	m(maAudioSetNumberOfLoops)\
	m(maAudioPrepare)\
	m(maAudioPlay)\
	m(maAudioSetPosition)\
	m(maAudioGetPosition)\
	m(maAudioSetVolume)\
	m(maAudioPause)\
	m(maAudioStop)\
	m(maLocationStart)\
	m(maLocationStop)\
	m(maGetSystemProperty)\
	m(maPlatformRequest)\
	m(maShowVirtualKeyboard)\
	m(maTextBox)\
	m(maWidgetCreate)\
	m(maWidgetDestroy)\
	m(maWidgetAddChild)\
	m(maWidgetInsertChild)\
	m(maWidgetRemoveChild)\
	m(maWidgetModalDialogShow)\
	m(maWidgetModalDialogHide)\
	m(maWidgetSetProperty)\
	m(maWidgetGetProperty)\
	m(maWidgetScreenAddOptionsMenuItem)\
	m(maWidgetScreenShow)\
	m(maWidgetScreenShowWithTransition)\
	m(maWidgetStackScreenPush)\
	m(maWidgetStackScreenPop)\
	m(maNotificationAdd)\
	m(maNotificationRemove)\
	m(maSendToBackground)\
	m(maScreenSetOrientation)\
	m(maScreenSetFullscreen)\
	m(maScreenSetSupportedOrientations)\
	m(maScreenGetSupportedOrientations)\
	m(maScreenGetCurrentOrientation)\
	m(maHomeScreenEventsOn)\
	m(maHomeScreenEventsOff)\
	m(maHomeScreenShortcutAdd)\
	m(maHomeScreenShortcutRemove)\
	m(maScreenStateEventsOn)\
	m(maScreenStateEventsOff)\
	m(maWakeLock)\
	m(maMessageBox)\
	m(maAlert)\
	m(maToast)\
	m(maImagePickerOpen)\
	m(maImagePickerOpenWithEventReturnType)\
	m(maOptionsBox)\
	m(maSaveImageToDeviceGallery)\
	m(maFileOpen)\
	m(maFileExists)\
	m(maFileClose)\
	m(maFileCreate)\
	m(maFileDelete)\
	m(maFileSize)\
	m(maFileAvailableSpace)\
	m(maFileTotalSpace)\
	m(maFileDate)\
	m(maFileRename)\
	m(maFileTruncate)\
	m(maFileWrite)\
	m(maFileWriteFromData)\
	m(maFileRead)\
	m(maFileReadToData)\
	m(maFileTell)\
	m(maFileSeek)\
	m(maFileListStart)\
	m(maFileListNext)\
	m(maFileListClose)\
	m(maFontLoadDefault)\
	m(maFontSetCurrent)\
	m(maFontGetCount)\
	m(maFontGetName)\
	m(maFontLoadWithName)\
	m(maFontDelete)\
	m(maCameraStart)\
	m(maCameraStop)\
	m(maCameraSnapshot)\
	m(maCameraSnapshotAsync)\
	m(maCameraSetPreview)\
	m(maCameraNumber)\
	m(maCameraSelect)\
	/*m(maCameraRecord)*/\
	m(maCameraFormatNumber)\
	m(maCameraFormat)\
	m(maCameraSetProperty)\
	m(maCameraGetProperty)\
	m(maCameraPreviewSize)\
	m(maCameraPreviewEventEnable)\
	m(maCameraPreviewEventDisable)\
	m(maCameraPreviewEventConsumed)\
	m(maSensorStart)\
	m(maSensorStop)\
	m(maPimListOpen)\
	m(maPimListNext)\
	m(maPimListNextSummary)\
	m(maPimListClose)\
	m(maPimItemCount)\
	m(maPimItemGetField)\
	m(maPimItemFieldCount)\
	m(maPimItemGetAttributes)\
	m(maPimItemSetLabel)\
	m(maPimItemGetLabel)\
	m(maPimFieldType)\
	m(maPimItemGetValue)\
	m(maPimItemSetValue)\
	m(maPimItemAddValue)\
	m(maPimItemRemoveValue)\
	m(maPimItemClose)\
	m(maPimItemCreate)\
	m(maPimItemRemove)\
	m(maExtensionModuleLoad)\
	m(maExtensionFunctionLoad)\
	m(maExtensionFunctionInvoke2)\
	m(maNFCStart)\
	m(maNFCStop)\
	m(maNFCReadTag)\
	m(maNFCDestroyTag)\
	m(maNFCConnectTag)\
	m(maNFCCloseTag)\
	m(maNFCIsType)\
	m(maNFCGetTypedTag)\
	m(maNFCBatchStart)\
	m(maNFCBatchCommit)\
	m(maNFCBatchRollback)\
	m(maNFCTransceive)\
	m(maNFCGetSize)\
	m(maNFCGetNDEFMessage)\
	m(maNFCReadNDEFMessage)\
	m(maNFCWriteNDEFMessage)\
	m(maNFCCreateNDEFMessage)\
	m(maNFCGetNDEFRecord)\
	m(maNFCGetNDEFRecordCount)\
	m(maNFCGetNDEFId)\
	m(maNFCGetNDEFPayload)\
	m(maNFCGetNDEFTnf)\
	m(maNFCGetNDEFType)\
	m(maNFCSetNDEFId)\
	m(maNFCSetNDEFPayload)\
	m(maNFCSetNDEFTnf)\
	m(maNFCSetNDEFType)\
	m(maNFCAuthenticateMifareSector)\
	m(maNFCGetMifareSectorCount)\
	m(maNFCGetMifareBlockCountInSector)\
	m(maNFCMifareSectorToBlock)\
	m(maNFCReadMifareBlocks)\
	m(maNFCReadMifarePages)\
	m(maNFCWriteMifareBlocks)\
	m(maNFCWriteMifarePages)\
	m(maNFCSetReadOnly)\
	m(maNFCIsReadOnly)\
	m(maAdsBannerCreate)\
	m(maAdsAddBannerToLayout)\
	m(maAdsRemoveBannerFromLayout)\
	m(maAdsBannerDestroy)\
	m(maAdsBannerSetProperty)\
	m(maAdsBannerGetProperty)\
	m(maNotificationLocalCreate)\
	m(maNotificationLocalDestroy)\
	m(maNotificationLocalSetProperty)\
	m(maNotificationLocalGetProperty)\
	m(maNotificationLocalSchedule)\
	m(maNotificationLocalUnschedule)\
	m(maNotificationPushRegister)\
	m(maNotificationPushGetRegistration)\
	m(maNotificationPushUnregister)\
	m(maNotificationPushGetData)\
	m(maNotificationPushDestroy)\
	m(maNotificationPushSetTickerText)\
	m(maNotificationPushSetMessageTitle)\
	m(maNotificationPushSetDisplayFlag)\
	m(maCaptureSetProperty)\
	m(maCaptureGetProperty)\
	m(maCaptureAction)\
	m(maCaptureWriteImage)\
	m(maCaptureGetImagePath)\
	m(maCaptureGetVideoPath)\
	m(maCaptureDestroyData)\
	m(maPurchaseSupported)\
	m(maPurchaseCreate)\
	m(maPurchaseSetPublicKey)\
	m(maPurchaseRequest)\
	m(maPurchaseGetName)\
	m(maPurchaseGetField)\
	m(maPurchaseVerifyReceipt)\
	m(maPurchaseRestoreTransactions)\
	m(maPurchaseDestroy)\
	m(maSyscallPanicsEnable)\
	m(maSyscallPanicsDisable)\
	m(maGetCellInfo)\
	m(maDBOpen)\
	m(maDBClose)\
	m(maDBExecSQL)\
	m(maDBExecSQLParams)\
	m(maDBCursorDestroy)\
	m(maDBCursorNext)\
	m(maDBCursorGetColumnData)\
	m(maDBCursorGetColumnText)\
	m(maDBCursorGetColumnInt)\
	m(maDBCursorGetColumnDouble)\

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "MoSyncJniIOCtl", __VA_ARGS__)

static bool testSingleIOCtl(JNIEnv* mJNIEnv, jclass cls, const char* name, const char* types) {
	jmethodID methodID = mJNIEnv->GetMethodID(cls, name, types);
	if(methodID == 0) {
		LOG("IOCtl(%s), %s is missing!\n", name, types);
		return true;
	}
	return false;
}

void testIOCtls(JNIEnv* mJNIEnv, jobject mJThis) {
	bool error = false;
	jclass cls = mJNIEnv->GetObjectClass(mJThis);
#define ANDROID_IOCTL_TEST(name) if(testSingleIOCtl(mJNIEnv, cls, #name, maIOCtl_##name##_types)) error = true;
	ANDROID_IMPLEMENTED_IOCTLS(ANDROID_IOCTL_TEST);
	mJNIEnv->DeleteLocalRef(cls);
	if(error) {
		jni::Panic(1, "one or more IOCtls missing!");
	}
	LOG("All IOCtls present and accounted for.\n");
}


static int JVMR(int a, int size) {
	jni::GetValidatedMemRange(a, size);
	return a;
}

#define GVMR(p, type) JVMR(p, sizeof(type))
#define GVS(p) jni::GetValidatedStr(p)
#define GVWS(p) jni::GetValidatedWStr(p)
#define GVMRA(type) GVMR(a, type)
#define VSV_ARGPTR_USE

long long jniIOCtl(JNIEnv* mJNIEnv, jobject mJThis, int function, int a, int b, int c MA_IOCTL_ELLIPSIS) {
	switch(function) {
#define ANDROID_IOCTL_CASE(name) maIOCtl_##name##_case(name)
		ANDROID_IMPLEMENTED_IOCTLS(ANDROID_IOCTL_CASE);
	default:
		return IOCTL_UNAVAILABLE;
	}
}
