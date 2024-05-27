#include "gradefilterrequestdto.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

void GradeFilterRequestDto::fromJson(const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();

    nameOfStudent = obj.value("nameOfStudent").toString();
    name = obj.value("name").toString();
    surName = obj.value("surName").toString();
    subject = obj.value("subject").toString();
    elementOfControl = obj.value("elementOfControl").toString();
    examinerLogin = obj.value("examinerLogin").toString();

    if (obj.value("modules").isArray()) {
        QJsonArray arr = obj.value("modules").toArray();
        for (auto v : arr) {
            modules.append(v.toString());
        }
    }
}

const QByteArray GradeFilterRequestDto::toJson() const
{
    QJsonObject obj;

    obj["nameOfStudent"] = getNameOfStudent();
    obj["name"] = getName();
    obj["surName"] = getSurName();
    obj["subject"] = getSubject();
    obj["elementOfControl"] = getElementOfControl();
    obj["examinerLogin"] = getExaminerLogin();

    QJsonArray arr;
    for (auto v : getModules()) {
        arr.append(QJsonValue(v));
    }

    obj["modules"] = arr;
    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}

QString GradeFilterRequestDto::getNameOfStudent() const
{
    return nameOfStudent;
}

void GradeFilterRequestDto::setNameOfStudent(const QString &newNameOfStudent)
{
    nameOfStudent = newNameOfStudent;
}

QString GradeFilterRequestDto::getName() const
{
    return name;
}

void GradeFilterRequestDto::setName(const QString &newName)
{
    name = newName;
}

QString GradeFilterRequestDto::getSurName() const
{
    return surName;
}

void GradeFilterRequestDto::setSurName(const QString &newSurName)
{
    surName = newSurName;
}

QString GradeFilterRequestDto::getSubject() const
{
    return subject;
}

void GradeFilterRequestDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

QStringList GradeFilterRequestDto::getModules() const
{
    return modules;
}

void GradeFilterRequestDto::setModules(const QStringList &newModules)
{
    modules = newModules;
}

QString GradeFilterRequestDto::getElementOfControl() const
{
    return elementOfControl;
}

void GradeFilterRequestDto::setElementOfControl(const QString &newElementOfControl)
{
    elementOfControl = newElementOfControl;
}

QString GradeFilterRequestDto::getExaminerLogin() const
{
    return examinerLogin;
}

void GradeFilterRequestDto::setExaminerLogin(const QString &newExaminerLogin)
{
    examinerLogin = newExaminerLogin;
}
