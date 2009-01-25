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

#include <QStringList>
#include "chatwindowstyle.h"
#include "chatwindowstyleoutput.h"

/*!
 * Конструктор класса ChatWindowStyleOutput.
 */
ChatWindowStyleOutput::ChatWindowStyleOutput(const QString &style, const QString &variant)
{

  m_style = new ChatWindowStyle(style);
  m_variant = variant;

}

ChatWindowStyleOutput::~ChatWindowStyleOutput()
{
  delete m_style;
}


/*!
 * Create a html skeleton. Future messages will be added to it
 * skeleton consist of styles, header and footer
 * it has a mark as well. Before this mark new messages should be added.
 *
 * \param chatName        Name of chat, example "Weekends plans".
 * \param ownerName       Name or nickname of program owner.
 * \param partnerName     Name or nicname of chating partner.
 * \param ownerIconPath   Path to image files representing ppl,
 *                        "" uses style provided pictures.
 * \param partnerIconPath See ownerIconPath.
 * \param time            It's time when the chat has started.
 *
 * \return Result HTML.
 */
QString ChatWindowStyleOutput::makeSkeleton(const QString &chatName,        const QString &ownerName,
                                            const QString &partnerName,     const QString &ownerIconPath,
                                            const QString &partnerIconPath, const QString &time)
{
  QString skeleton = m_style->getTemplateHtml();
  skeleton.replace(skeleton.indexOf("%@"), 2, m_style->getStyleBaseHref());
  skeleton.replace(skeleton.lastIndexOf("%@"), 2, m_style->getFooterHtml());
  skeleton.replace(skeleton.lastIndexOf("%@"), 2, m_style->getHeaderHtml());

  if (!m_variant.isEmpty())
    skeleton.replace(skeleton.lastIndexOf("%@"), 2, "Variants/" + m_variant + ".css");
  else
    skeleton.replace(skeleton.lastIndexOf("%@"), 2, "main.css");

  if (skeleton.contains("%@"))
    skeleton.replace(skeleton.indexOf("%@"),2,"@import url( \"main.css\" );");

  skeleton = skeleton.replace("%chatName%", Qt::escape(chatName));
  skeleton = skeleton.replace("%sourceName%", Qt::escape(ownerName));
  skeleton = skeleton.replace("%destinationName%", Qt::escape(partnerName));
  skeleton = skeleton.replace("%timeOpened%", Qt::escape(time));

  QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
  int pos = 0;
  while ((pos = timeRegExp.indexIn(skeleton, pos)) != -1)
    skeleton = skeleton.replace(pos, timeRegExp.cap(0).length(), time);

  if(ownerIconPath == "")
    skeleton = skeleton.replace("%outgoingIconPath%", "outgoing_icon.png"); // \todo Исправить outgoing_icon.png
  else
  #if defined(Q_WS_WIN)
  skeleton = skeleton.replace("%outgoingIconPath%", ownerIconPath);
  #else
  skeleton = skeleton.replace("%outgoingIconPath%", "file://" + ownerIconPath);
  #endif

  if(partnerIconPath == "")
    skeleton = skeleton.replace("%incomingIconPath%", "incoming_icon.png"); // \todo Исправить incoming_icon.png
  else
  #if defined(Q_WS_WIN)
  skeleton = skeleton.replace("%incomingIconPath%", partnerIconPath);
  #else
  skeleton = skeleton.replace("%incomingIconPath%", "file://" + partnerIconPath);
  #endif

  return skeleton;
}


/*!
 * Changes keywords to message atributes in html.
 *
 * \param sender     Sender's nickname.
 * \param message    Message text.
 * \param direction  \b true if it was send, \b false if it was recived.
 * \param sameSender \b true - same sender, \b false - message is send by another person.
 * \param avatarPath Path to avatar, if "" uses style provided picture.
 * \param action     \b true changes keywords to action atributes in html like "Bob is writing on the desk".
 * \param time       Time at witch message was send.
 * \param aligment   \b true if left-to-right, \b false if right-to-left.
 *
 * \return Result HTML.
 */
QString ChatWindowStyleOutput::makeMessage(const QString &sender, const QString &message,
                                           bool direction, bool sameSender,
                                           const QString &avatarPath, bool action,
                                           const QString &time, bool aligment)
{
  QString html;

  if (direction) {
    if (action)
      html = m_style->getActionOutgoingHtml();
    else
      html = sameSender ? m_style->getNextOutgoingHtml() : m_style->getOutgoingHtml();
  }
  else {
    if (action)
      html = m_style->getActionIncomingHtml();
    else
      html = sameSender ? m_style->getNextIncomingHtml() : m_style->getIncomingHtml();
  }

  html = html.replace("%sender%", sender);
  html = html.replace("%senderScreenName%", sender);
  html = html.replace("%service%", "schat");
  html = html.replace("%senderStatusIcon%", "");

  QString avatar = avatarPath;
  if(avatar.isEmpty()) { /// \todo Исправить.
    if(direction)
      avatar = (m_style->getStyleBaseHref() + "Outgoing/buddy_icon.png");
    else
      avatar = (m_style->getStyleBaseHref() + "Incoming/buddy_icon.png");
  }

  html = html.replace("%userIconPath%", avatar);

  // search for background colors and change them, so CSS would stay clean
  QString bgColor = "inherit";
  QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
  int textPos = 0;
  while((textPos = textBackgroundRegExp.indexIn(html, textPos)) != -1)
    html = html.replace(textPos, textBackgroundRegExp.cap(0).length(), bgColor);

  // Replace %messageDirection% with "rtl"(Right-To-Left) or "ltr"(Left-to-right)
  html = html.replace("%messageDirection%", aligment ? "ltr" : "rtl" );

  commonReplace(html, message, time);
  return html;
}


/*!
 * It is used for displaying system and user messages like "user gone offline",
 * "Marina is now away", "You are being ignored" etc.
 *
 * \param message Message by it self to be shown.
 * \param time    Timestamp.
 *
 * \return Result HTML.
 */
QString ChatWindowStyleOutput::makeStatus(const QString &message, const QString &time)
{
  QString html = m_style->getStatusHtml();
  commonReplace(html, message, time);
  return html;
}


/*!
 * Общие замены для обычного и статусного сообщения.
 *
 * \param html Строка, в которой будет производиться замены.
 * \param msg  Сообщение.
 * \param time Время.
 */
void ChatWindowStyleOutput::commonReplace(QString &html, const QString &msg, const QString &time)
{
  html = html.replace("%time%", Qt::escape(time));
  QRegExp timeRegExp("%time\\{([^}]*)\\}%");
  int pos = 0;
  while((pos = timeRegExp.indexIn(html, pos)) != -1)
  html = html.replace(pos, timeRegExp.cap(0).length(), Qt::escape(time));

  // Replace %messages%, replacing last to avoid errors if messages contains tags
  QString message = msg;
  html = html.replace("%message%", message.replace("\\","\\\\").remove('\r').replace("%","&#37;") + "&nbsp;");
}
