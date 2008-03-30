/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef WELCOMEDIALOG_H_
#define WELCOMEDIALOG_H_

#include <QDialog>

#include "ui_welcomedialog.h"

class SChatWindow;

class WelcomeDialog : public QDialog, public Ui::WelcomeDialog
{
  Q_OBJECT

public:
  WelcomeDialog(const QString &nick, const QString &name, quint8 sex, QWidget *parent = 0);
  bool welcome();
  QString fullName();
  QString nick();
  quint8 sex();
};

#endif /*WELCOMEDIALOG_H_*/
