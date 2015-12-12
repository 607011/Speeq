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


#include "waverenderarea.h"
#include "audioencoder.h"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QLineF>
#include <QMutexLocker>
#include <QDateTime>



class WaveRenderAreaPrivate
{
public:
  explicit WaveRenderAreaPrivate(QMutex *mutex)
    : maxAmplitude(-1)
    , sampleBufferMutex(mutex)
  {
    Q_ASSERT(mutex != Q_NULLPTR);
  }
  ~WaveRenderAreaPrivate() { /* ... */ }
  QAudioFormat audioFormat;
  QPixmap pixmap;
  quint32 maxAmplitude;
  QMutex *sampleBufferMutex;
  SampleBufferType sampleBuffer;

private:
};


WaveRenderArea::WaveRenderArea(QMutex *mutex, QWidget *parent)
  : QWidget(parent)
  , d_ptr(new WaveRenderAreaPrivate(mutex))
{
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setMinimumHeight(56);
}


WaveRenderArea::~WaveRenderArea() { /* ... */ }


QSize WaveRenderArea::sizeHint(void) const
{
 return QSize(512, 96);
}


void WaveRenderArea::paintEvent(QPaintEvent *)
{
  Q_D(WaveRenderArea);
  QPainter p(this);
  QMutexLocker(d->sampleBufferMutex);
  p.drawPixmap(0, 0, d->pixmap);
}


void WaveRenderArea::resizeEvent(QResizeEvent *e)
{
  Q_D(WaveRenderArea);
  QMutexLocker(d->sampleBufferMutex);
  d->pixmap = QPixmap(e->size());
  drawPixmap();
}


void WaveRenderArea::mousePressEvent(QMouseEvent *)
{
  // ...
}


void WaveRenderArea::mouseReleaseEvent(QMouseEvent *)
{
  // ...
}


void WaveRenderArea::mouseMoveEvent(QMouseEvent *)
{
  // ...
}


void WaveRenderArea::drawPixmap(void)
{
  Q_D(WaveRenderArea);
  if (!d->pixmap.isNull()) {
    QPainter p(&d->pixmap);
    static const QColor BackgroundColor(17, 33, 17);
    p.fillRect(d->pixmap.rect(), BackgroundColor);
    if (d->audioFormat.isValid() && d->maxAmplitude > 0 && !d->sampleBuffer.isEmpty()) {
      const int halfHeight = d->pixmap.height() / 2;
      QPointF origin(0, halfHeight);
      QLineF waveLine(origin, origin);
      const qreal xd = qreal(d->pixmap.width()) / d->sampleBuffer.size();
      qreal x = 0.0;
      for (int i = 0; i < d->sampleBuffer.size(); ++i) {
        p.setRenderHint(QPainter::Antialiasing, true);
        const qreal y = qreal(d->sampleBuffer.at(i)) / d->maxAmplitude;
        waveLine.setP2(QPointF(x, halfHeight - y * halfHeight));
        static const QPen WaveLinePen(QBrush(QColor(54, 255, 54)), 0.5);
        p.setPen(WaveLinePen);
        p.drawLine(waveLine);
        waveLine.setP1(waveLine.p2());
        x += xd;
      }
    }
    update();
  }
}


void WaveRenderArea::compress(void)
{
  // Q_D(WaveRenderArea);
}


void WaveRenderArea::setData(const SampleBufferType &data)
{
  Q_D(WaveRenderArea);
  QMutexLocker(d->sampleBufferMutex);
  d->sampleBuffer = data;
  compress();
  drawPixmap();
}


void WaveRenderArea::setAudioFormat(const QAudioFormat &format)
{
  Q_D(WaveRenderArea);
  d->audioFormat = format;
  d->maxAmplitude = AudioEncoder::maxAmplitudeForFormat(d->audioFormat);
}
