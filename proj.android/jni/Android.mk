LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                    ../../Classes/AppDelegate.cpp \
                    ../../Classes/HelloWorldScene.cpp \
                    ../../Classes/GameScene.cpp \
                    ../../Classes/NesLayer.cpp \
                    ../../Classes/PadControlLayer.cpp \
                    ../../Classes/NesGameMenuLayer.cpp \
                    ../../Classes/JoyStick.cpp \
                    ../../Classes/nes/APU.cpp \
                    ../../Classes/nes/blip_buff.cpp \
                    ../../Classes/nes/BlipBuffer.cpp \
                    ../../Classes/nes/Board.cpp \
                    ../../Classes/nes/CPU.cpp \
                    ../../Classes/nes/DMA.cpp \
                    ../../Classes/nes/DMC.cpp \
                    ../../Classes/nes/Input.cpp \
                    ../../Classes/nes/Interrupt.cpp \
                    ../../Classes/nes/Mapper000.cpp \
                    ../../Classes/nes/Mapper001.cpp \
                    ../../Classes/nes/Memory.cpp \
                    ../../Classes/nes/NesCartDatabase.cpp \
                    ../../Classes/nes/NesEmulator.cpp \
                    ../../Classes/nes/Noise.cpp \
                    ../../Classes/nes/NTSCPaletteGenerator.cpp \
                    ../../Classes/nes/PALBPaletteGenerator.cpp \
                    ../../Classes/nes/PPU.cpp \
                    ../../Classes/nes/Pulse1.cpp \
                    ../../Classes/nes/Pulse2.cpp \
                    ../../Classes/nes/Rom.cpp \
                    ../../Classes/nes/sha1.cpp \
                    ../../Classes/nes/Triangle.cpp \
                    ../../Classes/nes/util.cpp \
                    ../../Classes/nes/VSUnisystem.cpp



LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
