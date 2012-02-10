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

#ifndef SENDWIDGET_H_
#define SENDWIDGET_H_

#include <QPointer>
#include <QTextCharFormat>
#include <QWidget>

#include "translatewidget.h"
#include "widget/inputwidget.h"

class ColorButton;
class EmoticonSelector;
class InputWidget;
class QAction;
class QToolBar;
class QToolButton;
class SoundAction;

/*!
 * \brief Виджет полностью берущий на себя ввода текста.
 */
class SendWidget : public TranslateWidget
{
  Q_OBJECT

public:
  SendWidget(QWidget *parent = 0);
  inline QAction* aboutAction() const { return m_aboutAction; }
  inline QAction* quitAction() const  { return m_quitAction; }
  inline void clear()                 { m_input->clearMsg(); }
  QPair<SoundAction*, bool> soundAction() const;
  QToolButton* settingsButton() const;
  void setInputFocus();
  void setStyleSheet();

signals:
  void about();
  void closeChat();
  void needCopy();
  void sendMsg(const QString &message);
  void showSettingsPage(int page);

public slots:
  void copy()                          { m_input->copy(); }
  void insertHtml(const QString &text) { m_input->insertHtml(text); }

private slots:
  void cursorPositionChanged();
  void log();
  void setBold(bool b);
  void setItalic(bool b);
  void setSettings();
  void setStrike(bool b);
  void settingsPage();
  void setUnderline(bool b);
  void textColor(const QColor &color);

private:
  bool eventFilter(QObject *object, QEvent *event);
  QAction* createAction(const QString &name, QAction *before = 0);
  QAction* createSettingsPage(const QIcon &icon, int page);
  QMenu* availableActions();
  QStringList toolBarLayout() const;
  void availableAction(QMenu *menu, const QString &name, const QIcon &icon, const QString &text) const;
  void buildToolBar(const QStringList &actions, bool forceSend = true);
  void clearToolBar();
  void createPermanentButtons();
  void initToolBar();
  void mergeFormat(const QTextCharFormat &format);
  void retranslateUi();
  void saveToolBarLayout();

  const bool m_bigSendButton;        ///< Опция настроек "BigSendButton".
  InputWidget* const m_input;        ///< Виджет ввода текста.
  QAction *m_aboutAction;            ///< "О Simple Chat...".
  QAction *m_emoticonsPage;
  QAction *m_interfacePage;
  QAction *m_miscPage;
  QAction *m_networkPage;
  QAction *m_notificationPage;
  QAction *m_profilePage;
  QAction *m_quitAction;             ///< "Выход".
  QAction *m_soundPage;
  QAction *m_statusesPage;
  QMenu *m_prefMenu;
  QPointer<ColorButton> m_color;     ///< Кнопка для цветового веделения.
  QPointer<QAction> m_bold;          ///< "Полужирный".
  QPointer<QAction> m_italic;        ///< "Курсив".
  QPointer<QAction> m_logAction;
  QPointer<QAction> m_strike;        ///< "Зачёркнутый".
  QPointer<QAction> m_underline;     ///< "Подчёркнутый".
  QPointer<QToolButton> m_emoticons; ///< Кнопка для выбора смайлов.
  QPointer<QToolButton> m_send;      ///< Кнопка отправки.
  QStringList m_availableActions;    ///< Список кнопок которые можно добавить на панель инструментов.
  QStringList m_permanentButtons;    ///< Список не удаляемых кнопок.
  QToolBar *m_toolBar;               ///< Панель инструментов.
  QToolButton *m_settingsButton;     ///< Кнопка с меню настроек.
  SoundAction *m_soundAction;        ///< Действие для включения/выключения звука.
};

#endif /*SENDWIDGET_H_*/
