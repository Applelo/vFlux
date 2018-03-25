cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	if(DEFINED ENV{VITASDK})
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
	else()
		message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
	endif()
endif()

set(SHORT_NAME vflux)
set(PSVITAIP "192.168.1.21")
project(vFlux)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -nostartfiles")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_executable(${SHORT_NAME}.elf
				main.c
				blit.c
				font.c
				)

target_link_libraries(${SHORT_NAME}.elf
				taihen_stub
				SceLibKernel_stub
				SceDisplay_stub
				SceCtrl_stub
				scePower_stub
				k
				gcc
				)

set_target_properties(${SHORT_NAME}.elf
				PROPERTIES LINK_FLAGS "-nostdlib"
				)

vita_create_self(${PROJECT_NAME}.suprx ${SHORT_NAME}.elf
				CONFIG ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.yml
				)

add_custom_target(send
				COMMAND curl -T ${PROJECT_NAME}.suprx ftp://${PSVITAIP}:1337/ux0:/tai/
				DEPENDS ${PROJECT_NAME}.suprx
				)