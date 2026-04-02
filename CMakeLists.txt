cmake_minimum_required(VERSION 3.15)

set(APP_NAME VichMat)

project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(freeglut
    GIT_REPOSITORY https://github.com/freeglut/freeglut.git
    GIT_TAG master)

FetchContent_MakeAvailable(freeglut)


FetchContent_Declare(GLEW
GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
GIT_TAG master)

FetchContent_MakeAvailable(GLEW)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)

FetchContent_MakeAvailable(glm)

# Обновленная версия imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0  # Обновлено до более новой версии
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(implot3d
    GIT_REPOSITORY https://github.com/brenocq/implot3d.git
    GIT_TAG v0.3
)

FetchContent_MakeAvailable(implot3d)

FetchContent_Declare(implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.17
)

FetchContent_MakeAvailable(implot)

MACRO(HEADER_DIRECTORIES return_list search_path)
    # Find all header files recursively
    file(GLOB_RECURSE new_list "${search_path}/*.h" "${search_path}/*.hpp")
    
    set(dir_list "")
    # Extract the directory component from each file path
    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} PATH)
        list(APPEND dir_list ${dir_path})
    endforeach()
    
    # Remove duplicate directory entries
    list(REMOVE_DUPLICATES dir_list)
    
    # Set the output variable with the list of unique directories
    set(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(MY_INCLUDES src)

file(GLOB TARGET_SRC "src/*.cpp" "src/*/*.cpp")
file(GLOB TARGET_HEADERS "src/*.h" "src/*/*.h" "src/*/*/*.h")

set(BUILD_SHARED_LIBS OFF)
set(FREEGLUT_BUILD_STATIC_LIBS ON)

message(--TARGET_SRC-->${TARGET_SRC})
message(--TARGET_HEADERS-->${TARGET_HEADERS})
message(--INCLUDE-->${MY_INCLUDES})

add_executable(main ${TARGET_SRC})

target_include_directories(main PUBLIC ${MY_INCLUDES})

target_link_libraries(main PRIVATE freeglut)
target_link_libraries(main PRIVATE libglew_static)
target_link_libraries(main PRIVATE glm)
# target_link_libraries(main PRIVATE implot)
# target_link_libraries(main PRIVATE implot3d)

target_include_directories(main PUBLIC ${implot_SOURCE_DIR})
target_include_directories(main PUBLIC ${implot3d_SOURCE_DIR})

file(GLOB IMGUI_SOURCES 
    "${imgui_SOURCE_DIR}/*.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glut.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${implot_SOURCE_DIR}/*.cpp"
    "${implot3d_SOURCE_DIR}/*.cpp"
)

add_library(graphs STATIC ${IMGUI_SOURCES})
target_include_directories(graphs PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

target_link_libraries(main PRIVATE graphs)
target_include_directories(main PUBLIC ${imgui_SOURCE_DIR})

message(---${implot_SOURCE_DIR})

# Настройка выходных директорий
set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>
    OUTPUT_NAME ${APP_NAME}
)

# Добавляем .exe только на Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX .exe)
endif()

set_target_properties(freeglut
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)