file(
	GLOB_RECURSE FRONTEND_SRC
	"[a-z_]" *.h
	"[a-z_]" *.c
)

add_executable(
	emustation-frontend ${FRONTEND_SRC}
)

target_include_directories(
	emustation-frontend PRIVATE
	${CMAKE_SOURCE_DIR}/emustation
	${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(
	emustation-frontend PUBLIC
	emustation
)

set_target_properties(
	emustation-frontend PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	LINKER_LANGUAGE C
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
