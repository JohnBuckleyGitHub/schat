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

#include <QLabel>
#include <QVBoxLayout>
#include <QSpinBox>

#include "SendFilePage.h"
#include "sglobal.h"
#include "ChatCore.h"
#include "ChatSettings.h"

SendFilePage::SendFilePage(SendFilePluginImpl *plugin, QWidget *parent)
  : SettingsPage(QIcon(LS(":/images/sendfile/attach.png")), LS("sendfile"), parent)
  , m_plugin(plugin)
{
  m_label = new QLabel(this);

  m_portLabel = new QLabel(this);
  m_port = new QSpinBox(this);
  m_port->setButtonSymbols(QSpinBox::NoButtons);
  m_port->setRange(1, 65536);
  m_port->setPrefix(LS(" "));
  m_port->setSuffix(LS(" "));
  m_port->setValue(SCHAT_OPTION(LS("SendFile/Port")).toInt());
  m_portLabel2 = new QLabel(this);

  QHBoxLayout *portLay = new QHBoxLayout();
  portLay->addWidget(m_portLabel);
  portLay->addWidget(m_port);
  portLay->addWidget(m_portLabel2);
  portLay->addStretch();
  portLay->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addLayout(portLay);
  mainLay->addStretch();

  retranslateUi();

  connect(m_port, SIGNAL(valueChanged(int)), SLOT(portChanged(int)));
}


void SendFilePage::retranslateUi()
{
  m_name = tr("File transfer");
  m_portLabel->setText(tr("Use port"));
  m_portLabel2->setText(tr("for file transfer"));
  m_label->setText(LS("<b>") + m_name + LS("</b>"));
}


void SendFilePage::portChanged(int port)
{
  ChatCore::settings()->setValue(LS("SendFile/Port"), port);
}


SettingsPage* SendFilePageCreator::page(QWidget *parent)
{
  return new SendFilePage(m_plugin, parent);
}
