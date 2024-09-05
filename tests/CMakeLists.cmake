include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0 # Use the desired version
)
FetchContent_MakeAvailable(catch2)

add_executable(${APP_NAME}-test ${LIBRARY_SOURCES}
  tests/container.cpp
)

target_include_directories(${APP_NAME}-test PRIVATE
  ${CMAKE_SOURCE_DIR}/src/include
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_image_SOURCE_DIR}/include
  ${sdl3_net_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${libxmp_SOURCE_DIR}/include
  ${catch2_SOURCE_DIR}/src
)

if(SDL STREQUAL "STATIC")
  target_link_libraries(${APP_NAME}-test
    nlohmann_json
    SDL3::SDL3-static
    SDL3_image::SDL3_image-static
    SDL3_net::SDL3_net-static
    SDL3_ttf::SDL3_ttf-static ${FREETYPE_LIBRARIES}
    vorbisenc vorbisfile vorbis
    Catch2::Catch2WithMain
  )
else()
  target_link_libraries(${APP_NAME}-test
    nlohmann_json
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_net::SDL3_net
    SDL3_ttf::SDL3_ttf ${FREETYPE_LIBRARIES}
    vorbisenc vorbisfile vorbis
    Catch2::Catch2WithMain
  )
endif()

enable_testing()

# Add the test
add_test(NAME Tests COMMAND ${APP_NAME}-test)
