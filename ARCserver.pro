QT += gui
QT += sql
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    src/CompareServer/arccompareserver.cpp \
    src/AddFeatureServer/arcaddfeatureserver.cpp \
    src/GuestServer/arcguestserver.cpp \
    arcfaceengine.cpp \
    arcfacefeaturedb.cpp \
    arcfaceengines.cpp

HEADERS += \
    ARCFace/amcomdef.h \
    ARCFace/arcsoft_fsdk_face_detection.h \
    ARCFace/arcsoft_fsdk_face_recognition.h \
    ARCFace/asvloffscreen.h \
    ARCFace/merror.h \
    muduo/base/AsyncLogging.h \
    muduo/base/Atomic.h \
    muduo/base/BlockingQueue.h \
    muduo/base/BoundedBlockingQueue.h \
    muduo/base/Condition.h \
    muduo/base/copyable.h \
    muduo/base/CountDownLatch.h \
    muduo/base/CurrentThread.h \
    muduo/base/Date.h \
    muduo/base/Exception.h \
    muduo/base/FileUtil.h \
    muduo/base/GzipFile.h \
    muduo/base/LogFile.h \
    muduo/base/Logging.h \
    muduo/base/LogStream.h \
    muduo/base/Mutex.h \
    muduo/base/ProcessInfo.h \
    muduo/base/Singleton.h \
    muduo/base/StringPiece.h \
    muduo/base/Thread.h \
    muduo/base/ThreadLocal.h \
    muduo/base/ThreadLocalSingleton.h \
    muduo/base/ThreadPool.h \
    muduo/base/Timestamp.h \
    muduo/base/TimeZone.h \
    muduo/base/Types.h \
    muduo/base/WeakCallback.h \
    muduo/net/http/HttpContext.h \
    muduo/net/http/HttpRequest.h \
    muduo/net/http/HttpResponse.h \
    muduo/net/http/HttpServer.h \
    muduo/net/inspect/Inspector.h \
    muduo/net/Buffer.h \
    muduo/net/Callbacks.h \
    muduo/net/Channel.h \
    muduo/net/Endian.h \
    muduo/net/EventLoop.h \
    muduo/net/EventLoopThread.h \
    muduo/net/EventLoopThreadPool.h \
    muduo/net/InetAddress.h \
    muduo/net/TcpClient.h \
    muduo/net/TcpConnection.h \
    muduo/net/TcpServer.h \
    muduo/net/TimerId.h \
    ARCFace/amcomdef.h \
    ARCFace/arcsoft_fsdk_face_detection.h \
    ARCFace/arcsoft_fsdk_face_recognition.h \
    ARCFace/asvloffscreen.h \
    ARCFace/merror.h \
    src/CompareServer/arccompareserver.h \
    src/CompareServer/comparecodec.h \
    src/AddFeatureServer/arcaddfeatureserver.h \
    src/GuestServer/arcguestserver.h \
    arcfaceengine.h \
    key.h \
    arcfacefeaturedb.h \
    arcfaceengines.h \
    publicdatastruct.h
# muode lib
unix:!macx: LIBS += -L$$PWD/../../DevelopLIBS/release-install/lib/ -lmuduo_base_cpp11

INCLUDEPATH += $$PWD/../../DevelopLIBS/release-install/include
DEPENDPATH += $$PWD/../../DevelopLIBS/release-install/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../DevelopLIBS/release-install/lib/libmuduo_base_cpp11.a

unix:!macx: LIBS += -L$$PWD/../../DevelopLIBS/release-install/lib/ -lmuduo_net_cpp11

INCLUDEPATH += $$PWD/../../DevelopLIBS/release-install/include
DEPENDPATH += $$PWD/../../DevelopLIBS/release-install/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../DevelopLIBS/release-install/lib/libmuduo_net_cpp11.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DevelopLIBS/release/ -larcsoft_fsdk_face_detection
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../DevelopLIBS/debug/ -larcsoft_fsdk_face_detection
else:unix: LIBS += -L$$PWD/../../DevelopLIBS/ -larcsoft_fsdk_face_detection

INCLUDEPATH += $$PWD/../../DevelopLIBS
DEPENDPATH += $$PWD/../../DevelopLIBS

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DevelopLIBS/release/ -larcsoft_fsdk_face_recognition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../DevelopLIBS/debug/ -larcsoft_fsdk_face_recognition
else:unix: LIBS += -L$$PWD/../../DevelopLIBS/ -larcsoft_fsdk_face_recognition

INCLUDEPATH += $$PWD/../../DevelopLIBS
DEPENDPATH += $$PWD/../../DevelopLIBS
