#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

#include "dbsettings.h"
#include "singleton.h"

class AppSettings : public Singleton<AppSettings> {
 private:
  QSettings *settings;
  QString getEncryptedValue(QString key) const;

  public:
  std::unique_ptr<DBSettings> getDbSettings();
  QSettings *getSettings();

  QString getSslRootPath() const;
  QString getSslRootPassword() const;
  QString getSslCertPath() const;
  QString getSslCertPassword() const;
  QString getSslKeyPath() const;
  QString getSslKeyPassword() const;
  qint16 getServerPort() const;
  bool traceClientRequests() const;
  bool traceServerRequests() const;
  bool dumpClientRequests() const;

  bool isUseSSL() const;

  enum AppMode { LOCAL, REMOTE, MIXED };

  AppMode getMode() const;

  const QString getAssistLogin() const;
  const QString getStudentLogin() const;
  const QString getTeacherLogin() const;

  void setAssistLogin(const QString &login);
  void setStudentLogin(const QString &login);
  void setTeacherLogin(const QString &login);

  const QString getLastDirectory() const;
  void setLastDirectory(const QString &fileOrDirPath);
};
#endif  // APPSETTINGS_H
