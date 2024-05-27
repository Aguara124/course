#ifndef ASSISTANTGETGRADES_H
#define ASSISTANTGETGRADES_H

#include <QList>
#include <QObject>
#include <QThread>
#include "assistantrepository.h"
#include "graderepository.h"
#include "repositorymanager.h"
#include "studentrepository.h"

class AssistantGetGrades: public QObject
{
    Q_OBJECT
public:
    explicit AssistantGetGrades(const QString &assistantLogin,
                              QObject *parent = nullptr)
        : QObject{parent}
        , assistantLogin(assistantLogin)
    {}
    ~AssistantGetGrades() {}

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


        QList<std::shared_ptr<Grade>> gradeList = gradeRepository->getByExaminer(assistantLogin);

        emit finished();
        emit result(gradeList);
    }

signals:
    void finished();

    void result(const QList<std::shared_ptr<Grade>> &result);
    void error(QString err);

private:
    const QString assistantLogin;
};

#endif // ASSISTANTGETGRADES_H
