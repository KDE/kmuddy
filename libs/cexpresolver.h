//
// C++ Interface: cexpresolver
//
// Description: Variable/function resolver for the expression parser.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
