# logger library

find_package(Qt5 COMPONENTS Core REQUIRED
	PATHS ${QT_DIR}
	NO_DEFAULT_PATH
)

aux_source_directory(. LOGGER_SRC)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
	logger
	${LOGGER_SRC}
)

qt5_use_modules(logger
	Core
)
