#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include "controller.h"
#include "authrepository.h"
#include "repositorymanager.h"

class AuthController : public Controller
{
private:
    std::unique_ptr<IAuthRepository> authRepository;

public:
    AuthController()
        : authRepository(RepositoryManager::instance().authRepository())
    {}

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain);
};

#endif // AUTHCONTROLLER_H
