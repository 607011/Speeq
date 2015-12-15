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

#ifndef __UDPCLIENT_H_
#define __UDPCLIENT_H_

#include <QObject>
#include <QScopedPointer>

class UdpClientPrivate;

class UdpClient : public QObject
{
  Q_OBJECT
public:
  explicit UdpClient(QObject *parent = Q_NULLPTR);
  ~UdpClient();

private slots:

signals:

public slots:

private:
  static const int DefaultPort = 53548;

  QScopedPointer<UdpClientPrivate> d_ptr;
  Q_DECLARE_PRIVATE(UdpClient)
  Q_DISABLE_COPY(UdpClient)
};

#endif // __UDPCLIENT_H_
