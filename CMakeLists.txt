cmake_minimum_required(VERSION 3.22)

project(damping-dolphin)

set(DD_USE_TBB OFF CACHE BOOL "Compile TBB")
if (NOT QT_PATH)
    set(QT_PATH "$ENV{HOME}/Qt/6.10.1/gcc_64/lib/cmake" CACHE STRING "Path to Qt")
endif ()
set(CMAKE_PREFIX_PATH "${QT_PATH}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS form)
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(ARMA_DONT_USE_ATLAS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-implicit-fallthrough")
if (UNIX)
    add_compile_definitions(ARMA_USE_OPENMP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif ()

if (UNIX)
    link_directories(lib/linux)
elseif (MSVC)
    link_directories(lib/win-msvc)
else ()
    link_directories(lib/win-gcc)
endif ()

include_directories(include)
include_directories(include/QCustomPlot)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
)

set(SOURCES
        src/FitSetting.cpp
        include/QCustomPlot/qcustomplot.cpp
        src/About.cpp
        src/Guide.cpp
        src/damping-dolphin.cpp
        src/DampingCurve.cpp
        src/DampingMode.cpp
        src/MainWindow.cpp
        src/Scheme/ObjectiveFunction.cpp
        src/Scheme/ThreeWiseMen.cpp
        src/Scheme/Unicorn.cpp
        src/Scheme/TwoCities.cpp
        src/Scheme/ZeroDay.cpp
)

set(UIS
        form/About.ui
        form/FitSetting.ui
        form/Guide.ui
        form/MainWindow.ui
)

set(RESOURCES
        res/res.qrc
)

add_executable(${PROJECT_NAME} ${UIS} ${SOURCES} ${RESOURCES})

target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
        openblas
)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} gfortran quadmath gomp pthread)

    if (DD_USE_TBB)
        add_compile_definitions(DD_TBB_ENABLED)

        set(TBB_TEST OFF CACHE BOOL "Override option" FORCE)
        set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH OFF CACHE BOOL "Override option" FORCE)
        add_subdirectory(oneTBB)

        include_directories(oneTBB/include)

        target_link_libraries(${PROJECT_NAME} TBB::tbb TBB::tbbmalloc TBB::tbbmalloc_proxy)
    endif ()
endif ()
