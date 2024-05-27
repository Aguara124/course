#ifndef TEACHERGET_H
#define TEACHERGET_H

#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "teacherrepository.h"

class GetTeacher : public QObject
{
    Q_OBJECT
public:
    explicit GetTeacher(const QString &teacherLogin, QObject *parent = nullptr)
        : QObject{parent}
        , teacherLogin(teacherLogin)
    {}
    ~GetTeacher() {}

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
        QList<std::shared_ptr<Teacher>> resultList = teacherRepository->getByLogin(teacherLogin);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Teacher>> &result);
    void error(QString err);

private:
    const QString teacherLogin;
};

#endif // TEACHERGET_H
