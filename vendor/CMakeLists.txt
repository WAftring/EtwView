# CMakeList.txt : CMake project for EtwView, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Add source to this project's executable.
file(GLOB VENDOR_FILES "*.cpp")
add_library(ImGuiLib ${VENDOR_FILES})
target_include_directories(EtwView PRIVATE ${PROJECT_SOURCE_DIR}\\..\\include)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET EtwView PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
