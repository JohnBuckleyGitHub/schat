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

#ifndef MIGRATEPROGRESS_H_
#define MIGRATEPROGRESS_H_

#include <QWizardPage>

class Migrate;
class QLabel;
class QProgressBar;
class QTextEdit;

class MigrateProgress : public QWizardPage
{
  Q_OBJECT

public:
  MigrateProgress(const QString &url, Migrate *migrate, QWidget *parent = 0);
  inline bool isComplete() const { return m_completed; }
  void initializePage();

signals:
  void closeChat();

private slots:
  void done(int status);
  void downloadProgress(qint64 bytesReceived);
  void finished(int exitCode = 1);

private:
  bool m_completed;         ///< \b true если страница выполнила своё действие.
  Migrate *m_migrate;       ///< Класс для проверки и загрузки обновления.
  QLabel *m_label;          ///< Надпись сверху прогресс бара.
  QProgressBar *m_progress; ///< Прогресс бар.
  QString m_url;            ///< Адрес сервера.
  QTextEdit *m_log;         ///< Виджет для показа лога.
};

#endif /* MIGRATEPROGRESS_H_ */
