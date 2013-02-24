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

#include <QLabel>
#include <QVBoxLayout>

#include "migrate/MigrateIntro.h"

MigrateIntro::MigrateIntro(const QString &text, QWidget *parent)
  : QWizardPage(parent)
{
  setTitle(tr("Welcome to the update wizard to Simple Chat 2"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

  QLabel *label = new QLabel(text, this);
  label->setWordWrap(true);
  label->setOpenExternalLinks(true);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(label);
  layout->addStretch();
}
