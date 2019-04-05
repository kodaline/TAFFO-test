add_executable(inversek2j
  src/inversek2j.cpp
  src/kinematics.cpp
  src/kinematics.hpp)
find_package(Boost REQUIRED)
set_property(TARGET TAFFOCompiler PROPERTY CXX_STANDARD 11)
set_property(TARGET TAFFOCompiler PROPERTY CXX_STANDARD_REQUIRED ON)
target_include_directories(inversek2j PRIVATE ${PROJECT_SOURCE_DIR}/LibVCSupport/LibVC/include)
target_include_directories(inversek2j PRIVATE ${PROJECT_SOURCE_DIR}/LibVCSupport/TAFFOCompiler)
target_include_directories(inversek2j PRIVATE src ../common/src ${Boost_INCLUDE_DIRS})
target_link_libraries(inversek2j ${VC_LIB_NAME} TAFFOCompiler)

add_custom_command(TARGET inversek2j POST_BUILD
                   COMMAND cp $<TARGET_FILE:inversek2j> ${CMAKE_CURRENT_SOURCE_DIR}/bin)
