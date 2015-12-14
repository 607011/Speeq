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

#include "tcpclient.h"

#include <QDebug>
#include <QTextStream>
#include <QHostAddress>
#include <QVariantMap>

class TcpClientPrivate {
public:
  TcpClientPrivate(void)
    : tcpSocket(Q_NULLPTR)
  { /* ... */ }
  ~TcpClientPrivate(void) { /* ... */ }
  QTcpSocket *tcpSocket;
};


TcpClient::TcpClient(QObject *parent)
  : QObject(parent)
  , d_ptr(new TcpClientPrivate)
{
  Q_D(TcpClient);
  d->tcpSocket = new QTcpSocket(this);
  QObject::connect(d->tcpSocket, SIGNAL(readyRead()), this, SLOT(forwardIncomingMessage()));
}


TcpClient::~TcpClient()
{
  d_ptr->tcpSocket->close();
}


void TcpClient::connect(const QHostAddress &host, quint16 port)
{
  Q_D(TcpClient);
  d->tcpSocket->abort();
  d->tcpSocket->connectToHost(host, port);
  if (d->tcpSocket->waitForConnected()) {
    // TODO: connected
  }
  else {
    // TODO: connection timeout
  }
}


void TcpClient::forwardIncomingMessage(void)
{
  Q_D(TcpClient);
  QByteArray data = d->tcpSocket->readAll();
  // emit receivedData(data);
}
