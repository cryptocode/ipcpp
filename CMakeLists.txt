cmake_minimum_required (VERSION 2.8.2)
project (nanocate C CXX)

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
# Nanocate
#---------------------------------#

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# -- nanocate configuration
add_definitions (-DNANOCATE_DEBUG=1)

# -- dlmalloc configuration
add_definitions (-DHAVE_MORECORE=1)
add_definitions (-DMORECORE=shm_morecore)
add_definitions (-DMORECORE_CONTIGUOUS=1)
add_definitions (-DMORECORE_CANNOT_TRIM=1)
add_definitions (-DHAVE_MMAP=0)
add_definitions (-DHAVE_MREMAP=0)
add_definitions (-DUSE_DL_PREFIX=1)
add_definitions (-DUSE_LOCKS=0)

if (UNIX)
  set(NANOCATE_PLATFORM src/posix/shmem.cpp)
else ()
  set(NANOCATE_PLATFORM src/win/shmem.cpp)
endif (UNIX)

include_directories("${PROJECT_BINARY_DIR}/include")
add_library(nanocate STATIC ${NANOCATE_PLATFORM} include/dlmalloc.h src/dlmalloc.c src/morecore.cpp include/shalloc.hpp src/shalloc.cpp)

add_executable(tests test.cpp)
target_link_libraries(tests nanocate gtest_main)

