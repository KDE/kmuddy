//
// C++ Interface: vartrigplugin
//
// Description: vartrigplugin
//
/*
Copyright 2009-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VARTRIGPLUGIN_H
#define VARTRIGPLUGIN_H

#include <cactionbase.h>
#include <cplugin.h>
#include <QVariantList>

class cVarTrigPlugin : public cPlugin, public cActionBase
{
public:
  cVarTrigPlugin (QObject *, const QVariantList &);
  virtual ~cVarTrigPlugin ();

protected:
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &);
};


#endif // SCRIPTINGPLUGIN_H

