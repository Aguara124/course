#ifndef GETWORKS_H
#define GETWORKS_H

#include <QList>
#include <QObject>
#include <QThread>
#include "assignmentrepository.h"
#include "repositorymanager.h"
#include "workrepository.h"

class GetWorks : public QObject
{
    Q_OBJECT
public:
    explicit GetWorks(int assignmentId, QObject *parent = nullptr)
        : QObject{parent}
        , m_nAssignmentId(assignmentId)
    {}
    ~GetWorks() {}

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
        std::unique_ptr<IAssignmentRepository> assignmentRepository = RepositoryManager::instance()
                                                                          .assignmentRepository();

        std::unique_ptr<IWorkRepository> workRepository = RepositoryManager::instance()
                                                              .workRepository();
        std::unique_ptr<IStudentRepository> studentRepository = RepositoryManager::instance()
                                                                    .studentRepository();

        std::unique_ptr<IGradeRepository> gradeRepository = RepositoryManager::instance()
                                                                .gradeRepository();

        std::shared_ptr<Assignment> assignment = assignmentRepository->getById(m_nAssignmentId);
        QList<std::shared_ptr<Work>> works = workRepository->getByAssignmentId(m_nAssignmentId);
        QList<std::shared_ptr<Grade>> grades = gradeRepository->getByAssignmentId(m_nAssignmentId);

        QList<std::shared_ptr<Student>> students;

        QStringList studentList;
        for (int i = 0; i < works.size(); i++) {
            if (!studentList.contains(works.at(i)->getLogin())) {
                std::shared_ptr<Student> student = studentRepository->getByLogin(
                    works.at(i)->getLogin());
                if (student != nullptr) {
                    students.append(student);
                }
            }
        }

        emit finished();
        emit result(assignment, works, students, grades);
    }

signals:
    void finished();
    void result(const std::shared_ptr<Assignment> &assignment,
                const QList<std::shared_ptr<Work>> &works,
                const QList<std::shared_ptr<Student>> &students,
                const QList<std::shared_ptr<Grade>> &grades);
    void error(QString err);

private:
    int m_nAssignmentId;
};

#endif // GETWORKS_H
