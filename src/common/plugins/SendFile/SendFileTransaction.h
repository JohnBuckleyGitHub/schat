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
  SenderRole,
  ReceiverRole
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
 * Содержит информацию о передаче об одиночной передаче файлов.
 */
class Transaction
{
public:
  Transaction(const QByteArray &dest, const QString &file);
  bool addLocalFile(const QString &name);
  bool isValid() const;
  inline const File& file() const     { return m_file; }
  inline const QByteArray& id() const { return m_id; }
  QString fileName() const;
  QVariantMap toReceiver() const;

private:
  QByteArray m_id;     ///< Идентификатор транзакции.
  QByteArray m_user;   ///< Идентификатор отправителя или получателя в зависимости от роли.
  File m_file;         ///< Список файлов.
  Role m_role;         ///< Роль класса.
};

} // namespace SendFile

typedef QSharedPointer<SendFile::Transaction> SendFileTransaction;

#endif /* SENDFILETRANSACTION_H_ */
