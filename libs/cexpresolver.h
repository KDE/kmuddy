//
// C++ Interface: cexpresolver
//
// Description: Variable/function resolver for the expression parser.
//
/*
Copyright 2006-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CEXPRESOLVER_H
#define CEXPRESOLVER_H

#include <arith-exp.h>
#include <cvalue.h>
#include <kmuddy_export.h>

class cCmdQueue;

/**
Resolver for variables and functions, used by the expression processor.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cExpResolver : public arith_exp_server {
  public:
    cExpResolver (int _sess);
    virtual cValue get (QString varName);
    virtual cValue function_call (const QString &functionName, list<cValue> &arguments);
    void setQueue (cCmdQueue *q) { queue = q; };
  protected:
    int sess;
    cCmdQueue *queue;
};


#endif
