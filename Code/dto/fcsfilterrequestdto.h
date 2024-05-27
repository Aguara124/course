#ifndef FCSFILTERREQUESTDTO_H
#define FCSFILTERREQUESTDTO_H

#include "dto.h"

class FcsFilterRequestDto : public IDto
{
private:
    QString program;
    bool pilot;
    int course;

public:
    FcsFilterRequestDto() {}
    FcsFilterRequestDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

public:
    void fromJson(const QByteArray &content) override;
    const QByteArray toJson() const override;
    QString getProgram() const;
    void setProgram(const QString &newProgram);
    bool getPilot() const;
    void setPilot(bool newPilot);
    int getCourse() const;
    void setCourse(int newCourse);
};

#endif // FCSFILTERREQUESTDTO_H
