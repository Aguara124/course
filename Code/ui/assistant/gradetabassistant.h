#ifndef GRADETABASSISTANT_H
#define GRADETABASSISTANT_H

class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QtMaterialCard;

class QtMaterialRaisedButton;
class QtMaterialTextField;

class QScrollArea;
class QButtonGroup;
class QAbstractButton;
class QTableWidgetItem;
class ProfileEditDialog;

#include "assistantrepository.h"
#include "fcsrepository.h"
#include "graderepository.h"
#include "studentrepository.h"
#include "workers/AssistantGet.h"

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>


class GradeTabAssistant: public QWidget
{
    Q_OBJECT
public:

    explicit GradeTabAssistant(const QString &assistantLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void getStudents(const QList<std::shared_ptr<Student>> &studentsList);
    void getGrades(const QList<std::shared_ptr<Grade>> &gradeList);
    void setCard();

    void searchChanged(const QString &searchText);
    void startReceiveData();

    void onStudentClicked(QAbstractButton *pButton);
    void onGradeCellChanged(int, int);

private:
    QHBoxLayout *m_pGradeLayout;
    QTableWidget *m_pTableWidget;
    QtMaterialCard *m_pAssistantCard;

    QtMaterialRaisedButton *m_pReloadButton;
    QtMaterialTextField *m_pSearchField;

    QScrollArea *m_pStudentArea;
    QVBoxLayout *m_pStudentLayout;
    QButtonGroup *m_pButtonGroup;

    QMap<QAbstractButton *, std::shared_ptr<Student>> m_buttonToMap;

    const QString m_assistantLogin;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::shared_ptr<Assistant> assistant;
    QList<std::shared_ptr<Student>> studentsList;
    QList<std::shared_ptr<Grade>> gradeList;
    QList<std::shared_ptr<Fcs>> fcsList;

    std::unique_ptr<IGradeRepository> gradeRepository;
    bool avatarSet = 0;

    ProfileEditDialog *m_pEditProfile;

private:
    QTableWidgetItem *addGradeItem(int row,
                                   int column,
                                   const QString &text,
                                   bool isEditable = false); //
    void setGradeTableRecord(int index, const Grade &grade); //
    static bool matchBySearchField(const QString &searchText, const Grade &grade); //
    void setupGradeTableWidget(); //
    const std::shared_ptr<Grade> getRecordFromTable(int index); //
    QLayout *setupTableLayout(); //


    void setupFormulaTabLayout();

    void setupFormulaTableWidget();
    void addFormulaVariable(int rowIndex,
                            const QString &variableName,
                            const QString &period,
                            const QString &elementOfControl);

    std::shared_ptr<Grade> getGradeById(int id, const QList<std::shared_ptr<Grade>> &gradeList);

protected:
    void keyPressEvent(QKeyEvent *event) override; //
};

#endif // GRADETABASSISTANT_H
