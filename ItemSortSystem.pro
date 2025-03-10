QT += core gui
QT += network
QT += charts
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += $$PWD/yolov5

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera/camera.cpp \
    main.cpp \
    Mainwidget.cpp \
    temphumi/ClientHandler.cpp \
    temphumi/ThreadedTcpServer.cpp \
    temphumi/environment.cpp \
    weather/weatherinfo.cpp \
    yolov5/RK3568Infer.cpp \
    yolov5/postprocess.cpp \


HEADERS += \
    Mainwidget.h \
    camera/camera.h \
    temphumi/ClientHandler.h \
    temphumi/ThreadedTcpServer.h \
    temphumi/environment.h \
    weather/weatherinfo.h \
    yolov5/RK3568Infer.h \
    yolov5/postprocess.h \
    yolov5/rknn_api.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += link_pkgconfig
PKGCONFIG += opencv4 librga rockchip_mpp

LIBS+= -L$$PWD/lib -lrknnrt

RESOURCES += \
    rec.qrc

