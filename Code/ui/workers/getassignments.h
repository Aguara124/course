#ifndef GETASSIGNMENTS_H
#define GETASSIGNMENTS_H

#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "teacherrepository.h"

class GetAssignments : public QObject
{
    Q_OBJECT
public:
    explicit GetAssignments(const QString &examinerLogin, bool isTeacher, QObject *parent = nullptr)
        : QObject{parent}
        , examinerLogin(examinerLogin)
        , isTeacher(isTeacher)
    {}
    ~GetAssignments() {}

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
        QStringList examiners;
        examiners.append(examinerLogin);

        if (isTeacher) {
            std::unique_ptr<IAssistantRepository> assistantRepository
                = RepositoryManager::instance().assistantRepository();
            QList<std::shared_ptr<Assistant>> assistants = assistantRepository->getByTeacher(
                examinerLogin);

            for (int i = 0; i < assistants.size(); i++) {
                if (!examiners.contains(assistants.at(i)->getLogin())) {
                    examiners.append(assistants.at(i)->getLogin());
                }
            }
        }

        QList<std::shared_ptr<Assignment>> allAssignments;
        std::unique_ptr<IAssignmentRepository> assignmentRepository = RepositoryManager::instance()
                                                                          .assignmentRepository();
        for (QString examiner : examiners) {
            allAssignments.append(assignmentRepository->getByExaminerName(examiner));
        }

        emit finished();
        emit result(allAssignments);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Assignment>> &result);
    void error(QString err);

private:
    const QString examinerLogin;
    bool isTeacher;
};

#endif // GETASSIGNMENTS_H
