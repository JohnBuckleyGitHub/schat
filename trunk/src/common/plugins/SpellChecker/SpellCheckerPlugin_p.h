#ifndef SPELLCHECKERPLUGIN_P_H_
#define SPELLCHECKERPLUGIN_P_H_

#include <QDebug>

#include "Spellchecker.h"

#include "plugins/ChatPlugin.h"
#include "ui/tabs/SettingsTabHook.h"

class Spellchecker;

class SpellcheckerPluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  SpellcheckerPluginImpl(QObject *parent);
  ~SpellcheckerPluginImpl();

signals:

protected:

public slots:
//  void settingsChanged(const QString &key, const QVariant &value);
  void start();

private:
    Spellchecker *m_spellchecker;

};

#endif /* SPELLCHECKERPLUGIN_P_H_ */
