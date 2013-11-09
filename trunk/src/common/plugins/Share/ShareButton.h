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

#ifndef SHAREBUTTON_H_
#define SHAREBUTTON_H_

#include <QApplication>
#include <QToolButton>

#include "ui/ToolBarAction.h"

class Share;

class ShareButton : public QToolButton
{
  Q_OBJECT

public:
  ShareButton(Share *share, QWidget *parent = 0);

private:
  Share *m_share;
};


class ShareAction : public ToolBarActionCreator
{
  Q_DECLARE_TR_FUNCTIONS(ShareAction)

public:
  ShareAction(Share *share);
  QWidget* createWidget(QWidget *parent) const;
  inline QIcon icon() const { return m_icon; }
  QString title() const;

private:
  QIcon m_icon;
  Share *m_share;
};

#endif /* SHAREBUTTON_H_ */
