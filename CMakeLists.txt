cmake_minimum_required(VERSION 3.20)
project(BurmaAutomaton VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets Network)
find_package(gz-rendering8 QUIET)
find_package(gz-sim8 QUIET)
find_package(gz-common5 QUIET)
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 QUIET)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/PromptPanel.cpp
    src/ui/RenderWidget.cpp
    src/ui/PropertyEditor.cpp
    src/ui/EventLog.cpp
    src/ui/AssetBrowser.cpp
    src/ui/ExportPanel.cpp
    src/core/Application.cpp
    src/modules/BitNetClient.cpp
    src/modules/FuelFetcher.cpp
    src/modules/SDFBuilder.cpp
    src/modules/RvizConverter.cpp
    src/modules/MaterialManager.cpp
    src/utils/Logger.cpp
)

# Header files
set(HEADERS
    include/ui/MainWindow.h
    include/ui/PromptPanel.h
    include/ui/RenderWidget.h
    include/ui/PropertyEditor.h
    include/ui/EventLog.h
    include/ui/AssetBrowser.h
    include/ui/ExportPanel.h
    include/core/Application.h
    include/modules/BitNetClient.h
    include/modules/FuelFetcher.h
    include/modules/SDFBuilder.h
    include/modules/RvizConverter.h
    include/modules/MaterialManager.h
    include/utils/Logger.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Network
    CURL::libcurl
)

# Optional Gazebo linking
if(gz-rendering8_FOUND)
    target_link_libraries(${PROJECT_NAME} gz-rendering8::gz-rendering8)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_GAZEBO_RENDERING)
endif()

if(gz-sim8_FOUND)
    target_link_libraries(${PROJECT_NAME} gz-sim8::gz-sim8)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_GAZEBO_SIM)
endif()

if(gz-common5_FOUND)
    target_link_libraries(${PROJECT_NAME} gz-common5::gz-common5)
endif()

if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Desktop file
install(FILES resources/burma-automaton.desktop
    DESTINATION share/applications
)

# Icon
install(FILES resources/icons/burma-automaton.png
    DESTINATION share/icons/hicolor/256x256/apps
)
