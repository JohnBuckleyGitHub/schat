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

#ifndef INPUTWIDGET_H_
#define INPUTWIDGET_H_

#include <QTextEdit>

class ColorButton;
class QToolBar;
class QWidgetAction;

class InputWidget : public QTextEdit
{
  Q_OBJECT

public:
  enum Actions {
    Bold,
    Italic,
    Underline,
    Strike
  };

  InputWidget(QWidget *parent = 0);
  ColorButton *color();
  inline QAction *action(Actions action)    { return m_format.at(action); }
  inline const QStringList& history() const { return m_history; }
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  void setMsg(int index);

signals:
  void focusOut();
  void send(const QString &text);

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void resizeEvent(QResizeEvent *event);
  void showEvent(QShowEvent *event);

public slots:
  void clear();
  void paste();
  void send();

private slots:
  void cursorPositionChanged();
  void menuTriggered(QAction *action);
  void setBold(bool bold);
  void setItalic(bool italic);
  void setStrike(bool strike);
  void setTextColor(const QColor &color);
  void setUnderline(bool underline);
  void textChanged();

private:
  bool bypass(QKeyEvent *event);
  int textHeight(int lines = 0) const;
  void createActions();
  void mergeFormat(const QTextCharFormat &format);
  void nextMsg();
  void prevMsg();
  void retranslateUi();

  bool m_resizable;          ///< true если виджет может автоматически изменять свои размеры.
  ColorButton *m_color;      ///< Кнопка выбора цвета.
  int m_current;             ///< Текущее сообщение в истории.
  int m_lines;               ///< Минимальная высота текста в строчках.
  QAction *m_clear;          ///< Clear.
  QAction *m_copy;           ///< Copy.
  QAction *m_cut;            ///< Cut.
  QAction *m_paste;          ///< Paste.
  QAction *m_selectAll;      ///< Select All.
  QList<QAction *> m_format; ///< Действия связанные с форматированием текста.
  QMenu *m_menu;             ///< Контекстное меню.
  QStringList m_history;     ///< Отправленные сообщения.
  QTextCharFormat m_default; ///< Формат текста по умолчанию.
  QToolBar *m_toolBar;       ///< Панель инструментов в контекстом меню.
  QWidgetAction *m_action;   ///< Действие для добавления панели инструментов в меню.
};

#endif /* INPUTWIDGET_H_ */
