/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QString>
#include <QWidget>

class QComboBox;
class QLabel;
class QSpinBox;
class QStandardItemModel;
class Settings;

class NetworkWidget : public QWidget {
  Q_OBJECT

public:
  NetworkWidget(Settings *settings, QWidget *parent = 0);

private slots:
  void activated(int index);
  void currentIndexChanged(int index);
  void editTextChanged(const QString &text);
 
private:
  void createList();
  void init();
  
  QComboBox *m_selectCombo;
  QLabel *m_infoLabel;
  QLabel *m_portLabel;
  QSpinBox *m_portBox;
  QString m_networksPath;
  Settings *m_settings;
};

#endif /*NETWORKWIDGET_H_*/
