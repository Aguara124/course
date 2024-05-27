#ifndef FCSGETLECTURER_H
#define FCSGETLECTURER_H

#include <QList>
#include <QObject>
#include <QThread>
#include "fcsrepository.h"
#include "repositorymanager.h"

class FcsGetLecturer : public QObject
{
    Q_OBJECT
public:
    explicit FcsGetLecturer(const QList<std::shared_ptr<Teacher>> &teacherList,
                            QObject *parent = nullptr)
        : QObject{parent}
        , teacherList(teacherList)
    {}
    ~FcsGetLecturer() {}

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
        std::unique_ptr<IFcsRepository> fcsRepository = RepositoryManager::instance().fcsRepository();
        for (std::shared_ptr<Teacher> teacher : teacherList) {
            if (teacher->getLecturer()) {
                QList<std::shared_ptr<Fcs>> fcsList = fcsRepository
                                                          ->getFcsByLecturer(teacher->getLogin(),
                                                                             teacher->getProgram(),
                                                                             teacher->getSubject());
                this->resultList.append(fcsList);
            }
        }

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Fcs>> &result);
    void error(QString err);

private:
    const QString faculty;
    QList<std::shared_ptr<Fcs>> resultList;
    QList<std::shared_ptr<Teacher>> teacherList;
};

#endif // FCSGETLECTURER_H
