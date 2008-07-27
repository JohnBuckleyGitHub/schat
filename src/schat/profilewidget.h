/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROFILEWIDGET_H_
#define PROFILEWIDGET_H_

#include <QWidget>

class AbstractProfile;
class QComboBox;
class QLabel;
class QLineEdit;

class ProfileWidget : public QWidget {
  Q_OBJECT

public:
  ProfileWidget(AbstractProfile *p, QWidget *parent = 0);
  inline bool isModifiled() { return m_modifiled; }
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private slots:
  void validateNick(const QString &text);
  
private:
  AbstractProfile *m_profile;
  bool m_modifiled;
  QComboBox *m_gender;
  QLabel *m_genderLabel;
  QLabel *m_nameLabel;
  QLabel *m_nickLabel;
  QLineEdit *m_name;
  QLineEdit *m_nick;
};

#endif /*PROFILEWIDGET_H_*/
