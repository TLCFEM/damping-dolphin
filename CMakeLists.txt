cmake_minimum_required(VERSION 3.22)

project(damping-dolphin)

set(COMPILE_TBB OFF CACHE BOOL "Compile TBB")
set(QT_PATH "$ENV{HOME}/Qt5.12.12/5.12.12/gcc_64/lib/cmake" CACHE STRING "Path to Qt")
set(CMAKE_PREFIX_PATH "${QT_PATH}")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS form)
set(CMAKE_CXX_STANDARD 14)

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

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        )
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        )

if (NOT Qt5_FOUND AND NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt not found")
endif ()

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

if (Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets
            Qt6::PrintSupport
            openblas
            )
else ()
    target_link_libraries(${PROJECT_NAME}
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::PrintSupport
            openblas
            )
endif ()

if (UNIX)
    target_link_libraries(damping-dolphin gfortran quadmath gomp pthread)
endif ()

if (UNIX)
    add_compile_definitions(DD_TBB_ENABLED)

    if (COMPILE_TBB)
        set(TBB_TEST OFF CACHE BOOL "Override option" FORCE)
        set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH OFF CACHE BOOL "Override option" FORCE)
        add_subdirectory(oneTBB)

        include_directories(oneTBB/include)

        add_dependencies(${PROJECT_NAME} TBB::tbb TBB::tbbmalloc TBB::tbbmalloc_proxy)
    endif ()

    target_link_libraries(${PROJECT_NAME} tbb tbbmalloc tbbmalloc_proxy)
endif ()
