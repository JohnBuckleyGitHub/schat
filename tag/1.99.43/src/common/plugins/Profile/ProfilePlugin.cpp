/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QtPlugin>

#include "ChatCore.h"
#include "CountryField.h"
#include "Profile.h"
#include "ProfileChatView.h"
#include "ProfilePlugin.h"
#include "ProfilePlugin_p.h"
#include "sglobal.h"
#include "Tr.h"
#include "Translation.h"

class ProfilePluginTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(ProfilePluginTr)

public:
  ProfilePluginTr() : Tr() { m_prefix = LS("field-"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("country"))    return tr("Country");
    else if (key == LS("city"))  return tr("City");
    else if (key == LS("site"))  return tr("Site");
    else if (key == LS("email")) return tr("E-Mail");
    else if (key == LS("isp"))   return tr("ISP");
    return QString();
  }
};


class CountryTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(CountryTr)

public:
  CountryTr() : Tr() { m_prefix = LS("country-"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key.size() != 2)
      return QString();

    char second = key.at(1).toLatin1();
    switch (key.at(0).toLatin1()) {
      case 'a':
        switch (second) {
          case 'd': return tr("Andorra");
          case 'e': return tr("United Arab Emirates");
          case 'f': return tr("Afghanistan");
          case 'g': return tr("Antigua and Barbuda");
          case 'i': return tr("Anguilla");
          case 'l': return tr("Albania");
          case 'm': return tr("Armenia");
          case 'o': return tr("Angola");
          case 'q': return tr("Antarctica");
          case 'r': return tr("Argentina");
          case 's': return tr("American Samoa");
          case 't': return tr("Austria");
          case 'u': return tr("Australia");
          case 'w': return tr("Aruba");
          case 'x': return tr("Aland Islands");
          case 'z': return tr("Azerbaijan");
        }
        break;

      case 'b':
        switch (second) {
          case 'a': return tr("Bosnia and Herzegovina");
          case 'b': return tr("Barbados");
          case 'd': return tr("Bangladesh");
          case 'e': return tr("Belgium");
          case 'f': return tr("Burkina Faso");
          case 'g': return tr("Bulgaria");
          case 'h': return tr("Bahrain");
          case 'i': return tr("Burundi");
          case 'j': return tr("Benin");
          case 'l': return tr("Saint Barthelemy");
          case 'm': return tr("Bermuda");
          case 'n': return tr("Brunei");
          case 'o': return tr("Bolivia");
          case 'q': return tr("Caribbean Netherlands");
          case 'r': return tr("Brazil");
          case 's': return tr("Bahamas");
          case 't': return tr("Bhutan");
          case 'v': return tr("Bouvet Island");
          case 'w': return tr("Botswana");
          case 'y': return tr("Belarus");
          case 'z': return tr("Belize");
        }
        break;

      case 'c':
        switch (second) {
          case 'a': return tr("Canada");
          case 'c': return tr("Cocos (Keeling) Islands");
          case 'd': return tr("Democratic Republic of the Congo");
          case 'f': return tr("Central African Republic");
          case 'g': return tr("Congo");
          case 'h': return tr("Switzerland");
          case 'i': return tr("Cote d'Ivoire");
          case 'k': return tr("Cook Islands");
          case 'l': return tr("Chile");
          case 'm': return tr("Cameroon");
          case 'n': return tr("China");
          case 'o': return tr("Colombia");
          case 'r': return tr("Costa Rica");
          case 'u': return tr("Cuba");
          case 'v': return tr("Cape Verde");
          case 'w': return tr("Curacao");
          case 'x': return tr("Christmas Island");
          case 'y': return tr("Cyprus");
          case 'z': return tr("Czech Republic");
        }
        break;

      case 'd':
        switch (second) {
          case 'e': return tr("Germany");
          case 'j': return tr("Djibouti");
          case 'k': return tr("Denmark");
          case 'm': return tr("Dominica");
          case 'o': return tr("Dominican Republic");
          case 'z': return tr("Algeria");
        }
        break;

      case 'e':
        switch (second) {
          case 'c': return tr("Ecuador");
          case 'e': return tr("Estonia");
          case 'g': return tr("Egypt");
          case 'h': return tr("Western Sahara");
          case 'r': return tr("Eritrea");
          case 's': return tr("Spain");
          case 't': return tr("Ethiopia");
        }
        break;

      case 'f':
        switch (second) {
          case 'i': return tr("Finland");
          case 'j': return tr("Fiji");
          case 'k': return tr("Falkland Islands");
          case 'm': return tr("Federated States of Micronesia");
          case 'o': return tr("Faroe Islands");
          case 'r': return tr("France");
        }
        break;

      case 'g':
        switch (second) {
          case 'a': return tr("Gabon");
          case 'b': return tr("United Kingdom");
          case 'd': return tr("Grenada");
          case 'e': return tr("Georgia");
          case 'f': return tr("French Guiana");
          case 'g': return tr("Guernsey");
          case 'h': return tr("Ghana");
          case 'i': return tr("Gibraltar");
          case 'l': return tr("Greenland");
          case 'm': return tr("Gambia");
          case 'n': return tr("Guinea");
          case 'p': return tr("Guadeloupe");
          case 'q': return tr("Equatorial Guinea");
          case 'r': return tr("Greece");
          case 's': return tr("South Georgia and the South Sandwich Islands");
          case 't': return tr("Guatemala");
          case 'u': return tr("Guam");
          case 'w': return tr("Guinea-Bissau");
          case 'y': return tr("Guyana");
        }
        break;

      case 'h':
        switch (second) {
          case 'k': return tr("Hong Kong");
          case 'm': return tr("Heard Island and McDonald Islands");
          case 'n': return tr("Honduras");
          case 'r': return tr("Croatia");
          case 't': return tr("Haiti");
          case 'u': return tr("Hungary");
        }
        break;

      case 'i':
        switch (second) {
          case 'd': return tr("Indonesia");
          case 'e': return tr("Ireland");
          case 'l': return tr("Israel");
          case 'm': return tr("Isle of Man");
          case 'n': return tr("India");
          case 'o': return tr("British Indian Ocean Territory");
          case 'q': return tr("Iraq");
          case 'r': return tr("Iran");
          case 's': return tr("Iceland");
          case 't': return tr("Italy");
        }
        break;

      case 'j':
        switch (second) {
          case 'e': return tr("Jersey");
          case 'm': return tr("Jamaica");
          case 'o': return tr("Jordan");
          case 'p': return tr("Japan");
        }
        break;

      case 'k':
        switch (second) {
          case 'e': return tr("Kenya");
          case 'g': return tr("Kyrgyzstan");
          case 'h': return tr("Cambodia");
          case 'i': return tr("Kiribati");
          case 'm': return tr("Comoros");
          case 'n': return tr("Saint Kitts and Nevis");
          case 'p': return tr("North Korea");
          case 'r': return tr("South Korea");
          case 'w': return tr("Kuwait");
          case 'y': return tr("Cayman Islands");
          case 'z': return tr("Kazakhstan");
        }
        break;

      case 'l':
        switch (second) {
          case 'a': return tr("Laos");
          case 'b': return tr("Lebanon");
          case 'c': return tr("Saint Lucia");
          case 'i': return tr("Liechtenstein");
          case 'k': return tr("Sri Lanka");
          case 'r': return tr("Liberia");
          case 's': return tr("Lesotho");
          case 't': return tr("Lithuania");
          case 'u': return tr("Luxembourg");
          case 'v': return tr("Latvia");
          case 'y': return tr("Libya");
        }
        break;

      case 'm':
        switch (second) {
          case 'a': return tr("Morocco");
          case 'c': return tr("Monaco");
          case 'd': return tr("Moldova");
          case 'e': return tr("Montenegro");
          case 'f': return tr("Saint Martin (French part)");
          case 'g': return tr("Madagascar");
          case 'h': return tr("Marshall Islands");
          case 'k': return tr("Republic of Macedonia");
          case 'l': return tr("Mali");
          case 'm': return tr("Myanmar");
          case 'n': return tr("Mongolia");
          case 'o': return tr("Macao");
          case 'p': return tr("Northern Mariana Islands");
          case 'q': return tr("Martinique");
          case 'r': return tr("Mauritania");
          case 's': return tr("Montserrat");
          case 't': return tr("Malta");
          case 'u': return tr("Mauritius");
          case 'v': return tr("Maldives");
          case 'w': return tr("Malawi");
          case 'x': return tr("Mexico");
          case 'y': return tr("Malaysia");
          case 'z': return tr("Mozambique");
        }
        break;

      case 'n':
        switch (second) {
          case 'a': return tr("Namibia");
          case 'c': return tr("New Caledonia");
          case 'e': return tr("Niger");
          case 'f': return tr("Norfolk Island");
          case 'g': return tr("Nigeria");
          case 'i': return tr("Nicaragua");
          case 'l': return tr("Netherlands");
          case 'o': return tr("Norway");
          case 'p': return tr("Nepal");
          case 'r': return tr("Nauru");
          case 'u': return tr("Niue");
          case 'z': return tr("New Zealand");
        }
        break;

      case 'o':
        switch (second) {
          case 'm': return tr("Oman");
        }
        break;

      case 'p':
        switch (second) {
          case 'a': return tr("Panama");
          case 'e': return tr("Peru");
          case 'f': return tr("French Polynesia");
          case 'g': return tr("Papua New Guinea");
          case 'h': return tr("Philippines");
          case 'k': return tr("Pakistan");
          case 'l': return tr("Poland");
          case 'm': return tr("Saint Pierre and Miquelon");
          case 'n': return tr("Pitcairn");
          case 'r': return tr("Puerto Rico");
          case 's': return tr("Palestinian Territory");
          case 't': return tr("Portugal");
          case 'w': return tr("Palau");
          case 'y': return tr("Paraguay");
        }
        break;

      case 'q':
        switch (second) {
          case 'a': return tr("Qatar");
        }
        break;

      case 'r':
        switch (second) {
          case 'e': return tr("Reunion");
          case 'o': return tr("Romania");
          case 's': return tr("Serbia");
          case 'u': return tr("Russia");
          case 'w': return tr("Rwanda");
        }
        break;

      case 's':
        switch (second) {
          case 'a': return tr("Saudi Arabia");
          case 'b': return tr("Solomon Islands");
          case 'c': return tr("Seychelles");
          case 'd': return tr("Sudan");
          case 'e': return tr("Sweden");
          case 'g': return tr("Singapore");
          case 'h': return tr("Saint Helena, Ascension and Tristan da Cunha");
          case 'i': return tr("Slovenia");
          case 'j': return tr("Svalbard and Jan Mayen");
          case 'k': return tr("Slovakia");
          case 'l': return tr("Sierra Leone");
          case 'm': return tr("San Marino");
          case 'n': return tr("Senegal");
          case 'o': return tr("Somalia");
          case 'r': return tr("Suriname");
          case 's': return tr("South Sudan");
          case 't': return tr("Sao Tome and Principe");
          case 'v': return tr("El Salvador");
          case 'x': return tr("Sint Maarten");
          case 'y': return tr("Syria");
          case 'z': return tr("Swaziland");
        }
        break;

      case 't':
        switch (second) {
          case 'c': return tr("Turks and Caicos Islands");
          case 'd': return tr("Chad");
          case 'f': return tr("French Southern Territories");
          case 'g': return tr("Togo");
          case 'h': return tr("Thailand");
          case 'j': return tr("Tajikistan");
          case 'k': return tr("Tokelau");
          case 'l': return tr("Timor-Leste");
          case 'm': return tr("Turkmenistan");
          case 'n': return tr("Tunisia");
          case 'o': return tr("Tonga");
          case 'r': return tr("Turkey");
          case 't': return tr("Trinidad and Tobago");
          case 'v': return tr("Tuvalu");
          case 'w': return tr("Taiwan");
          case 'z': return tr("Tanzania");
        }
        break;

      case 'u':
        switch (second) {
          case 'a': return tr("Ukraine");
          case 'g': return tr("Uganda");
          case 'm': return tr("United States Minor Outlying Islands");
          case 's': return tr("United States");
          case 'y': return tr("Uruguay");
          case 'z': return tr("Uzbekistan");
        }
        break;

      case 'v':
        switch (second) {
          case 'a': return tr("Vatican City");
          case 'c': return tr("Saint Vincent and the Grenadines");
          case 'e': return tr("Venezuela");
          case 'g': return tr("British Virgin Islands");
          case 'i': return tr("United States Virgin Islands");
          case 'n': return tr("Viet Nam");
          case 'u': return tr("Vanuatu");
        }
        break;

      case 'w':
        switch (second) {
          case 'f': return tr("Wallis and Futuna");
          case 's': return tr("Samoa");
        }
        break;

      case 'y':
        switch (second) {
          case 'e': return tr("Yemen");
          case 't': return tr("Mayotte");
        }
        break;

      case 'z':
        switch (second) {
          case 'a': return tr("South Africa");
          case 'm': return tr("Zambia");
          case 'w': return tr("Zimbabwe");
        }
        break;

      default:
        break;
    }

    return QString();
  }
};


ProfilePluginImpl::ProfilePluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  m_tr = new ProfilePluginTr();
  m_country = new CountryTr();
  new ProfileChatView(this);

  Profile::addField(LS("country"), 1900);
  Profile::addField(LS("city"),    2000);
  Profile::addField(LS("site"),    4000);
  Profile::addField(LS("email"),   4050);

  ProfileFieldFactory::add(new CountryFieldCreator());

  ChatCore::translation()->addOther(LS("profile"));
}


ProfilePluginImpl::~ProfilePluginImpl()
{
  delete m_tr;
  delete m_country;
}


ChatPlugin *ProfilePlugin::create()
{
  m_plugin = new ProfilePluginImpl(this);
  return m_plugin;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Profile, ProfilePlugin);
#endif
