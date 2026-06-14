#-------------------------------------------------
#
# Project created by QtCreator 2025-07-18T11:01:33
#
#-------------------------------------------------


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartParkingSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    view/openwidget.cpp \
    Thread/qvideoshow.cpp \
    view/mainwidget.cpp \
    view/settingwidget.cpp \
    view/loginwidget.cpp \
    Thread/readthread.cpp \
    Thread/writethread.cpp \
    socketconnect/socketconnect.cpp \
    view/reginwidget.cpp \
    view/mycode.cpp \
    view/parkinginnermagwidget.cpp \
    tool/ctool.cpp \
    /opt/EasyPR/src/core/chars_identify.cpp \
    /opt/EasyPR/src/core/chars_recognise.cpp \
    /opt/EasyPR/src/core/chars_segment.cpp \
    /opt/EasyPR/src/core/core_func.cpp \
    /opt/EasyPR/src/core/feature.cpp \
    /opt/EasyPR/src/core/params.cpp \
    /opt/EasyPR/src/core/plate_detect.cpp \
    /opt/EasyPR/src/core/plate_judge.cpp \
    /opt/EasyPR/src/core/plate_locate.cpp \
    /opt/EasyPR/src/core/plate_recognize.cpp \
    /opt/EasyPR/src/train/ann_train.cpp \
    /opt/EasyPR/src/train/annCh_train.cpp \
    /opt/EasyPR/src/train/create_data.cpp \
    /opt/EasyPR/src/train/svm_train.cpp \
    /opt/EasyPR/src/train/train.cpp \
    /opt/EasyPR/src/util/kv.cpp \
    /opt/EasyPR/src/util/program_options.cpp \
    /opt/EasyPR/src/util/util.cpp \
    /opt/EasyPR/thirdparty/LBP/helper.cpp \
    /opt/EasyPR/thirdparty/LBP/lbp.cpp \
    /opt/EasyPR/thirdparty/mser/mser2.cpp \
    /opt/EasyPR/thirdparty/svm/corrected_svm.cpp \
    /opt/EasyPR/thirdparty/textDetect/erfilter.cpp \
    /opt/EasyPR/thirdparty/xmlParser/xmlParser.cpp \
    view/videoplayerwidget.cpp \
    view/carinfocheckwidget.cpp \
    Thread/cfilesplit.cpp \
    Thread/myrunnable.cpp \
    view/monitorreplaywin.cpp \
    Thread/heartboomthread.cpp \
    Thread/camerathread.cpp



HEADERS += \
    view/openwidget.h \
    Thread/qvideoshow.h \
    view/mainwidget.h \
    view/settingwidget.h \
    view/loginwidget.h \
    Thread/readthread.h \
    Thread/writethread.h \
    socketconnect/socketconnect.h \
    view/reginwidget.h \
    protocol/protocol.h \
    view/mycode.h \
    view/parkinginnermagwidget.h \
    tool/ctool.h \
    /opt/EasyPR/include/easypr/core/character.hpp \
    /opt/EasyPR/include/easypr/core/chars_identify.h \
    /opt/EasyPR/include/easypr/core/chars_recognise.h \
    /opt/EasyPR/include/easypr/core/chars_segment.h \
    /opt/EasyPR/include/easypr/core/core_func.h \
    /opt/EasyPR/include/easypr/core/feature.h \
    /opt/EasyPR/include/easypr/core/params.h \
    /opt/EasyPR/include/easypr/core/plate.hpp \
    /opt/EasyPR/include/easypr/core/plate_detect.h \
    /opt/EasyPR/include/easypr/core/plate_judge.h \
    /opt/EasyPR/include/easypr/core/plate_locate.h \
    /opt/EasyPR/include/easypr/core/plate_recognize.h \
    /opt/EasyPR/include/easypr/train/ann_train.h \
    /opt/EasyPR/include/easypr/train/annCh_train.h \
    /opt/EasyPR/include/easypr/train/create_data.h \
    /opt/EasyPR/include/easypr/train/svm_train.h \
    /opt/EasyPR/include/easypr/train/train.h \
    /opt/EasyPR/include/easypr/util/kv.h \
    /opt/EasyPR/include/easypr/util/program_options.h \
    /opt/EasyPR/include/easypr/util/switch.hpp \
    /opt/EasyPR/include/easypr/util/util.h \
    /opt/EasyPR/include/easypr/api.hpp \
    /opt/EasyPR/include/easypr/config.h \
    /opt/EasyPR/include/easypr/version.h \
    /opt/EasyPR/include/easypr.h \
    /opt/EasyPR/test/accuracy.hpp \
    /opt/EasyPR/test/chars.hpp \
    /opt/EasyPR/test/config.hpp \
    /opt/EasyPR/test/plate.hpp \
    /opt/EasyPR/test/result.hpp \
    /opt/EasyPR/thirdparty/LBP/helper.hpp \
    /opt/EasyPR/thirdparty/LBP/lbp.hpp \
    /opt/EasyPR/thirdparty/mser/mser2.hpp \
    /opt/EasyPR/thirdparty/svm/precomp.hpp \
    /opt/EasyPR/thirdparty/textDetect/erfilter.hpp \
    /opt/EasyPR/thirdparty/xmlParser/xmlParser.h \
    view/videoplayerwidget.h \
    view/carinfocheckwidget.h \
    Thread/cfilesplit.h \
    Thread/myrunnable.h \
    view/monitorreplaywin.h \
    Thread/heartboomthread.h \
    Thread/camerathread.h


FORMS += \
    view/mainwidget.ui \
    view/openwidget.ui \
    view/settingwidget.ui \
    view/loginwidget.ui \
    view/reginwidget.ui \
    view/parkinginnermagwidget.ui \
    view/videoplayerwidget.ui \
    view/carinfocheckwidget.ui \
    view/monitorreplaywin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += /usr/local/include/
LIBS += /usr/local/lib/libopencv_world.so

INCLUDEPATH += /opt/EasyPR/include/

DISTFILES += \
    bin/model/ann_chinese.xml \
    bin/model/ann.xml \
    bin/model/annCh.xml \
    bin/model/svm_hist.xml

DESTDIR=bin

RESOURCES += \
    resource.qrc






















