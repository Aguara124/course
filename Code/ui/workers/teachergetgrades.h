#ifndef TEACHERGETGRADES_H
#define TEACHERGETGRADES_H

#include <QList>
#include <QObject>
#include <QThread>
#include "assistantrepository.h"
#include "graderepository.h"
#include "repositorymanager.h"
#include "teacherrepository.h"

class TeacherGetGrades : public QObject
{
    Q_OBJECT
public:
    explicit TeacherGetGrades(const QString &teacherLogin,
                              const QList<std::shared_ptr<Assistant>> &assistants,
                              QObject *parent = nullptr)
        : QObject{parent}
        , teacherLogin(teacherLogin)
        , assistantsList(assistants)
    {}
    ~TeacherGetGrades() {}

    void start()
    {
        QThread *thread = new QThread;
        qDebug() << "before starting thread: " << QThread::currentThreadId();

        moveToThread(thread);
        connect(thread, SIGNAL(started()), this, SLOT(process()));
        connect(this, SIGNAL(finished()), thread, SLOT(quit()));
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        qDebug() << "starting thread: " << QThread::currentThreadId();
        thread->start();
    }

public slots:
    void process()
    {
        qDebug() << "running on thread: " << QThread::currentThreadId();

        std::unique_ptr<IGradeRepository> gradeRepository = RepositoryManager::instance()
                                                                .gradeRepository();

        QStringList examinerList;
        examinerList.append(teacherLogin);

        QList<std::shared_ptr<Grade>> gradeList = gradeRepository->getByExaminer(teacherLogin);

        for (std::shared_ptr<Assistant> assistant : assistantsList) {
            if (!examinerList.contains(assistant->getLogin())) {
                examinerList.append(assistant->getLogin());
                gradeList.append(gradeRepository->getByExaminer(assistant->getLogin()));
            }
        }

        emit finished();
        emit result(gradeList);
    }

signals:
    void finished();

    void result(const QList<std::shared_ptr<Grade>> &result);
    void error(QString err);

private:
    const QString teacherLogin;
    const QList<std::shared_ptr<Assistant>> assistantsList;
};

#endif // TEACHERGETGRADES_H
