HOST_NAME ?= master.beattogether.systems
PORT ?= 2328
STATUS_URL ?= "http://master.beattogether.systems/status"
LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI := $(APP_ABI)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Creating prebuilt for dependency: beatsaber-hook
include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_$(BSHOOK)
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook
LOCAL_SRC_FILES := extern/libbeatsaber-hook_$(BSHOOK).so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)

# Creating prebuilt for dependency: modloader
include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_EXPORT_C_INCLUDES := extern/modloader
LOCAL_SRC_FILES := extern/libmodloader.so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)

# Creating prebuilt for dependency: codegen
include $(CLEAR_VARS)
LOCAL_MODULE := codegen
LOCAL_EXPORT_C_INCLUDES := extern/codegen
LOCAL_SRC_FILES := extern/libcodegen.so
LOCAL_CPP_FEATURES += exceptions
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := "$(LIBNAME)"
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.c)
LOCAL_CPPFLAGS += -std=c++2a
LOCAL_LDLIBS += -llog
LOCAL_SHARED_LIBRARIES += beatsaber-hook_$(BSHOOK)
LOCAL_SHARED_LIBRARIES += modloader
LOCAL_SHARED_LIBRARIES += codegen
LOCAL_CFLAGS += -DID='"$(MODID)"'
LOCAL_CFLAGS += -DVERSION='"$(VERSION)"'
LOCAL_CFLAGS += -isystem'./extern/codegen/include'
LOCAL_CFLAGS += -I'./shared'
LOCAL_CFLAGS += -I'./extern'
LOCAL_CFLAGS += -I'extern/libil2cpp/il2cpp/libil2cpp'
LOCAL_CFLAGS += -DHOST_NAME='"$(HOST_NAME)"'
LOCAL_CFLAGS += -DPORT='$(PORT)'
LOCAL_CFLAGS += -DSTATUS_URL='$(STATUS_URL)'
LOCAL_C_INCLUDES += ./include ./src
include $(BUILD_SHARED_LIBRARY)
