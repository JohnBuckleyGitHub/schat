#ifndef SENDFILEPAGE_H_
#define SENDFILEPAGE_H_

#include <QList>
#include <QTableWidget>
#include "SpellBackend.h"
#include "Spellchecker.h"

#include "ui/tabs/SettingsTabHook.h"

class QLabel;
class SpellcheckerPluginImpl;
class QTableWidget;
class Spellchecker;

class SpellcheckerPage : public SettingsPage
{
  Q_OBJECT

public:
  SpellcheckerPage(SpellcheckerPluginImpl *plugin, QWidget *parent = 0);
  void retranslateUi();

protected slots:
  void modified(QTableWidgetItem *item);

private:
  QLabel *m_label;              ///< Надпись вверху страницы.
  QTableWidget *m_table;

  SpellcheckerPluginImpl *m_plugin; ///< Указатель на объект плагина.
};


class SpellcheckerPageCreator : public SettingsPageCreator
{
public:
  SpellcheckerPageCreator(SpellcheckerPluginImpl *plugin)
  : SettingsPageCreator(5100)
  , m_plugin(plugin)
  {}

  SettingsPage* page(QWidget *parent = 0);

private:
  SpellcheckerPluginImpl *m_plugin; ///< Указатель на объект плагина.

};

#endif /* SENDFILEPAGE_H_ */
