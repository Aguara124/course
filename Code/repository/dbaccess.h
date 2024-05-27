#ifndef DBACCESS_H
#define DBACCESS_H

#include <qsqldatabase.h>

#include <QSqlError>

#include <QThread>
#include "QMessageBox"
#include "appsettings.h"
#include "singleton.h"

#include <QCache>
#include <QReadWriteLock>

class DBAccess : public Singleton<DBAccess> {
 private:
  QSqlDatabase db;

  QReadWriteLock dbCacheLock;
  QCache<uint64_t, QSqlDatabase> dbCache;

  public:
  const QSqlDatabase getDatabase()
  {
      if (!db.isValid()) {
          qDebug() << "db is not valid";
          db = QSqlDatabase::addDatabase("QMARIADB");

          const std::unique_ptr<DBSettings> dbSettings = AppSettings::instance().getDbSettings();

          db.setHostName(dbSettings->getHost());
          db.setPort(dbSettings->getPort());
          db.setDatabaseName(dbSettings->getDatabaseName());

          db.setUserName(dbSettings->getUsername());
          db.setPassword(dbSettings->getPassword());
          db.open();

          if (db.isOpenError()) {
              qDebug() << db.lastError().text();
              QMessageBox messageBox(QMessageBox::Icon::Critical,
                                     "Доступ к БД",
                                     db.lastError().text());
              messageBox.exec();
          }
      }

      return db;
  }

  QSqlDatabase *getDatabaseCopy()
  {
      const uint64_t threadId = (uint64_t) QThread::currentThreadId();

      try {
          dbCacheLock.lockForRead();
          if (dbCache.contains(threadId)) {
              QSqlDatabase *threadDb = dbCache[threadId];
              dbCacheLock.unlock();
              return threadDb;
          }

          dbCacheLock.unlock();
      } catch (...) {
          dbCacheLock.unlock();
      }

      const QSqlDatabase mainDb = getDatabase();
      QSqlDatabase *cloneDb = new QSqlDatabase(
          QSqlDatabase::cloneDatabase(mainDb,
                                      QString("%1-%2").arg(mainDb.connectionName()).arg(threadId)));

      cloneDb->open();
      if (cloneDb->isOpenError()) {
          qDebug() << cloneDb->lastError().text();
      } else {
          try {
              dbCacheLock.lockForWrite();
              dbCache.insert(threadId, cloneDb);
              dbCacheLock.unlock();
          } catch (...) {
              dbCacheLock.unlock();
          }
      }

      return cloneDb;
  }
};

#endif  // DBACCESS_H
