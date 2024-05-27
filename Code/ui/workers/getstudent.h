#ifndef GETSTUDENT_H
#define GETSTUDENT_H

#include <QList>
#include <QObject>
#include <QThread>

#include "repositorymanager.h"
#include "studentrepository.h"

class GetStudent : public QObject
{
    Q_OBJECT
public:
    explicit GetStudent(const QString &studentLogin, QObject *parent = nullptr)
        : QObject{parent}
        , studentLogin(studentLogin)
    {}
    ~GetStudent() {}

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
        std::unique_ptr<IStudentRepository> studentRepository = RepositoryManager::instance()
                                                                    .studentRepository();

        const std::shared_ptr<Student> resultList = studentRepository->getByLogin(studentLogin);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const std::shared_ptr<Student> &result);
    void error(QString err);

private:
    const QString studentLogin;
};

#endif // GETSTUDENT_H
