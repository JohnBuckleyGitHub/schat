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

#include <QCheckBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>

#include "languagebox.h"
#include "profilewidget.h"
#include "settings.h"
#include "simplechatapp.h"
#include "translation.h"
#include "widget/networkwidget.h"
#include "widget/welcome.h"

/*!
 * Конструктор класса WelcomeWidget.
 */
WelcomeWidget::WelcomeWidget(QWidget *parent)
  : TranslateWidget(parent)
{
  m_profile = new ProfileWidget(false, this);
  connect(m_profile, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));

  m_language = new LanguageBox(SimpleChatApp::instance()->translation(), this);
  connect(m_language, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(languageChanged(const QString &)));

  m_network = new NetworkWidget(this, NetworkWidget::NetworkLabel);
  m_connect = new QPushButton(QIcon(":/images/dialog-ok.png"), "", this);
  m_connect->setDefault(true);
  connect(m_connect, SIGNAL(clicked()), this, SLOT(link()));

  m_ask = new QCheckBox(this);
  if (SimpleSettings->getBool("FirstRun"))
    m_ask->setChecked(true);

  m_grid = new QGridLayout(this);
  m_grid->addWidget(m_profile, 0, 0, 1, 2);
  m_grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2, 1, 1);
  m_grid->addWidget(m_language, 0, 3, 1, 1, Qt::AlignTop);
  m_grid->addWidget(m_network, 1, 0, 1 ,2);
  m_grid->addWidget(m_connect, 2, 1);
  m_grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 1, 1, 3);
  m_grid->addWidget(m_ask, 4, 0, 1, 4);
  m_grid->setColumnStretch(0, 1);
  m_grid->setColumnStretch(2, 2);

  retranslateUi();
}


void WelcomeWidget::notify(int code)
{
  if (code == Settings::HideWelcomeChanged) {
    m_ask->setChecked(SimpleSettings->getBool("HideWelcome"));
  }
  else if (code == Settings::ProfileSettingsChanged) {
    m_profile->reload();
  }
}


void WelcomeWidget::keyPressEvent(QKeyEvent *event)
{
  QKeySequence seq = event->key() + event->modifiers();
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    link();
  }
  else
    QWidget::keyPressEvent(event);
}


void WelcomeWidget::languageChanged(const QString &text)
{
  Q_UNUSED(text)

  if (m_language->save()) {
    SimpleSettings->setString("Translation", SimpleChatApp::instance()->translation()->name());
  }
}


void WelcomeWidget::link()
{
  m_profile->save();
  m_network->save(false);
  SimpleSettings->setBool("HideWelcome", m_ask->isChecked());
  SimpleSettings->notify(Settings::ServerChanged);
}


void WelcomeWidget::validNick(bool valid)
{
  m_connect->setEnabled(valid);
}


void WelcomeWidget::retranslateUi()
{
  m_connect->setText(tr("Connect"));
  m_ask->setText(tr("Always use this connection"));
  m_language->setCurrentIndex(m_language->findText(CURRENT_LANG));
}
