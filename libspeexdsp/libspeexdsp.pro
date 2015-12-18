# Copyright (c) 2015 Oliver Lau <ola@ct.de>, Heise Medien GmbH & Co. KG
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

QT -= gui

CONFIG += staticlib warn_off

include(../Speeq.pri)

DEFINES += HAVE_CONFIG_H
DEFINES += SPEEQ_VERSION=\\\"$${SPEEQ_VERSION}\\\"

VER_MAJ = 1
VER_MIN = 2
VER_PAT = 0

TARGET = speexdsp
TEMPLATE = lib

win32-msvc* {
    QMAKE_CXXFLAGS += /wd4100 /wd4244 /wd4305
    DEFINES += _SCL_SECURE_NO_WARNINGS
    LIBS += User32.lib
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
    DEFINES -= UNICODE
}

HEADERS += \
    _kiss_fft_guts.h \
    arch.h \
    bfin.h \
    config.h \
    fftwrap.h \
    filterbank.h \
    fixed_arm4.h \
    fixed_arm5e.h \
    fixed_bfin.h \
    fixed_debug.h \
    fixed_generic.h \
    kiss_fft.h \
    kiss_fftr.h \
    math_approx.h \
    misc_bfin.h \
    os_support.h \
    pseudofloat.h \
    resample_neon.h \
    resample_sse.h \
    smallft.h \
    stack_alloc.h \
    vorbis_psy.h

SOURCES += \
    buffer.c \
    fftwrap.c \
    filterbank.c \
    jitter.c \
    kiss_fft.c \
    kiss_fftr.c \
    mdf.c \
    preprocess.c \
    resample.c \
    scal.c \
    smallft.c
