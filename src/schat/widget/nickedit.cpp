/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "abstractprofile.h"
#include "nickedit.h"
#include "settings.h"

/*!
 * Конструктор класса NickEdit.
 */
NickEdit::NickEdit(QWidget *parent, Options options)
  : QWidget(parent),
  m_applyButton(0),
  m_genderButton(0)
{
  m_edit = new QLineEdit(this);
  m_edit->setMaxLength(AbstractProfile::MaxNickLength);

  m_mainLay = new QHBoxLayout(this);
  m_mainLay->addWidget(m_edit);

  if (options & GenderButton) {
    QMenu *menu = new QMenu(this);
    menu->addAction(QIcon(":/images/male.png"), tr("Мужской"));
    menu->addAction(QIcon(":/images/female.png"), tr("Женский"));

    m_genderButton = new QToolButton(this);
    m_genderButton->setIcon(QIcon(":/images/male.png"));
    m_genderButton->setToolTip(tr("Добавить смайлик"));
    m_genderButton->setAutoRaise(true);
    m_genderButton->setPopupMode(QToolButton::InstantPopup);
    m_genderButton->setMenu(menu);
    m_mainLay->addWidget(m_genderButton);
  }

  if (options & ApplyButton) {
    m_applyButton = new QToolButton(this);
    m_applyButton->setIcon(QIcon(":/images/dialog-ok.png"));
    m_applyButton->setToolTip(tr("Добавить смайлик"));
    m_applyButton->setAutoRaise(true);
    m_applyButton->setPopupMode(QToolButton::InstantPopup);
    m_mainLay->addWidget(m_applyButton);

    connect(m_applyButton, SIGNAL(clicked(bool)), SLOT(save()));
  }
  m_mainLay->setMargin(0);
  m_mainLay->setSpacing(0);

  connect(m_edit, SIGNAL(textChanged(const QString &)), SLOT(validateNick(const QString &)));
}


QString NickEdit::nick() const
{
  return m_edit->text();
}


/*!
 * Сброс введённых данных на стандартные значения.
 */
void NickEdit::reset()
{
  m_edit->setText(QDir::home().dirName());
}


void NickEdit::setMargin(int margin)
{
  m_mainLay->setMargin(margin);
}


/*!
 * Сохранение настроек.
 */
int NickEdit::save(int notify)
{
  int modified = 0;

  if (SimpleSettings->profile()->nick() != nick()) {
    SimpleSettings->profile()->setNick(nick());
    modified++;
  }

  if (notify && modified)
    SimpleSettings->notify(Settings::ProfileSettingsChanged);

  if (m_applyButton) {
    QMenu *popup = qobject_cast<QMenu *>(parentWidget());
    if (isVisible() && popup)
      popup->close();
  }

  return modified;
}


/*!
 * Обработка события показа виджета.
 * В поле редактирования устанавливается текущий ник.
 */
void NickEdit::showEvent(QShowEvent * /*event*/)
{
  m_edit->setText(SimpleSettings->profile()->nick());
  int editHeight = m_edit->height();
  if (m_genderButton)
    m_genderButton->setMaximumSize(editHeight, editHeight);

  if (m_applyButton)
    m_applyButton->setMaximumSize(editHeight, editHeight);
}


/*!
 * Проверка правильности ника, в случае если ник не корректный,
 * то устанавливается красный фон.
 *
 * \todo Необходимо учитывать возможную коллизию с уже присутствующими в чате пользователями.
 */
void NickEdit::validateNick(const QString &text)
{
  QPalette pal = m_edit->palette();
  bool valid = AbstractProfile::isValidNick(text);

  if (valid)
    pal.setColor(QPalette::Active, QPalette::Base, Qt::white);
  else
    pal.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));

  m_edit->setPalette(pal);
  if (m_applyButton) m_applyButton->setEnabled(valid);
  emit validNick(valid);
}
