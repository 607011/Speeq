# Copyright (c) 2015 Oliver Lau <ola@ct.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

TARGET = Speeq
TEMPLATE = app

include(../Speeq.pri)

QT       += core gui widgets multimedia network

VERSION = -$${SPEEQ_VERSION}
DEFINES += SPEEQ_VERSION=\\\"$${SPEEQ_VERSION}\\\"


SOURCES += main.cpp\
        mainwindow.cpp \
    global.cpp \
    util.cpp \
    volumerenderarea.cpp \
    waverenderarea.cpp \
    audioencoder.cpp \
    audiodecoder.cpp \
    udpclient.cpp

HEADERS  += mainwindow.h \
    global.h \
    util.h \
    volumerenderarea.h \
    waverenderarea.h \
    audioencoder.h \
    audiodecoder.h \
    udpclient.h

FORMS    += mainwindow.ui

DISTFILES += \
    Speeq.rc \
    ../README.md

RESOURCES += \
    speeq.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libspeex/release/ -lspeex
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libspeex/debug/ -lspeex
else:unix: LIBS += -L$$OUT_PWD/../libspeex/ -lspeex

INCLUDEPATH += $$PWD/../libspeex
DEPENDPATH += $$PWD/../libspeex

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeex/release/libspeex.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeex/debug/libspeex.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeex/release/speex.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeex/debug/speex.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libspeex/libspeex.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libspeexdsp/release/ -lspeexdsp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libspeexdsp/debug/ -lspeexdsp
else:unix: LIBS += -L$$OUT_PWD/../libspeexdsp/ -lspeexdsp

INCLUDEPATH += $$PWD/../libspeexdsp
DEPENDPATH += $$PWD/../libspeexdsp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeexdsp/release/libspeexdsp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeexdsp/debug/libspeexdsp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeexdsp/release/speexdsp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libspeexdsp/debug/speexdsp.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libspeexdsp/libspeexdsp.a
