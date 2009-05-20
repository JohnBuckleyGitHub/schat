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

/*!
 * Конструктор класса NickEdit.
 */
NickEdit::NickEdit(const QString &nick, QWidget *parent)
  : QWidget(parent)
{
  m_edit = new QLineEdit(nick, this);
  m_edit->setMaxLength(AbstractProfile::MaxNickLength);
  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_edit);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

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

  emit validNick(valid);
  m_edit->setPalette(pal);
}
