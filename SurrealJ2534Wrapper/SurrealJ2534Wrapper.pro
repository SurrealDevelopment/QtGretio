QT -= gui
QT += websockets widgets network

TEMPLATE = lib
DEFINES += SURREALJ2534WRAPPER_LIBRARY
DEFINES= QZEROCONF_STATIC



CONFIG += c++11 shared

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# to ensure compatibility its important to static link the c/c++ libs
QMAKE_LFLAGS_WINDOWS += -Wl,--add-stdcall-alias -static-libgcc -static-libstdc++

QMAKE_CXXFLAGS += -Wl,--enable-stdcall-fixup


SOURCES += \
    J2534.cpp \
    dllmain.cpp \
    gretioauthhandler.cpp \
    gretiomainwindow.cpp \
    gretioservicefinder.cpp \
    gretiowebcontext.cpp \
    gretiowebsocketclient.cpp \
    isolateqapp.cpp \
    surrealj2534wrapper.cpp

HEADERS += \
    GretioWebConstants.h \
    J2534.h \
    SurrealJ2534Wrapper_global.h \
    gretioauthhandler.h \
    gretiomainwindow.h \
    gretioservicefinder.h \
    gretiowebcontext.h \
    gretiowebsocketclient.h \
    isolateqapp.h \
    surrealj2534wrapper.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

include($$PWD/../qtzeroconf/qtzeroconf.pri)
