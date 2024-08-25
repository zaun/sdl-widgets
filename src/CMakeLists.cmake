include(src/CMakeLists.3rd-party.cmake)

# Add sources
set(SOURCES
  ${CMAKE_SOURCE_DIR}/src/audioplayer.cpp
  ${CMAKE_SOURCE_DIR}/src/button.cpp
  ${CMAKE_SOURCE_DIR}/src/container.cpp
  ${CMAKE_SOURCE_DIR}/src/flat.cpp
  ${CMAKE_SOURCE_DIR}/src/flatbutton.cpp
  ${CMAKE_SOURCE_DIR}/src/flatinput.cpp
  ${CMAKE_SOURCE_DIR}/src/flatlabel.cpp
  ${CMAKE_SOURCE_DIR}/src/flatpanel.cpp
  ${CMAKE_SOURCE_DIR}/src/flatoption.cpp
  ${CMAKE_SOURCE_DIR}/src/flatselect.cpp
  ${CMAKE_SOURCE_DIR}/src/flatslider.cpp
  ${CMAKE_SOURCE_DIR}/src/flattext.cpp
  ${CMAKE_SOURCE_DIR}/src/flatvideo.cpp
  ${CMAKE_SOURCE_DIR}/src/fontbook.cpp
  ${CMAKE_SOURCE_DIR}/src/optiongroup.cpp
  ${CMAKE_SOURCE_DIR}/src/panel.cpp
  ${CMAKE_SOURCE_DIR}/src/state.cpp
  ${CMAKE_SOURCE_DIR}/src/widget.cpp
  ${CMAKE_SOURCE_DIR}/src/window.cpp
  ${CMAKE_SOURCE_DIR}/src/main.cpp
)

# Add executable
add_executable(${APP_NAME} ${SOURCES})

target_include_directories(${APP_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/src/include
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_image_SOURCE_DIR}/include
  ${sdl3_net_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${libxmp_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${APP_NAME}
  nlohmann_json
  SDL3::SDL3-static
  SDL3_image::SDL3_image-static
  SDL3_net::SDL3_net-static
  SDL3_ttf::SDL3_ttf-static freetype
  vorbisenc vorbisfile vorbis xmp_static
)
