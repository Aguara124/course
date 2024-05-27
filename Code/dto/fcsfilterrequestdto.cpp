#include "fcsfilterrequestdto.h"
#include <QJsonDocument>
#include <QJsonObject>

bool FcsFilterRequestDto::getPilot() const
{
    return pilot;
}

void FcsFilterRequestDto::setPilot(bool newPilot)
{
    pilot = newPilot;
}

int FcsFilterRequestDto::getCourse() const
{
    return course;
}

void FcsFilterRequestDto::setCourse(int newCourse)
{
    course = newCourse;
}

void FcsFilterRequestDto::fromJson(const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();

    program = obj.value("program").toString();
    course = obj.value("course").toInt();
    pilot = obj.value("pilot").toBool();
}

const QByteArray FcsFilterRequestDto::toJson() const
{
    QJsonObject obj;

    obj["program"] = getProgram();
    obj["course"] = getCourse();
    obj["pilot"] = getPilot();

    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}

QString FcsFilterRequestDto::getProgram() const
{
    return program;
}

void FcsFilterRequestDto::setProgram(const QString &newProgram)
{
    program = newProgram;
}
