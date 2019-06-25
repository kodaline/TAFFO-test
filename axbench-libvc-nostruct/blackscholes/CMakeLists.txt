add_executable(blackscholes
  src/blackscholes.cpp)

set_property(TARGET blackscholes PROPERTY CXX_STANDARD 11)
set_property(TARGET blackscholes PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(blackscholes PRIVATE ${PROJECT_SOURCE_DIR}/libVersioningCompiler/include)
target_include_directories(blackscholes PRIVATE src ../common/src)
target_link_libraries(blackscholes VersioningCompiler)

add_custom_command(TARGET blackscholes POST_BUILD
COMMAND cp $<TARGET_FILE:blackscholes> ${CMAKE_CURRENT_SOURCE_DIR}/bin)
