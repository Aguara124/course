#ifndef FCSDTO_H
#define FCSDTO_H

#include <QJsonObject>
#include "dto.h"

class FcsDto : public IDto
{
private:
    int id;
    QString faculty;
    QString facultyName;
    QString program;
    QString programName;
    QString subject;
    int course;
    bool pilot;
    QString formula1;
    QString formula2;
    QString formulaFinal;
    QString lecturer;

    void initFromJsonObject(const QJsonObject &object);

public:
    FcsDto() {}
    FcsDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }
    FcsDto(const QJsonObject &object);

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
    int getId() const;
    void setId(int newId);
    QString getProgram() const;
    void setProgram(const QString &newProgram);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    int getCourse() const;
    void setCourse(int newCourse);
    bool getPilot() const;
    void setPilot(bool newPilot);
    QString getFormula1() const;
    void setFormula1(const QString &newFormula1);
    QString getFormula2() const;
    void setFormula2(const QString &newFormula2);
    QString getFormulaFinal() const;
    void setFormulaFinal(const QString &newFormulaFinal);
    QString getLecturer() const;
    void setLecturer(const QString &newLecturer);

    QString getFaculty() const;
    void setFaculty(const QString &newFaculty);
    QString getFacultyName() const;
    void setFacultyName(const QString &newFacultyName);
    QString getProgramName() const;
    void setProgramName(const QString &newProgramName);
};

#endif // FCSDTO_H
