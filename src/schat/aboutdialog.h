/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QDialog>

class QLabel;
class QTabWidget;


/**
 * Класс AboutDialog
 */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  AboutDialog(QWidget *parent = 0);

private:
  QPushButton *closeButton;
  QTabWidget *tabWidget;
};


/**
 * Класс MainTab
 */
class MainTab : public QWidget
{
  Q_OBJECT

public:
  MainTab(QWidget *parent = 0); 
};


/**
 * Класс LicenseTab
 */
class LicenseTab : public QWidget
{
  Q_OBJECT

public:
  LicenseTab(QWidget *parent = 0);
};

#endif /*ABOUTDIALOG_H_*/
