#ifndef TEACHERGETASSISTANT_H
#define TEACHERGETASSISTANT_H

#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "teacherrepository.h"
#include <functional>

class GetAssistants : public QObject
{
    Q_OBJECT
public:
    explicit GetAssistants(const QString &teacherLogin, QObject *parent = nullptr)
        : QObject{parent}
        , teacherLogin(teacherLogin)
    {}
    ~GetAssistants() {}

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

    QList<std::shared_ptr<Assistant>> getResultList() const { return resultList; }

public slots:
    void process()
    {
        std::unique_ptr<IAssistantRepository> assistantRepository = RepositoryManager::instance()
                                                                        .assistantRepository();
        resultList = assistantRepository->getByTeacher(teacherLogin);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Assistant>> &result);
    void error(QString err);

private:
    QList<std::shared_ptr<Assistant>> resultList;
    const QString teacherLogin;
};

#endif // TEACHERGETASSISTANT_H
