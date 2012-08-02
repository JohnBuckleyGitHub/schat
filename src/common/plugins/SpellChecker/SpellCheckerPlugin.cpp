#include <QApplication>
#include <QtPlugin>
#include <QTimer>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "SpellcheckerPlugin.h"
#include "SpellcheckerPlugin_p.h"
#include "SpellcheckerPage.h"
#include "sglobal.h"

SpellcheckerPluginImpl::SpellcheckerPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  QTimer::singleShot(0, this, SLOT(start()));
}

SpellcheckerPluginImpl::~SpellcheckerPluginImpl()
{

}

void SpellcheckerPluginImpl::start()
{
  QStringList defaultDicts;
  defaultDicts.append("en_US");
  defaultDicts.append(QLocale().name().toUtf8().constData());

  ChatCore::settings()->setLocalDefault(LS("Spellchecker/EnabledDicts"), defaultDicts);

  Spellchecker::instance();
  SettingsTabHook::add(new SpellcheckerPageCreator(this));

}

ChatPlugin *SpellcheckerPlugin::create()
{
  m_plugin = new SpellcheckerPluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Spellchecker, SpellcheckerPlugin);
