cmake_minimum_required(VERSION 3.22)

project(damping-dolphin)

set(QT_PATH "$ENV{HOME}/Qt5.12.12/5.12.12/gcc_64/lib/cmake" CACHE STRING "Path to Qt")
set(CMAKE_PREFIX_PATH "${QT_PATH}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS form)
set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(ARMA_DONT_USE_ATLAS)
if (UNIX)
    add_compile_definitions(DD_TBB_ENABLED)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

if (UNIX)
    link_directories(lib/linux)
elseif (WIN32)
    if (MSVC)
        link_directories(lib/win-msvc)
    else ()
        link_directories(lib/win-gcc)
    endif ()
endif ()

include_directories(include)
include_directories(include/QCustomPlot)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        REQUIRED)

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

set(TBB_TEST OFF CACHE BOOL "Override option" FORCE)
set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH OFF CACHE BOOL "Override option" FORCE)
add_subdirectory(oneTBB)

include_directories(oneTBB/include)

add_executable(damping-dolphin ${UIS} ${SOURCES} ${RESOURCES})

target_link_libraries(damping-dolphin
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        )

if (WIN32)
    if (MSVC)
        target_link_libraries(damping-dolphin openblas tbb tbbmalloc tbbmalloc_proxy)
    else ()
        target_link_libraries(damping-dolphin openblas)
    endif ()
elseif (UNIX)
    target_link_libraries(damping-dolphin openblas gfortran quadmath gomp tbb tbbmalloc tbbmalloc_proxy)
endif ()
