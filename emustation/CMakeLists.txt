file(
	GLOB_RECURSE EMUSTATION_SRC
	"[a-z_]" *.h
	"[a-z_]" *.c
)

add_library(
	emustation SHARED ${EMUSTATION_SRC}
)

target_include_directories(
	emustation PRIVATE
	${CMAKE_SOURCE_DIR}/emustation
	${CMAKE_SOURCE_DIR}/vendor
)

# target_link_libraries(
# 	emustation-frontend PUBLIC
# 	emustation
# )

set_target_properties(
	emustation PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	LINKER_LANGUAGE C
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
