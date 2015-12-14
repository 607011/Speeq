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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "volumerenderarea.h"
#include "waverenderarea.h"
#include "audioencoder.h"
#include "global.h"

#include <QDebug>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QSysInfo>
#include <QVBoxLayout>
#include <QSlider>
#include <QMutex>
#include <QFile>
#include <QSettings>
#include <QElapsedTimer>
#include <QDateTime>
#include <QIcon>
#include <limits>


class MainWindowPrivate {
public:
  MainWindowPrivate(void)
    : startIcon(":/images/start.png")
    , stopIcon(":/images/stop.png")
    , audioDeviceInfo(QAudioDeviceInfo::defaultInputDevice())
    , audio(Q_NULLPTR)
    , audioInput(Q_NULLPTR)
    , volumeRenderArea(Q_NULLPTR)
    , waveRenderArea(Q_NULLPTR)
    , sampleBufferMutex(new QMutex)
    , paused(false)
    , settings(QSettings::IniFormat, QSettings::UserScope, AppCompanyName, AppName)
  {
// #define USE_PREFERRED_AUDIO_FORMAT
#ifdef USE_PREFERRED_AUDIO_FORMAT
    audioFormat = audioDeviceInfo.preferredFormat();
#else
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setSampleSize(16);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
#endif
    qDebug() << audioFormat;

  }
  ~MainWindowPrivate()
  {
    /* ... */
  }
  QIcon startIcon;
  QIcon stopIcon;
  QAudioDeviceInfo audioDeviceInfo;
  QAudioFormat audioFormat;
  QAudioInput *audio;
  AudioEncoder *audioInput;
  VolumeRenderArea *volumeRenderArea;
  WaveRenderArea *waveRenderArea;
  QMutex *sampleBufferMutex;
  bool paused;
  QSettings settings;
};


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , d_ptr(new MainWindowPrivate)
{
  Q_D(MainWindow);
  ui->setupUi(this);
  setWindowIcon(QIcon(":/images/speeq.ico"));
  d->volumeRenderArea = new VolumeRenderArea;
  d->waveRenderArea = new WaveRenderArea(d->sampleBufferMutex);
  d->waveRenderArea->setAudioFormat(d->audioFormat);
  d->audioInput  = new AudioEncoder(d->audioFormat, d->sampleBufferMutex, this);
  QObject::connect(d->audioInput, SIGNAL(update()), SLOT(refreshDisplay()));
  QObject::connect(ui->startStopButton, SIGNAL(clicked(bool)), SLOT(startStop()));
  QObject::connect(ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(onVolumeSliderChanged(int)));
  d->audio = new QAudioInput(d->audioDeviceInfo, d->audioFormat, this);
  ui->volumeSlider->setValue(qRound(100 * d->audio->volume()));
  QObject::connect(d->audio, SIGNAL(stateChanged(QAudio::State)), SLOT(onAudioStateChanged(QAudio::State)));
  ui->graphLayout->addWidget(d->waveRenderArea);
  ui->graphLayout->addWidget(d->volumeRenderArea);
  ui->statusBar->showMessage(d->audioDeviceInfo.deviceName());
  restoreSettings();
  d->audioInput->start();
  d->audio->start(d->audioInput);
  if (!d->paused) {
    start();
  }
}


MainWindow::~MainWindow()
{
  Q_D(MainWindow);
  d->audio->stop();
  d->audioInput->stop();
  delete ui;
}


void MainWindow::closeEvent(QCloseEvent *e)
{
  saveSettings();
  e->accept();
}


void MainWindow::saveSettings(void)
{
  Q_D(MainWindow);
  d->settings.setValue("mainwindow/geometry", saveGeometry());
  d->settings.setValue("mainwindow/paused", d->paused);
  d->settings.sync();
}


void MainWindow::restoreSettings(void)
{
  Q_D(MainWindow);
  restoreGeometry(d->settings.value("mainwindow/geometry").toByteArray());
  d->paused = d->settings.value("mainwindow/paused", false).toBool();
}


void MainWindow::refreshDisplay(void)
{
  Q_D(MainWindow);
  if (!d->paused) {
    d->volumeRenderArea->setLevel(d->audioInput->level());
    d->waveRenderArea->setData(d->audioInput->sampleBuffer());
  }
}


void MainWindow::onVolumeSliderChanged(int value)
{
  Q_D(MainWindow);
  if (d->audio != Q_NULLPTR) {
    d->audio->setVolume(1e-2 * value);
  }
}


void MainWindow::start(void)
{
  Q_D(MainWindow);
  d->paused = false;
  ui->startStopButton->setIcon(d->stopIcon);
}


void MainWindow::stop(void)
{
  Q_D(MainWindow);
  d->paused = true;
  ui->startStopButton->setIcon(d->startIcon);
}


void MainWindow::log(const QString &msg)
{
  ui->logPlainTextEdit->appendPlainText(tr("[%1] %2").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(msg));
}


void MainWindow::startStop(void)
{
  Q_D(MainWindow);
  if (d->paused) {
    start();
  }
  else {
    d->paused = false;
  }
}


void MainWindow::onAudioStateChanged(QAudio::State audioState)
{
  Q_D(MainWindow);
  switch (audioState) {
  case QAudio::StoppedState:
    if (d->audio->error() != QAudio::NoError) {
      // Error handling
    }
    else {
      // Finished recording
    }
    break;
  case QAudio::ActiveState:
    break;
  default:
    break;
  }
}
