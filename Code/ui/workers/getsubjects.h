#ifndef GETSUBJECTS_H
#define GETSUBJECTS_H

#include <QList>
#include <QObject>
#include <QThread>
#include "fcsrepository.h"
#include "repositorymanager.h"

class GetSubjects : public QObject
{
    Q_OBJECT
public:
    explicit GetSubjects(const QString &program, bool pilot, int course, QObject *parent = nullptr)
        : QObject{parent}
        , m_program(program)
        , m_pilot(pilot)
        , m_course(course)
    {}
    ~GetSubjects() {}

    void start()
    {
        QThread *thread = new QThread;
        moveToThread(thread);
        connect(this, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), this, SLOT(process()));
        connect(this, SIGNAL(finished()), thread, SLOT(quit()));
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }

public slots:
    void process()
    {
        std::unique_ptr<IFcsRepository> fcsRepository = RepositoryManager::instance().fcsRepository();
        QList<std::shared_ptr<Fcs>> resultList = fcsRepository->getFcsByFilter(m_program,
                                                                               m_pilot,
                                                                               m_course);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Fcs>> &result);
    void error(QString err);

private:
    const QString m_program;
    bool m_pilot;
    int m_course;
};

#endif // GETSUBJECTS_H
