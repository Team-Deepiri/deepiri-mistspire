cmake_minimum_required(VERSION 3.16)
project(mistspire_xr_sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mistspire_xr_sandbox main.cpp)

# Prefer pkg-config (Debian/Ubuntu libopenxr-dev); fall back to CMake package.
find_package(PkgConfig QUIET)
set(MISTSPIRE_XR_LINKED FALSE)

if(PKG_CONFIG_FOUND)
	pkg_check_modules(OPENXR QUIET openxr)
	if(OPENXR_FOUND)
		target_include_directories(mistspire_xr_sandbox PRIVATE ${OPENXR_INCLUDE_DIRS})
		target_link_libraries(mistspire_xr_sandbox PRIVATE ${OPENXR_LIBRARIES})
		set(MISTSPIRE_XR_LINKED TRUE)
	endif()
endif()

if(NOT MISTSPIRE_XR_LINKED)
	find_package(OpenXR REQUIRED)
	target_link_libraries(mistspire_xr_sandbox PRIVATE OpenXR::openxr_loader)
endif()

install(TARGETS mistspire_xr_sandbox RUNTIME DESTINATION bin)
