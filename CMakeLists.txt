cmake_minimum_required(VERSION 3.25)
project(saucer-desktop LANGUAGES CXX VERSION 4.0.2)

# --------------------------------------------------------------------------------------------------------
# Setup Library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} STATIC)
add_library(saucer::desktop ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

if (PROJECT_IS_TOP_LEVEL AND NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -pedantic -pedantic-errors -Wfatal-errors)
endif()

# --------------------------------------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC  "include")
target_include_directories(${PROJECT_NAME} PRIVATE "private")

# --------------------------------------------------------------------------------------------------------
# Setup Dependencies
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

if (PROJECT_IS_TOP_LEVEL)
  CPMAddPackage(
    NAME           saucer
    GIT_TAG        master
    GIT_REPOSITORY "https://github.com/saucer/saucer"
  )
endif()

if (NOT TARGET saucer::saucer)
  message(FATAL_ERROR "Could not find saucer target!")
endif()

saucer_add_module(${PROJECT_NAME})

# --------------------------------------------------------------------------------------------------------
# Setup Sources
# --------------------------------------------------------------------------------------------------------

target_sources(${PROJECT_NAME} PRIVATE 
    "src/desktop.cpp"
)

# --------------------------------------------------------------------------------------------------------
# Setup Backends
# --------------------------------------------------------------------------------------------------------

if (saucer_backend MATCHES "^Qt.$")
  file(GLOB qt_sources
    "src/qt.*cpp"
  )

  target_sources(${PROJECT_NAME} PRIVATE ${qt_sources})
endif()

if (saucer_backend STREQUAL "WebKit")
  enable_language(OBJCXX)
  
  file(GLOB cocoa_sources
    "src/cocoa.*mm"
  )

  target_sources(${PROJECT_NAME} PRIVATE ${cocoa_sources})
endif()

if (saucer_backend STREQUAL "WebKitGtk")
  file(GLOB gtk_sources
    "src/gtk.*cpp"
  )

  target_sources(${PROJECT_NAME} PRIVATE ${gtk_sources})
endif()

if (saucer_backend STREQUAL "WebView2")
  file(GLOB win_sources
    "src/win32.*cpp"
  )

  target_sources(${PROJECT_NAME} PRIVATE ${win_sources})
endif()
