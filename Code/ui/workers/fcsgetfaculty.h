#ifndef FCSGETFACULTY_H
#define FCSGETFACULTY_H

#include <QList>
#include <QObject>
#include <QThread>
#include "fcsrepository.h"
#include "repositorymanager.h"

class FcsGetFaculty : public QObject
{
    Q_OBJECT
public:
    explicit FcsGetFaculty(QObject *parent = nullptr)
        : QObject{parent}
    {}
    ~FcsGetFaculty() {}

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

    QList<std::shared_ptr<Fcs>> getResultList() const { return resultList; }

public slots:
    void process()
    {
        std::unique_ptr<IFcsRepository> fcsRepository = RepositoryManager::instance().fcsRepository();
        resultList = fcsRepository->getFaculty();

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Fcs>> &result);
    void error(QString err);

private:
    QList<std::shared_ptr<Fcs>> resultList;
};

#endif // FCSGETFACULTY_H
