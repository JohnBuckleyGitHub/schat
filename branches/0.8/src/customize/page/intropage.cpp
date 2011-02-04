/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include "intropage.h"

/*!
 * \brief Конструктор класса IntroPage.
 */
IntroPage::IntroPage(QWidget *parent)
  : QWizardPage(parent)
{
  setTitle(tr("Welcome to the distribution package wizard for your network"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

  QLabel *introLabel = new QLabel(tr(
      "<p>This wizard will create a distribution package, adapted to your network,"
      " and create a local mirror for updates.</p>") +
      tr("For detailed information, please, refer to the <a href='http://simple.impomezia.com/Customize' style='text-decoration:none; color:#1a4d82;'>documentation</a>.") +
      tr("<p>Click <b>&quot;Next&quot;</b> to continue.</p>"), this);
  introLabel->setWordWrap(true);
  introLabel->setOpenExternalLinks(true);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(introLabel);
  mainLay->addStretch();
}
