/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
 *
 * This file based on "kopetechatwindowstyle.cpp" - A Chat Window Style.
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

#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>

#include "chatwindowstyle.h"
#include "settings.h"

class ChatWindowStyle::Private
{
public:
  bool defaultStyle;
  QString baseHref;
  QString styleName;
  StyleVariants variantsList;

  QString templateHtml;
  QString headerHtml;
  QString footerHtml;
  QString incomingHtml;
  QString nextIncomingHtml;
  QString outgoingHtml;
  QString nextOutgoingHtml;
  QString statusHtml;
  QString actionIncomingHtml;
  QString actionOutgoingHtml;
  QString fileTransferIncomingHtml;
  QString outgoingStateSendingHtml;
  QString outgoingStateErrorHtml;
  QString outgoingStateSentHtml;
  QString outgoingStateUnknownHtml;
};


/*!
 * \brief Build a single chat window style.
 *
 */
ChatWindowStyle::ChatWindowStyle(const QString &styleName)
  : d(new Private)
{
  init(styleName);
}


ChatWindowStyle::~ChatWindowStyle()
{
  delete d;
}


/*!
 * Check if the style has the support for Kopete Action template (Kopete extension).
 *
 * \return true if the style has Action template.
 */
bool ChatWindowStyle::hasActionTemplate() const
{
  return (!d->actionIncomingHtml.isEmpty() && !d->actionOutgoingHtml.isEmpty());
}


/*!
 * Get the style resource directory.
 * Resources directory is the base where all CSS, HTML and images are located.
 *
 * Adium(and now Kopete too) style directories are disposed like this:
 * StyleName/
 *          Contents/
 *            Resources/
 *
 * \return the path to the resource directory.
 */
QString ChatWindowStyle::styleBaseHref() const
{
  if(d->defaultStyle)
    return "qrc" + d->baseHref;

  return QUrl::fromLocalFile(d->baseHref).toString();
}


/*!
 * Get the style path.
 * The style path points to the directory where the style is located.
 * ex: ~/.kde/share/apps/kopete/styles/StyleName/
 *
 * \return the style path based.
 */
QString ChatWindowStyle::styleName() const { return d->styleName; }


/*!
 * Проверяет директорию со стилем на соответствие минимальным требованиям.
 */
bool ChatWindowStyle::isValid(const QString &style)
{
  int validResult = 0;

  if (QDir().exists(style + "/Contents")) {
    validResult++;
  }
  if (QDir().exists(style + "/Contents/Resources")) {
    validResult++;
  }
  if (QDir().exists(style + "/Contents/Resources/Incoming")) {
    validResult++;
  }
  if (QDir().exists(style + "/Contents/Resources/Outgoing")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/main.css")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/Footer.html")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/Status.html")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/Header.html")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/Incoming/Content.html")) {
    validResult++;
  }
  if (QFile::exists(style + "/Contents/Resources/Outgoing/Content.html")) {
    validResult++;
  }

  if (validResult >= 8)
    return true;
  else
    return false;
}


/*!
 * Формирует карту вариантов стиля.
 *
 * \param variantsPath Путь к папке с вариантами стиля.
 */
ChatWindowStyle::StyleVariants ChatWindowStyle::variants(const QString &variantsPath)
{
  QDir variantDir(variantsPath);
  variantDir.setFilter(QDir::Files);
  variantDir.setSorting(QDir::Name);

  QStringList entryList = variantDir.entryList(QStringList("*.css"));
  StyleVariants out;

  foreach (QString variantName, entryList) {
    out.insert(QFileInfo(variantName).completeBaseName(), "Variants/" + variantName);
  }

  return out;
}


/*!
 * Get the list of all variants for this theme.
 * If the variant aren't listed, it call the lister
 * before returning the list of the Variants.
 * If the variant are listed, it just return the cached
 * variant list.
 * \return the StyleVariants QHash.
 */
ChatWindowStyle::StyleVariants ChatWindowStyle::variants()
{
  // If the variantList is empty, list available variants.
  if(d->variantsList.isEmpty())
    listVariants();

  return d->variantsList;
}


/*!
 * Reload style from disk.
 */
void ChatWindowStyle::reload()
{
  d->variantsList.clear();
  readStyleFiles();
  listVariants();
}


QString ChatWindowStyle::templateHtml() const             { return d->templateHtml; }
QString ChatWindowStyle::headerHtml() const               { return d->headerHtml; }
QString ChatWindowStyle::footerHtml() const               { return d->footerHtml; }
QString ChatWindowStyle::incomingHtml() const             { return d->incomingHtml; }
QString ChatWindowStyle::nextIncomingHtml() const         { return d->nextIncomingHtml; }
QString ChatWindowStyle::outgoingHtml() const             { return d->outgoingHtml; }
QString ChatWindowStyle::nextOutgoingHtml() const         { return d->nextOutgoingHtml; }
QString ChatWindowStyle::statusHtml() const               { return d->statusHtml; }
QString ChatWindowStyle::actionIncomingHtml() const       { return d->actionIncomingHtml; }
QString ChatWindowStyle::actionOutgoingHtml() const       { return d->actionOutgoingHtml; }
QString ChatWindowStyle::fileTransferIncomingHtml() const { return d->fileTransferIncomingHtml; }
QString ChatWindowStyle::outgoingStateSendingHtml() const { return d->outgoingStateSendingHtml; }
QString ChatWindowStyle::outgoingStateSentHtml() const    { return d->outgoingStateSentHtml; }
QString ChatWindowStyle::outgoingStateErrorHtml() const   { return d->outgoingStateErrorHtml; }
QString ChatWindowStyle::outgoingStateUnknownHtml() const { return d->outgoingStateUnknownHtml; }


/*!
 * Чтение файла стиля.
 *
 * \param out Строка в которую будет записан результат.
 * \param fileName Относительно имя файла.
 * \param failBack \a true если файл не будет найден, то прочитать файл по умолчанию из
 * ресурсов, иначе вернуть ошибку.
 *
 * \return \a true в случае успеха.
 */
bool ChatWindowStyle::readStyleFile(QString &out, const QString &fileName, bool failBack) const
{
  QString realFileName = d->baseHref + fileName;

  if (!QFile::exists(realFileName)) {
    if (failBack)
      realFileName = ":/webkit/Contents/Resources/" + fileName;
    else
      return false;
  }

  QFile file(realFileName);

  if (!file.open(QIODevice::ReadOnly))
    return false;

  QTextStream stream(&file);
  stream.setCodec(QTextCodec::codecForName("UTF-8"));
  out = stream.readAll();
  file.close();

  return true;
}


/*!
 * Init this class.
 */
void ChatWindowStyle::init(const QString &styleName)
{
  QString basePath;
  bool valid = false;

  if (styleName != "Default") {
    QStringList stylesPath = SimpleSettings->path(Settings::StylesPath);
    for (int i = 0; i < stylesPath.size(); ++i) {
      if (QDir().exists(stylesPath.at(i) + '/' + styleName) && isValid(stylesPath.at(i) + '/' + styleName)) {
        basePath = stylesPath.at(i) + '/';
        valid = true;
        break;
      }
    }
  }

  if (!valid) {
    d->styleName = "Default";
    d->baseHref = ":/webkit/Contents/Resources/";
    d->defaultStyle = true;
  }
  else {
    d->styleName = styleName;
    d->baseHref = basePath + styleName + "/Contents/Resources/";
    d->defaultStyle = false;
  }

  readStyleFiles();
  listVariants();
}


/*!
 * List available variants for the current style.
 */
void ChatWindowStyle::listVariants()
{
  d->variantsList = variants(d->baseHref + "Variants/");
}


/*!
 * Read style HTML files from disk.
 */
void ChatWindowStyle::readStyleFiles()
{
  readStyleFile(d->templateHtml,             "Template.html");
  readStyleFile(d->headerHtml,               "Header.html");
  readStyleFile(d->footerHtml,               "Footer.html");
  readStyleFile(d->incomingHtml,             "Incoming/Content.html");
  readStyleFile(d->nextIncomingHtml,         "Incoming/NextContent.html");
  readStyleFile(d->outgoingHtml,             "Outgoing/Content.html");
  readStyleFile(d->nextOutgoingHtml,         "Outgoing/NextContent.html");
  readStyleFile(d->statusHtml,               "Status.html");
  readStyleFile(d->actionIncomingHtml,       "Incoming/Action.html", false);
  readStyleFile(d->actionOutgoingHtml,       "Outgoing/Action.html", false);
  readStyleFile(d->fileTransferIncomingHtml, "Incoming/FileTransferRequest.html");
  readStyleFile(d->outgoingStateUnknownHtml, "Outgoing/StateUnknown.html");
  readStyleFile(d->outgoingStateSendingHtml, "Outgoing/StateSending.html");
  readStyleFile(d->outgoingStateSentHtml,    "Outgoing/StateSent.html");
  readStyleFile(d->outgoingStateErrorHtml,   "Outgoing/StateError.html");

  if ( d->fileTransferIncomingHtml.isEmpty() ||
       ( !d->fileTransferIncomingHtml.contains( "saveFileHandlerId" ) &&
         !d->fileTransferIncomingHtml.contains( "saveFileAsHandlerId" ) ) )
  {  // Create default html
    d->fileTransferIncomingHtml = d->incomingHtml;
    QString message = QString( "%message%\n"
                               "<div>\n"
                               " <div style=\"width:37px; float:left;\">\n"
                               "  <img src=\"%fileIconPath%\" style=\"width:32px; height:32px; vertical-align:middle;\" />\n"
                               " </div>\n"
                               " <div>\n"
                               "  <span><b>%fileName%</b> (%fileSize%)</span><br>\n"
                               "  <span>\n"
                               "   <input id=\"%saveFileAsHandlerId%\" type=\"button\" value=\"%1\">\n"
                               "   <input id=\"%cancelRequestHandlerId%\" type=\"button\" value=\"%2\">\n"
                               "  </span>\n"
                               " </div>\n"
                               "</div>" )
                               .arg("Download", "Cancel");
    d->fileTransferIncomingHtml.replace( QLatin1String("%message%"), message );
  }
}
