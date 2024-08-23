# Windows-specific CMake configuration

# Windows specific settings
set(WINDOWS_APP_NAME "MySDL2App")

# Copy assets to the build directory
copy_assets()

# Add executable
add_executable(${WINDOWS_APP_NAME} WIN32 main.cpp)

# Copy icon file
file(COPY ${CMAKE_SOURCE_DIR}/assets/windows DESTINATION ${CMAKE_BINARY_DIR})

# Set icon for the executable
set_target_properties(${WINDOWS_APP_NAME} PROPERTIES
    WIN32_EXECUTABLE true
    LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    RESOURCE ${CMAKE_SOURCE_DIR}/assets/windows/icons.ico
)

target_link_libraries(${WINDOWS_APP_NAME} PRIVATE SDL2 SDL2_image SDL2_mixer SDL2_net)
