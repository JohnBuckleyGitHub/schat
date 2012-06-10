/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SENDFILESOCKET_H_
#define SENDFILESOCKET_H_

#include <QTcpSocket>

class QBasicTimer;
class QFile;

namespace SendFile {

class Socket : public QTcpSocket
{
  Q_OBJECT

public:
  /// Режим работы сокета.
  enum Mode {
    UnknownMode,     ///< Не известный режим.
    DiscoveringMode, ///< Режим поиска удалённой стороны.
    HandshakeMode,   ///< Ожидание рукопожатия.
    DataMode,        ///< Режим передачи данных.
    FinishedMode     ///< Завершена передача файла.
  };

  Socket(QObject *parent = 0);
  Socket(const QString& host, quint16 port, const QByteArray &id, QObject *parent = 0);
  ~Socket();
  inline const QByteArray& id() const { return m_id; }
  inline Mode mode() const            { return m_mode; }
  void accept(char code = 'A');
  void leave(bool remove = false);
  void reject();
  void setFile(int role, QFile *file, qint64 size);

signals:
  void accepted();
  void finished();
  void handshake(const QByteArray &id);
  void progress(qint64 current, qint64 total);

protected:
  void timerEvent(QTimerEvent *event);

private slots:
  void connected();
  void disconnected();
  void discovery();
  void error(QAbstractSocket::SocketError socketError);
  void readyRead();

private:
  void init();
  void progress(qint64 pos);
  void readPacket();
  void setMode(Mode mode);

  bool m_release;          ///< true если сокет находится в состоянии закрытия.
  int m_role;              ///< Роль сокета, отправитель или получатель файла.
  Mode m_mode;             ///< Режим работы сокета.
  QBasicTimer *m_timer;    ///< Таймер обслуживающий соединение.
  QByteArray m_id;         ///< Идентификатор транзакции.
  QFile *m_file;           ///< Файл для записи или чтения.
  qint64 m_size;           ///< Размер файла.
  QString m_host;          ///< Адрес удалённой стороны.
  quint16 m_port;          ///< Порт удалённой стороны.
  quint32 m_nextBlockSize; ///< Размер следующего блока данных.
};

} // namespace SendFile

#endif /* SENDFILESOCKET_H_ */
