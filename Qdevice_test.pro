QT       += core gui network mqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

DEFINES += QT_DEPRECATED_WARNINGS IsServer

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogSrc/customMessageHandle.cpp \
    TCP_PUBLIC/ETCTcpServer.cpp \
    main.cpp \
    deviceServer.cpp \
    TCP_PUBLIC/PTcpSocket.cpp \
    MQTT/pmqtt.cpp

HEADERS += \
    DataStruct.h \
    LogSrc/customMessageHandle.h \
    TCP_PUBLIC/ETCTcpServer.h \
    deviceServer.h \
    TCP_PUBLIC/PTcpSocket.h \
    MQTT/pmqtt.h

FORMS += \
	deviceServer.ui

RESOURCES += \
    Resource.qrc \
	deviceServer.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
