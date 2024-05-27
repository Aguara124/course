#ifndef REGISTRATIONCONTROLLER_H
#define REGISTRATIONCONTROLLER_H

#include "authcontroller.h"
#include "controller.h"
#include "fcscontroller.h"
#include "repositorymanager.h"
#include "studentcontroller.h"

class RegistrationController : public Controller
{
private:
    std::unique_ptr<AuthController> authController;
    std::unique_ptr<StudentController> studentController;
    std::unique_ptr<FcsController> fcsController;

public:
    RegistrationController()
        : authController(new AuthController)
        , studentController(new StudentController)
        , fcsController(new FcsController)
    {}

public:
    QHttpServerResponse handleRequest(const QString &controller,
                                      const QString &method,
                                      const QHttpServerRequest *request,
                                      QLinkedList<Controller *> &chain) override;
};

#endif // REGISTRATIONCONTROLLER_H
