#ifndef CONSULTATIONTAB_H
#define CONSULTATIONTAB_H

#include <QWidget>

class QHBoxLayout;
class QtMaterialTextField;
class QLabel;
class QTableWidget;
class QGroupBox;
class Teacher;
class Fcs;
class QtMaterialSelectBox;
class QtMaterialRadioButton;
class QtMaterialRaisedButton;
class QScrollArea;
class QVBoxLayout;
class QtMaterialCheckBox;

#include <QMap>

#include "consultationrepository.h"
#include "student.h"

using QStringMap = QMap<QString, QString>;

class ConsultationTab : public QWidget
{
    Q_OBJECT
public:
    explicit ConsultationTab(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:

signals:

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QWidget *setupFilterBox();
    QWidget *setupRequestBox();
    QWidget *setupRequestListBox();
    void updateRequests(const QString &studentLogin);
    Student *getStudent(const QString &studentLogin);
    void showRequest(Student *pStudent, Consultation *pConsultation);

private:
    const QString teacherLogin;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::unique_ptr<IConsultationRepository> consultationRepository;
    QList<std::shared_ptr<Consultation>> consultations;
    QList<std::shared_ptr<Student>> students;

    QScrollArea *m_pStudentArea;
    QVBoxLayout *m_pStudentLayout;

    QVBoxLayout *m_pRequestLayout;
    QGroupBox *m_pRequestBox;
    QStringMap m_mapProgramToName;
    QHBoxLayout *m_pTabLayout;

    QtMaterialTextField *m_pSearchText;
    QtMaterialCheckBox *m_pViewed;
    QtMaterialCheckBox *m_pNotViewed;

    QGroupBox *m_pRequestListGroupBox;

    Consultation *m_pCurrentRequest;
};

#endif // CONSULTATIONTAB_H
