cmake_minimum_required(VERSION 3.7)
project(filament-glfw-share-context C CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -stdlib=libc++)

if (NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
endif()

set(FILAMENT_INSTALL_PATH "" CACHE PATH "Path to a Filament installation directory")

if (NOT FILAMENT_INSTALL_PATH)
  message(FATAL_ERROR "Must specify -DFILAMENT_INSTALL_PATH=<path>")
endif()

find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_custom_command(
  OUTPUT 
      "${PROJECT_BINARY_DIR}/bakedTexture.c"
  COMMAND ${FILAMENT_INSTALL_PATH}/bin/matc -a opengl -a metal -o ${PROJECT_BINARY_DIR}/bakedTexture.matc ${CMAKE_CURRENT_SOURCE_DIR}/bakedTexture.mat
  COMMAND xxd -i "bakedTexture.matc" > bakedTexture.c
  COMMENT "Creating ${PROJECT_BINARY_DIR}/bakedTexture.c"
  DEPENDS 
      bakedTexture.mat
      ${FILAMENT_INSTALL_PATH}/bin/matc
  VERBATIM
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

include_directories(${FILAMENT_INSTALL_PATH}/include)

set(_gl_srcs 
  glfw-shared-gl-context.cpp
  ${PROJECT_BINARY_DIR}/bakedTexture.c
)

if (APPLE)
  list(APPEND _gl_srcs mac_helpers.mm)
endif()

set(_filament_libs
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libbackend.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libbluegl.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libbluevk.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilabridge.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilaflat.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilamat.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilamat_lite.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilament.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libfilameshio.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libgeometry.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libgltfio.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libgltfio_core.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libibl.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libimage.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libmatdbg.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libmeshoptimizer.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libshaders.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libsmol-v.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libutils.a
  ${FILAMENT_INSTALL_PATH}/lib/x86_64/libvkshaders.a
)

add_executable(glfw-shared-gl-context
  ${_gl_srcs}
)
target_link_libraries(glfw-shared-gl-context 
  ${_filament_libs}
  
  OpenGL::GL 
  glfw
)

if (APPLE)
  target_link_libraries(glfw-shared-gl-context
    "-framework Cocoa"
    "-framework CoreVideo"
    "-framework Metal"
    "-framework QuartzCore"
    "-framework OpenGL"
  )
else()
  target_link_libraries(glfw-shared-gl-context
    X11
    dl
    Xxf86vm
    Xrandr
    pthread
    dl
    Xi
    Xinerama
    Xcursor
  )
endif()

if (APPLE)
  add_executable(glfw-shared-metal
    glfw-shared-metal.cpp
    mac_helpers.mm
    metal_texture.cpp
    ${PROJECT_BINARY_DIR}/bakedTexture.c
  )

  target_link_libraries(glfw-shared-metal
    ${_filament_libs}
  
    glfw
    "-framework Cocoa"
    "-framework CoreVideo"
    "-framework Metal"
    "-framework QuartzCore"
  )

  target_include_directories(glfw-shared-metal PRIVATE metal-cpp)

endif()