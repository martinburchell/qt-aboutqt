QT += core
QT += gui
QT += widgets


CONFIG += mobility
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060500

MOBILITY =

gcc | clang {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
}

gcc {
    QMAKE_CXXFLAGS += -Werror
}
msvc {
    QMAKE_CXXFLAGS += /W3
    QMAKE_CXXFLAGS += /WX
}
clang {
    QMAKE_CXXFLAGS += -Werror
}

if (gcc | clang):!ios:!android:!macx {
    !lessThan(COMPILER_MAJOR_VERSION, 9) {
        QMAKE_CXXFLAGS += -Wno-deprecated-copy
    }
}

if (ios | macx) {
    QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy
}

gcc {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}
msvc {
    QMAKE_CXXFLAGS_RELEASE -= /O1
    QMAKE_CXXFLAGS_RELEASE += /O2

    QMAKE_CXXFLAGS_RELEASE += /MD
}

gcc {
    QMAKE_CXXFLAGS += -fvisibility=hidden
}
android {
    QMAKE_RESOURCE_FLAGS += "-compress-algo zlib"
}

TARGET = qt-aboutqt
TEMPLATE = app


OBJ_EXT = ".o"

ios {
    disable_warning.name = "GCC_WARN_64_TO_32_BIT_CONVERSION"
    disable_warning.value = "No"
    QMAKE_MAC_XCODE_SETTINGS += disable_warning

    QMAKE_TARGET_BUNDLE_PREFIX = "uk.ac.cam.psychiatry"
    QMAKE_BUNDLE = "aboutqt"
}

SOURCES += \
    app.cpp \
    main.cpp \
    mainmenu.cpp

HEADERS += \
    app.h \
    mainmenu.h
