//
// C++ Interface: cnotifymanager
//
// Description: Notification manager for the /notify macro
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005-2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CNOTIFYMANAGER_H
#define CNOTIFYMANAGER_H

#include <qobject.h>

#include <list>

class cEventNotification;
class QTcpSocket;

/**
Notification manager. Used by the /notify macro.

@author Tomas Mecir
*/
class cNotifyManager : public QObject
{
Q_OBJECT
public:
  cNotifyManager ();

  ~cNotifyManager();
  
  /** send information to external IP port */
  void doNotify(const int  port, QString &ip_data);

public slots:
  /** information sent to external IP port */
  void portNotified (cEventNotification *event_notify);
    
protected:
  
  std::list<cEventNotification *> notifications;
};

/**
class cEventNotification is used to send notification data to externally running scripts using 
the non-blocking TCP/IP functionality of QSocket

 *@author Alex Bache
 */

class cEventNotification: public QObject
{
  Q_OBJECT

  private:
    QTcpSocket *sock;
    int              port;
    std::list<QString>    data_list;
     
  public slots:
     // Connected to IP port successfully
    void connected();
     
     // Wrote some data to the IP port
    void wroteAll();
     
     // Connection closed
    void connectionClosed();
     
     // An error occured
    void error();
     
  signals:
     // Communication with IP port finished
    void finished(cEventNotification *item);
     
  public:
    cEventNotification(const int      ip_port,
                       const QString &ip_data);
     
    ~cEventNotification();
     
     // Send the information to the specified IP port
    void send();
     
     // Which port are we sending information to?
    int port_num() const   { return port; }
     
     // Queue data for sending at next opportunity
    void queue(const QString &ip_data);
  private:
    void writeNext ();
     
}; // cEventNotification
  


#endif
