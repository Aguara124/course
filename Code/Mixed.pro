include(Course.pro)

#для мультимодульного проекта не забыть задать 
# ./build/%{Asciify:%{ActiveProject:Name}-%{Kit:FileSystemName}-%{BuildConfig:Name}}
# in Preferences/Build & Run/Default Build Properties/Default Build Directory


Release:DESTDIR = mixed/release
Release:OBJECTS_DIR = mixed/release/.obj
Release:MOC_DIR = mixed/release/.moc
Release:RCC_DIR = mixed/release/.rcc
Release:UI_DIR = mixed/release/.ui

Debug:DESTDIR = mixed/debug
Debug:OBJECTS_DIR = mixed/debug/.obj
Debug:MOC_DIR = mixed/debug/.moc
Debug:RCC_DIR = mixed/debug/.rcc
Debug:UI_DIR = mixed/debug/.ui


CONFIG(debug, debug|release) {
    VARIANT = debug
} else {
    VARIANT = release
}

# using shell_path() to correct path depending on platform
# escaping quotes and backslashes for file paths
copydata.commands = xcopy.exe /E /Y \"$$shell_path($$PWD/distfiles/mixed)\" \"$$shell_path($$OUT_PWD/mixed/$$VARIANT)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Release/components/release/ -lcomponents
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Debug/components/debug/ -lcomponents

INCLUDEPATH += $$PWD/../../qt-material-widgets-master/components
DEPENDPATH += $$PWD/../../qt-material-widgets-master/components

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Release/components/release/libcomponents.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Debug/components/debug/libcomponents.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Release/components/release/components.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../qt-material-widgets-master/build/qt-material-widgets-Desktop_Qt_6_7_0_shared_MinGW_w64_MINGW64_MSYS2-Debug/components/debug/components.lib

