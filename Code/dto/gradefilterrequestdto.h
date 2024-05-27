#ifndef GRADEFILTERREQUESTDTO_H
#define GRADEFILTERREQUESTDTO_H

#include <QStringList>
#include "dto.h"

class GradeFilterRequestDto : public IDto
{
private:
    QString nameOfStudent;
    QString name;
    QString surName;

    QString subject;
    QStringList modules;
    QString elementOfControl;
    QString examinerLogin;

public:
    GradeFilterRequestDto() {}
    GradeFilterRequestDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
    QString getNameOfStudent() const;
    void setNameOfStudent(const QString &newNameOfStudent);
    QString getName() const;
    void setName(const QString &newName);
    QString getSurName() const;
    void setSurName(const QString &newSurName);
    QString getSubject() const;
    void setSubject(const QString &newSubject);
    QStringList getModules() const;
    void setModules(const QStringList &newModules);
    QString getElementOfControl() const;
    void setElementOfControl(const QString &newElementOfControl);
    QString getExaminerLogin() const;
    void setExaminerLogin(const QString &newExaminerLogin);
};

#endif // GRADEFILTERREQUESTDTO_H
