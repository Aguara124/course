#ifndef ASSISTANTREPOSITORY_H
#define ASSISTANTREPOSITORY_H

#include <QList>
#include "assistant.h"

class IAssistantRepository
{
public:
    virtual ~IAssistantRepository() {}
    virtual const QList<std::shared_ptr<Assistant>> getByLogin(const QString &assistantLogin) = 0;
    virtual const QList<std::shared_ptr<Assistant>> getByGroup(int group, int subgroup) = 0;
    virtual const QList<std::shared_ptr<Assistant>> getByTeacher(const QString &teacherLogin) = 0;

    virtual bool updateAssistant(const Assistant &assistant) = 0;
};

#endif // ASSISTANTREPOSITORY_H
