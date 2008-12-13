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
  setTitle(tr("Вас приветствует мастер создания дистрибутива для Вашей сети"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

  QLabel *introLabel = new QLabel(tr(
      "<p>Нажмите кнопку <b>&quot;Далее&quot;</b> для продолжения.</p>"
      ), this);
  introLabel->setWordWrap(true);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(introLabel);
  mainLay->addStretch();
}
