/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * This file based on "kopetechatwindowstyle.h" - A Chat Window Style.
 * Copyright © 2005      by Michaël Larouche      <larouche@kde.org>
 * Kopete    © 2002-2008 by the Kopete developers <kopete-devel@kde.org>
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

#ifndef CHATWINDOWSTYLE_H_
#define CHATWINDOWSTYLE_H_

#include <QHash>

/*!
 * \brief This class represent a single chat window style.
 *
 * \author Michaël Larouche <larouche@kde.org>
 */
class ChatWindowStyle
{
public:
  /*!
   * StyleVariants is a typedef to a QHash
   * key = Variant Name
   * value = Path to variant CSS file.
   * Path is relative to Resources directory.
   */
  typedef QHash<QString, QString> StyleVariants;

  explicit ChatWindowStyle(const QString &styleName);
  ~ChatWindowStyle();

  bool hasActionTemplate() const;
  QString styleBaseHref() const;
  QString styleName() const;
  static bool isValid(const QString &style);
  static StyleVariants variants(const QString &variantsPath);
  StyleVariants variants();
  void reload();

  QString templateHtml() const;
  QString headerHtml() const;
  QString footerHtml() const;
  QString incomingHtml() const;
  QString nextIncomingHtml() const;
  QString outgoingHtml() const;
  QString nextOutgoingHtml() const;
  QString statusHtml() const;
  QString actionIncomingHtml() const;
  QString actionOutgoingHtml() const;
  QString fileTransferIncomingHtml() const;
  QString outgoingStateSendingHtml() const;
  QString outgoingStateSentHtml() const;
  QString outgoingStateErrorHtml() const;
  QString outgoingStateUnknownHtml() const;

private:
  bool readStyleFile(QString &out, const QString &fileName, bool failBack = true) const;
  void init(const QString &styleName);
  void listVariants();
  void readStyleFiles();

  class Private;
  Private * const d;
};

#endif /*CHATWINDOWSTYLE_H_*/
