#ifndef GETSTUDENTS_H
#define GETSTUDENTS_H

#include <QList>
#include <QObject>
#include <QThread>

#include "repositorymanager.h"
#include "studentrepository.h"

class GetStudents : public QObject
{
    Q_OBJECT
public:
    explicit GetStudents(int gr, int subgr, const QString& assistLogin, QObject *parent = nullptr)
        : QObject{parent}
        , group(gr), subgroup(subgr), assistantLogin(assistLogin)
    {}
    ~GetStudents() {}

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

        resultList = studentRepository->getByGroup(group, subgroup, assistantLogin);

        emit finished();
        emit result(resultList);
    }

signals:
    void finished();
    void result(const QList<std::shared_ptr<Student>> &result);
    void error(QString err);

private:
    QList<std::shared_ptr<Student>> resultList;
    int group, subgroup;
    const QString assistantLogin;
};

#endif // GETSTUDENTS_H
