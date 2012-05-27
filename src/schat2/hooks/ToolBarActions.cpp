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

#include "hooks/ToolBarActions.h"
#include "ui/InputWidget.h"
#include "sglobal.h"
#include "ui/SendWidget.h"
#include "ui/ColorButton.h"

TextEditAction::TextEditAction(int action)
  : ToolBarActionCreator(1000 + action * 10)
  , m_inputAction(action)
{
  switch (action) {
    case InputWidget::Bold:
      m_name = LS("bold");
      break;

    case InputWidget::Italic:
      m_name = LS("italic");
      break;

    case InputWidget::Underline:
      m_name = LS("underline");
      break;

    case InputWidget::Strike:
      m_name = LS("strike");
      break;

    default:
      break;
  }
}


QAction* TextEditAction::createAction(QObject *parent) const
{
  Q_UNUSED(parent)

  if (m_name.isEmpty())
    return 0;

  return SendWidget::i()->input()->action(static_cast<InputWidget::Actions>(m_inputAction));
}


ColorAction::ColorAction()
  : ToolBarActionCreator(2000, LS("color"))
{
  m_type = Widget;
}


QWidget* ColorAction::createWidget(QWidget *parent) const
{
  Q_UNUSED(parent)
  return SendWidget::i()->input()->color();
}


StretchAction::StretchAction()
  : ToolBarActionCreator(10000, LS("stretch"))
{
  m_type = Widget;
}


QWidget* StretchAction::createWidget(QWidget *parent) const
{
  QWidget *stretch = new QWidget(parent);
  stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  return stretch;
}
