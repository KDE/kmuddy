//
// C++ Implementation: cnotifymanager
//
// Description: Notification manager for the /notify macro
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005-2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cnotifymanager.h"

#include "kdebug.h"
#include <QTcpSocket>

cNotifyManager::cNotifyManager ()
{
  notifications.clear();
}

cNotifyManager::~cNotifyManager()
{
}

/** send information to external IP port */
void cNotifyManager::doNotify(const int  port,
                             QString   &ip_data)
{
  std::list <cEventNotification *>::iterator ptr  = notifications.begin();
  std::list <cEventNotification *>::iterator last = notifications.end();

  while ((ptr                != last)&&
          ((*ptr)->port_num() != port)  )
  {
    ++ptr;
  } // endwhile more 'in progress' event notifications to search

  if (ptr == last)
  {
    cEventNotification *notify_event = new cEventNotification(port,ip_data);

    notifications.push_back(notify_event);

    connect(notify_event,
            SIGNAL (finished(cEventNotification *)),
            this,
            SLOT (portNotified(cEventNotification *)));

    notify_event->send();

  } // endif no other notifications in progress to this IP port at this time
  else
  {
    // Queue data for sending later
    (*ptr)->queue(ip_data);

  } // endelse need to queue notification

}


void cNotifyManager::portNotified(cEventNotification *event_notify)
{
  std::list<cEventNotification *>::iterator ptr  = notifications.begin();
  std::list<cEventNotification *>::iterator last = notifications.end();

  while ((ptr  != last)        &&
          (*ptr != event_notify)  )
  {
    ++ptr;
  } // endwhile event notification not found

  if (ptr != last)
  {
    delete *ptr;

    notifications.erase(ptr);
  } // endif found relevant event notification

}


// cEventNotification implementation from here on

//***************************************************************
// Constructor - set up signals and initialise variables
//***************************************************************

cEventNotification::cEventNotification(const int      ip_port,
                                       const QString &ip_data)
{
  QString data;

  port = ip_port;
  data = ip_data+"\n";

  data_list.push_back(data);

  // Set up signal handling

  sock = new QTcpSocket;

  connect(sock, SIGNAL(connected()), this, SLOT(connected()));
  connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
  connect(sock, SIGNAL(bytesWritten(qint64)), this, SLOT(wroteAll()));
  connect(sock, SIGNAL(disconnected()), this, SLOT (connectionClosed()));
} // cEventNotification::cEventNotification


//***************************************************************
// Destructor
//***************************************************************

cEventNotification::~cEventNotification()
{
  connectionClosed ();
} // cEventNotification::~cEventNotification


//***************************************************************
// Function to initiate information sending to the specified IP port
//***************************************************************

void cEventNotification::send()
{
  sock->connectToHost ("127.0.0.1", port);
} // cEventNotification::send


//***************************************************************
// Queue data for sending at next opportunity
//***************************************************************

void cEventNotification::queue(const QString &ip_data)
{
  QString data = ip_data+"\n";

  data_list.push_back(data);

  if (data_list.size() == 1)
    writeNext ();
} // cEventNotification::queue


//***************************************************************
// Called when connected to IP port successfully
//***************************************************************

void cEventNotification::connected()
{
  writeNext ();
} // cEventNotification::connected



//***************************************************************
// Wrote some data to the IP port
//***************************************************************

void cEventNotification::wroteAll()
{
   // At this point, we can try writing more data
  data_list.pop_front();

  writeNext();
} // cEventNotification::wroteAll



//***************************************************************
// Connection closed - all done
//***************************************************************

void cEventNotification::connectionClosed()
{
   // Empty notification queue
  data_list.clear();

  if (!sock) return;

  sock->blockSignals (true);

   // delete socket
  sock->deleteLater();
  sock = 0;

  if (port != -1)
  {
    port = -1;
    emit finished(this);
  } // endif not already announced we're finished

} // cEventNotification::connectionClosed


//***************************************************************
// An error occured
//***************************************************************

void cEventNotification::error()
{
   // Usually called when we attempt to connect to a port that's no longer
   // got a server attached to it.

  data_list.clear();

  if (port != -1)
  {
    port = -1;

    emit finished(this);

  } // endif not already announced we're finished

} // cEventNotification::error

void cEventNotification::writeNext ()
{
  if (data_list.empty()) return;
  std::list<QString>::iterator ptr = data_list.begin();

   // Note: this might not buffer the entire message contents.  Not completely sure
  qint64 written = sock->write(ptr->toLatin1());

  if (written != ptr->length())
  {
      // NOTE: Far as I can tell, this never happens.  It's just here in case it ever does
    kWarning() << "/notify warning: write() was unable to buffer all data" << endl;
  } // endif unable to buffer all requested data

  if (written == -1)
  {
     // NOTE: This shouldn't ever happen because we wait until the buffer is empty before
     // trying to send any more data, but just in case...
    kWarning() << "/notify warning: write() buffer is full" << endl;
    data_list.clear();

    connectionClosed();
  } // endif unable to write data (write buffer is full)
}


#include "cnotifymanager.moc"
