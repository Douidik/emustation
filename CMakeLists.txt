cmake_minimum_required(VERSION 3.18)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
	emustation
	DESCRIPTION "ps1 emulator backend"
	LANGUAGES C
)

project(
	emustation-frontend
	DESCRIPTION "ps1 emulator frontend"
	LANGUAGES C
)

add_subdirectory(emustation)
add_subdirectory(frontend)
