# macOS-specific CMake configuration

# macOS specific settings
set(MACOSX_BUNDLE_NAME ${APP_NAME})
set(MACOSX_BUNDLE_IDENTIFIER "com.mycompany.myapp")
set(MACOSX_BUNDLE_VERSION "1.0.0")

find_library(COREFOUNDATION_LIBRARY CoreFoundation)
find_library(CORESERVICES_LIBRARY CoreServices)

target_link_libraries(${APP_NAME}
  ${COREFOUNDATION_LIBRARY}
  ${CORESERVICES_LIBRARY}
)

target_sources(${APP_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/platforms/macos/src/platform.mm)

# Create Info.plist
configure_file(${CMAKE_SOURCE_DIR}/platforms/macos/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist)

# Specify the MACOSX_BUNDLE property
set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE TRUE)

set(ENTITLEMENTS ${CMAKE_SOURCE_DIR}/platforms/macos/app.entitlements)

# Include Info.plist and icons
set_target_properties(${APP_NAME} PROPERTIES
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
  RESOURCE ${CMAKE_SOURCE_DIR}/platforms/macos/AppIcon.icns
)

get_target_property(SDL3_LIBS SDL3::SDL3 IMPORTED_LOCATION_NOCONFIG)

# Copy assets to the app bundle
add_custom_command(TARGET ${APP_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources"

  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/resources" "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/platforms/macos/AppIcon.icns" "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources"
)
