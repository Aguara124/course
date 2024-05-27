#ifndef GETCONSULTATION_H
#define GETCONSULTATION_H

#include <QList>
#include <QObject>
#include <QThread>
#include "repositorymanager.h"
#include "teacherrepository.h"

class GetConsultationRequest : public QObject
{
    Q_OBJECT
public:
    explicit GetConsultationRequest(const QString &examinerLogin, QObject *parent = nullptr)
        : QObject{parent}
        , examinerLogin(examinerLogin)
    {}
    ~GetConsultationRequest() {}

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
        std::unique_ptr<IConsultationRepository> consultationRepository
            = RepositoryManager::instance().consultationRepository();
        QList<std::shared_ptr<Consultation>> resultList
            = consultationRepository->getConsultations(examinerLogin, false);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Consultation>> &result);
    void error(QString err);

private:
    const QString examinerLogin;
};

#endif // GETCONSULTATION_H
