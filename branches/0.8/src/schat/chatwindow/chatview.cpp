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

#include <QAction>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QDir>
#include <QMenu>
#include <QScrollBar>
#include <QTextDocument>

#ifndef SCHAT_NO_WEBKIT
# include <QQueue>
# include <QWebFrame>
# include <QWebHitTestResult>

# include "chatwindowstyle.h"
# include "chatwindowstyleoutput.h"
#endif

#include "abstractprofile.h"
#include "channellog.h"
#include "chatview.h"
#include "chatview_p.h"
#include "protocol.h"
#include "settings.h"
#include "simplechatapp.h"
#include "text/PlainTextFilter.h"

/*!
 * \brief Конструктор класса ChatViewPrivate.
 */
#ifndef SCHAT_NO_WEBKIT
ChatViewPrivate::ChatViewPrivate(const QString &styleName, const QString &styleVariant, ChatView *parent)
  : empty(true),
  q(parent),
  loaded(false),
  chatStyle(styleName),
  chatStyleVariant(styleVariant)
#else
ChatViewPrivate::ChatViewPrivate(ChatView *parent)
  : empty(true),
  q(parent),
#endif
{
  #ifndef SCHAT_NO_WEBKIT
    style = new ChatWindowStyleOutput(chatStyle, chatStyleVariant);
    grouping = SimpleSettings->getBool("MessageGrouping");
  #endif

  strict = Emoticons::strictParse();
}


ChatViewPrivate::~ChatViewPrivate()
{
  #ifndef SCHAT_NO_WEBKIT
    if (style)
      delete style;
  #endif
}


/*!
 * Проверяет входящее сообщение на наличие команды и при
 * необходимости вырезает эту команду из строки.
 *
 * \param cmd Команда.
 * \param msg Сообщение.
 * \param cut Разрешает вырезание команды из строки.
 *
 * \return Возвращает \a true если команда была найдена.
 */
bool ChatViewPrivate::prepareCmd(const QString &cmd, QString &msg, bool cut)
{
  if (PlainTextFilter::filter(msg).startsWith(cmd, Qt::CaseInsensitive)) {
    if (cut) {
      QString c = cmd;
      int index = msg.indexOf(c, 0, Qt::CaseInsensitive);
      if (index == -1 && c.endsWith(' ')) {
        c = c.left(c.size() - 1);
        index = msg.indexOf(c, 0, Qt::CaseInsensitive);
      }

      if (index != -1)
        msg.remove(index, c.size());
    }
    return true;
  }

  return false;
}


/*!
 * Записывает строку в журнал.
 */
void ChatViewPrivate::toLog(const QString &text)
{
  if (log) {
    if (!channelLog) {
      channelLog = new ChannelLog(SimpleSettings->isUnixLike() ? SCHAT_UNIX_CONFIG("log") : QApplication::applicationDirPath() + "/log", q);
      channelLog->setChannel(channel);
    }
    channelLog->msg(text);
  }
}


#ifndef SCHAT_NO_WEBKIT
bool ChatViewPrivate::cleanStyle(const QString &styleName, const QString &styleVariant)
{
  if (chatStyle != styleName || chatStyleVariant != styleVariant) {
    chatStyle = styleName;
    chatStyleVariant = styleVariant;
    delete style;
    style = new ChatWindowStyleOutput(chatStyle, chatStyleVariant);
    return true;
  }
  else
    return false;
}


#else
QString ChatViewPrivate::makeMessage(const QString &sender, const QString &message, bool action)
{
  if (action)
    return makeStatus(QString("<span class='meSender'>%1</span> <span class='me'>%2</span>").arg(sender).arg(message));
  else
    return makeStatus(QString("<b class='sender'>%1:</b> %2").arg(sender).arg(message));
}


QString ChatViewPrivate::makeStatus(const QString &message)
{
  return QString("<div><small class='ts'>%1 |</small> %2</div>").arg(QTime::currentTime().toString("hh:mm:ss")).arg(message);
}
#endif


/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
#ifndef SCHAT_NO_WEBKIT
  : QWebView(parent)
#else
  : QTextBrowser(parent)
#endif
{
  #ifndef SCHAT_NO_WEBKIT
    d = new ChatViewPrivate(SimpleSettings->getString("ChatStyle"), SimpleSettings->getString("ChatStyleVariant"), this);
    connect(this, SIGNAL(loadFinished(bool)), SLOT(loadFinished()));

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setHtml(d->style->makeSkeleton());
    connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
    setAcceptDrops(false);

    QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, fontInfo().pixelSize());
    QWebSettings::globalSettings()->setFontFamily(QWebSettings::StandardFont, fontInfo().family());
  #else
    d = new ChatViewPrivate(this);

    setOpenLinks(false);
    document()->setDefaultStyleSheet(SimpleSettings->richTextCSS());
    setFrameShape(QFrame::NoFrame);
    connect(this, SIGNAL(anchorClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
    #if QT_VERSION >= 0x040500
      document()->setDocumentMargin(2);
    #endif
  #endif

  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(notify(int)));
  setFocusPolicy(Qt::NoFocus);

  createActions();
  retranslateUi();
}


ChatView::~ChatView()
{
  delete d;

  #if QT_VERSION >= 0x040600 && !defined(SCHAT_NO_WEBKIT)
  QWebSettings::clearMemoryCaches();
  #endif
}



QString ChatView::channel() const
{
  return d->channel;
}


/*!
 * Уведомление о смене пользователем ника.
 */
QString ChatView::statusChangedNick(quint8 gender, const QString &oldNick, const QString &newNick)
{
  QString nick = Qt::escape(oldNick);
  QString link = "<a href='nick:" + newNick.toUtf8().toHex() + "'>" + Qt::escape(newNick) + "</a>";
  QString html;

  if (gender)
    html = tr("<b>%1</b> is now known as %2", "Female").arg(nick).arg(link);
  else
    html = tr("<b>%1</b> is now known as %2", "Male").arg(nick).arg(link);

  return "<span class='changedNick'>" + html + "</span>";
}


/*!
 * Уведомление об подключении нового пользователя.
 */
QString ChatView::statusNewUser(quint8 gender, const QString &nick)
{
  QString link = "<a href='nick:" + nick.toUtf8().toHex() + "'>" + Qt::escape(nick) + "</a>";
  QString html;

  if (gender)
    html = tr("%1 entered chat", "Female").arg(link);
  else
    html = tr("%1 entered chat", "Male").arg(link);

  return "<span class='newUser'>" + html + "</span>";
}


/*!
 * Уведомление об отключении пользователя.
 */
QString ChatView::statusUserLeft(quint8 gender, const QString &nick, const QString &bye)
{
  QString html;
  QString link = "<a href='nick:" + nick.toUtf8().toHex() + "'>" + Qt::escape(nick) + "</a>";

  QString byeMsg;
  if (!bye.isEmpty())
    byeMsg = ": <span style='color:#909090;'>" + Qt::escape(bye) + "</span>";

  if (gender)
    html = tr("%1 left the chat%2", "Female").arg(link).arg(byeMsg);
  else
    html = tr("%1 left the chat%2", "Male").arg(link).arg(byeMsg);

  return "<span class='userLeft'>" + html + "</span>";
}


/*!
 * Универсальное фильтрованное сервисное сообщение.
 */
void ChatView::addFilteredMsg(const QString &msg, bool strict)
{
  QTextDocument doc;
  doc.setDefaultStyleSheet(SimpleSettings->richTextCSS());
  doc.setHtml(msg);

  QString html = ChannelLog::htmlFilter(doc.toHtml(), 0, strict);
  html = QString("<span class='preSb'>%1</span><div class='sb'>%2</div>").arg(tr("Service message:")).arg(html);

  addServiceMsg(html);
}


/*!
 * Добавление сообщения от пользователя.
 */
void ChatView::addMsg(const QString &sender, const QString &message, int options, bool notice)
{
  d->empty = false;
  if (SimpleSettings->profile()->nick() == sender)
    d->autoScroll->setChecked(true);

  QTextDocument doc;
  doc.setDefaultStyleSheet(SimpleSettings->richTextCSS());
  doc.setHtml(message);

  QString html = ChannelLog::htmlFilter(doc.toHtml());
  html = ChannelLog::parseLinks(html);

  QString escapedNick = Qt::escape(sender);
  bool action = false;

  #ifndef SCHAT_NO_WEBKIT
    bool same = false;
  #endif

  if (d->prepareCmd("/me ", html)) {
    action = true;

    #ifndef SCHAT_NO_WEBKIT
      d->prev = "";
    #endif

    d->toLog(QString("<span class='me'>%1 %2</span>").arg(escapedNick).arg(html));
  }
  else {
    #ifndef SCHAT_NO_WEBKIT
      if (d->prev.isEmpty() || d->prev != sender)
        d->prev = sender;
      else
        same = true;

      if (!d->grouping)
        same = false;
    #endif

    d->toLog(QString("<b class='sender'>%1:</b> %2").arg(escapedNick).arg(html));
  }

  // Парсинг смайлов
  if (SimpleSettings->emoticons()) {
    EmoticonsTheme::ParseMode mode = EmoticonsTheme::StrictParse;
    if (!d->strict)
      mode = EmoticonsTheme::RelaxedParse;

    html = SimpleSettings->emoticons()->theme().parseEmoticons(html, mode);
  }

  QString name = "<a href='nick:" + sender.toUtf8().toHex() + "'>" + escapedNick + "</a>";
  #ifndef SCHAT_NO_WEBKIT
    appendMessage(d->style->makeMessage(name, html, options & MsgSend, same, "", action), same);
  #else
    appendMessage(d->makeMessage(name, html, action));
  #endif

  // Поддержка извещателя.
  if (!notice)
    return;

  if (SimpleSettings->profile()->status() == schat::StatusDnD && SimpleSettings->getBool("NoNotificationInDnD"))
    return;

  if (options & MsgPublic) {
    if (SimpleSettings->getBool("NotificationPublic") && d->prepareCmd(SimpleSettings->profile()->nick(), html, false))
      emit popupMsg(sender, QDateTime::currentDateTime().toString("hh:mm:ss"), html, true);
  }
  else {
    if (SimpleSettings->getBool("Notification"))
      emit popupMsg(sender, QDateTime::currentDateTime().toString("hh:mm:ss"), html, false);
  }
}


/*!
 * Универсальное сервисное сообщение.
 */
void ChatView::addServiceMsg(const QString &msg)
{
  d->empty = false;

  #ifndef SCHAT_NO_WEBKIT
    d->prev = "";
  #endif

  d->toLog(msg);
  #ifndef SCHAT_NO_WEBKIT
    appendMessage(d->style->makeStatus(msg));
  #else
    appendMessage(d->makeStatus(msg));
  #endif
}


void ChatView::channel(const QString &ch)
{
  d->channel = ch;

  if (d->channelLog)
    d->channelLog->setChannel(ch);
}


void ChatView::log(bool enable)
{
  d->log = enable;

  if (enable) {
    if (!d->channelLog) {
      d->channelLog = new ChannelLog(SimpleSettings->isUnixLike() ? SCHAT_UNIX_CONFIG("log") : QApplication::applicationDirPath() + "/log", this);
      d->channelLog->setChannel(d->channel);
    }
  }
  else if (d->channelLog)
    d->channelLog->deleteLater();
}


void ChatView::scroll()
{
  if (!d->autoScroll->isChecked())
    return;

  #ifndef SCHAT_NO_WEBKIT
    page()->mainFrame()->evaluateJavaScript("alignChat(true)");
  #else
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
  #endif
}


/*!
 * Возвращает \a true в случае успешного копирования.
 */
bool ChatView::copy()
{
  #ifndef SCHAT_NO_WEBKIT
    if (selectedText().isEmpty())
      return false;

    triggerPageAction(QWebPage::Copy);
  #else
    if (!textCursor().hasSelection())
      return false;

    QTextBrowser::copy();
  #endif

  return true;
}


/*!
 * Очистка окна чата.
 */
void ChatView::clear()
{
  #ifndef SCHAT_NO_WEBKIT
    d->loaded = false;

    #if QT_VERSION >= 0x040600
    QWebSettings::clearMemoryCaches();
    #endif
    setHtml(d->style->makeSkeleton());
    d->prev = "";
  #else
    QTextBrowser::clear();
  #endif

  d->empty = true;
}


void ChatView::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  #ifndef SCHAT_NO_WEBKIT
  QWebView::changeEvent(event);
  #else
  QTextBrowser::changeEvent(event);
  #endif
}


void ChatView::contextMenuEvent(QContextMenuEvent *event)
{
# ifndef SCHAT_NO_WEBKIT
  d->copy->setEnabled(!selectedText().isEmpty());
# else
  d->copy->setEnabled(textCursor().hasSelection());
# endif

# if QT_VERSION >= 0x040500
  d->selectAll->setEnabled(!d->empty);
# endif

  QString copyLinkText = tr("Copy &link");
  d->clear->setEnabled(!d->empty);
  QAction *copyLink = 0;

  QMenu menu(this);
  menu.addAction(d->copy);

# ifndef SCHAT_NO_WEBKIT
  QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
  QUrl url = r.linkUrl();
  if (!url.isEmpty() && url.scheme() != "nick"  && url.scheme() != "smile") {
    copyLink = pageAction(QWebPage::CopyLinkToClipboard);
    copyLink->setText(copyLinkText);
  }
# else
  QUrl url(anchorAt(event->pos()));
  if (!url.isEmpty() && url.scheme() != "nick" && url.scheme() != "smile")
    copyLink = menu.addAction(copyLinkText);
# endif

  if (copyLink)
    menu.addAction(copyLink);

  menu.addSeparator();
  menu.addAction(d->autoScroll);
  menu.addAction(d->serviceMessages);
  menu.addSeparator();
  menu.addAction(d->clear);

# if QT_VERSION >= 0x040500
  menu.addAction(d->selectAll);
# endif

# ifndef SCHAT_NO_WEBKIT
  menu.exec(event->globalPos());
# else
  QAction *action = menu.exec(event->globalPos());

  if (action == copyLink)
    QApplication::clipboard()->setText(url.toString());
# endif
}


/*!
 * Обработка нажатия на ссылку.
 */
void ChatView::linkClicked(const QUrl &url)
{
  QString scheme = url.scheme();

  if (scheme == "nick") {
    emit nickClicked(QByteArray::fromHex(url.toString(QUrl::RemoveScheme).toLatin1()));
  }
  else if (scheme == "smile") {
    emit emoticonsClicked("&nbsp;" + QByteArray::fromHex(url.toString(QUrl::RemoveScheme).toLatin1()) + "&nbsp;");
  }
  #if QT_VERSION >= 0x040600
  else if (scheme == "file") {
    QDesktopServices::openUrl(QDir::toNativeSeparators(url.toString(QUrl::RemoveScheme)));
  }
  #endif
  else
    QDesktopServices::openUrl(url);
}


void ChatView::notify(int notify)
{
  if (notify == Settings::EmoticonsChanged) {
    d->strict = Emoticons::strictParse();
  }
  #ifndef SCHAT_NO_WEBKIT
  else if (notify == Settings::InterfaceSettingsChanged) {
    d->grouping = SimpleSettings->getBool("MessageGrouping");
    if (d->cleanStyle(SimpleSettings->getString("ChatStyle"), SimpleSettings->getString("ChatStyleVariant")))
      clear();
  }
  #endif
}


void ChatView::toggleServiceMessages(bool checked)
{
  SimpleSettings->setBool("ServiceMessages", checked);
}


#ifndef SCHAT_NO_WEBKIT
/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  d->loaded = true;

  while (!d->pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(d->pendingJs.dequeue());
}


void ChatView::selectAll()
{
# if QT_VERSION >= 0x040500
  triggerPageAction(QWebPage::SelectAll);
# endif
}
#endif


void ChatView::appendMessage(const QString &message, bool sameFrom)
{
  #ifndef SCHAT_NO_WEBKIT
    QString jsMessage = message;
    jsMessage.replace("\"", "\\\"");
    jsMessage.replace("\n", "\\n");
    jsMessage = QString("append%2Message(\"%1\");").arg(jsMessage).arg(sameFrom ? "Next" : "");
    if (d->loaded) {
      page()->mainFrame()->evaluateJavaScript(jsMessage);
      scroll();
    }
    else
      d->pendingJs.enqueue(jsMessage);
  #else
    Q_UNUSED(sameFrom)
    append(message);
    scroll();
  #endif
}


void ChatView::createActions()
{
  d->autoScroll = new QAction(QIcon(":/images/note2.png"), "", this);
  d->autoScroll->setCheckable(true);
  d->autoScroll->setChecked(true);

  d->serviceMessages = new QAction(QIcon(":/images/balloon-white.png"), "", this);
  d->serviceMessages->setCheckable(true);
  d->serviceMessages->setChecked(SimpleSettings->getBool("ServiceMessages"));
  connect(d->serviceMessages, SIGNAL(toggled(bool)), SLOT(toggleServiceMessages(bool)));

  d->copy = new QAction(SimpleChatApp::iconFromTheme("edit-copy"), "", this);
  d->copy->setShortcut(Qt::CTRL + Qt::Key_C);
  connect(d->copy, SIGNAL(triggered()), SLOT(copy()));

  d->clear = new QAction(SimpleChatApp::iconFromTheme("edit-clear"), "", this);
  connect(d->clear, SIGNAL(triggered()), SLOT(clear()));

# if QT_VERSION >= 0x040500
  d->selectAll = new QAction(SimpleChatApp::iconFromTheme("edit-select-all"), "", this);
  connect(d->selectAll, SIGNAL(triggered()), SLOT(selectAll()));
# endif
}


void ChatView::retranslateUi()
{
  d->autoScroll->setText(tr("Autoscroll"));
  d->serviceMessages->setText(tr("Service messages"));
  d->copy->setText(tr("&Copy"));
  d->clear->setText(tr("Clear"));

# if QT_VERSION >= 0x040500
  d->selectAll->setText(tr("Select All"));
# endif
}
