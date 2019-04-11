add_executable(sobel
  src/sobel.cpp)

set_property(TARGET sobel PROPERTY CXX_STANDARD 11)
set_property(TARGET sobel PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(sobel PRIVATE ${PROJECT_SOURCE_DIR}/libVersioningCompiler/include)
target_include_directories(sobel PRIVATE src ../common/src)
target_link_libraries(sobel VersioningCompiler)

add_custom_command(TARGET sobel POST_BUILD
                   COMMAND cp $<TARGET_FILE:sobel> ${CMAKE_CURRENT_SOURCE_DIR}/bin)
