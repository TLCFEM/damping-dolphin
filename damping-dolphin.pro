QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14

RC_ICONS = res/dolphin.ico

unix: QMAKE_CXXFLAGS += -Wno-deprecated-copy -Wno-implicit-fallthrough -fopenmp

unix:!macx: LIBS += -L$$PWD/lib/linux -lopenblas -lgfortran -lquadmath -lgomp

win32{
msvc:LIBS += -L$$PWD/lib/win-msvc -llibopenblas
gcc:LIBS += -L$$PWD/lib/win-gcc -lopenblas -lgfortran -lquadmath -lgomp
msvc:QMAKE_CXXFLAGS += /openmp:experimental
gcc: QMAKE_CXXFLAGS += -fopenmp
}

DEFINES += ARMA_DONT_USE_ATLAS

win32{
DEFINES += ARMA_USE_OPENMP
}

exists(tbb){
message("Enable tbb.")
msvc{
LIBS += -L$$PWD/tbb/lib -ltbb
}
gcc{
LIBS += -L$$PWD/tbb/lib -llibtbb12
}
DEFINES += DD_TBB_ENABLED
INCLUDEPATH += $$PWD/tbb/include
}

INCLUDEPATH += include \
    include/QCustomPlot \
    src

SOURCES += \
    src/FitSetting.cpp \
    include/QCustomPlot/qcustomplot.cpp \
    src/About.cpp \
    src/Guide.cpp \
    src/damping-dolphin.cpp \
    src/DampingCurve.cpp \
    src/DampingMode.cpp \
    src/MainWindow.cpp \
    src/Scheme/ObjectiveFunction.cpp \
    src/Scheme/ThreeWiseMen.cpp \
    src/Scheme/Unicorn.cpp \
    src/Scheme/TwoCities.cpp \
    src/Scheme/ZeroDay.cpp

HEADERS += \
    src/FitSetting.h \
    include/QCustomPlot/qcustomplot.h \
    src/About.h \
    src/Guide.h \
    src/damping-dolphin.h\
    src/DampingCurve.h \
    src/DampingMode.h \
    src/MainWindow.h \
    src/Scheme/OptimizerTuning.hpp \
    src/Scheme/ObjectiveFunction.h \
    src/Scheme/ThreeWiseMen.h \
    src/Scheme/Unicorn.h \
    src/Scheme/TwoCities.h \
    src/Scheme/ZeroDay.h

FORMS += \
    form/About.ui \
    form/FitSetting.ui \
    form/Guide.ui \
    form/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    include/QCustomPlot/GPL.txt

RESOURCES += \
    res/res.qrc
