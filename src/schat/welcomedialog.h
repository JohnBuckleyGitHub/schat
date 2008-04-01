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
  bool hideWelcome() { return askCheckBox->isChecked(); }
  QString fullName() { return nameEdit->text(); }
  QString nick() { return nickEdit->text(); }
  QString server() { return serverEdit->text(); }
  quint8 sex();
  void setHideWelcome(bool w) { askCheckBox->setChecked(w); }
  void setServer(const QString &s) { serverEdit->setText(s); }

private slots:
  void changeIcon(bool s);
};

#endif /*WELCOMEDIALOG_H_*/
