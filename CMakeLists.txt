cmake_minimum_required(VERSION 3.0.0)
project(WebRTCServer)

set(WEBRTCROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE STRING "WEBRTC root directory")
set(WEBRTCDESKTOPCAPTURE "ON" CACHE STRING "WEBRTC Desktop capture")

message(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()
MESSAGE("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}\nWEBRTCROOT = ${WEBRTCROOT}\nWEBRTCDESKTOPCAPTURE= ${WEBRTCDESKTOPCAPTURE}\nCMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# build version identifier
find_package(Git)

add_definitions(-DVERSION=\"${VERSION}\")

# set CMAKE_FIND_ROOT_PATH to search package in WebRTC sysroot
if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv.*")
    set(CMAKE_FIND_ROOT_PATH ${WEBRTCROOT}/src/build/linux/debian_bullseye_arm-sysroot)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    set(CMAKE_FIND_ROOT_PATH ${WEBRTCROOT}/src/build/linux/debian_bullseye_arm64-sysroot)
endif ()
MESSAGE("CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}")

# alsa ?
find_package(ALSA QUIET)
MESSAGE("ALSA_FOUND = ${ALSA_FOUND}")

# pulse ?
find_package(PulseAudio QUIET)
MESSAGE("PulseAudio_FOUND = ${PulseAudio_FOUND}")

# webrtc build
if (NOT EXISTS ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE}/args.gn)
    set(WEBRTCARGS rtc_include_tests=true\nrtc_enable_protobuf=true\nrtc_build_examples=true\nrtc_build_tools=false\ntreat_warnings_as_errors=false\nrtc_enable_libevent=false\nrtc_build_libevent=false\n)
    set(WEBRTCARGS use_custom_libcxx=false\n${WEBRTCARGS})
    # debug/release
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(WEBRTCARGS is_debug=false\n${WEBRTCARGS})
    else ()
        set(WEBRTCARGS is_debug=true\n${WEBRTCARGS})
    endif ()

    # enable H264 support
    set(WEBRTCARGS is_chrome_branded=true\n${WEBRTCARGS})
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv6.*")
        set(ENV{PATH} "${WEBRTCROOT}/src/third_party/llvm-build/Release+Asserts/bin:$ENV{PATH}")
        file(READ ${WEBRTCROOT}/src/third_party/ffmpeg/chromium/scripts/build_ffmpeg.py filecontent)
        string(REPLACE "--enable-armv6t2" "--disable-armv6t2" filecontent "${filecontent}")
        string(REPLACE "--enable-thumb" "--disable-thumb" filecontent "${filecontent}")
        string(REPLACE "--extra-cflags=-march=armv7-a" "--extra-cflags=-march=armv6" filecontent "${filecontent}")
        string(REPLACE "debian_sid_arm-sysroot" "debian_bullseye_arm-sysroot" filecontent "${filecontent}")
        file(WRITE ${WEBRTCROOT}/src/third_party/ffmpeg/chromium/scripts/build_ffmpeg.py "${filecontent}")
        EXECUTE_PROCESS(WORKING_DIRECTORY ${WEBRTCROOT}/src/third_party/ffmpeg COMMAND python3 chromium/scripts/build_ffmpeg.py linux arm --config-only)
        EXECUTE_PROCESS(WORKING_DIRECTORY ${WEBRTCROOT}/src/third_party/ffmpeg COMMAND chromium/scripts/copy_config.sh)
    endif ()

    # sound support
    if (APPLE)
        set(WEBRTCARGS rtc_include_internal_audio_device=true\n${WEBRTCARGS})
        set(WEBRTCARGS rtc_include_pulse_audio=true\n${WEBRTCARGS})
    elseif (NOT WIN32)
        if (NOT PulseAudio_FOUND)
            set(WEBRTCARGS rtc_include_pulse_audio=false\n${WEBRTCARGS})
        endif ()
        if (NOT ALSA_FOUND)
            set(WEBRTCARGS rtc_include_internal_audio_device=false\n${WEBRTCARGS})
        endif ()
    endif ()

    # compilation mode depending on target
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv6.*")
        set(WEBRTCARGS target_cpu="arm"\narm_version=6\narm_float_abi="hard"\nenable_libaom=false\n${WEBRTCARGS})
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "armv.*")
        set(WEBRTCARGS target_cpu="arm"\n${WEBRTCARGS})
    elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(WEBRTCARGS target_cpu="arm64"\n${WEBRTCARGS})
    endif ()
    if (DEFINED CMAKE_FIND_ROOT_PATH)
        set(WEBRTCARGS use_sysroot=true\n${WEBRTCARGS})
    else ()
        set(WEBRTCARGS use_sysroot=false\n${WEBRTCARGS})
    endif ()
    set(WEBRTCARGS is_clang=true\n${WEBRTCARGS})

    # screen capture support
    find_package(PkgConfig QUIET)
    pkg_check_modules(GTK3 QUIET gtk+-3.0)
    MESSAGE("GTK3_FOUND = ${GTK3_FOUND}")
    if (NOT GTK3_FOUND OR (WEBRTCDESKTOPCAPTURE STREQUAL "OFF"))
        set(WEBRTCARGS rtc_use_x11=false\nrtc_use_pipewire=false\n${WEBRTCARGS})
    endif ()

    # write conf file
    FILE(WRITE ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE}/args.gn ${WEBRTCARGS})
endif ()

set(WEBRTCOBJS ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE}/obj)

if (NOT EXISTS ${WEBRTCOBJS}/${CMAKE_STATIC_LIBRARY_PREFIX}webrtc${CMAKE_STATIC_LIBRARY_SUFFIX})
    if (WIN32)
        SET(SHELLCOMMAND cmd /c)
    endif ()
    EXECUTE_PROCESS(WORKING_DIRECTORY ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE} COMMAND ${SHELLCOMMAND} gn gen .)

    SET(NINJA_TARGET webrtc rtc_json jsoncpp builtin_video_decoder_factory builtin_video_encoder_factory peerconnection p2p_server_utils task_queue default_task_queue_factory)
    EXECUTE_PROCESS(WORKING_DIRECTORY ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE} COMMAND ninja ${NINJA_TARGET})
endif ()

FILE(GLOB_RECURSE WEBRTJSONCPPCOBJS ${WEBRTCOBJS}/third_party/jsoncpp/jsoncpp/*${CMAKE_C_OUTPUT_EXTENSION})
FILE(GLOB_RECURSE WEBRTP2POBJ ${WEBRTCOBJS}/p2p/p2p_server_utils/*${CMAKE_C_OUTPUT_EXTENSION})
SET(WEBRTCEXTRAOBJS ${WEBRTJSONCPPCOBJS} ${WEBRTP2POBJ} ${WEBRTTASK})

add_subdirectory(src)