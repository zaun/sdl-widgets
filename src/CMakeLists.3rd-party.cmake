
if(APPLE)
enable_language(OBJC)
endif()

include(FetchContent)
include(ExternalProject)

# Set up Doxygen
# FetchContent_Declare(
#   doxygen
#   URL     https://github.com/doxygen/doxygen/archive/Release_1_9_6.zip
#   URL_HASH SHA256=1f3e3a6476147e1567008e377973b1c8474ff05d14200a1ffbdfca2231238d8a
# )
# FetchContent_MakeAvailable(doxygen)

# set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
# set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
# set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
# configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
# add_custom_target(doc ALL
#   COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
#   COMMAND ${CMAKE_COMMAND} -E chdir ${DOXYGEN_OUTPUT_DIR} ${CMAKE_BINARY_DIR}/_deps/doxygen-src/build/bin/doxygen ${DOXYGEN_OUT}
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMENT "Generating API documentation with Doxygen"
#   VERBATIM
# )
# add_dependencies(${APP_NAME} doc)

# Fetch FreeType
if(NOT ENABLE_EXTERNAL_STATIC)
  find_package(Freetype)
endif()

if(NOT Freetype_FOUND OR ENABLE_EXTERNAL_STATIC)
  FetchContent_Declare(
    Freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG VER-2-10-4  # Specify the version you need
  )
  FetchContent_MakeAvailable(freetype)
  set(FREETYPE_LIBRARIES ${Freetype_BINARY_DIR}/libfreetype.a)
  set(FREETYPE_INCLUDE_DIRS ${Freetype_SOURCE_DIR}/include)
endif()

# Declare SDL3 as a dependency
if(NOT ENABLE_EXTERNAL_STATIC)
  find_package(SDL3)
  find_package(SDL3_image)
  find_package(SDL3_ttf)
  find_package(SDL3_net)
endif()

if(NOT SDL3_FOUND OR ENABLE_EXTERNAL_STATIC)
  set(SDL_FRAMEWORK OFF)
  set(SDL_SHARED OFF)
  set(SDL_STATIC ON)
  set(SDL_DISABLE_UNINSTALL ON)
  set(BUILD_SHARED_LIBS OFF)
  set(ENABLE_CPACK OFF)
  set(ENABLE_PACKAGE_CONFIG OFF)
  set(BUILD_TESTING OFF)
  set(INSTALL_MANPAGES OFF)

  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
  )
  FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG main
  )
  FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG main
  )
  FetchContent_Declare(
    SDL3_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf SDL3_net)
  SET(SDL "STATIC")
endif()

#
# Audio
#

if(NOT ENABLE_EXTERNAL_STATIC)
  find_package(Ogg)
  find_package(Vorbis)
endif()

if(NOT Ogg_FOUND OR ENABLE_EXTERNAL_STATIC)
  FetchContent_Declare(
    libogg
    GIT_REPOSITORY https://github.com/xiph/ogg.git
    GIT_TAG v1.3.5
  )
  FetchContent_MakeAvailable(libogg)
  set(OGG_LIBRARY ${libogg_BINARY_DIR}/libogg.a)
  set(OGG_INCLUDE_DIR ${libogg_BINARY_DIR}/include)
else()
  set(OGG_LIBRARY ${Ogg_LIBRARIES})
  set(OGG_INCLUDE_DIR ${Ogg_INCLUDE_DIRS})
endif()

if(NOT Vorbis_FOUND OR ENABLE_EXTERNAL_STATIC)
  FetchContent_Declare(
    libvorbis
    GIT_REPOSITORY https://github.com/xiph/vorbis.git
    GIT_TAG v1.3.7
  )
  FetchContent_MakeAvailable(libvorbis)
  set(VORBIS_LIBRARY ${libvorbis_LIBRARIES})
endif()

#
# JSON
#

if(NOT ENABLE_EXTERNAL_STATIC)
  find_package(nlohmann_json)
endif()

if(NOT nlohmann_json_FOUND OR ENABLE_EXTERNAL_STATIC)
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()
