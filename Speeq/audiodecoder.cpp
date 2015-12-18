/*

    Copyright (c) 2015 Oliver Lau <ola@ct.de>, Heise Medien GmbH & Co. KG

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "audiodecoder.h"
#include <QDebug>
#include <QtEndian>
#include <QFile>

#include <speex/speex.h>

static const int MAX_FRAME_SIZE = 2000;
static const int MAX_FRAME_BYTES = 2000;

class AudioDecoderPrivate {
public:
  AudioDecoderPrivate(const QAudioFormat &format, QMutex *mutex)
    : format(format)
    , sampleBufferMutex(mutex)
  {
    init_speex();
  }
  ~AudioDecoderPrivate()
  {
    deinit_speex();
  }
  const QAudioFormat format;
  SampleBufferType sampleBuffer;
  QMutex *sampleBufferMutex;
  QFile audioFile;
  SpeexBits bits;
  spx_int32_t complexity;
  const SpeexMode *mode;
  void *st;
  int modeID;
  short input[MAX_FRAME_SIZE];
  char cbits[MAX_FRAME_BYTES];
  const char* speex_version;

private:
  void init_speex(void)
  {
    speex_bits_init(&bits);
    modeID = SPEEX_MODEID_NB;
    mode = speex_lib_get_mode(modeID);
    st = speex_decoder_init(mode);
  }

  void deinit_speex(void)
  {
    speex_decoder_destroy(st);
    speex_bits_destroy(&bits);
  }
};


AudioDecoder::AudioDecoder(const QAudioFormat &format, QMutex *mutex, QObject *parent)
  : QIODevice(parent)
  , d_ptr(new AudioDecoderPrivate(format, mutex))
{
  Q_D(AudioDecoder);
  d->audioFile.setFileName("D:\\Temp\\ready.spx");
  d->audioFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}


AudioDecoder::~AudioDecoder()
{
  // Q_D(AudioDecoder);
  stop();
}


void AudioDecoder::start(void)
{
  open(QIODevice::ReadOnly);
}


void AudioDecoder::stop(void)
{
  close();
}


const SampleBufferType &AudioDecoder::sampleBuffer(void) const
{
  return d_ptr->sampleBuffer;
}


qint64 AudioDecoder::readData(char *data, qint64 maxlen)
{
  Q_UNUSED(data)
  Q_UNUSED(maxlen)
  qDebug() << "AudioDecoder::readData(..., " << maxlen << ")";
  return maxlen;
}


qint64 AudioDecoder::writeData(const char *data, qint64 len)
{
  Q_UNUSED(data)
  Q_UNUSED(len)
  return 0;
}
