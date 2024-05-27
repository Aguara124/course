#include "fcsdto.h"
#include <QJsonDocument>
#include <QJsonObject>

FcsDto::FcsDto(const QJsonObject &object)
{
    initFromJsonObject(object);
}

void FcsDto::fromJson(const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

    if (jsonError.error != QJsonParseError::ParseError::NoError) {
        qDebug() << jsonError.errorString() << " error: " << jsonError.error;
        return;
    };

    QJsonObject obj = doc.object();
    initFromJsonObject(obj);
}

QString FcsDto::getFormula1() const
{
    return formula1;
}

void FcsDto::setFormula1(const QString &newFormula1)
{
    formula1 = newFormula1;
}

QString FcsDto::getFormula2() const
{
    return formula2;
}

void FcsDto::setFormula2(const QString &newFormula2)
{
    formula2 = newFormula2;
}

QString FcsDto::getFormulaFinal() const
{
    return formulaFinal;
}

void FcsDto::setFormulaFinal(const QString &newFormulaFinal)
{
    formulaFinal = newFormulaFinal;
}

QString FcsDto::getLecturer() const
{
    return lecturer;
}

void FcsDto::setLecturer(const QString &newLecturer)
{
    lecturer = newLecturer;
}

QString FcsDto::getFaculty() const
{
    return faculty;
}

void FcsDto::setFaculty(const QString &newFaculty)
{
    faculty = newFaculty;
}

QString FcsDto::getFacultyName() const
{
    return facultyName;
}

void FcsDto::setFacultyName(const QString &newFacultyName)
{
    facultyName = newFacultyName;
}

QString FcsDto::getProgramName() const
{
    return programName;
}

void FcsDto::setProgramName(const QString &newProgramName)
{
    programName = newProgramName;
}

void FcsDto::initFromJsonObject(const QJsonObject &object)
{
    id = object.value("id").toInt();
    faculty = object.value("faculty").toString();
    facultyName = object.value("facultyName").toString();

    program = object.value("program").toString();
    programName = object.value("programName").toString();
    subject = object.value("subject").toString();
    course = object.value("course").toInt();
    pilot = object.value("course").toBool();
    formula1 = object.value("formula1").toString();
    formula2 = object.value("formula2").toString();
    formulaFinal = object.value("formulaFinal").toString();
    lecturer = object.value("lecturer").toString();
}

const QByteArray FcsDto::toJson() const
{
    QJsonObject obj;

    obj["id"] = getId();
    obj["faculty"] = getFaculty();
    obj["facultyName"] = getFacultyName();

    obj["program"] = getProgram();
    obj["programName"] = getProgramName();
    obj["subject"] = getSubject();
    obj["course"] = getCourse();
    obj["pilot"] = getPilot();

    obj["formula1"] = getFormula1();
    obj["formula2"] = getFormula2();
    obj["formulaFinal"] = getFormulaFinal();
    obj["lecturer"] = getLecturer();

    QJsonDocument jsonDoc(obj);
    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
}

int FcsDto::getId() const
{
    return id;
}

void FcsDto::setId(int newId)
{
    id = newId;
}

QString FcsDto::getProgram() const
{
    return program;
}

void FcsDto::setProgram(const QString &newProgram)
{
    program = newProgram;
}

QString FcsDto::getSubject() const
{
    return subject;
}

void FcsDto::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

int FcsDto::getCourse() const
{
    return course;
}

void FcsDto::setCourse(int newCourse)
{
    course = newCourse;
}

bool FcsDto::getPilot() const
{
    return pilot;
}

void FcsDto::setPilot(bool newPilot)
{
    pilot = newPilot;
}
