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

#include "udpclient.h"

#include <QDebug>
#include <QTextStream>
#include <QHostAddress>
#include <QByteArray>
#include <QUdpSocket>

class UdpClientPrivate {
public:
  UdpClientPrivate(void)
    : udpSocket(Q_NULLPTR)
  { /* ... */ }
  ~UdpClientPrivate(void) { /* ... */ }
  QUdpSocket *udpSocket;
};


UdpClient::UdpClient(QObject *parent)
  : QObject(parent)
  , d_ptr(new UdpClientPrivate)
{
  // Q_D(UdpClient);
}


UdpClient::~UdpClient()
{
  d_ptr->udpSocket->close();
}
