#include "fcs.h"
#include <QSqlQuery>
#include <QString>

QString Fcs::getProgram() const
{
    return program;
}

void Fcs::setProgram(const QString &newProgram)
{
    program = newProgram;
}

QString Fcs::getSubject() const
{
    return subject;
}

void Fcs::setSubject(const QString &newSubject)
{
    subject = newSubject;
}

int Fcs::getCourse() const
{
    return course;
}

void Fcs::setCourse(int newCourse)
{
    course = newCourse;
}

bool Fcs::getPilot() const
{
    return pilot;
}

void Fcs::setPilot(bool newPilot)
{
    pilot = newPilot;
}

int Fcs::getId() const
{
    return id;
}

void Fcs::setId(int newId)
{
    id = newId;
}

QString Fcs::getFormula1() const
{
    return formula1;
}

void Fcs::setFormula1(const QString &newFormula1)
{
    formula1 = newFormula1;
}

QString Fcs::getFormula2() const
{
    return formula2;
}

void Fcs::setFormula2(const QString &newFormula2)
{
    formula2 = newFormula2;
}

QString Fcs::getFormulaFinal() const
{
    return formulaFinal;
}

void Fcs::setFormulaFinal(const QString &newFormulaFinal)
{
    formulaFinal = newFormulaFinal;
}

QString Fcs::getLecturer() const
{
    return lecturer;
}

void Fcs::setLecturer(const QString &newLecturer)
{
    lecturer = newLecturer;
}

QString Fcs::getFaculty() const
{
    return faculty;
}

void Fcs::setFaculty(const QString &newFaculty)
{
    faculty = newFaculty;
}

QString Fcs::getFacultyName() const
{
    return facultyName;
}

void Fcs::setFacultyName(const QString &newFacultyName)
{
    facultyName = newFacultyName;
}

QString Fcs::getProgramName() const
{
    return programName;
}

void Fcs::setProgramName(const QString &newProgramName)
{
    programName = newProgramName;
}

Fcs::Fcs(const QSqlQuery &query)
{

    id = query.value("idsubjects").toInt();
    faculty = query.value("faculty").toString();
    facultyName = query.value("facultyName").toString();

    program = query.value("program").toString();
    programName = query.value("programName").toString();

    subject = query.value("subject").toString();
    course = query.value("course").toInt();
    pilot = query.value("pilot").toBool();
    formula1 = query.value("formula1st").toString();
    formula2 = query.value("formula2nd").toString();
    formulaFinal = query.value("formulaFinal").toString();
    lecturer = query.value("lecturer").toString();
}

Fcs::Fcs(const Fcs &source)
{
    id = source.getId();
    faculty = source.getFaculty();
    facultyName = source.getFacultyName();
    program = source.getProgram();
    programName = source.getProgramName();
    subject = source.getSubject();
    course = source.getCourse();
    pilot = source.getPilot();
    formula1 = source.getFormula1();
    formula2 = source.getFormula2();
    formulaFinal = source.getFormulaFinal();
    lecturer = source.getLecturer();
}

Fcs &Fcs::operator=(const Fcs &source)
{
    if (this != &source)
    {
        id = source.getId();
        faculty = source.getFaculty();
        facultyName = source.getFacultyName();
        program = source.getProgram();
        programName = source.getProgramName();
        subject = source.getSubject();
        course = source.getCourse();
        pilot = source.getPilot();
        formula1 = source.getFormula1();
        formula2 = source.getFormula2();
        formulaFinal = source.getFormulaFinal();
        lecturer = source.getLecturer();
    }

    return *this;
}
