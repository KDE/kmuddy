/***************************************************************************
                          dlgstatistics.cpp  -  Statistics dialog
                             -------------------
    begin                : Pi mar 14 2003
    copyright            : (C) 2003 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dlgstatistics.h"

#include "cactionmanager.h"
#include "cconnection.h"
#include "csessionmanager.h"
#include "cstatus.h"
#include "ctelnet.h"
#include "ctextprocessor.h"
#ifdef HAVE_MXP
#include "cmxpmanager.h"
#endif

#include <QLabel>
#include <QTreeWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <klocale.h>

dlgStatistics::dlgStatistics (QWidget *parent)
  : QDockWidget (parent), cActionBase ("dialog-statistics", 0)
{
  //initial size
  //setSize (QSize (250, 300));
  setWindowTitle (i18n ("Connection statistics"));

  //main widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);
  setWidget (page);

  setFocusPolicy (Qt::StrongFocus);

  QLabel *label = new QLabel (i18n ("&Statistics, updated every second:"), page);
  view = new QTreeWidget (page);
  label->setBuddy (view);
  view->setHeaderLabels (QStringList() << i18n ("Counter") << i18n ("Value"));
  view->setUniformRowHeights (true);
  view->setRootIsDecorated (false);
  view->setSelectionMode (QAbstractItemView::NoSelection);
  
  layout->setSpacing (5);
  layout->addWidget (label);
  layout->addWidget (view);

  //establish auto-updating of data
  updateTimer = new QTimer;
  connect (updateTimer, SIGNAL (timeout()), this, SLOT (update()));
  updateTimer->start (1000);

  //no focus - we don't want this dialog to get focus
  setFocusPolicy (Qt::NoFocus);
  view->setFocusPolicy (Qt::NoFocus);

  addEventHandler ("session-activated", 50, PT_INT);
}

dlgStatistics::~dlgStatistics ()
{
  removeEventHandler ("session-activated");

  updateTimer->stop ();
  delete updateTimer;
}

void dlgStatistics::eventIntHandler (QString event, int, int, int)
{
  if (event == "session-activated")
    update ();
}


void dlgStatistics::update ()
{
  view->clear ();

  //fetch required information
  int sess = cSessionManager::self()->activeSession();
  if (!sess) return;
  cActionManager *am = cActionManager::self();
  cTelnet *telnet = dynamic_cast<cTelnet *>(am->object ("telnet", sess));
  cConnection *connection = dynamic_cast<cConnection *>(am->object ("connection", sess));
  cStatus *status = dynamic_cast<cStatus *>(am->object ("status", sess));
  cTextProcessor *textproc = dynamic_cast<cTextProcessor *>(am->object ("textproc", sess));
  int compr = telnet->compressedBytes();
  int decompr = telnet->uncompressedBytes();
  QString connTime = status->connTimeString();
  int sentbytes = telnet->sentBytes();
  int sentcmds = connection->sentCommands();
  int recvlines = textproc->linesReceived();
  bool usingmccp = telnet->usingMCCP();
  int mccpver = telnet->MCCPVer();
  bool usingmsp = telnet->usingMSP();
#ifdef HAVE_MXP
  cMXPManager *mxp = dynamic_cast<cMXPManager *>(am->object ("mxpmanager", sess));
  bool usingmxp = mxp->isMXPActive();
#endif
  
  //fill in those information

  QList<QTreeWidgetItem *> items;
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Using MCCP") << 
      (usingmccp ? i18n ("Yes") : i18n ("No")));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("MCCP version") << 
      (usingmccp ? QString::number (mccpver) : i18n ("N/A")));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Using MSP") << 
      (usingmsp ? i18n ("Yes") : i18n ("No")));
#ifdef HAVE_MXP
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Using MXP") << 
      (usingmxp ? i18n ("Yes") : i18n ("No")));
#endif
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Sent bytes") << 
      QString::number (sentbytes));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Sent commands") << 
      QString::number (sentcmds));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Received bytes") << 
      QString::number (compr));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("... after decompression") << 
      QString::number (decompr));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Received lines") << 
      QString::number (recvlines));
  items << new QTreeWidgetItem (view, QStringList() << i18n ("Connection time") << 
      connTime);

  view->addTopLevelItems (items);
}

#include "dlgstatistics.moc"

