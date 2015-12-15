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


#include "audioencoder.h"
#include <QDebug>
#include <QtEndian>
#include <QFile>
#include <QVector>

#include <speex/speex.h>
#include <speex/speex_header.h>

static const int MAX_FRAME_SIZE = 2000;
static const int MAX_FRAME_BYTES = 2000;

class AudioEncoderPrivate {
public:
  AudioEncoderPrivate(const QAudioFormat &format, QMutex *mutex)
    : format(format)
    , maxAmplitude(AudioEncoder::maxAmplitudeForFormat(format))
    , level(0.0)
    , sampleBufferMutex(mutex)
  {
    init_speex();
  }
  ~AudioEncoderPrivate()
  {
    deinit_speex();
  }
  const QAudioFormat format;
  int maxAmplitude;
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
    vbr_enabled = true;
    vbr_quality = -1;
    vbr_max = 0;
    abr_enabled = false;
    vad_enabled = false;
    dtx_enabled = false;
    print_bitrate = 0;
    modeID = SPEEX_MODEID_NB;
    mode = speex_lib_get_mode(modeID);
    st = speex_encoder_init(mode);
    rate = 8000;
    chan = 1;
    fmt = 16;
    quality = -1;
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
    speex_bits_destroy(&bits);
    speex_encoder_destroy(st);
  }
};


AudioEncoder::AudioEncoder(const QAudioFormat &format, QMutex *mutex, QObject *parent)
  : QIODevice(parent)
  , d_ptr(new AudioEncoderPrivate(format, mutex))
{
  Q_D(AudioEncoder);
  d->audioFile.setFileName("D:\\Temp\\mic.spx");
  d->audioFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}


AudioEncoder::~AudioEncoder()
{
  Q_D(AudioEncoder);
  d->audioFile.close();
  stop();
}


int AudioEncoder::maxAmplitudeForFormat(const QAudioFormat &format)
{
  int maxAmplitude;
  switch (format.sampleSize()) {
  case 8:
    switch (format.sampleType()) {
    case QAudioFormat::UnSignedInt:
      maxAmplitude = 255;
      break;
    case QAudioFormat::SignedInt:
      maxAmplitude = 127;
      break;
    default:
      break;
    }
    break;
  case 16:
    switch (format.sampleType()) {
    case QAudioFormat::UnSignedInt:
      maxAmplitude = 65535;
      break;
    case QAudioFormat::SignedInt:
      maxAmplitude = 32767;
      break;
    default:
      break;
    }
    break;
  case 32:
    switch (format.sampleType()) {
    case QAudioFormat::UnSignedInt:
      maxAmplitude = 0xffffffff;
      break;
    case QAudioFormat::SignedInt:
      maxAmplitude = 0x7fffffff;
      break;
    case QAudioFormat::Float:
      maxAmplitude = 0x7fffffff; // Kind of
    default:
      break;
    }
    break;
  default:
    break;
  }
  return maxAmplitude;
}


void AudioEncoder::start(void)
{
  open(QIODevice::WriteOnly);
}


void AudioEncoder::stop(void)
{
  close();
}


qreal AudioEncoder::level(void) const
{
  return d_ptr->level;
}


int AudioEncoder::maxAmplitude(void) const
{
  return d_ptr->maxAmplitude;
}


const SampleBufferType &AudioEncoder::sampleBuffer(void) const
{
  return d_ptr->sampleBuffer;
}


qint64 AudioEncoder::readData(char *data, qint64 maxlen)
{
  Q_UNUSED(data)
  Q_UNUSED(maxlen)
  return 0;
}


qint64 AudioEncoder::writeData(const char *data, qint64 len)
{
  Q_D(AudioEncoder);
  d->sampleBufferMutex->lock();
  if (d->maxAmplitude != 0) {
    Q_ASSERT(d->format.sampleSize() % 8 == 0);
    const int channelBytes = d->format.sampleSize() / 8;
    const int sampleBytes = d->format.channelCount() * channelBytes;
    Q_ASSERT(len % sampleBytes == 0);
    const int nSamples = len / sampleBytes;
    d->sampleBuffer.resize(nSamples);
    int maxValue = 0;
    const uchar *ptr = reinterpret_cast<const uchar *>(data);
    for (int i = 0; i < nSamples; ++i) {
      for (int j = 0; j < d->format.channelCount(); ++j) {
        int value = 0;
        if (d->format.sampleSize() == 8 && d->format.sampleType() == QAudioFormat::UnSignedInt) {
          value = *reinterpret_cast<const quint8*>(ptr);
        }
        else if (d->format.sampleSize() == 8 && d->format.sampleType() == QAudioFormat::SignedInt) {
          value = *reinterpret_cast<const qint8*>(ptr);
        }
        else if (d->format.sampleSize() == 16 && d->format.sampleType() == QAudioFormat::UnSignedInt) {
          if (d->format.byteOrder() == QAudioFormat::LittleEndian) {
            value = qFromLittleEndian<quint16>(ptr);
          }
          else {
            value = qFromBigEndian<quint16>(ptr);
          }
        }
        else if (d->format.sampleSize() == 16 && d->format.sampleType() == QAudioFormat::SignedInt) {
          if (d->format.byteOrder() == QAudioFormat::LittleEndian) {
            value = qFromLittleEndian<qint16>(ptr);
          }
          else {
            value = qFromBigEndian<qint16>(ptr);
          }
        }
        else if (d->format.sampleSize() == 32 && d->format.sampleType() == QAudioFormat::UnSignedInt) {
          if (d->format.byteOrder() == QAudioFormat::LittleEndian) {
            value = qFromLittleEndian<quint32>(ptr);
          }
          else {
            value = qFromBigEndian<quint32>(ptr);
          }
        }
        else if (d->format.sampleSize() == 32 && d->format.sampleType() == QAudioFormat::SignedInt) {
          if (d->format.byteOrder() == QAudioFormat::LittleEndian) {
            value = qFromLittleEndian<qint32>(ptr);
          }
          else {
            value = qFromBigEndian<qint32>(ptr);
          }
        }
        else if (d->format.sampleSize() == 32 && d->format.sampleType() == QAudioFormat::Float) {
          value = *reinterpret_cast<const float*>(ptr) * 0x7fffffff; // assumes 0-1.0
        }
        maxValue = qMax(qAbs(value), maxValue);
        ptr += channelBytes;
        d->sampleBuffer[i] = value;
      }
    }
    maxValue = qMin(maxValue, d->maxAmplitude);
    d->level = qreal(maxValue) / d->maxAmplitude;

    speex_bits_reset(&d->bits);
    speex_encode_int(d->st, (spx_int16_t*)data, &d->bits);
    int blockSize = speex_bits_nbytes(&d->bits);
    int tmp;
    speex_encoder_ctl(d->st, SPEEX_GET_BITRATE, &tmp);
    qDebug().nospace() << "bitrate: " << tmp << "bps, blockSize = " << blockSize;
    if (d->audioFile.isOpen()) {
      // speex_bits_insert_terminator(&d->bits);
      d->nbBytes = speex_bits_write(&d->bits, d->cbits, MAX_FRAME_BYTES);
      d->audioFile.write(d->cbits,  d->nbBytes);
    }
  }
  d->sampleBufferMutex->unlock();

  emit update();
  return len;
}
