add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glad)
add_subdirectory(stb)

file(GLOB IMGUI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp")
file(GLOB IMGUI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h")
set(IMGUI_BACKEND_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h"
                          "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.h")

add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_HEADERS} ${IMGUI_BACKEND_HEADERS})
target_link_libraries(imgui PUBLIC glfw)

if(GLDL_BUILD_REDIST)
  message(STATUS "Building in Redist Mode")
  set(COPY_DIR "${CMAKE_SOURCE_DIR}/out/headers")

  set(VENDOR_HEADER_DIRS "")
  foreach(target glm glad stb)
    get_target_property(VENDOR_HEADER_DIR ${target} INTERFACE_INCLUDE_DIRECTORIES)
    if(VENDOR_HEADER_DIR)
      list(APPEND VENDOR_HEADER_DIRS ${VENDOR_HEADER_DIR})
    endif()
  endforeach()
  list(REMOVE_DUPLICATES VENDOR_HEADER_DIRS)

  add_custom_target(
    copy_headers ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${COPY_DIR}"
    COMMENT "Copying header files to ${COPY_DIR}")

  foreach(DIR IN LISTS VENDOR_HEADER_DIRS)
    add_custom_command(
      TARGET copy_headers
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${DIR} "${COPY_DIR}"
      COMMENT "Copying headers from ${DIR} to ${COPY_DIR}")
  endforeach()

  file(MAKE_DIRECTORY ${COPY_DIR}/imgui/backends)
  file(COPY ${IMGUI_HEADERS} DESTINATION "${COPY_DIR}/imgui")
  file(COPY ${IMGUI_BACKEND_HEADERS} DESTINATION "${COPY_DIR}/imgui/backends")
  target_include_directories(imgui PUBLIC ${COPY_DIR}/imgui)
else()
  target_include_directories(imgui PUBLIC imgui)
endif()
