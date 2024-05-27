#include <QMessageBox>
#include <QSettings>
#include <QSqlError>

#include <QDir>
#include <QFileInfo>
#include <qsqldatabase.h>

#include "appsettings.h"
#include "dbsettings.h"
#include "encryption.h"
#include "singleton.h"

QString AppSettings::getSslRootPath() const {
  return AppSettings::instance()
      .getSettings()
      ->value("SSL/caCertificate")
      .toString();
}

QString AppSettings::getSslRootPassword() const
{
    return getEncryptedValue("SSL/caPassword");
}

QString AppSettings::getSslCertPath() const {
  return AppSettings::instance()
      .getSettings()
      ->value("SSL/nodeCertificate")
      .toString();
}

QString AppSettings::getSslCertPassword() const
{
    return getEncryptedValue("SSL/nodePassword");
}

QString AppSettings::getSslKeyPath() const {
  return AppSettings::instance()
      .getSettings()
      ->value("SSL/privateKey")
      .toString();
}

QString AppSettings::getSslKeyPassword() const
{
    return getEncryptedValue("SSL/privateKeyPassword");
}

qint16 AppSettings::getServerPort() const
{
    qint16 port = 8080;

    const QSettings *settings = AppSettings::instance().getSettings();
    if (settings->allKeys().contains("Main/serverPort")
        && settings->value("Main/serverPort").canConvert(QMetaType(QMetaType::UInt))) {
        port = settings->value("Main/serverPort").toUInt();
    }

    return port;
}

bool AppSettings::traceClientRequests() const
{
    return AppSettings::instance().getSettings()->value("Debug/traceClientRequests", "true").toBool();
}

bool AppSettings::traceServerRequests() const
{
    return AppSettings::instance().getSettings()->value("Debug/traceServerRequests", "true").toBool();
}

bool AppSettings::dumpClientRequests() const
{
    return AppSettings::instance().getSettings()->value("Debug/dumpClientRequests", "true").toBool();
}

bool AppSettings::isUseSSL() const {
  const QSettings *settings = AppSettings::instance().getSettings();
  return settings->contains("SSL/useSSL") &&
         settings->value("SSL/useSSL").toBool();
}

QString AppSettings::getEncryptedValue(QString key) const
{
    QVariant encryptedPassword = AppSettings::instance().getSettings()->value(key);
    if (encryptedPassword.isNull() || encryptedPassword.toString().isEmpty()) {
        return encryptedPassword.toByteArray();
    }

    return EncryptionHelper::decrypt(QByteArray::fromBase64(encryptedPassword.toByteArray()));
}

std::unique_ptr<DBSettings> AppSettings::getDbSettings()
{
    QSettings *mainSettings = AppSettings::instance().getSettings();
    qDebug() << mainSettings->childGroups();

    if (mainSettings->childGroups().contains("DBAccess")) {
        mainSettings->beginGroup("DBAccess");

        qDebug() << "mainSettings.dbAccess" << mainSettings->allKeys();
        std::unique_ptr<DBSettings> dbSettings(new DBSettings());
        dbSettings->setDatabaseName(mainSettings->value("db"));
        dbSettings->setHost(mainSettings->value("host"));
        dbSettings->setPort(mainSettings->value("port"));
        dbSettings->setUsername(mainSettings->value("username"));
        dbSettings->setPassword(QString(EncryptionHelper::decrypt(
            QByteArray::fromBase64(mainSettings->value("password").toByteArray()))));

        mainSettings->endGroup();

        return dbSettings;
    }

    return nullptr;
}

QSettings *AppSettings::getSettings() {
  if (settings == nullptr) {
    qDebug() << "init settings";

    settings = new QSettings("hsegrade-settings.conf", QSettings::IniFormat);
    qDebug() << "Location of settings file: " << settings->fileName();

    qDebug() << "allKeys" << settings->allKeys();
    if (settings->status() != QSettings::Status::NoError) {
      QMessageBox messageBox;
      messageBox.setText("Unreadable hsegrade-settings.conf");
    }
  }

  return settings;
}

AppSettings::AppMode AppSettings::getMode() const {
  QString mode = AppSettings::instance()
                     .getSettings()
                     ->value("Main/mode")
                     .toString()
                     .toLower();

  if (mode == "local") {
    return AppMode::LOCAL;
  } else if (mode == "mixed") {
      return AppMode::MIXED;
  }

  return AppMode::REMOTE;
}

const QString AppSettings::getAssistLogin() const
{
    return AppSettings::instance().getSettings()->value("Main/assistLogin").toString();
}

const QString AppSettings::getStudentLogin() const
{
    return AppSettings::instance().getSettings()->value("Main/studentLogin").toString();
}

const QString AppSettings::getTeacherLogin() const
{
    return AppSettings::instance().getSettings()->value("Main/teacherLogin").toString();
}

void AppSettings::setAssistLogin(const QString &login)
{
    QSettings *settings = AppSettings::instance().getSettings();
    settings->setValue("Main/assistLogin", login);
    settings->sync();
}

void AppSettings::setStudentLogin(const QString &login)
{
    QSettings *settings = AppSettings::instance().getSettings();
    settings->setValue("Main/studentLogin", login);
    settings->sync();
}

void AppSettings::setTeacherLogin(const QString &login)
{
    QSettings *settings = AppSettings::instance().getSettings();
    settings->setValue("Main/teacherLogin", login);
    settings->sync();
}

const QString AppSettings::getLastDirectory() const
{
    const QString lastDir
        = AppSettings::instance().getSettings()->value("Main/lastDirectory").toString();
    return lastDir.isNull() || lastDir.isEmpty() ? QDir::homePath() : lastDir;
}

void AppSettings::setLastDirectory(const QString &fileOrDirPath)
{
    try {
        QFileInfo fileInfo(fileOrDirPath);
        QSettings *settings = AppSettings::instance().getSettings();
        settings->setValue("Main/lastDirectory", fileInfo.absolutePath());
        settings->sync();

    } catch (...) {
        qDebug() << "error parsing " << fileOrDirPath;
    }
}
