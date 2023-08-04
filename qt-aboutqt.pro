
message("+++ Qt-Aboutqt qmake starting.")

# =============================================================================
# Prerequisites: environment variables
# =============================================================================
# Need environment variable QT-ABOUTQT_QT6_BASE_DIR
# Put something like this in your ~/.profile:
#   export QT-ABOUTQT_QT6_BASE_DIR="/home/rudolf/dev/qt_local_build"

# This file is read by qmake.
# Use $$(...) to read an environment variable at the time of qmake.
# Use $(...) to read an environment variable at the time of make.
# Use $$... or $${...} to read a Qt project file variable.
# See
# - http://doc.qt.io/qt-4.8/qmake-advanced-usage.html#variables
# - http://doc.qt.io/qt-5/qmake-test-function-reference.html
# Here, we copy an environment variable to a Qt project file variable:

message("... QT_ARCH: $${QT_ARCH}")

# =============================================================================
# Parts of Qt
# =============================================================================

# SQLite/Android and OpenSSL/anything requires a custom Qt build.
# ALSO TRY:
#   qmake -query  # for the qmake of the Qt build you're using

# http://doc.qt.io/qt-5/qtmultimedia-index.html
# http://wiki.qt.io/Qt_5.5.0_Multimedia_Backends
# http://doc.qt.io/qt-4.8/qmake-variable-reference.html#qt
# http://doc.qt.io/qt-5/qtmodules.html

QT += core  # included by default; QtCore module
QT += gui  # included by default; QtGui module
QT += widgets  # required to #include <QApplication>


# =============================================================================
# Overall configuration
# =============================================================================

# CONFIG += debug
    # ... no, use the QtCreator debug/release settings, which adds
    # e.g. "CONFIG+=debug CONFIG+=qml_debug" to the call to qmake
CONFIG += mobility
CONFIG += c++11

# For SQLCipher (see its README.md):
# http://stackoverflow.com/questions/16244040/is-the-qt-defines-doing-the-same-thing-as-define-in-c
DEFINES += SQLITE_HAS_CODEC
DEFINES += SQLITE_TEMP_STORE=2

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Fail to compile if you use deprecated APIs.
DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060500

MOBILITY =

# PKGCONFIG += openssl
# ... http://stackoverflow.com/questions/14681012/how-to-include-openssl-in-a-qt-project
# ... but no effect? Not mentioned in variable reference (above).
# ... ah, here's the reference:
#     http://doc.qt.io/qt-5/qmake-project-files.html
# LIBS += -lssl
# ... not working either? Doesn't complain, but ldd still shows that system libssl.so is in use

# =============================================================================
# Compiler and linker flags
# =============================================================================

gcc | clang {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
}

# Warning become errors
gcc {
    # GCC
    message("Compiler is GCC")
    QMAKE_CXXFLAGS += -Werror  # warnings become errors
}
msvc {
    # Microsoft Visual C++
    message("Compiler is Microsoft Visual C++")
    QMAKE_CXXFLAGS += /W3
        # ... /W4 is the highest level of warnings bar "/Wall"
        # ... but we get "D9025: overriding '/W4' with '/W3'"
    QMAKE_CXXFLAGS += /WX  # treat warnings as errors
    # QMAKE_CXXFLAGS += /showIncludes  # if you think the wrong ones are being included!
}
clang {
    message("Compiler is clang")
    QMAKE_CXXFLAGS += -Werror  # warnings become errors
}

# Until we use a version of Qt that can cope, disable "-Werror=deprecated-copy".
# In general, note "-Werrmsg" to enable and "-Wno-errmsg" to disable:
# https://stackoverflow.com/questions/925179/selectively-remove-warning-message-gcc
# (This problem arose on 2020-06-29 with Ubuntu 20.04 which brought gcc 9.3.0.)
# 2021-02-05: also true of clang v10.0.0.
if (gcc | clang):!ios:!android:!macx {
    !lessThan(COMPILER_MAJOR_VERSION, 9) {
        QMAKE_CXXFLAGS += -Wno-deprecated-copy
    }
}

# Later versions of clang on iOS *do* support (no-)deprecated-copy but the order
# of warning flags seems to be important and removing !ios above doesn't work
# QMAKE_CXXFLAGS_WARN_ON defaults to -Wall -W and our overrides need to come
# after that
if (ios | macx) {
    QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
    QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy
}

# In release mode, optimize heavily:
gcc {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3  # optimize heavily
}
msvc {
    QMAKE_CXXFLAGS_RELEASE -= /O1  # /O1: minimum size
    QMAKE_CXXFLAGS_RELEASE += /O2  # /O2: maximum speed

    # We also get "LNK4098: defaultlib 'LIBCMT' conflicts with use of other
    # libs; use /NODEFAULTLIB:library" but this may be because the /MD, /MT,
    # or /LD setting conflicts.
    # /MD: multithread-specific and DLL-specific version of the runtime library
    # /MT: multithread, static version of the runtime library [fails; LIBCMT conflicts...]
    # /LD: creates a DLL
    QMAKE_CXXFLAGS_RELEASE += /MD
}

# For compilers that support it, make the visibility hidden
# Note: there may be linker warnings relating to access to Qt functions, but
# they're only warnings.
gcc {
    # http://gcc.gnu.org/wiki/Visibility
    QMAKE_CXXFLAGS += -fvisibility=hidden
}
# But WARNING: this will also hide main(), which isn't a problem in Linux/
# Windows executables, but is a major problem for Android, where the Qt Java
# application wends its way to C++ then calls main() from libqt-aboutqt.so. The
# end result is an instant crash on startup. So main() must be marked for
# symbol export. [Understood/fixed 2019-06-20.]

# QMAKE_LFLAGS += --verbose  # make the linker verbose

android {
    QMAKE_RESOURCE_FLAGS += "-compress-algo zlib"
}

# =============================================================================
# Build targets
# =============================================================================

TARGET = qt-aboutqt
TEMPLATE = app


# =============================================================================
# OS-specific options
# =============================================================================
# https://wiki.qt.io/Technical_FAQ#How_can_I_detect_in_the_.pro_file_if_I_am_compiling_for_a_32_bit_or_a_64_bit_platform.3F

# -----------------------------------------------------------------------------
# Architecture
# -----------------------------------------------------------------------------

OBJ_EXT = ".o"  # unless otherwise set

# Set OS-specific variables
# Operating system tests include "linux", "unix", "macx", "android", "windows",
# "ios".
-
linux : !android {
    # -------------------------------------------------------------------------
    # LINUX -- and not Android Linux!
    # -------------------------------------------------------------------------
    STATIC_LIB_EXT = ".a"
    DYNAMIC_LIB_EXT = ".so"
    QT-ABOUTQT_QT_LINKAGE = "static"
    # QT-ABOUTQT_QT_LINKAGE = "dynamic"
    QT-ABOUTQT_OPENSSL_LINKAGE = "static"

    # https://stackoverflow.com/questions/33117822
    # https://stackoverflow.com/questions/356666
    contains(QT_ARCH, x86_64) {
        message("Building for Linux/x86_64")
    } else {  # will be "i386"
        message("Building for Linux/x86_32")
    }
}
android {
    # -------------------------------------------------------------------------
    # ANDROID
    # -------------------------------------------------------------------------
    STATIC_LIB_EXT = ".a"
    DYNAMIC_LIB_EXT = ".so"
    QT-ABOUTQT_QT_LINKAGE = "dynamic"
    # QT-ABOUTQT_OPENSSL_LINKAGE = "static"
    QT-ABOUTQT_OPENSSL_LINKAGE = "dynamic"

    contains(ANDROID_TARGET_ARCH, x86) {
        message("Building for Android/x86 (e.g. Android emulator)")
    }
    contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
        message("Building for Android/ARMv7 32-bit architecture")
    }
    contains(ANDROID_TARGET_ARCH, arm64-v8a) {
        message("Building for Android/ARMv8 64-bit architecture")
    }

}
windows {
    # -------------------------------------------------------------------------
    # WINDOWS
    # -------------------------------------------------------------------------
    STATIC_LIB_EXT = ".lib"
    DYNAMIC_LIB_EXT = ".dll"
    OBJ_EXT = ".obj"
    QT-ABOUTQT_QT_LINKAGE = "static"
    QT-ABOUTQT_OPENSSL_LINKAGE = "static"

    # https://stackoverflow.com/questions/26373143
    # https://stackoverflow.com/questions/33117822
    # https://stackoverflow.com/questions/356666
    contains(QT_ARCH, x86_64) {
        message("Building for Windows/x86_64 architecture")
    } else {
        message("Building for Windows/x86_32 architecture")
    }

    RC_FILE = windows/qt-aboutqt.rc
}
macx {
    # -------------------------------------------------------------------------
    # MacOS (formerly OS X)
    # -------------------------------------------------------------------------
    STATIC_LIB_EXT = ".a"
    DYNAMIC_LIB_EXT = ".dylib"
    QT-ABOUTQT_QT_LINKAGE = "static"
    QT-ABOUTQT_OPENSSL_LINKAGE = "static"

    contains(QT_ARCH, x86_64) {
        message("Building for MacOS/x86_64 architecture")
    } else {
        message("Building for MacOS/x86_32 architecture")
    }
}
ios {
    # -------------------------------------------------------------------------
    # iOS
    # -------------------------------------------------------------------------
    STATIC_LIB_EXT = ".a"
    DYNAMIC_LIB_EXT = ".dylib"
    QT-ABOUTQT_QT_LINKAGE = "static"
    QT-ABOUTQT_OPENSSL_LINKAGE = "static"

    # Both iphoneos and iphonesimulator are set ?!
    CONFIG(iphoneos, iphoneos|iphonesimulator) {
        message("Building for iPhone OS")
        contains(QT_ARCH, arm64) {
            message("Building for iOS/ARM v8 64-bit architecture")
        } else {
            message("Building for iOS/ARM v7 (32-bit) architecture")
        }
    }

    CONFIG(iphonesimulator, iphoneos|iphonesimulator) {
        message("Building for iPhone Simulator")
    }

    disable_warning.name = "GCC_WARN_64_TO_32_BIT_CONVERSION"
    disable_warning.value = "No"
    QMAKE_MAC_XCODE_SETTINGS += disable_warning

    QMAKE_TARGET_BUNDLE_PREFIX = "uk.ac.cam.psychiatry"
    QMAKE_BUNDLE = "qt-aboutqt"

}

# -----------------------------------------------------------------------------
# Linkage method
# -----------------------------------------------------------------------------

# To have the linker show its working:
# LIBS += "-Wl,--verbose"

equals(QT-ABOUTQT_QT_LINKAGE, "static") {  # http://doc.qt.io/qt-5/qmake-test-function-reference.html
    message("Using static linkage from Qt-Aboutqt to Qt")
    CONFIG += static
} else:equals(QT-ABOUTQT_QT_LINKAGE, "dynamic") {
    message("Using dynamic linkage from Qt-Aboutqt to Qt")
} else {
    error("Linkage method from Qt-Aboutqt to Qt not specified")
}

equals(QT-ABOUTQT_OPENSSL_LINKAGE, "static") {
    message("Using static linkage from Qt-Aboutqt to OpenSSL")
} else:equals(QT-ABOUTQT_OPENSSL_LINKAGE, "dynamic") {
    message("Using dynamic linkage from Qt-Aboutqt to OpenSSL")
} else {
    error("Linkage method from Qt-Aboutqt to OpenSSL not specified")
}

# Quick tutorial on linking, since I seem to need it:
#
# FILE TYPES
#
# - Standard Linux file types [5, 6]:
#       .o      object file
#       .a      archive (= collection of one or more object files)
#       .so     shared object (approx. equiv. to Windows DLL)
#
# COMPILER AND LINKER
#
# - g++ is the C++ compiler from the Gnu Compiler Collection but also calls the
#   linker [1]. Use "g++ --version" to find the version; currently I have
#   5.4.0.
#
# - qmake will call g++, which will call GNU's ld (the linker).
#
# - ld parameters include
#       -L<path> (as per "man g++") or -L <path> (as per "man ld")
#       -l<library>
#
# - When you specify "-lxyz", the linker looks for "libxyz.a", in standard
#   system directories plus any you specify with -L [2].
#   "Normally the files found this way are library files—archive files whose
#   members are object files. The linker handles an archive file by scanning
#   through it for members which define symbols that have so far been
#   referenced but not defined. But if the file that is found is an ordinary
#   object file, it is linked in the usual fashion. The only difference
#   between using an -l option and specifying a file name is that -l surrounds
#   library with ‘lib’ and ‘.a’ and searches several directories."
#
# - There is also the "-l:filename" object, which skips the "lib" and ".a"
#   parts [3].
#
# INFORMATION ON OBJECT/LIBRARY/EXECUTABLES
#
# - ldd prints the shared objects (shared libraries) required by a program or
#   shared object (so use it for executables and .so files).
#
# - An alternative is
#       objdump -p TARGET | grep NEEDED  # or omit the grep for more info
#   ... objdump works for .so, .a, and executables.
#
# - You can also use
#       objdump -t TARGET
#   to view its symbols.
#   For example, you might find that a linked version of qt-aboutqt not only
#   requires libcrypto.so.1.1, but that it contains symbols present in
#   libcrypto, such as EVP_aes_256_cfb, if we add a specific libcrypto.a to the
#   linker... so something is requiring the dynamic library even as we're
#   trying to link statically (despite configuring Qt with -openssl-linked).
#
# - Show files in an archive (.a) [4]:
#       ar -t libxyz.a
#
# APPLIED TO QT-ABOUTQT
#
# - If you have the linker show its working with
#       -Wl,--verbose
#   then you can also see it finding libcrypto.so during the link process, but
#   then the executable not finding it when it runs, e.g.:
#
#   link:
#       $ make
#       attempt to open /home/rudolf/dev/qt_local_build/openssl_linux_x86_64_build/openssl-1.1.0g/libcrypto.so succeeded
#       -lcrypto (/home/rudolf/dev/qt_local_build/openssl_linux_x86_64_build/openssl-1.1.0g/libcrypto.so)
#   run:
#       $ ./qt-aboutqt
#       ./qt-aboutqt: error while loading shared libraries: libssl.so.1.1: cannot open shared object file: No such file or directory
#   run with manual library path:
#       $ LD_LIBRARY_PATH=/home/rudolf/dev/qt_local_build/openssl_linux_x86_64_build/openssl-1.1.0g ./qt-aboutqt
#       ... runs.
#
# REFERENCES
#
# [1] https://stackoverflow.com/questions/172587/what-is-the-difference-between-g-and-gcc
# [2] https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc/Link-Options.html#Link-Options
# [3] https://stackoverflow.com/questions/6578484/telling-gcc-directly-to-link-a-library-statically
# [4] http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html
# [5] https://stackoverflow.com/questions/9688200/difference-between-shared-objects-so-static-libraries-a-and-dlls-so
# [6] https://en.wikipedia.org/wiki/Ar_(Unix)



# -----------------------------------------------------------------------------
# All set up
# -----------------------------------------------------------------------------
message("QMAKE_CFLAGS=$${QMAKE_CFLAGS}")
message("QMAKE_CXXFLAGS=$${QMAKE_CXXFLAGS}")
message("QMAKE_RESOURCE_FLAGS=$${QMAKE_RESOURCE_FLAGS}")
message("INCLUDEPATH=$${INCLUDEPATH}")
message("LIBS=$${LIBS}")
message("... qmake will add more to INCLUDEPATH and LIBS; see Makefile")
# ... view the Makefile at the end; qmake will have added others
# ... and run "ldd qt-aboutqt" to view dynamic library dependencies
android {
    message("ANDROID_EXTRA_LIBS=$${ANDROID_EXTRA_LIBS}")
}

# =============================================================================
# Resources and source files
# =============================================================================

SOURCES += \
    common/dpi.cpp \
    core/app.cpp \
    main.cpp \
    menu/mainmenu.cpp \
    menulib/menuitem.cpp \
    widgets/openablewidget.cpp

HEADERS += \
    common/dpi.h \
    core/app.h \
    menu/mainmenu.h \
    menulib/menuitem.h \
    widgets/openablewidget.h

message("--- Qt-Aboutqt qmake finishing.")
