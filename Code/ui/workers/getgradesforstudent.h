#ifndef GETGRADESFORSTUDENT_H
#define GETGRADESFORSTUDENT_H

#include <QList>
#include <QObject>
#include <QThread>
#include "assistantrepository.h"
#include "graderepository.h"
#include "repositorymanager.h"
#include "teacherrepository.h"

class GetGradesForStudent : public QObject
{
    Q_OBJECT
public:
    explicit GetGradesForStudent(const QString &studentLogin, QObject *parent = nullptr)
        : QObject{parent}
        , m_studentLogin(studentLogin)
    {}
    ~GetGradesForStudent() {}

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

        QList<std::shared_ptr<Grade>> gradeList = gradeRepository->getByStudent(m_studentLogin);

        emit finished();
        emit result(gradeList);
    }

signals:
    void finished();

    void result(const QList<std::shared_ptr<Grade>> &result);
    void error(QString err);

private:
    const QString m_studentLogin;
};

#endif // GETGRADESFORSTUDENT_H
