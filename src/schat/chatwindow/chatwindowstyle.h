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
  typedef QHash<QString,QString> StyleVariants;

  /*!
   * This enum specifies the mode of the constructor
   * - StyleBuildFast : Build the style the fatest possible
   * - StyleBuildNormal : List all variants of this style. Require a async dir list.
   */
  enum StyleBuildMode { StyleBuildFast, StyleBuildNormal};

  explicit ChatWindowStyle(const QString &styleName, StyleBuildMode styleBuildMode = StyleBuildNormal);
  ChatWindowStyle(const QString &styleName, const QString &variantPath, StyleBuildMode styleBuildMode = StyleBuildFast);
  ~ChatWindowStyle();

  StyleVariants getVariants();
  QString getStyleName() const;
  QString getStyleBaseHref() const;

  QString getTemplateHtml() const;
  QString getHeaderHtml() const;
  QString getFooterHtml() const;
  QString getIncomingHtml() const;
  QString getNextIncomingHtml() const;
  QString getOutgoingHtml() const;
  QString getNextOutgoingHtml() const;
  QString getStatusHtml() const;
  QString getActionIncomingHtml() const;
  QString getActionOutgoingHtml() const;
  QString getFileTransferIncomingHtml() const;
  QString getOutgoingStateSendingHtml() const;
  QString getOutgoingStateSentHtml() const;
  QString getOutgoingStateErrorHtml() const;
  QString getOutgoingStateUnknownHtml() const;
//  QString getMainCSS() const;

  bool hasActionTemplate() const;
  bool hasCompact(const QString &variant) const;
  QString compact(const QString &variant) const;
  void reload();

private:
  bool readStyleFile(QString &out, const QString &fileName, bool failBack = true);
  void init(const QString &styleName, StyleBuildMode styleBuildMode);
  void listVariants();
  void readStyleFiles();

  class Private;
  Private * const d;
};

#endif /*CHATWINDOWSTYLE_H_*/
