# Tests needing a ROOT instance
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/root/cmake)
find_package(ROOT)

if (ROOT_FOUND)
  include_directories(${ROOT_INCLUDE_DIR})
  link_directories(${ROOT_LIBRARY_DIR})
  
  if(${ROOT_VERSION} LESS 6.0)
    set(GCC_COMPILE_FLAGS "-Wno-shadow -fPIC")
  else()
    set(GCC_COMPILE_FLAGS "-Wno-shadow -fPIC -std=c++11")
  endif()
  add_definitions(${GCC_COMPILE_FLAGS})
endif()

# File reader
add_library(reader_lib OBJECT ${PROJECT_SOURCE_DIR}/src/FileReader.cpp)
  
#add_executable(reader_oneedge ${PROJECT_SOURCE_DIR}/test/reader_oneedge.cpp $<TARGET_OBJECTS:reader_lib>)
#set_property(TARGET reader_oneedge PROPERTY EXCLUDE_FROM_ALL true)

if (ROOT_FOUND)

  add_executable(reader ${PROJECT_SOURCE_DIR}/test/reader.cpp $<TARGET_OBJECTS:reader_lib>)
  target_link_libraries(reader ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
  set_property(TARGET reader PROPERTY EXCLUDE_FROM_ALL true)

endif()
