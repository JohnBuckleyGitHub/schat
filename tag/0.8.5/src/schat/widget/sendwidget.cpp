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

#include <QAction>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QGridLayout>
#include <QMenu>
#include <QToolBar>
#include <QWidgetAction>

#include "3rdparty/qtwin.h"
#include "abstractprofile.h"
#include "colorbutton.h"
#include "emoticons/emoticonselector.h"
#include "settings.h"
#include "settingsdialog.h"
#include "soundaction.h"
#include "widget/nickedit.h"
#include "widget/sendwidget.h"

/*!
 * Конструктор класса SendWidget.
 */
SendWidget::SendWidget(QWidget *parent)
  : TranslateWidget(parent),
  m_bigSendButton(SimpleSettings->getBool("BigSendButton")),
  m_input(new InputWidget(this)),
  m_toolBar(0)
{
  m_availableActions << "bold" << "italic" << "underline" << "color" << "emoticons" << "stretch" << "log" << "send" << "separator" << "strike";
  createPermanentButtons();
  initToolBar();
  setSettings();

  QGridLayout *mainLay = new QGridLayout(this);

  #ifndef Q_OS_WINCE
  mainLay->addWidget(m_toolBar, 0, 0);
  mainLay->addWidget(m_input, 1, 0);
  #else
  mainLay->addWidget(m_input, 0, 0);
  mainLay->addWidget(m_toolBar, 1, 0);
  #endif
  if (m_bigSendButton && m_send)
    mainLay->addWidget(m_send, 0, 1, 2, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  connect(m_input, SIGNAL(sendMsg(const QString &)), SIGNAL(sendMsg(const QString &)));
  connect(m_input, SIGNAL(needCopy()), SIGNAL(needCopy()));
  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(setSettings()));
  connect(m_input, SIGNAL(cursorPositionChanged()), SLOT(cursorPositionChanged()));

  retranslateUi();
}


/*!
 * Возвращает пару: указатель на действие и флаг доступности.
 */
QPair<SoundAction*, bool> SendWidget::soundAction() const
{
  QPair<SoundAction*, bool> pair(m_soundAction, m_availableActions.contains("sound"));
  return pair;
}


QToolButton* SendWidget::settingsButton() const
{
  if (m_availableActions.contains("settings"))
    return m_settingsButton;

  return 0;
}


void SendWidget::setInputFocus()
{
  m_input->setFocus();
}


void SendWidget::setStyleSheet()
{
  if (!m_toolBar)
    return;

  #if !defined(Q_OS_MAC)
    #if defined(Q_WS_WIN)
    m_toolBar->setStyleSheet(QString("QToolBar { background-color: %1; margin:0px; border:0px; }").arg(palette().color(QPalette::Window).name()));
    #else
    m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }");
    #endif
  #endif
}


/*!
 * \brief Обработка изменения позиции курсора для обновления кнопок.
 */
void SendWidget::cursorPositionChanged()
{
  QTextCursor cursor = m_input->textCursor();
  if (cursor.hasSelection()) {
    int position = cursor.position();
    if (position < cursor.anchor())
      cursor.setPosition(position + 1);
  }

  QTextCharFormat charFormat = cursor.charFormat();
  if (m_bold)      m_bold->setChecked(charFormat.font().bold());
  if (m_italic)    m_italic->setChecked(charFormat.font().italic());
  if (m_underline) m_underline->setChecked(charFormat.font().underline());
  if (m_strike)    m_strike->setChecked(charFormat.font().strikeOut());
  if (m_color)     m_color->setAltColor(charFormat.foreground().color());
}


void SendWidget::log()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(SimpleSettings->path(Settings::LogPath).at(0)));
}


/*!
 * Изменение состояние текса "Полужирный" \a Ctrl+B.
 */
void SendWidget::setBold(bool b)
{
  QTextCharFormat format;
  format.setFontWeight(b ? QFont::Bold : QFont::Normal);

  mergeFormat(format);
}


/*!
 * \brief Изменение состояние текса "Курсив" \a Ctrl+I.
 */
void SendWidget::setItalic(bool b)
{
  QTextCharFormat format;
  format.setFontItalic(b);

  mergeFormat(format);
}


void SendWidget::setSettings()
{
  if (m_emoticons) m_emoticons->setEnabled(SimpleSettings->getBool("UseEmoticons"));
}


/*!
 * Изменение состояние текса "Зачёркнутый".
 */
void SendWidget::setStrike(bool b)
{
  QTextCharFormat format;
  format.setFontStrikeOut(b);

  mergeFormat(format);
}


void SendWidget::settingsPage()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    emit showSettingsPage(action->data().toInt());
}


/*!
 * Изменение состояние текса "Подчёркнутый" \a Ctrl+U.
 */
void SendWidget::setUnderline(bool b)
{
  QTextCharFormat format;
  format.setFontUnderline(b);

  mergeFormat(format);
}


void SendWidget::textColor(const QColor &color)
{
  if (!color.isValid())
    return;

  QTextCharFormat format;
  format.setForeground(color);

  mergeFormat(format);
}


/*!
 * Фильтр событий.
 */
bool SendWidget::eventFilter(QObject *object, QEvent *event)
{
  if (event->type() == QEvent::ContextMenu) {
    QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent *>(event);
    QMenu menu(this);
    QMenu *addMenu = availableActions();
    if (addMenu)
      menu.addMenu(addMenu);
    QAction *removeAction = 0;

    QAction *action = m_toolBar->actionAt(menuEvent->pos());
    QString name;
    if (action) {
      name = action->data().toString();
      if (!name.isEmpty()) {
        removeAction = menu.addAction(QIcon(":/images/edit-delete.png"), tr("Delete"));
      }
    }

    QAction *resetAction = 0;
    QStringList list = toolBarLayout();
    if (m_bigSendButton && !list.contains("send"))
      list << "send";
    if (list != schat::DefaultToolBarLayout) {
      menu.addSeparator();
      resetAction = menu.addAction(QIcon(":/images/undo.png"), tr("Default"));
    }

    QAction *result = menu.exec(menuEvent->globalPos());
    if (result) {
      if (result == removeAction) {
        m_toolBar->removeAction(action);
        if (!m_permanentButtons.contains(name))
          action->deleteLater();
        if (!m_availableActions.contains(name))
          m_availableActions << name;
      }
      else if (result == resetAction) {
        clearToolBar();
        buildToolBar(schat::DefaultToolBarLayout, false);
      }
      else {
        QString name = result->data().toString();
        if (!name.isEmpty()) {
          createAction(name, action);
        }
      }

      saveToolBarLayout();
    }
    return true;
  }
  else if (m_settingsButton->menu() == object && event->type() == QEvent::KeyPress) {
    if (static_cast<QKeyEvent *>(event)->modifiers() == Qt::AltModifier)
      return true;
  }

  return QWidget::eventFilter(object, event);
}


/*!
 * Создание кнопки на панели инструментов.
 *
 * \param name   Символьное имя кнопки.
 * \param before Кнопка будет вставлена до этой кнопки, если 0, то кнопка будет вставлена в конец.
 * \return Указатель на созданный объект QAction, или 0 в случае ошибки.
 */
QAction* SendWidget::createAction(const QString &name, QAction *before)
{
  QString lowerName = name.toLower();
  if (!m_availableActions.contains(name))
    return 0;

  QAction *action = 0;

  if (lowerName == "bold") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-bold.png"), "", this, SLOT(setBold(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_B);
    m_bold = action;
  }
  else if (lowerName == "italic") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-italic.png"), "", this, SLOT(setItalic(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_I);
    m_italic = action;
  }
  else if (lowerName == "underline") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-underline.png"), "", this, SLOT(setUnderline(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_U);
    m_underline = action;
  }
  else if (lowerName == "strike") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-strikethrough.png"), "", this, SLOT(setStrike(bool)));
    action->setCheckable(true);
    m_strike = action;
  }
  else if (lowerName == "settings") {
    action = m_toolBar->addWidget(m_settingsButton);
    action->setVisible(true);
  }
  else if (lowerName == "separator") {
    action = m_toolBar->addSeparator();
  }
  else if (lowerName == "color") {
    if (!m_color) {
      m_color = new ColorButton(m_input->textColor(), this);
      connect(m_color, SIGNAL(newColor(const QColor &)), SLOT(textColor(const QColor &)));
    }
    action = m_toolBar->addWidget(m_color);
  }
  else if (lowerName == "emoticons") {
    QMenu *menu = new QMenu(this);
    EmoticonSelector *emoticonSelector = new EmoticonSelector(this);
    QWidgetAction *act = new QWidgetAction(this);
    act->setDefaultWidget(emoticonSelector);
    menu->addAction(act);
    connect(menu, SIGNAL(aboutToShow()), emoticonSelector, SLOT(prepareList()));
    connect(menu, SIGNAL(aboutToHide()), emoticonSelector, SLOT(aboutToHide()));
    connect(emoticonSelector, SIGNAL(itemSelected(const QString &)), m_input, SLOT(insertPlainText(const QString &)));

    m_emoticons = new QToolButton(this);
    m_emoticons->setIcon(QIcon(":/images/emoticon.png"));
    m_emoticons->setAutoRaise(true);
    m_emoticons->setMenu(menu);
    m_emoticons->setPopupMode(QToolButton::InstantPopup);
    m_emoticons->setShortcut(Qt::CTRL + Qt::Key_E);
    action = m_toolBar->addWidget(m_emoticons);
  }
  else if (lowerName == "stretch") {
    QWidget *stretch = new QWidget(this);
    stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    action = m_toolBar->addWidget(stretch);
  }
  else if (lowerName == "sound") {
    m_toolBar->addAction(m_soundAction);
    action = m_soundAction;
  }
  else if (lowerName == "log") {
    action = m_toolBar->addAction(QIcon(":/images/book.png"), "", this, SLOT(log()));
    m_logAction = action;
  }
  else if (lowerName == "send") {
    m_send = new QToolButton(this);
    m_send->setAutoRaise(true);
    connect(m_send, SIGNAL(clicked()), m_input, SLOT(sendMsg()));

    if (m_bigSendButton) {
      m_send->setIcon(QIcon(":/images/go-jump-locationbar-v.png"));
      #ifdef SCHAT_WINCE_VGA
      m_send->setIconSize(QSize(55, 72));
      #else
      m_send->setIconSize(QSize(27, 36));
      #endif
    }
    else {
      m_send->setIcon(QIcon(":/images/go-jump-locationbar.png"));
      action = m_toolBar->addWidget(m_send);
    }
  }

  if (action) {
    m_availableActions.removeAll(lowerName);
    action->setData(lowerName);
    if (lowerName != "separator" && !m_availableActions.contains("separator"))
      m_availableActions << "separator";

    if (before) {
      m_toolBar->removeAction(action);
      m_toolBar->insertAction(before, action);
    }
  }

  return action;
}


QAction* SendWidget::createSettingsPage(const QIcon &icon, int page)
{
  QAction *action = m_prefMenu->addAction(icon, "", this, SLOT(settingsPage()));
  action->setData(page);
  return action;
}


/*!
 * Текущий список кнопок.
 */
QStringList SendWidget::toolBarLayout() const
{
  QStringList out;
  foreach (QAction *action, m_toolBar->actions()) {
    QString name = action->data().toString();
    if (!name.isEmpty())
      out << name;
  }
  return out;
}


/*!
 * Формирует меню с доступными кнопками для добавления на панель инструментов.
 *
 * \return Возвращает сформированное меню, или 0 если нет доступных кнопок.
 */
QMenu* SendWidget::availableActions()
{
  if (m_availableActions.isEmpty())
    return 0;

  QMenu *menu = new QMenu(tr("Add"), this);
  menu->setIcon(QIcon(":/images/edit-add.png"));

  availableAction(menu, "bold",      QIcon(":/images/format-text-bold.png"),          tr("Bold"));
  availableAction(menu, "italic",    QIcon(":/images/format-text-italic.png"),        tr("Italic"));
  availableAction(menu, "underline", QIcon(":/images/format-text-underline.png"),     tr("Underline"));
  availableAction(menu, "strike",    QIcon(":/images/format-text-strikethrough.png"), tr("Strikeout"));
  #ifdef Q_OS_WINCE
  availableAction(menu, "settings",  QIcon(":/images/configure.png"),                 tr("Preferences"));
  #endif
  availableAction(menu, "color",     QIcon(":/images/color.png"),                     tr("Color"));
  availableAction(menu, "emoticons", QIcon(":/images/emoticon.png"),                  tr("Emoticons"));
  availableAction(menu, "log",       QIcon(":/images/book.png"),                      tr("Logs"));
  #ifdef Q_OS_WINCE
  availableAction(menu, "sound",     QIcon(":/images/sound.png"),                     tr("Sound"));
  #endif
  availableAction(menu, "send",      QIcon(":/images/go-jump-locationbar.png"),       tr("Send"));

  bool separator = false;

  if (m_availableActions.contains("separator")) {
    separator = true;
    menu->addSeparator();
    menu->addAction(tr("Separator"))->setData("separator");
  }

  if (m_availableActions.contains("stretch")) {
    if (!separator)
      menu->addSeparator();
    menu->addAction(tr("Stretch"))->setData("stretch");
  }

  if (menu->actions().isEmpty()) {
    menu->deleteLater();
    return 0;
  }
  else
    return menu;
}


void SendWidget::availableAction(QMenu *menu, const QString &name, const QIcon &icon, const QString &text) const
{
  if (m_availableActions.contains(name))
    menu->addAction(icon, text)->setData(name);
}


/*!
 * Создание кнопок на панели инструментов по списку.
 *
 * \param actions   Список кнопок.
 * \param forceSend Форсированное добавление кнопки отправки, при использовании большой кнопки отправки.
 */
void SendWidget::buildToolBar(const QStringList &actions, bool forceSend)
{
  foreach (QString name, actions) {
    createAction(name);
  }

  if (forceSend && m_bigSendButton && !actions.contains("send"))
    createAction("send");

  saveToolBarLayout();
}


/*!
 * Очистка панели инструментов от кнопок.
 */
void SendWidget::clearToolBar()
{
  QList<QAction *> list = m_toolBar->actions();
  m_toolBar->clear();
  foreach (QAction *a, list) {
    QString name = a->data().toString();
    if (!name.isEmpty() && !m_availableActions.contains(name))
      m_availableActions << name;

    if (!m_permanentButtons.contains(name))
      a->deleteLater();
  }
}


/*!
 * Создание кнопки настройки.
 */
void SendWidget::createPermanentButtons()
{
  m_settingsButton = new QToolButton(this);
  m_settingsButton->setIcon(QIcon(":/images/configure.png"));
  m_settingsButton->setAutoRaise(true);
  m_settingsButton->setPopupMode(QToolButton::InstantPopup);
  m_settingsButton->setVisible(false);

  QMenu *menu = new QMenu(m_settingsButton);
  menu->installEventFilter(this);

  QWidgetAction *act = new QWidgetAction(this);
  NickEdit *nickEdit = new NickEdit(this, NickEdit::GenderButton | NickEdit::ApplyButton);
  nickEdit->setMargin(2);
  act->setDefaultWidget(nickEdit);
  menu->addAction(act);
  menu->addSeparator();

  m_prefMenu = menu->addMenu(QIcon(":/images/configure2.png"), "");
  m_profilePage      = createSettingsPage(QIcon(":/images/profile.png"), SettingsDialog::ProfilePage);
  m_networkPage      = createSettingsPage(QIcon(":/images/network.png"), SettingsDialog::NetworkPage);
  m_interfacePage    = createSettingsPage(QIcon(":/images/applications-graphics.png"), SettingsDialog::InterfacePage);
  m_emoticonsPage    = createSettingsPage(QIcon(":/images/emoticon.png"), SettingsDialog::EmoticonsPage);
  m_soundPage        = createSettingsPage(QIcon(":/images/sound.png"), SettingsDialog::SoundPage);
  m_notificationPage = createSettingsPage(QIcon(":/images/notification.png"), SettingsDialog::NotificationPage);
  m_statusesPage     = createSettingsPage(QIcon(":/images/statuses.png"), SettingsDialog::StatusesPage);
  m_miscPage         = createSettingsPage(QIcon(":/images/application-x-desktop.png"), SettingsDialog::MiscPage);

  m_aboutAction = menu->addAction("", this, SIGNAL(about()));
  if (Settings::isNewYear())
    m_aboutAction->setIcon(QIcon(":/images/schat16-ny.png"));
  else
    m_aboutAction->setIcon(QIcon(":/images/schat16.png"));

  menu->addSeparator();
  m_quitAction = menu->addAction(QIcon(":/images/exit.png"), "", this, SIGNAL(closeChat()));
  #if QT_VERSION >= 0x040600
  m_quitAction->setShortcut(QKeySequence::Quit);
  #endif

  m_settingsButton->setMenu(menu);
  m_soundAction = new SoundAction(this);
  m_permanentButtons << "settings" << "sound";
  m_availableActions << m_permanentButtons;
}


/*!
 * Инициализация панели инструментов.
 */
void SendWidget::initToolBar()
{
  m_toolBar = new QToolBar(this);
  m_toolBar->setAttribute(Qt::WA_NoSystemBackground, false);
  m_toolBar->installEventFilter(this);
  #if !defined(Q_OS_WINCE)
  m_toolBar->setIconSize(QSize(16, 16));
  #elif defined(SCHAT_WINCE_VGA)
  m_toolBar->setIconSize(QSize(36, 36));
  #endif
  setStyleSheet();
  buildToolBar(SimpleSettings->getList("ToolBarLayout"));
}


/*!
 * Слияние формата.
 */
void SendWidget::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = m_input->textCursor();

  cursor.mergeCharFormat(format);
  m_input->mergeCurrentCharFormat(format);
}


void SendWidget::retranslateUi()
{
  m_settingsButton->setToolTip(tr("Menu"));
  m_prefMenu->setTitle(tr("Preferences"));
  m_aboutAction->setText(tr("About Simple Chat..."));
  m_quitAction->setText(tr("Quit"));
  m_soundAction->retranslateUi();

  m_profilePage->setText(tr("Personal data..."));
  m_networkPage->setText(tr("Network..."));
  m_interfacePage->setText(tr("Interface..."));
  m_emoticonsPage->setText(tr("Emoticons..."));
  m_soundPage->setText(tr("Sounds..."));
  m_notificationPage->setText(tr("Notifications..."));
  m_statusesPage->setText(tr("Statuses..."));
  m_miscPage->setText(tr("Others..."));

  if (m_bold)      m_bold->setText(tr("Bold"));
  if (m_italic)    m_italic->setText(tr("Italic"));
  if (m_underline) m_underline->setText(tr("Underline"));
  if (m_strike)    m_strike->setText(tr("Strikeout"));
  if (m_emoticons) m_emoticons->setToolTip(tr("Add emoticon"));
  if (m_logAction) m_logAction->setText(tr("Logs"));
  if (m_send)      m_send->setToolTip(tr("Send"));

}


void SendWidget::saveToolBarLayout()
{
  SimpleSettings->setList("ToolBarLayout", toolBarLayout());
}
