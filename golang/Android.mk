LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libhaicam-p2p
LOCAL_SRC_FILES := build/$(TARGET_ARCH_ABI)/libhaicam-p2p.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libhaicam-ifaddrs
LOCAL_SRC_FILES := build/$(TARGET_ARCH_ABI)/libhaicam-ifaddrs.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libhaicam_ext
LOCAL_SRC_FILES := build/$(TARGET_ARCH_ABI)/libhaicam_ext.so
include $(PREBUILT_SHARED_LIBRARY)

