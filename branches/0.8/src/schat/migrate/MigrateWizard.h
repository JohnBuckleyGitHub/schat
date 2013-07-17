/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef MIGRATEWIZARD_H_
#define MIGRATEWIZARD_H_

#include <QVariant>
#include <QWizard>

class Migrate;

class MigrateWizard : public QWizard
{
  Q_OBJECT

public:
  enum Pages {
    PageIntro,
    PageManual,
    PagePrepare,
    PageProgress
  };

  MigrateWizard(const QString &data, QWidget *parent = 0);
  static QString bytesToHuman(qint64 size);

signals:
  void closeChat();

private:
  Migrate *m_migrate; ///< Класс для проверки и загрузки обновления.
  QVariantMap m_data; ///< Информация полученная от сервера.
};

#endif /* MIGRATEWIZARD_H_ */