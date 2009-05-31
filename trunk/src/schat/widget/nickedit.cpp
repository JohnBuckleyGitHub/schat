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
  m_male(true),
  m_maxRecentItems(SimpleSettings->getInt("Profile/MaxRecentItems")),
  m_applyButton(0),
  m_genderButton(0)
{
  m_edit = new QLineEdit(this);
  m_edit->setMaxLength(AbstractProfile::MaxNickLength);
  initCompleter();

  m_mainLay = new QHBoxLayout(this);
  m_mainLay->addWidget(m_edit);

  if (options & GenderButton) {
    QMenu *menu = new QMenu(this);
    menu->addAction(QIcon(":/images/male.png"),   tr("Мужской"), this, SLOT(genderChange()))->setData(true);
    menu->addAction(QIcon(":/images/female.png"), tr("Женский"), this, SLOT(genderChange()))->setData(false);

    m_genderButton = new QToolButton(this);
    m_genderButton->setToolTip(tr("Пол"));
    m_genderButton->setAutoRaise(true);
    m_genderButton->setPopupMode(QToolButton::InstantPopup);
    m_genderButton->setMenu(menu);
    m_mainLay->addWidget(m_genderButton);
  }

  if (options & ApplyButton) {
    m_applyButton = new QToolButton(this);
    m_applyButton->setIcon(QIcon(":/images/dialog-ok.png"));
    m_applyButton->setToolTip(tr("Применить"));
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
 * Модификация списка для автодополнения введённых данных.
 *
 * \param key    Ключ настроек.
 * \param value  Новое значение для добавления в список.
 * \param remove При \a false значение будет добавлено только если, в исходном списке оно отсутствует.
 */
void NickEdit::modifyRecentList(const QString &key, const QString &value, bool remove)
{
  int maxSize = SimpleSettings->getInt("Profile/MaxRecentItems");
  if (maxSize < 1 || value.isEmpty())
    return;

  QStringList recentList = SimpleSettings->getList(key);
  bool contains = recentList.contains(value);
  if (contains && remove)
    recentList.removeAll(value);

  if (!(!remove && contains))
    recentList.prepend(value);

  while (recentList.size() > maxSize)
    recentList.removeLast();

  SimpleSettings->setList(key, recentList);
}


/*!
 * Сброс введённых данных на стандартные значения.
 */
void NickEdit::reset()
{
  m_edit->setText(AbstractProfile::defaultNick());
  if (m_genderButton)
    setGender(0);
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

  if (SimpleSettings->profile()->isMale() != isMale()) {
    SimpleSettings->profile()->setGender(isMale());
    modified++;
  }

  if (notify && modified)
    SimpleSettings->notify(Settings::ProfileSettingsChanged);

  if (m_applyButton) {
    QMenu *popup = qobject_cast<QMenu *>(parentWidget());
    if (isVisible() && popup)
      popup->close();
  }

  if (m_maxRecentItems && modified)
    modifyRecentList("Profile/RecentNicks", nick());

  return modified;
}


/*!
 * Переопределение нажатие на \b Enter, при использовании
 * опции NickEdit::ApplyButton это вызовет применение введённых настроек.
 */
void NickEdit::keyPressEvent(QKeyEvent *event)
{
  if (m_applyButton && m_applyButton->isEnabled() && event->key() == Qt::Key_Return)
    save();
  else
    QWidget::keyPressEvent(event);
}

/*!
 * Обработка события показа виджета.
 * В поле редактирования устанавливается текущий ник.
 */
void NickEdit::showEvent(QShowEvent * /*event*/)
{
  m_edit->setText(SimpleSettings->profile()->nick());
  int editHeight = m_edit->height();
  if (m_genderButton) {
    m_genderButton->setMaximumSize(editHeight, editHeight);
    setMale(SimpleSettings->profile()->isMale());
  }

  if (m_applyButton)
    m_applyButton->setMaximumSize(editHeight, editHeight);

  if (m_maxRecentItems)
    m_model->setStringList(SimpleSettings->getList("Profile/RecentNicks"));
}


/*!
 * Обработка изменения пола через кнопку встроенную в виджет.
 */
void NickEdit::genderChange()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    setMale(action->data().toBool());
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


void NickEdit::initCompleter()
{
  if (m_maxRecentItems) {
    QCompleter *completer = new QCompleter(m_edit);
    m_edit->setCompleter(completer);

    m_model = new QStringListModel(this);
    completer->setModel(m_model);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
  }
}


/*!
 * Установка пола.
 *
 * \param male \a true если пол мужской.
 */
void NickEdit::setMale(bool male)
{
  if (m_genderButton) {
    if (male)
      m_genderButton->setIcon(QIcon(":/images/male.png"));
    else
      m_genderButton->setIcon(QIcon(":/images/female.png"));
  }

  m_male = male;
}
