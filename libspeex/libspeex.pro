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

TARGET = speex
TEMPLATE = lib

win32-msvc* {
    QMAKE_CXXFLAGS += /wd4100 /wd4244 /wd4305
    DEFINES += _SCL_SECURE_NO_WARNINGS
    LIBS += User32.lib
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
    DEFINES -= UNICODE
}

HEADERS += \
    $$PWD/_kiss_fft_guts.h \
    $$PWD/arch.h \
    $$PWD/bfin.h \
    $$PWD/cb_search.h \
    $$PWD/cb_search_arm4.h \
    $$PWD/cb_search_bfin.h \
    $$PWD/cb_search_sse.h \
    $$PWD/fftwrap.h \
    $$PWD/filters.h \
    $$PWD/filters_arm4.h \
    $$PWD/filters_bfin.h \
    $$PWD/filters_sse.h \
    $$PWD/fixed_arm4.h \
    $$PWD/fixed_arm5e.h \
    $$PWD/fixed_bfin.h \
    $$PWD/fixed_debug.h \
    $$PWD/fixed_generic.h \
    $$PWD/kiss_fft.h \
    $$PWD/kiss_fftr.h \
    $$PWD/lpc.h \
    $$PWD/lpc_bfin.h \
    $$PWD/lsp.h \
    $$PWD/lsp_bfin.h \
    $$PWD/ltp.h \
    $$PWD/ltp_arm4.h \
    $$PWD/ltp_bfin.h \
    $$PWD/ltp_sse.h \
    $$PWD/math_approx.h \
    $$PWD/misc_bfin.h \
    $$PWD/modes.h \
    $$PWD/nb_celp.h \
    $$PWD/os_support.h \
    $$PWD/quant_lsp.h \
    $$PWD/quant_lsp_bfin.h \
    $$PWD/sb_celp.h \
    $$PWD/smallft.h \
    $$PWD/stack_alloc.h \
    $$PWD/vbr.h \
    $$PWD/vorbis_psy.h \
    $$PWD/vq.h \
    $$PWD/vq_arm4.h \
    $$PWD/vq_bfin.h \
    $$PWD/vq_sse.h \
    $$PWD/config.h \
    $$PWD/speex/speex.h \
    $$PWD/speex/speex_bits.h \
    $$PWD/speex/speex_callbacks.h \
    $$PWD/speex/speex_header.h \
    $$PWD/speex/speex_stereo.h \
    $$PWD/speex/speex_types.h

SOURCES += \
    $$PWD/bits.c \
    $$PWD/cb_search.c \
    $$PWD/exc_5_64_table.c \
    $$PWD/exc_5_256_table.c \
    $$PWD/exc_8_128_table.c \
    $$PWD/exc_10_16_table.c \
    $$PWD/exc_10_32_table.c \
    $$PWD/exc_20_32_table.c \
    $$PWD/fftwrap.c \
    $$PWD/filters.c \
    $$PWD/gain_table.c \
    $$PWD/gain_table_lbr.c \
    $$PWD/hexc_10_32_table.c \
    $$PWD/hexc_table.c \
    $$PWD/high_lsp_tables.c \
    $$PWD/kiss_fft.c \
    $$PWD/kiss_fftr.c \
    $$PWD/lpc.c \
    $$PWD/lsp.c \
    $$PWD/lsp_tables_nb.c \
    $$PWD/ltp.c \
    $$PWD/modes.c \
    $$PWD/modes_wb.c \
    $$PWD/nb_celp.c \
    $$PWD/quant_lsp.c \
    $$PWD/sb_celp.c \
    $$PWD/smallft.c \
    $$PWD/speex.c \
    $$PWD/speex_callbacks.c \
    $$PWD/speex_header.c \
    $$PWD/stereo.c \
    $$PWD/vbr.c \
    $$PWD/vorbis_psy.c \
    $$PWD/vq.c \
    $$PWD/window.c
