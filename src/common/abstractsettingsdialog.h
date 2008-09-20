/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABSTRACTSETTINGSDIALOG_H_
#define ABSTRACTSETTINGSDIALOG_H_

#include <QDialog>

class AbstractSettingsPage;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;

/*!
 * \brief Абстрактный базовый класс диалога настроек.
 */
class AbstractSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  AbstractSettingsDialog(QWidget *parent = 0);
  void setPage(int page = 0);

signals:
  void reset(int page);
  void save();

public slots:
  void accept();
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  void reset();

protected:
  void createPage(const QIcon &icon, const QString &text, AbstractSettingsPage *page);

  QListWidget *m_contentsWidget;
  QPushButton *m_cancelButton;
  QPushButton *m_okButton;
  QPushButton *m_resetButton;
  QStackedWidget *m_pagesWidget;
};

#endif /*ABSTRACTSETTINGSDIALOG_H_*/
