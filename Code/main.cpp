#include <QApplication>
#include <QtCore>
#include <iostream>

#include "appsettings.h"
#include "assignmentcontroller.h"
#include "assistantcontroller.h"
#include "authcontroller.h"
#include "consultationcontroller.h"
#include "fcscontroller.h"
#include "gradecontroller.h"
#include "httpsupport.h"
#include "missinghandlercontroller.h"
#include "registrationcontroller.h"
#include "studentcontroller.h"
#include "teachercontroller.h"
#include "workcontroller.h"

#include "ui/mainappwindow.h"

#include "hint.h"
#include "lib/qtmaterialstyle.h"
#include "lib/qtmaterialtheme.h"
#include "workdto.h"

void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    FILE *stream = fopen("course.log", "a");
    if (stream == nullptr) {
        stream = stderr;
    }

    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stream,
                "Debug: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
    case QtInfoMsg:
        fprintf(stream,
                "Info: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
    case QtWarningMsg:
        fprintf(stream,
                "Warning: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
    case QtCriticalMsg:
        fprintf(stream,
                "Critical: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
    case QtFatalMsg:
        fprintf(stream,
                "Fatal: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        abort();
    }

    if (stream != stderr) {
        fclose(stream);
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(logOutput);

    QApplication a(argc, argv);

    std::unique_ptr<HttpSupport> httpServer;
    MainAppWindow *pMainWindow;

    QtMaterialStyle::instance().setTheme(new QtMaterialTheme());

    AppSettings::AppMode startMode = AppSettings::instance().getMode();

    if (startMode == AppSettings::LOCAL) {
        httpServer = std::unique_ptr<HttpSupport>(new HttpSupport());

        httpServer->addController(new RegistrationController());
        httpServer->addController(new AuthController());
        httpServer->addController(new StudentController());
        httpServer->addController(new GradeController());
        httpServer->addController(new FcsController());
        httpServer->addController(new AssignmentController());
        httpServer->addController(new AssistantController());
        httpServer->addController(new ConsultationController());
        httpServer->addController(new TeacherController());
        httpServer->addController(new WorkController());

        httpServer->addController(new MissingHandlerController());
        httpServer->startServer();


    } else if (startMode == AppSettings::REMOTE || startMode == AppSettings::MIXED) {
        pMainWindow = MainAppWindow::ptrInstance();
        pMainWindow->show();
    }

    return a.exec();
}
