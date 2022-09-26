add_executable(SpaceServer)

file(GLOB SRC
        "*.h"
        "*.cc"
        )

target_sources(SpaceServer
        PRIVATE
        ${SRC}
#        main.cpp
        )


target_compile_definitions(SpaceServer
        PUBLIC
        WEBRTC_LINUX
        WEBRTC_POSIX
        )

target_link_libraries(SpaceServer
        PUBLIC
        ${WEBRTC_LIBRARY_DEBUG}
#        /LinuxData/Kickback/Webrtc_Source_Code/webrtc-checkout/src/out/Default/obj/examples/libSpacePlugin.a
#        /LinuxData/Kickback/Webrtc_Source_Code/webrtc-checkout/src/out/Default/obj/modules/video_capture/libvideo_capture_module.a
        Threads::Threads
        ${CMAKE_DL_LIBS}
        )

target_include_directories(SpaceServer
        SYSTEM
        PUBLIC
        ${WEBRTC_INCLUDE_DIR}
        )

target_include_directories(SpaceServer
        PUBLIC
        .
        )