# Linux-specific CMake configuration

# Set installation directories
set(CMAKE_INSTALL_PREFIX /usr)

# Install executable
install(TARGETS ${APP_NAME} DESTINATION bin)
