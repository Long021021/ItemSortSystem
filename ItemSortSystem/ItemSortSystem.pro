QT += core gui
QT += network
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ClientHandler.cpp \
    ThreadedTcpServer.cpp \
    camera.cpp \
    environment.cpp \
    main.cpp \
    Mainwidget.cpp \
    weatherinfo.cpp

HEADERS += \
    ClientHandler.h \
    Mainwidget.h \
    ThreadedTcpServer.h \
    camera.h \
    environment.h \
    weatherinfo.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += link_pkgconfig
PKGCONFIG += opencv4

RESOURCES += \
    rec.qrc
