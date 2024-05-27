QT       += core gui sql printsupport httpserver network core5compat qml scxml statemachine charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#CONFIG += c++17
CONFIG += c++11
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config/appsettings.cpp \
    dto/assignmentdto.cpp \
    dto/assistantdto.cpp \
    dto/consultationdto.cpp \
    dto/fcsdto.cpp \
    dto/fcsfilterrequestdto.cpp \
    dto/gradedto.cpp \
    dto/gradefilterrequestdto.cpp \
    dto/listdto.cpp \
    dto/studentdto.cpp \
    dto/teacherdto.cpp \
    dto/workdto.cpp \
    hint.cpp \
    http/assignmentcontroller.cpp \
    http/assistantcontroller.cpp \
    http/consultationcontroller.cpp \
    http/fcscontroller.cpp \
    http/gradecontroller.cpp \
    http/registrationcontroller.cpp \
    http/studentcontroller.cpp \
    http/teachercontroller.cpp \
    http/workcontroller.cpp \
    main.cpp \
    ui/avatar/imagecropper.cpp \
    ui/avatar/imageedit.cpp \
    ui/common/submittedworkdialog.cpp \
    ui/logindialog.cpp \
    ui/mainappwindow.cpp \
    repository/assignmentdbrepository.cpp \
    repository/assignmentnetworkrepository.cpp \
    repository/assistantdbrepository.cpp \
    repository/assistantnetworkrepository.cpp \
    repository/assistantrepository.cpp \
    repository/consultationdbrepository.cpp \
    repository/consultationnetworkrepository.cpp \
    repository/dto2modelmapper.cpp \
    repository/fcsdbrepository.cpp \
    repository/fcsnetworkrepository.cpp \
    repository/gradedbrepository.cpp \
    repository/gradenetworkrepository.cpp \
    repository/model/assignment.cpp \
    repository/model/assistant.cpp \
    repository/model/consultation.cpp \
    repository/model/fcs.cpp \
    repository/model/grade.cpp \
    repository/model/teacher.cpp \
    repository/model/work.cpp \
    repository/studentdbrepository.cpp \
    repository/studentnetworkrepository.cpp \
    repository/teacherdbrepository.cpp \
    repository/teachernetworkrepository.cpp \
    repository/workdbrepository.cpp \
    repository/worknetworkrepository.cpp \
    repository/workrepository.cpp \
    ui/profileeditdialog.cpp \
    ui/registrationformdialog.cpp \
    ui/student/askconsultation.cpp \
    ui/student/studentassignmenttab.cpp \
    ui/student/studentform.cpp \
    ui/student/studentformulatab.cpp \
    ui/student/studentgradetab.cpp \
    ui/student/submitworkdialog.cpp \
    ui/teacher/assignmenttab.cpp \
    ui/teacher/consultationtab.cpp \
    ui/teacher/formulatab.cpp \
    ui/teacher/gradetab.cpp \
    ui/teacher/teacherform.cpp \
    utils/authdata.cpp \
    utils/imageutils.cpp \
    dto/authinfo.cpp \
    dto/responsedto.cpp \
    http/authcontroller.cpp \
    http/simplerequester.cpp \
    dto/authrequestdto.cpp \
    repository/authdbrepository.cpp \
    repository/authnetworkrepository.cpp \
    repository/model/student.cpp \
    repository/model/token.cpp \
    repository/repositorymanager.cpp \
    repository/tokendbrepository.cpp \
    utils/encryption.cpp \
    http/controller.cpp \
    http/httpsupport.cpp \
    http/missinghandlercontroller.cpp \
    http/requester.cpp \
    ui/assistant/assignmenttabassistant.cpp \
    ui/assistant/assistantform.cpp \
    ui/assistant/consultationassistant.cpp \
    ui/assistant/gradetabassistant.cpp


HEADERS += \
    dto/VariableDto.h \
    dto/assignmentdto.h \
    dto/assistantdto.h \
    dto/consultationdto.h \
    dto/dto.h \
    dto/fcsdto.h \
    dto/fcsfilterrequestdto.h \
    dto/formuladto.h \
    dto/gradedto.h \
    dto/gradefilterrequestdto.h \
    dto/listdto.h \
    dto/studentdto.h \
    dto/teacherdto.h \
    dto/workdto.h \
    hint.h \
    http/assignmentcontroller.h \
    http/assistantcontroller.h \
    http/consultationcontroller.h \
    http/fcscontroller.h \
    http/gradecontroller.h \
    http/registrationcontroller.h \
    http/studentcontroller.h \
    http/teachercontroller.h \
    http/workcontroller.h \
    ui/avatar/imagecropper.h \
    ui/avatar/imagecropper_e.h \
    ui/avatar/imagecropper_p.h \
    ui/avatar/imageedit.h \
    ui/common/submittedworkdialog.h \
    ui/logindialog.h \
    ui/mainappwindow.h \
    repository/assignmentdbrepository.h \
    repository/assignmentnetworkrepository.h \
    repository/assignmentrepository.h \
    repository/assistantdbrepository.h \
    repository/assistantnetworkrepository.h \
    repository/assistantrepository.h \
    repository/consultationdbrepository.h \
    repository/consultationnetworkrepository.h \
    repository/consultationrepository.h \
    repository/dto2modelmapper.h \
    repository/fcsdbrepository.h \
    repository/fcsnetworkrepository.h \
    repository/fcsrepository.h \
    repository/gradedbrepository.h \
    repository/gradenetworkrepository.h \
    repository/graderepository.h \
    repository/model/assignment.h \
    repository/model/assistant.h \
    repository/model/consultation.h \
    repository/model/fcs.h \
    repository/model/grade.h \
    repository/model/teacher.h \
    repository/model/work.h \
    repository/studentdbrepository.h \
    repository/studentnetworkrepository.h \
    repository/studentrepository.h \
    repository/teacherdbrepository.h \
    repository/teachernetworkrepository.h \
    repository/teacherrepository.h \
    repository/workdbrepository.h \
    repository/worknetworkrepository.h \
    repository/workrepository.h \
    ui/profileeditdialog.h \
    ui/registrationformdialog.h \
    ui/student/askconsultation.h \
    ui/student/studentassignmenttab.h \
    ui/student/studentform.h \
    ui/student/studentformulatab.h \
    ui/student/studentgradetab.h \
    ui/student/submitworkdialog.h \
    ui/teacher/assignmenttab.h \
    ui/teacher/consultationtab.h \
    ui/teacher/formulatab.h \
    ui/teacher/gradetab.h \
    ui/teacher/teacherform.h \
    ui/workers/fcsgetfaculty.h \
    ui/workers/fcsgetlecturer.h \
    ui/workers/fcsgetprogram.h \
    ui/workers/getassignments.h \
    ui/workers/getassignmentsforstudent.h \
    ui/workers/getconsultation.h \
    ui/workers/getgradesforstudent.h \
    ui/workers/getstudent.h \
    ui/workers/getsubjects.h \
    ui/workers/getworks.h \
    ui/workers/teacherget.h \
    ui/workers/teachergetassistant.h \
    ui/workers/teachergetgrades.h \
    utils/authdata.h \
    utils/imageutils.h \
    utils/rwlockhelper.h \
    config/appsettings.h \
    config/dbsettings.h \
    dto/authinfo.h \
    dto/responsedto.h \
    http/authcontroller.h \
    http/simplerequester.h \
    dto/authrequestdto.h \
    repository/authdbrepository.h \
    repository/authnetworkrepository.h \
    repository/authrepository.h \
    repository/dbaccess.h \
    config/dbsettings.h \
    repository/model/student.h \
    repository/model/token.h \
    repository/repositorymanager.h \
    repository/tokendbrepository.h \
    repository/tokenrepository.h \
    utils/AuthException.h \
    utils/encryption.h \
    http/controller.h \
    http/httpsupport.h \
    http/missinghandlercontroller.h \
    http/requester.h \
    utils/singleton.h \
    ui/assistant/assignmenttabassistant.h \
    ui/assistant/assistantform.h \
    ui/assistant/consultationassistant.h \
    ui/assistant/gradetabassistant.h \
    ui/workers/AssistantGet.h \
    ui/workers/GetStudents.h \
    ui/workers/assistantgetgrades.h

FORMS += \
    hint.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-Course-Desktop_Qt_6_5_3_MinGW_64_bit-Debug/debug/buffer.csv


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../msys64/mingw64/release/ -llibpoppler-qt6.dll -lcrypto -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../msys64/mingw64/debug/ -llibpoppler-qt6.dll -lcrypto -llibcurl

INCLUDEPATH += $$PWD/''
INCLUDEPATH += $$PWD/utils
INCLUDEPATH += $$PWD/repository
INCLUDEPATH += $$PWD/repository/model
INCLUDEPATH += $$PWD/dto
INCLUDEPATH += $$PWD/config
INCLUDEPATH += $$PWD/http
INCLUDEPATH += $$PWD/calculator
INCLUDEPATH += $$PWD/ui


DEPENDPATH += $$PWD/''

RESOURCES += \
    qtresource.qrc
