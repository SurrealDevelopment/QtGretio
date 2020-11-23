QT -= gui
QT += websockets

TEMPLATE = lib
DEFINES += SURREALJ2534WRAPPER_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    J2534.cpp \
    gretioauthhandler.cpp \
    gretiowebcontext.cpp \
    gretiowebsocketclient.cpp \
    surrealj2534wrapper.cpp

HEADERS += \
    J2534.h \
    SurrealJ2534Wrapper_global.h \
    gretioauthhandler.h \
    gretiowebcontext.h \
    gretiowebsocketclient.h \
    surrealj2534wrapper.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
