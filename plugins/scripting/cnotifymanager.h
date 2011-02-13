//
// C++ Interface: cnotifymanager
//
// Description: Notification manager for the /notify macro
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>
Copyright 2005 Alex Bache <alexbache@ntlworld.com>

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
