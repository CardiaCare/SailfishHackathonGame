TEMPLATE = lib
TARGET = smartslog

QT += qml quick
CONFIG += qt plugin c++11
CONFIG += object_parallel_to_source

DESTDIR = ../app/SmartSlog
TARGET = $$qtLibraryTarget($$TARGET)
uri = SmartSlog

INCLUDEPATH += \
            vendor/SmartSlog \
            vendor/SmartSlog/expat \

SOURCES += \
    smartslog_plugin.cpp \
    qmlsmartslog.cpp \
    $$system(ls vendor/SmartSlog/utils/*.c) \
    $$system(ls vendor/SmartSlog/expat/*.c) \
    $$system(ls vendor/SmartSlog/scew/*.c)  \
    $$system(ls vendor/SmartSlog/ckpi/*.c)  \
    $$system(ls vendor/SmartSlog/*.c)

HEADERS += \
    smartslog_plugin.h \
    qmlsmartslog.h

QMAKE_CFLAGS += -std=c99 -DHAVE_MEMMOVE -DMTENABLE -DHAVE_EXPAT_CONFIG_H -Wall

DEFINES += _GNU_SOURCE

DESTPATH=../app/SmartSlog

target.path=$$DESTPATH
qmldir.files=$$PWD/qmldir
qmldir.path=$$DESTPATH
INSTALLS += target qmldir

OTHER_FILES += qmldir

QMAKE_POST_LINK += $$QMAKE_COPY $$replace($$list($$quote($$PWD/qmldir) $$DESTDIR), /, $$QMAKE_DIR_SEP)
