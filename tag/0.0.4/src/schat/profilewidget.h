/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROFILEWIDGET_H_
#define PROFILEWIDGET_H_

#include <QWidget>

class Profile;
class QComboBox;
class QLabel;
class QLineEdit;

class ProfileWidget : public QWidget {
  Q_OBJECT

public:
  ProfileWidget(Profile *p, QWidget *parent = 0);
  inline bool isModifiled() { return modifiled; }
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private slots:
  void validateNick(const QString &text);
  
private:
  bool modifiled;
  Profile *profile;
  QComboBox *sexBox;
  QLabel *nameLabel;
  QLabel *nickLabel;
  QLabel *sexLabel;
  QLineEdit *nameEdit;
  QLineEdit *nickEdit;
};

#endif /*PROFILEWIDGET_H_*/
