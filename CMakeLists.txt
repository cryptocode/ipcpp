cmake_minimum_required (VERSION 2.8.2)
project (ipcpp C CXX)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

#---------------------------------#
# Google Test
#---------------------------------#
configure_file(CMakeLists.gtest.txt googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

if(result)
	message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

#---------------------------------#
# ipcpp
#---------------------------------#

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# -- ipcpp configuration
add_definitions (-DIPCPP_DEBUG=1)

# -- ipcmalloc configuration
add_definitions (-DHAVE_MORECORE=1)
add_definitions (-DMORECORE=shm_morecore)
add_definitions (-DMORECORE_CONTIGUOUS=1)
add_definitions (-DMORECORE_CANNOT_TRIM=1)
add_definitions (-DHAVE_MMAP=0)
add_definitions (-DHAVE_MREMAP=0)
add_definitions (-DUSE_DL_PREFIX=1)
add_definitions (-DUSE_LOCKS=0)

# Platform bits
if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
  add_definitions (-DIPCPP_IS_64BIT=0)
  message (STATUS "32 bit system")
else ()
  add_definitions (-DIPCPP_IS_64BIT=1)
  message (STATUS "64 bit system")
endif ()

# Platform files
if (UNIX)
  set(IPCPP_PLATFORM src/posix/platform.cpp)
else ()
  set(IPCPP_PLATFORM src/win/platform.cpp)
endif (UNIX)

# Posix linking
if (UNIX AND NOT APPLE)
  set (LIB_RT rt)
endif ()

include_directories("${PROJECT_BINARY_DIR}/include")
add_library(ipcpp STATIC ${IPCPP_PLATFORM} include/posix.hpp include/ipcmalloc.h src/ipcmalloc.c src/morecore.cpp include/murmurhash.hpp include/shalloc.hpp)

add_executable(test-suite test/test.cpp)
target_link_libraries(test-suite ipcpp gtest_main ${LIB_RT})

add_executable(test-producer test/producer.cpp)
target_link_libraries(test-producer ipcpp ${LIB_RT})

add_executable(test-consumer test/consumer.cpp)
target_link_libraries(test-consumer ipcpp ${LIB_RT})