# project target
FILE(GLOB SOURCE *.cc)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE} ${WEBRTCEXTRAOBJS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE src)

# webrtc
set(WEBRTCINCLUDE ${WEBRTCROOT}/src ${WEBRTCROOT}/src/third_party/abseil-cpp ${WEBRTCROOT}/src/third_party/jsoncpp/source/include ${WEBRTCROOT}/src/third_party/jsoncpp/generated ${WEBRTCROOT}/src/third_party/libyuv/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${WEBRTCINCLUDE})
target_link_libraries(${CMAKE_PROJECT_NAME} ${WEBRTCOBJS}/${CMAKE_STATIC_LIBRARY_PREFIX}webrtc${CMAKE_STATIC_LIBRARY_SUFFIX} ${WEBRTCOBJS}/rtc_base/${CMAKE_STATIC_LIBRARY_PREFIX}rtc_json${CMAKE_STATIC_LIBRARY_SUFFIX})
add_definitions(-DHAVE_JPEG)

# thread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads)

# rtmp ?
find_package(PkgConfig QUIET)
pkg_check_modules(RTMP QUIET librtmp)
MESSAGE("RTMP_FOUND = ${RTMP_FOUND}")
if (RTMP_FOUND)
    add_definitions(-DHAVE_RTMP)
    target_link_libraries(${CMAKE_PROJECT_NAME} ${RTMP_LIBRARIES})
endif ()

# webrtc
add_definitions(-DWEBRTC_POSIX -fno-rtti)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv6.*")
    add_definitions(-marm -march=armv6 -mfpu=vfp -mfloat-abi=hard)
endif ()
if (EXISTS ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE}/obj/modules/desktop_capture/desktop_capture.ninja)
    #    add_definitions(-DUSE_X11)
    #    target_link_libraries(${CMAKE_PROJECT_NAME} X11 Xext Xdamage Xfixes Xcomposite Xrandr Xtst)
endif ()
# civetweb
add_definitions(-DCRYPTO_LIB="libcrypto.so.1.1" -DSSL_LIB="libssl.so.1.1")
target_link_libraries(${CMAKE_PROJECT_NAME} dl)
#alsa
if (ALSA_FOUND)
    add_definitions(-DHAVE_SOUND)
endif ()

# static link of libatomic on armv6l
if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv6.*")
    target_link_libraries (${CMAKE_PROJECT_NAME} -Wl,-Bstatic -latomic -Wl,-Bdynamic)
endif()
# static link of stdc++ if available
if (NOT APPLE)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-static-libgcc" CXX_SUPPORTS_STATIC_LIBGCC)
    if (CXX_SUPPORTS_STATIC_LIBGCC)
        target_link_libraries (${CMAKE_PROJECT_NAME} -static-libgcc)
    endif()
    CHECK_CXX_COMPILER_FLAG("-static-libstdc++" CXX_SUPPORTS_STATIC_LIBSTDCPP)
    if (CXX_SUPPORTS_STATIC_LIBSTDCPP)
        target_link_libraries (${CMAKE_PROJECT_NAME} -static-libstdc++)
    endif()
endif()

#cpack
install (TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION .)
install (DIRECTORY html DESTINATION .)
install (FILES config.json DESTINATION .)
install (FILES Procfile DESTINATION .)
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
SET(CPACK_PACKAGE_CONTACT "michel.promonet@free.fr")
if(PROJECTVERSION)
    SET(CPACK_PACKAGE_VERSION "${PROJECTVERSION}")
endif()
INCLUDE(CPack)

# compiler specific
if (WIN32)
    # overide compilation flags
    set(CompilerFlags CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE)
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

    # getopt
    add_library (getopt getopt/getopt.c)
    target_include_directories(getopt PUBLIC getopt) 
    target_link_libraries (${CMAKE_PROJECT_NAME} getopt)

    # webrtc
    add_definitions(-DWEBRTC_WIN -EHsc -DNOMINMAX -D__PRETTY_FUNCTION__=__FUNCTION__ -D_HAS_ITERATOR_DEBUGGING=0)
    add_definitions(-DUSE_X11 -DHAVE_SOUND -D_WINSOCKAPI_)
    target_link_libraries (${CMAKE_PROJECT_NAME} secur32 dmoguids wmcodecdspuuid strmiids msdmo winmm dxgi d3d11 iphlpapi dwmapi)
elseif (APPLE)
    add_definitions(-DNEED_XLOCALE_H=1)
    # webrtc
    add_definitions(-DHAVE_SOUND)
    add_definitions(-DWEBRTC_MAC -DWEBRTC_POSIX -fno-rtti)
    # civetweb
    add_definitions(-DCRYPTO_LIB="libcrypto.so.1.1" -DSSL_LIB="libssl.so.1.1")

    find_library(CORE_FOUNDATION Foundation)
    find_library(APPLICATION_SERVICES ApplicationServices)
    find_library(CORE_SERVICES CoreServices)
    find_library(CORE_AUDIO CoreAudio)
    find_library(AUDIO_TOOLBOX AudioToolBox)
    target_link_libraries (${CMAKE_PROJECT_NAME} ${CORE_FOUNDATION} ${APPLICATION_SERVICES} ${CORE_SERVICES}
                           ${CORE_AUDIO} ${AUDIO_TOOLBOX})
else()
    # webrtc
    add_definitions(-DWEBRTC_POSIX -fno-rtti)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv6.*")
        add_definitions(-marm -march=armv6 -mfpu=vfp -mfloat-abi=hard)
    endif()
    if (EXISTS ${WEBRTCROOT}/src/out/${CMAKE_BUILD_TYPE}/obj/modules/desktop_capture/desktop_capture.ninja)
        add_definitions(-DUSE_X11)
        target_link_libraries (${CMAKE_PROJECT_NAME} X11 Xext Xdamage Xfixes Xcomposite Xrandr Xtst)
    endif()
    # civetweb
    add_definitions(-DCRYPTO_LIB="libcrypto.so.1.1" -DSSL_LIB="libssl.so.1.1")
    target_link_libraries (${CMAKE_PROJECT_NAME} dl)
    #alsa
    if (ALSA_FOUND)
        add_definitions(-DHAVE_SOUND)
    endif()
endif()