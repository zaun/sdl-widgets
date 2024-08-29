# Add subdirectories for platform-specific builds
if(APPLE)
    include(platforms/macos/CMakeLists.cmake)
elseif(UNIX)
    include(platforms/Linux.cmake)
elseif(WIN32)
    include(platforms/Windows.cmake)
endif()
