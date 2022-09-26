# Find WebRTC include path

set(WEBRTC_DIR "/LinuxData/Kickback/Webrtc_Source_Code/webrtc-checkout/src")

set(WEBRTC_INCLUDE_DIR
        ${WEBRTC_DIR}
        ${WEBRTC_DIR}/third_party/abseil-cpp
        ${WEBRTC_DIR}/third_party/jsoncpp/source/include
        ${WEBRTC_DIR}/third_party/jsoncpp/generated
        ${WEBRTC_DIR}/third_party/libyuv/include
        ${WEBRTC_DIR}/test/vcm_capturer.h
        ${WEBRTC_DIR}/modules
        ${WEBRTC_DIR}/modules/video_capture
        )

set(WEBRTC_OBJC_INCLUDE_DIR
        ${WEBRTC_DIR}/sdk/objc
        ${WEBRTC_DIR}/sdk/objc/base
        )

set(WEBRTC_LIBRARY_DIR
        ${WEBRTC_DIR}/out/Default/obj
        )

find_library(WEBRTC_LIBRARY_DEBUG
        NAMES webrtc
        PATHS ${WEBRTC_LIBRARY_DIR}
        NO_CMAKE_FIND_ROOT_PATH
        )

find_library(WEBRTC_LIBRARY_RELEASE
        NAMES webrtc
        PATHS ${WEBRTC_LIBRARY_DIR}
        NO_CMAKE_FIND_ROOT_PATH
        )

set(WEBRTC_LIBRARY
        debug ${WEBRTC_LIBRARY_DEBUG}
        optimized ${WEBRTC_LIBRARY_RELEASE}
        CACHE STRING "WebRTC library"
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WebRTC
        DEFAULT_MSG
#        WEBRTC_LIBRARY
          WEBRTC_LIBRARY_DEBUG
        #  WEBRTC_LIBRARY_RELEASE
        WEBRTC_INCLUDE_DIR
        )