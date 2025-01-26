cmake_minimum_required(VERSION 3.29)
project(Bubbles CXX)
set(CMAKE_CXX_STANDARD 23)
include(FetchContent)
#

# https://github.com/ArthurSonzogni/FTXUI.git
#FetchContent_Declare(
#	ftxui	
#	GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
#	GIT_TAG v5.0.0
#	GIT_PROGRESS TRUE
#)
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG v1.15.0
	GIT_PROGRESS TRUE
)
message("Fetched Spdlog")
FetchContent_Declare(
	clipp
	GIT_REPOSITORY https://github.com/muellan/clipp.git
	GIT_TAG v1.2.3
	GIT_PROGRESS TRUE
)
message("Fetched Clipp")
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
message("Fetched Nlohmann JSON")
FetchContent_Declare(
	raylib
	GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
	GIT_TAG "master"
	GIT_PROGRESS TRUE
)
message("Fetched Raylib")
FetchContent_Declare(
	bullet3
	GIT_REPOSITORY "https://github.com/physics-playground/Bullet.git"
	GIT_TAG "main"
	GIT_PROGRESS TRUE
)
message("Fetched Bullet3 Physics Engine")
#
set(ASSIMP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(raylib)
message("Raylib Made Availible")
FetchContent_MakeAvailable(json)
message("Nlohmann JSON Made Availible")
set(BUILD_CPU_EXAMPLES OFF)
set(BUILD_OPENGL3_DEMOS OFF)
set(BUILD_CPU_DEMOS OFF)
set(BUILD_UNIT_TESTS OFF)
set(USE_GRAPHICAL_BENCHMARK OFF)
set(BUILD_BULLET2_DEMOS OFF)
FetchContent_MakeAvailable(bullet3)
message("Bullet3 Physics Engine Made Availible")

#FetchContent_MakeAvailable(ftxui)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(clipp)
#FetchContent_GetProperties(ftxui)
#if(NOT ftxui_POPULATED)
#  FetchContent_Populate(ftxui)
#  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
#endif()

add_compile_options(-fconcepts-diagnostics-depth=10)
file(GLOB LIBRARY_SOURCES source/*.cpp)
file(GLOB TEST_SOURCES test/*.cpp)
file(GLOB SCRATCHPAD_SOURCE scratchpad/*.cpp)
include_directories(include ${json_SOURCE_DIR}/include ${raylib3d_SOURCE_DIR} ${bullet3_SOURCE_DIR}/src ${ftxui_SOURCE_DIR}/include ${clipp_SOURCE_DIR}/include ${spdlog_SOURCE_DIR}/include)
message("Bullet3 Physics Engine Include Directory Made Availible")
#add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
#target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL af afopencl)
add_executable(scratchpad ${SCRATCHPAD_SOURCE})
add_compile_options("-g")
target_link_libraries(scratchpad PRIVATE 
#	ftxui::component 
#	ftxui::dom 
#	ftxui::screen
	spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
	raylib 
	BulletDynamics 
	BulletCollision 
	LinearMath
)

# add_executable(test ${TESTS})
# target_link_libraries(test PRIVATE raylib xtensor xtensor::optimize xtensor::use_xsimd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

