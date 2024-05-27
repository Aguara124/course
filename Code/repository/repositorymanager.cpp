#include "repositorymanager.h"
#include "appsettings.h"
#include "tokendbrepository.h"

#include "assignmentdbrepository.h"
#include "assignmentnetworkrepository.h"
#include "assistantdbrepository.h"
#include "assistantnetworkrepository.h"
#include "authdbrepository.h"
#include "authnetworkrepository.h"
#include "consultationdbrepository.h"
#include "consultationnetworkrepository.h"
#include "fcsdbrepository.h"
#include "fcsnetworkrepository.h"
#include "gradedbrepository.h"
#include "gradenetworkrepository.h"
#include "studentdbrepository.h"
#include "studentnetworkrepository.h"
#include "teacherdbrepository.h"
#include "teachernetworkrepository.h"
#include "workdbrepository.h"
#include "worknetworkrepository.h"

RepositoryManager::RepositoryManager()
    : appMode(AppSettings::instance().getMode())
{}

std::unique_ptr<ITokenRepository> RepositoryManager::tokenRepository() const
{
    return std::unique_ptr<ITokenRepository>(new TokenDbRepository());
}

std::unique_ptr<IAuthRepository> RepositoryManager::authRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IAuthRepository>(new AuthDbRepository())
               : std::unique_ptr<IAuthRepository>(new AuthNetworkRepository());
}

std::unique_ptr<IFcsRepository> RepositoryManager::fcsRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IFcsRepository>(new FcsDbRepository())
               : std::unique_ptr<IFcsRepository>(new FcsNetworkRepository());
}

std::unique_ptr<IGradeRepository> RepositoryManager::gradeRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IGradeRepository>(new GradeDbRepository())
               : std::unique_ptr<IGradeRepository>(new GradeNetworkRepository());
}

std::unique_ptr<IStudentRepository> RepositoryManager::studentRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IStudentRepository>(new StudentDbRepository())
               : std::unique_ptr<IStudentRepository>(new StudentNetworkRepository());
}

std::unique_ptr<IAssistantRepository> RepositoryManager::assistantRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IAssistantRepository>(new AssistantDbRepository())
               : std::unique_ptr<IAssistantRepository>(new AssistantNetworkRepository());
}

std::unique_ptr<IWorkRepository> RepositoryManager::workRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IWorkRepository>(new WorkDbRepository())
               : std::unique_ptr<IWorkRepository>(new WorkNetworkRepository());
}

std::unique_ptr<IAssignmentRepository> RepositoryManager::assignmentRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IAssignmentRepository>(new AssignmentDbRepository())
               : std::unique_ptr<IAssignmentRepository>(new AssignmentNetworkRepository());
}

std::unique_ptr<IConsultationRepository> RepositoryManager::consultationRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<IConsultationRepository>(new ConsultationDbRepository())
               : std::unique_ptr<IConsultationRepository>(new ConsultationNetworkRepository());
}

std::unique_ptr<ITeacherRepository> RepositoryManager::teacherRepository() const
{
    return appMode == AppSettings::AppMode::LOCAL || appMode == AppSettings::AppMode::MIXED
               ? std::unique_ptr<ITeacherRepository>(new TeacherDbRepository())
               : std::unique_ptr<ITeacherRepository>(new TeacherNetworkRepository());
}
