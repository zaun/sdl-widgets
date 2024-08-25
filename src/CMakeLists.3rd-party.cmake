
if(APPLE)
enable_language(OBJC)
endif()

include(ExternalProject)

set(SDL_FRAMEWORK OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_DISABLE_UNINSTALL ON)
set(BUILD_SHARED_LIBS OFF)
set(ENABLE_CPACK OFF)
set(ENABLE_PACKAGE_CONFIG OFF)
set(BUILD_TESTING OFF)
set(INSTALL_MANPAGES OFF)

# Fetch FreeType
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-10-4  # You can specify the version you need
)
FetchContent_MakeAvailable(freetype)
SET(FREETYPE_LIBRARY ${freetype_BINARY_DIR}/libfreetype.a)
SET(FREETYPE_INCLUDE_DIRS ${freetype_SOURCE_DIR}/include)

# Declare SDL3 as a dependency
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG main
  # URL file:///Users/justinzaun/Development/3rdparty-repos/SDL 
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

#
# Audio
#

# Fetch libogg
FetchContent_Declare(
  libogg
  GIT_REPOSITORY https://github.com/xiph/ogg.git
  GIT_TAG v1.3.5
)
FetchContent_MakeAvailable(libogg)

# Fetch libvorbis
FetchContent_Declare(
  libvorbis
  GIT_REPOSITORY https://github.com/xiph/vorbis.git
  GIT_TAG v1.3.7
)
SET(OGG_LIBRARY ${libogg_BUILD_DIR}/libogg.a)
SET(OGG_INCLUDE_DIR ${libogg_BUILD_DIR}/include)
FetchContent_MakeAvailable(libvorbis)

# Fetch libogg
FetchContent_Declare(
  libxmp
  GIT_REPOSITORY https://github.com/libxmp/libxmp.git
  GIT_TAG master
)
FetchContent_MakeAvailable(libxmp)

#
# JSON
#

# Declare and fetch nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2 # Use the latest stable version or the version you need
)
FetchContent_MakeAvailable(nlohmann_json)
