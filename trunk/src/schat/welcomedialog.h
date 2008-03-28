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
  WelcomeDialog(QWidget *parent = 0);
  QString nick();
  QString fullName();
  quint8 sex();
};

#endif /*WELCOMEDIALOG_H_*/
