/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef NICKEDIT_H_
#define NICKEDIT_H_

#include "translatewidget.h"

class QHBoxLayout;
class QLineEdit;
class QStringListModel;
class QToolBar;
class QToolButton;

/*!
 * \brief Обеспечивает редактирование ника и пола пользователя.
 */
class NickEdit : public TranslateWidget
{
  Q_OBJECT

public:
  enum OptionsFlag {
    NoOptions    = 0x0,
    GenderButton = 0x1,
    ApplyButton  = 0x2
  };

  Q_DECLARE_FLAGS(Options, OptionsFlag)

  NickEdit(QWidget *parent = 0, Options options = NoOptions);
  inline bool isMale() const        { return m_male; }
  inline void setGender(int gender) { setMale(!(bool) gender); }
  QString nick() const;
  static void modifyRecentList(const QString &key, const QString &value, bool remove = true);
  void reload();
  void reset();
  void setMargin(int margin);

signals:
  void validNick(bool valid);

public slots:
  int save(bool notify = true);

protected:
  void keyPressEvent(QKeyEvent *event);
  void showEvent(QShowEvent *event);

private slots:
  void genderChange();
  void validateNick(const QString &text);

private:
  void initCompleter();
  void retranslateUi();
  void setMale(bool male);
  void setOptimalSize();

  bool m_male;
  int m_maxRecentItems;
  QAction *m_femaleAction;
  QAction *m_maleAction;
  QHBoxLayout *m_mainLay;
  QLineEdit *m_edit;
  QStringListModel *m_model;
  QToolBar *m_toolBar;
  QToolButton *m_applyButton;
  QToolButton *m_genderButton;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NickEdit::Options)

#endif /* NICKEDIT_H_ */
