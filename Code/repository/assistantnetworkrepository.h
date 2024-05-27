#ifndef ASSISTANTNETWORKREPOSITORY_H
#define ASSISTANTNETWORKREPOSITORY_H

#include "assistantrepository.h"

class AssistantNetworkRepository : public IAssistantRepository
{
public:
    AssistantNetworkRepository() {}

public:
    const QList<std::shared_ptr<Assistant> > getByLogin(const QString &assistantLogin) override;
    const QList<std::shared_ptr<Assistant> > getByGroup(int group, int subgroup) override;
    const QList<std::shared_ptr<Assistant> > getByTeacher(const QString &teacherLogin) override;
    bool updateAssistant(const Assistant &assistant) override;
};

#endif // ASSISTANTNETWORKREPOSITORY_H
