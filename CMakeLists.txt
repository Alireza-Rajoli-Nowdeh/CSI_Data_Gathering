# Fetch the Git version info
execute_process(
    COMMAND git describe --always --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_PROJECT_VER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set the source files and include directories for the component
set(SRC_FILES "main.cc")
set(REQUIRES "esp_wifi")
set(INCLUDE_DIRS "." "../../_components")

# Register the component with source files and include directories
set(COMPONENT_SRCS ${SRC_FILES})
set(COMPONENT_ADD_INCLUDEDIRS ${INCLUDE_DIRS})

# Register the component (required for ESP-IDF v3.3.1)
register_component()

# Add Git version as a preprocessor macro
add_definitions(-DGIT_PROJECT_VER="${GIT_PROJECT_VER}")
