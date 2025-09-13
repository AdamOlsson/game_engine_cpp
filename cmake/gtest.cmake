cmake_policy(SET CMP0135 NEW) 
include(FetchContent)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

FetchContent_MakeAvailable(googletest)

# Explicitly set the standard library for Google Test targets
# if(TARGET gtest)
#     target_compile_options(gtest PRIVATE -stdlib=libc++)
#     target_link_options(gtest PRIVATE -stdlib=libc++)
# endif()
#
# if(TARGET gtest_main)
#     target_compile_options(gtest_main PRIVATE -stdlib=libc++)
#     target_link_options(gtest_main PRIVATE -stdlib=libc++)
# endif()
#
# if(TARGET gmock)
#     target_compile_options(gmock PRIVATE -stdlib=libc++)
#     target_link_options(gmock PRIVATE -stdlib=libc++)
# endif()
#
# if(TARGET gmock_main)
#     target_compile_options(gmock_main PRIVATE -stdlib=libc++)
#     target_link_options(gmock_main PRIVATE -stdlib=libc++)
# endif()
