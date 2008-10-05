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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "chatbrowser.h"
#include "emoticonmovie.h"
#include "settings.h"


struct PrepareEmoticons {
  int position;
  QString name;
  QString file;
  PrepareEmoticons(int _position, QString _name, QString _file) { position = _position; name = _name; file = _file; }
};

/*!
 * \class ChatBrowser
 * \brief Обеспечивает отображение текста в чате.
 */

/*!
 * \brief Конструктор класса ChatBrowser.
 */
ChatBrowser::ChatBrowser(Settings *settings, QWidget *parent)
  : QTextBrowser(parent), m_settings(settings)
{
  m_channelLog = new ChannelLog(this);
  setFocusPolicy(Qt::NoFocus);
  setOpenLinks(false);

  m_style = QString(
      ".gr    { color:#90a4b3; }"
      ".green { color:#6bb521; }"
      ".err   { color:#da251d; }"
      ".info  { color:#5096cf; }"
      ".me    { color:#cd00cd; }"
      "a      { color:#1a4d82; text-decoration:none; }"
      "a.nick { color:#90a4b3; font-weight:bold; }"
      );

  document()->setDefaultStyleSheet(m_style);

  setSettings();
  m_animateTimer.setInterval(m_settings->getInt("EmoticonsRefreshTime"));
  connect(&m_animateTimer, SIGNAL(timeout()), SLOT(animate()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(setSettings()));
  connect(this, SIGNAL(anchorClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));

  createActions();
}


/** [public]
 *
 */
void ChatBrowser::msg(const QString &text)
{
  m_channelLog->msg(text);
  append(tr("<div><small class='gr'>(%1)</small> %2</div>").arg(currentTime()).arg(text));
  scroll();
}


QString ChatBrowser::msgBadNickName(const QString &nick)
{
  return tr("<i class='err'>Выбранный ник: <b>%2</b>, не допустим в чате, выберите другой</i>").arg(Qt::escape(nick));
}


QString ChatBrowser::msgChangedNick(quint16 sex, const QString &oldNick, const QString &newNick)
{
  if (sex) /// \todo Добавить ссылку на ник
    return tr("<i class='info'><b>%1</b> теперь известна как <b>%2</b></i>").arg(Qt::escape(oldNick)).arg(Qt::escape(newNick));
  else
    return tr("<i class='info'><b>%1</b> теперь известен как <b>%2</b></i>").arg(Qt::escape(oldNick)).arg(Qt::escape(newNick));
}


QString ChatBrowser::msgDisconnect()
{
  return tr("<i class='err'>Соединение разорвано</i>");
}


QString ChatBrowser::msgNewUser(quint8 sex, const QString &nick)
{ /// \todo Добавить ссылку на ник
  if (sex)
    return tr("<i class='gr'><b>%1</b> зашла в чат</i>").arg(Qt::escape(nick));
  else
    return tr("<i class='gr'><b>%1</b> зашёл в чат</i>").arg(Qt::escape(nick));
}


QString ChatBrowser::msgOldClientProtocol()
{
  return tr("<span class='err'>Ваш чат использует устаревшую версию протокола, подключение не возможно, пожалуйста обновите программу.</span>");
}


QString ChatBrowser::msgOldServerProtocol()
{
  return tr("<span class='err'>Сервер использует устаревшую версию протокола, подключение не возможно.</span>");
}


QString ChatBrowser::msgReadyForUse(const QString &addr)
{
  return tr("<i class='green'>Успешно подключены к серверу %1</i>").arg(addr);
}


QString ChatBrowser::msgReadyForUse(const QString &network, const QString &addr)
{
  return tr("<i class='green'>Успешно подключены к сети <b>%1</b> (%2)</i>").arg(network).arg(addr);
}


QString ChatBrowser::msgUserLeft(quint8 sex, const QString &nick, const QString &bye)
{ /// \todo Добавить ссылку на ник
  QString byeMsg;
  if (!bye.isEmpty())
    byeMsg = ": <span style='color:#909090;'>" + Qt::escape(bye) + "</span>";

  if (sex)
    return tr("<i class='gr'><b>%1</b> вышла из чата%2</i>").arg(Qt::escape(nick)).arg(byeMsg);
  else
    return tr("<i class='gr'><b>%1</b> вышел из чата%2</i>").arg(Qt::escape(nick)).arg(byeMsg);
}


/** [public]
 * Принудительно скролит текст
 */
void ChatBrowser::scroll()
{
  QScrollBar *bar = verticalScrollBar();
  bar->setValue(bar->maximum());
}


/*!
 * Контекстное меню
 */
void ChatBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  m_copyAction->setEnabled(textCursor().hasSelection());
  bool empty = (bool) document()->toPlainText().size();
  m_clearAction->setEnabled(empty);
  m_selectAllAction->setEnabled(empty);

  QMenu menu(this);
  menu.addAction(m_copyAction);

  QUrl link(anchorAt(event->pos()));
  QAction *copyLinkAction = 0;
  if (!link.isEmpty() && link.isValid() && link.scheme() != "nick")
    copyLinkAction = menu.addAction(tr("Копировать &ссылку"));

  menu.addSeparator();
  menu.addAction(m_clearAction);
  menu.addAction(m_selectAllAction);
  QAction *action = menu.exec(event->globalPos());

  if (action == copyLinkAction)
    QApplication::clipboard()->setText(link.toString());
}


void ChatBrowser::clear()
{
  if (m_animateTimer.isActive())
    m_animateTimer.stop();

  m_animateQueue.clear();

  if (!m_aemoticon.isEmpty()) {
    QHashIterator <QString, EmoticonMovie*> i(m_aemoticon);
    while (i.hasNext()) {
      i.next();
      i.value()->deleteLater();
    }
    m_aemoticon.clear();
  }

  QTextBrowser::clear();
}


/*!
 * \param nick Ник отправителя сообщения.
 * \param message Сообщение.
 */
void ChatBrowser::msgNewMessage(const QString &nick, const QString &message)
{
  QTextDocument doc;
  doc.setDefaultStyleSheet(m_style);
  QTextCursor docCursor(&doc);
  QString msg = message;

  if (ChannelLog::toPlainText(message).startsWith("/me ", Qt::CaseInsensitive)) {
    msg.remove("/me ", Qt::CaseInsensitive);
    msg = "<span class='me'>" + msg + "</span>";
    QString escapedNick = Qt::escape(nick);
    docCursor.insertHtml(QString("<small class='gr'>(%1)</small> <a href='nick:%2' class='me'>%3</span> ")
        .arg(currentTime())
        .arg(QString(nick.toUtf8().toHex()))
        .arg(escapedNick));
    m_channelLog->msg(QString("<span class='me'>%1 %2</span>").arg(escapedNick).arg(msg));
  }
  else {
    QString escapedNick = Qt::escape(nick);
    docCursor.insertHtml(QString("<small class='gr'>(%1)</small> <a class='nick' href='nick:%2'>%3:</a> ")
        .arg(currentTime())
        .arg(QString(nick.toUtf8().toHex()))
        .arg(escapedNick));
    m_channelLog->msg(QString("<b class='gr'>%1:</b> %2").arg(escapedNick).arg(msg));
  }

  docCursor.movePosition(QTextCursor::End);
  int offset = docCursor.position();
  docCursor.insertHtml(msg);
  QString plainMsg = doc.toPlainText().mid(offset);

  if (m_useEmoticons) {
    QList<Emoticons> emoticons = m_settings->emoticons(plainMsg);
    QMap<int, PrepareEmoticons> prepareEmoticons;

    if (!emoticons.isEmpty()) {
      int size    = toPlainText().size();
      int docSize = doc.toPlainText().size();

      foreach (Emoticons emoticon, emoticons) {
        docCursor.setPosition(offset);

        do {
          if (emoticon.file.isEmpty())
            continue;

          bool ok = false;
          docCursor = doc.find(emoticon.name, docCursor);

          if (docCursor.selectedText() == emoticon.name) {
            if (m_emoticonsRequireSpaces) {
              QTextCursor findCursor(&doc);

              // Сообщение содержит только один смайлик и больше ничего.
              if (docCursor.anchor() == offset && docCursor.position() == docSize)
                ok = true;
              // Код смайлика находится в начале строки, после него должен следовать пробел.
              else if (docCursor.anchor() == offset) {
                findCursor.setPosition(docCursor.position());
                findCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                if (findCursor.selectedText().at(0).isSpace())
                  ok = true;
              }
              // Код смайлика находится в конце строки, перед ним должен находится пробел.
              else if (docCursor.position() == docSize) {
                findCursor.setPosition(docCursor.anchor());
                findCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                if (findCursor.selectedText().at(0).isSpace())
                  ok = true;
              }
              // Код смайлика находится внутри строки, с обоих сторон должны находится пробелы.
              else {
                findCursor.setPosition(docCursor.anchor());
                findCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                QChar prev = findCursor.selectedText().at(0);
                findCursor.setPosition(docCursor.position());
                findCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                QChar next = findCursor.selectedText().at(0);
                if (prev.isSpace() && next.isSpace())
                  ok = true;
              }
            }
            else
              ok = true;
          }

          if (ok)
            prepareEmoticons.insert(docCursor.anchor(), PrepareEmoticons(docCursor.position(), emoticon.name, emoticon.file));

        } while (!docCursor.isNull());
      }

      if (!prepareEmoticons.isEmpty()) {
        int blockStart = size + offset;
        int fix        = 0;
        QString emoticonsPath = qApp->applicationDirPath() + "/emoticons/" + m_settings->getString("EmoticonTheme") + "/";
        QTextCursor emoCursor(&doc);

        QMapIterator<int, PrepareEmoticons> i(prepareEmoticons);
        while (i.hasNext()) {
          i.next();
          emoCursor.setPosition(i.key() - fix);
          emoCursor.setPosition(i.value().position - fix, QTextCursor::KeepAnchor);
          emoCursor.insertImage(i.value().file);
          fix += i.value().name.size() - 1;
          addAnimation(emoticonsPath + i.value().file, size + emoCursor.position(), blockStart);
        }
      }

    }
  }

  append(doc.toHtml());

  scroll();
}


void ChatBrowser::animate()
{
  QTextCursor cursor(document());
  cursor.beginEditBlock();
  while (!m_animateQueue.isEmpty()) {
    cursor.setPosition(m_animateQueue.dequeue());
    cursor.insertText(" ");
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    cursor.insertText("");
  }
  cursor.endEditBlock();
}


void ChatBrowser::animate(const QString &key)
{
  if (m_aemoticon.contains(key)) {
    if (m_useAnimatedEmoticons)
      document()->addResource(QTextDocument::ImageResource, key, m_aemoticon.value(key)->currentPixmap());

    QList<int> starts = m_aemoticon.value(key)->starts();
    foreach (int start, starts) {
      if (!m_animateQueue.contains(start))
        m_animateQueue.enqueue(start);
    }
  }
}


void ChatBrowser::linkClicked(const QUrl &link)
{
  QString scheme = link.scheme();

  if (scheme == "nick")
    emit nickClicked(link.toString(QUrl::RemoveScheme));
  else if (scheme == "http" || scheme == "https" || scheme == "ftp" || scheme == "ftps" || scheme == "mailto")
    QDesktopServices::openUrl(link);
}


void ChatBrowser::setAnimations()
{
  int min = cursorForPosition(QPoint(0, 0)).position();
  int max = cursorForPosition(QPoint(size().width(), size().height())).position();

  emit pauseIfHidden(min, max);
}


void ChatBrowser::setSettings()
{
  m_useEmoticons           = m_settings->getBool("UseEmoticons");
  m_useAnimatedEmoticons   = m_settings->getBool("UseAnimatedEmoticons");
  m_emoticonsRequireSpaces = m_settings->getBool("EmoticonsRequireSpaces");
  setPauseAnimations(!m_useAnimatedEmoticons);
  m_animateTimer.setInterval(m_settings->getInt("EmoticonsRefreshTime"));
}


/*!
 * Добавление смайлика в документ.
 *
 * \param fileName Полное имя файла.
 * \param pos      Позиция смайлика в документе.
 * \param starts   Общая начальная позиция всех смайликов в данном блоке.
 */
void ChatBrowser::addAnimation(const QString &fileName, int pos, int starts)
{
  QString name = QFileInfo(fileName).fileName();
  if (m_aemoticon.contains(name)) {
    m_aemoticon.value(name)->addPos(pos);
    m_aemoticon.value(name)->addStarts(starts);
  }
  else {
    EmoticonMovie *movie = new EmoticonMovie(fileName, pos, starts, this);
    m_aemoticon.insert(name, movie);
    document()->addResource(QTextDocument::ImageResource, name, movie->currentPixmap());
    connect(movie, SIGNAL(frameChanged(const QString &)), SLOT(animate(const QString &)));
    connect(this, SIGNAL(pauseAnimations(bool)), movie, SLOT(setPaused(bool)));
    connect(this, SIGNAL(pauseIfHidden(int, int)), movie, SLOT(pauseIfHidden(int, int)));
  }

  if (!m_animateTimer.isActive() && m_useAnimatedEmoticons)
    m_animateTimer.start();
}


/*!
 * Создание объектов \a QAction.
 */
void ChatBrowser::createActions()
{
  m_copyAction = new QAction(QIcon(":/images/editcopy.png"), tr("&Копировать"), this);
  m_copyAction->setShortcut(Qt::CTRL + Qt::Key_C);
  connect(m_copyAction, SIGNAL(triggered()), SLOT(copy()));

  m_clearAction = new QAction(QIcon(":/images/editclear.png"), tr("&Очистить"), this);
  connect(m_clearAction, SIGNAL(triggered()), SLOT(clear()));

  m_selectAllAction = new QAction(tr("&Выделить всё"), this);
  connect(m_selectAllAction, SIGNAL(triggered()), SLOT(selectAll()));
}


void ChatBrowser::setPauseAnimations(bool paused)
{
  if (m_useAnimatedEmoticons)
    emit pauseAnimations(paused);
  else
    emit pauseAnimations(true);

  if (paused) {
    if (m_animateTimer.isActive())
      m_animateTimer.stop();
  }
  else if (!m_aemoticon.isEmpty() && !m_animateTimer.isActive() && m_useAnimatedEmoticons)
    m_animateTimer.start();
}
