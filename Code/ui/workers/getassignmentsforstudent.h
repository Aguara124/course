#ifndef GETASSIGNMENTSBYGROUP_H
#define GETASSIGNMENTSBYGROUP_H

#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "teacherrepository.h"

class GetAssignmentsForStudent : public QObject
{
    Q_OBJECT
public:
    explicit GetAssignmentsForStudent(const QString &studentLogin,
                                      const int group,
                                      const int subgroup,
                                      QObject *parent = nullptr)
        : QObject{parent}
        , m_studentLogin(studentLogin)
        , m_group(group)
        , m_subGroup(subgroup)
    {}
    ~GetAssignmentsForStudent() {}

    void start()
    {
        QThread *thread = new QThread;
        moveToThread(thread);
        connect(thread, SIGNAL(started()), this, SLOT(process()));
        connect(this, SIGNAL(finished()), thread, SLOT(quit()));
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }

public slots:
    void process()
    {
        std::unique_ptr<ITeacherRepository> teacherRepository = RepositoryManager::instance()
                                                                    .teacherRepository();
        std::unique_ptr<IAssistantRepository> assistantRepository = RepositoryManager::instance()
                                                                        .assistantRepository();

        QList<std::shared_ptr<Teacher>> teachers;
        QList<std::shared_ptr<Assistant>> assistants;

        QList<std::shared_ptr<Assignment>> assignments
            = RepositoryManager::instance().assignmentRepository()->getByGroup(m_group, m_subGroup);
        QList<std::shared_ptr<Work>> works
            = RepositoryManager::instance().workRepository()->getByStudentLogin(m_studentLogin);
        QList<std::shared_ptr<Grade>> grades
            = RepositoryManager::instance().gradeRepository()->getByStudent(m_studentLogin);

        QStringList assigner;
        for (int i = 0; i < assignments.size(); i++) {
            if (!assigner.contains(assignments.at(i)->getExaminerName())) {
                QList<std::shared_ptr<Assistant>> assistantList = assistantRepository->getByLogin(
                    assignments.at(i)->getExaminerName());
                if (assistantList.isEmpty()) {
                    teachers.append(
                        teacherRepository->getByLogin(assignments.at(i)->getExaminerName()));
                } else {
                    assistants.append(assistantList);
                }
            }
        }

        emit finished();
        emit result(assignments, works, grades, teachers, assistants);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Assignment>> &result,
                const QList<std::shared_ptr<Work>> &works,
                const QList<std::shared_ptr<Grade>> &grades,
                const QList<std::shared_ptr<Teacher>> &teachers,
                const QList<std::shared_ptr<Assistant>> &assistants);
    void error(QString err);

private:
    const QString m_studentLogin;
    const int m_group;
    const int m_subGroup;
};

#endif // GETASSIGNMENTSBYGROUP_H
