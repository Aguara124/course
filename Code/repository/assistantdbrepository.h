#ifndef ASSISTANTDBREPOSITORY_H
#define ASSISTANTDBREPOSITORY_H

#include "assistantrepository.h"

class AssistantDbRepository : public IAssistantRepository
{
public:
    AssistantDbRepository() {}

public:
    const QList<std::shared_ptr<Assistant> > getByLogin(const QString &assistantLogin) override;
    const QList<std::shared_ptr<Assistant> > getByGroup(int group, int subgroup) override;
    const QList<std::shared_ptr<Assistant> > getByTeacher(const QString &teacherLogin) override;

private:
    const QString getAllowedLogin();

public:
    bool updateAssistant(const Assistant &assistant) override;
};

#endif // ASSISTANTDBREPOSITORY_H
