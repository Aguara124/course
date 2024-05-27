#ifndef REPOSITORYMANAGER_H
#define REPOSITORYMANAGER_H

#include <QtCore>
#include "appsettings.h"
#include "assignmentrepository.h"
#include "assistantrepository.h"
#include "authrepository.h"
#include "consultationrepository.h"
#include "fcsrepository.h"
#include "graderepository.h"
#include "studentrepository.h"
#include "teacherrepository.h"
#include "tokenrepository.h"
#include "workrepository.h"

class RepositoryManager : public Singleton<RepositoryManager>
{
    const AppSettings::AppMode appMode;

public:
    RepositoryManager();

    std::unique_ptr<ITokenRepository> tokenRepository() const;

    std::unique_ptr<IAuthRepository> authRepository() const;

    std::unique_ptr<IFcsRepository> fcsRepository() const;

    std::unique_ptr<IGradeRepository> gradeRepository() const;

    std::unique_ptr<IStudentRepository> studentRepository() const;

    std::unique_ptr<IAssistantRepository> assistantRepository() const;

    std::unique_ptr<IWorkRepository> workRepository() const;

    std::unique_ptr<IAssignmentRepository> assignmentRepository() const;

    std::unique_ptr<IConsultationRepository> consultationRepository() const;

    std::unique_ptr<ITeacherRepository> teacherRepository() const;
};

#endif // REPOSITORYMANAGER_H
