add_executable(kmeans
  src/kmeans.cpp)

set_property(TARGET kmeans PROPERTY CXX_STANDARD 11)
set_property(TARGET kmeans PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(kmeans PRIVATE ${PROJECT_SOURCE_DIR}/libVersioningCompiler/include)
target_include_directories(kmeans PRIVATE src ../common/src)
target_link_libraries(kmeans VersioningCompiler)

add_custom_command(TARGET kmeans POST_BUILD
                   COMMAND cp $<TARGET_FILE:kmeans> ${CMAKE_CURRENT_SOURCE_DIR}/bin)
