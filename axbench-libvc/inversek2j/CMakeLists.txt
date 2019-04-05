add_executable(inversek2j
  src/inversek2j.cpp)

set_property(TARGET inversek2j PROPERTY CXX_STANDARD 11)
set_property(TARGET inversek2j PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(inversek2j PRIVATE ${PROJECT_SOURCE_DIR}/libVersioningCompiler/include)
target_include_directories(inversek2j PRIVATE src ../common/src)
get_property(VC_LIB_DEPS GLOBAL PROPERTY VC_LIB_DEPS_P)
target_link_directories(inversek2j PRIVATE ${VC_LIB_DEPS})
target_link_libraries(inversek2j VersioningCompiler)

add_custom_command(TARGET inversek2j POST_BUILD
                   COMMAND cp $<TARGET_FILE:inversek2j> ${CMAKE_CURRENT_SOURCE_DIR}/bin)
