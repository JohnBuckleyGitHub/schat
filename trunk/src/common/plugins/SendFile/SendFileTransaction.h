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

#ifndef SENDFILETRANSACTION_H_
#define SENDFILETRANSACTION_H_

#include <QSharedPointer>
#include <QString>

namespace SendFile {

/// Определяет роль класса Transaction.
enum Role {
  SenderRole,  ///< Отправитель.
  ReceiverRole ///< Получатель.
};


/// Тип адреса.
enum AddressType {
  External,
  Internal
};


/*!
 * Информация о файле.
 */
struct File
{
  File()
  : size(0)
  {}

  File(const QString &name, qint64 size)
  : name(name)
  , size(size)
  {}

  QString name; ///< Имя файла.
  qint64 size;  ///< Размер файла.
};


/*!
 * Информация о внешнем и внутреннем адресе и порте.
 */
class Hosts
{
public:
  inline Hosts()
  : m_externalPort(0)
  , m_internalPort(0)
  {}

  inline Hosts(const QString &externalAddress, quint16 externalPort, const QString &internalAddress, quint16 internalPort)
  : m_externalAddress(externalAddress)
  , m_internalAddress(internalAddress)
  , m_externalPort(externalPort)
  , m_internalPort(internalPort)
  {}

  Hosts(const QVariantList &json);

  bool isValid() const;
  inline const QString& address(AddressType type = External) const { if (type) return m_internalAddress; return m_externalAddress; }
  inline quint16 port(AddressType type = External) const           { if (type) return m_internalPort; return m_externalPort; }
  QVariantList toJSON() const;

private:
  QString m_externalAddress; ///< Внешний адрес.
  QString m_internalAddress; ///< Внутренний адрес.
  quint16 m_externalPort;    ///< Внешний порт.
  quint16 m_internalPort;    ///< Внутренний порт.
};


/*!
 * Содержит информацию о передаче файла.
 */
class Transaction
{
public:
  Transaction(const QByteArray &dest, const QByteArray &id, const QString &file);
  Transaction(const QByteArray &sender, const QByteArray &id, const QVariantMap &data);
  bool addLocalFile(const QString &name);
  bool isValid() const;
  inline const File& file() const           { return m_file; }
  inline const Hosts& local() const         { return m_local; }
  inline const Hosts& remote() const        { return m_remote; }
  inline const QByteArray& id() const       { return m_id; }
  inline const QByteArray& user() const     { return m_user; }
  inline Role role() const                  { return m_role; }
  inline void setLocal(const Hosts &hosts)  { m_local = hosts; }
  inline void setRemote(const Hosts &hosts) { m_remote = hosts; }
  QString fileName() const;
  QVariantMap toReceiver() const;

private:
  File m_file;         ///< Список файлов.
  Hosts m_local;       ///< Локальные адреса и порты.
  Hosts m_remote;      ///< Удалёные адреса и порты.
  QByteArray m_id;     ///< Идентификатор транзакции.
  QByteArray m_user;   ///< Идентификатор отправителя или получателя в зависимости от роли.
  Role m_role;         ///< Роль класса.
};

} // namespace SendFile

typedef QSharedPointer<SendFile::Transaction> SendFileTransaction;

#endif /* SENDFILETRANSACTION_H_ */
