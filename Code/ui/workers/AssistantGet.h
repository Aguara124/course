#ifndef ASSISTANTGET_H
#define ASSISTANTGET_H


#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "assistantrepository.h"

class GetAssistant: public QObject
{
    Q_OBJECT
public:
    explicit GetAssistant(const QString &assistantLogin, QObject *parent = nullptr)
        : QObject{parent}
        , assistantLogin(assistantLogin)
    {}
    ~GetAssistant() {}

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
        std::unique_ptr<IAssistantRepository> assistantRepository = RepositoryManager::instance()
                                                                    .assistantRepository();
        QList<std::shared_ptr<Assistant>> resultList = assistantRepository->getByLogin(assistantLogin);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Assistant>> &result);
    void error(QString err);

private:
    const QString assistantLogin;
};

#endif // ASSISTANTGET_H
