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
#include <QVector>

#include <speex/speex.h>
#include <speex/speex_header.h>

static const int MAX_FRAME_SIZE = 2000;
static const int MAX_FRAME_BYTES = 2000;

class AudioDecoderPrivate {
public:
  AudioDecoderPrivate(const QAudioFormat &format, QMutex *mutex)
    : format(format)
    , level(0.0)
    , sampleBufferMutex(mutex)
  {
    init_speex();
  }
  ~AudioDecoderPrivate()
  {
    deinit_speex();
  }
  const QAudioFormat format;
  qreal level;
  SampleBufferType sampleBuffer;
  QMutex *sampleBufferMutex;
  QFile audioFile;
  SpeexBits bits;
  SpeexHeader header;
  int nframes;
  int nb_samples;
  int total_samples;
  int nb_encoded;
  spx_int32_t complexity;
  const SpeexMode *mode;
  void *st;
  int modeID;
  short input[MAX_FRAME_SIZE];
  char cbits[MAX_FRAME_BYTES];
  spx_int32_t frame_size;
  spx_int32_t vbr_enabled;
  spx_int32_t vbr_max;
  spx_int32_t abr_enabled;
  spx_int32_t vad_enabled;
  spx_int32_t dtx_enabled;
  int nbBytes;
  int print_bitrate;
  spx_int32_t rate;
  spx_int32_t size;
  int chan;
  int fmt;
  spx_int32_t quality;
  float vbr_quality;
  int lsb;
  spx_int32_t tmp;
  spx_int32_t highpass_enabled;
  int output_rate;
  spx_int32_t lookahead;
  spx_int32_t bitrate;
  const char* speex_version;

private:
  void init_speex(void)
  {
    speex_lib_ctl(SPEEX_LIB_GET_VERSION_STRING, (void*)&speex_version);
    qDebug() << speex_version;
    nb_samples = 0;
    total_samples = 0;
    nb_encoded = 0;
    vbr_max = 0;
    vbr_enabled = true;
    abr_enabled = false;
    vad_enabled = false;
    dtx_enabled = false;
    print_bitrate = 0;
    modeID = SPEEX_MODEID_WB;
    mode = speex_lib_get_mode(modeID);
    st = speex_encoder_init(mode);
    rate = 8000;
    chan = 1;
    fmt = 16;
    quality = -1;
    vbr_quality = -1;
    lsb = 1;
    tmp = 1;
    highpass_enabled = true;
    output_rate = 0;
    lookahead = 0;
    bitrate = 0;
    complexity = 3;
    nframes = 1;
    speex_init_header(&header, rate, 1, mode);
    header.frames_per_packet = nframes;
    header.vbr = vbr_enabled;
    header.nb_channels = chan;
    speex_encoder_ctl(st, SPEEX_GET_FRAME_SIZE, &frame_size);
    speex_encoder_ctl(st, SPEEX_SET_COMPLEXITY, &complexity);
    speex_encoder_ctl(st, SPEEX_SET_SAMPLING_RATE, &rate);
    if (quality >= 0) {
      if (vbr_enabled) {
        if (vbr_max > 0) {
          speex_encoder_ctl(st, SPEEX_SET_VBR_MAX_BITRATE, &vbr_max);
        }
        speex_encoder_ctl(st, SPEEX_SET_VBR_QUALITY, &vbr_quality);
      }
      else {
        speex_encoder_ctl(st, SPEEX_SET_QUALITY, &quality);
      }
    }
    if (bitrate) {
      if (quality >= 0 && vbr_enabled)
        fprintf (stderr, "Warning: --bitrate option is overriding --quality\n");
      speex_encoder_ctl(st, SPEEX_SET_BITRATE, &bitrate);
    }
    if (vbr_enabled) {
      tmp = 1;
      speex_encoder_ctl(st, SPEEX_SET_VBR, &tmp);
    }
    else if (vad_enabled) {
      tmp = 1;
      speex_encoder_ctl(st, SPEEX_SET_VAD, &tmp);
    }
    if (dtx_enabled) {
      speex_encoder_ctl(st, SPEEX_SET_DTX, &tmp);
    }
    if (dtx_enabled && !(vbr_enabled || abr_enabled || vad_enabled)) {
      qWarning() << "Warning: --dtx is useless without --vad, --vbr or --abr";
    }
    else if ((vbr_enabled || abr_enabled) && (vad_enabled)) {
      qWarning() << "Warning: --vad is already implied by --vbr or --abr";
    }
    if (abr_enabled) {
       speex_encoder_ctl(st, SPEEX_SET_ABR, &abr_enabled);
    }
    speex_encoder_ctl(st, SPEEX_SET_HIGHPASS, &highpass_enabled);
    speex_encoder_ctl(st, SPEEX_GET_LOOKAHEAD, &lookahead);
    speex_bits_init(&bits);
  }

  void deinit_speex(void)
  {
    speex_encoder_destroy(st);
    speex_bits_destroy(&bits);
  }
};


AudioDecoder::AudioDecoder(const QAudioFormat &format, QMutex *mutex, QObject *parent)
  : QIODevice(parent)
  , d_ptr(new AudioDecoderPrivate(format, mutex))
{
  Q_D(AudioDecoder);
  d->audioFile.setFileName("D:\\Temp\\mic.spx");
  d->audioFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}


AudioDecoder::~AudioDecoder()
{
  Q_D(AudioDecoder);
  d->audioFile.close();
  stop();
}


void AudioDecoder::start(void)
{
  open(QIODevice::WriteOnly);
}


void AudioDecoder::stop(void)
{
  close();
}


qreal AudioDecoder::level(void) const
{
  return d_ptr->level;
}


const SampleBufferType &AudioDecoder::sampleBuffer(void) const
{
  return d_ptr->sampleBuffer;
}


qint64 AudioDecoder::readData(char *data, qint64 maxlen)
{
  Q_UNUSED(data)
  Q_UNUSED(maxlen)
  return 0;
}


qint64 AudioDecoder::writeData(const char *data, qint64 len)
{
  Q_UNUSED(data)
  Q_UNUSED(len)
  return 0;
}
