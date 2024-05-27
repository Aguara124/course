#ifndef GRADETAB_H
#define GRADETAB_H

#include <QWidget>

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
#include "teacherrepository.h"

class GradeTab : public QWidget
{
    Q_OBJECT
public:
    explicit GradeTab(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void getTeacher(const QList<std::shared_ptr<Teacher>> &teacherList);
    void getAssistants(const QList<std::shared_ptr<Assistant>> &assistantsList);
    void getGrades(const QList<std::shared_ptr<Grade>> &gradeList);
    void getFcs(const QList<std::shared_ptr<Fcs>> &fcsList);

    void searchChanged(const QString &searchText);
    void startReceiveData();

    void onAssistantClicked(QAbstractButton *pButton);
    void onGradeCellChanged(int, int);

private:
    QHBoxLayout *m_pGradeLayout;
    QTableWidget *m_pTableWidget;
    QtMaterialCard *m_pTeacherCard;

    QtMaterialRaisedButton *m_pReloadButton;
    QtMaterialTextField *m_pSearchField;

    QScrollArea *m_pAssistantArea;
    QVBoxLayout *m_pAssistantLayout;
    QButtonGroup *m_pButtonGroup;
    ProfileEditDialog *m_pEditProfile;

    QMap<QAbstractButton *, std::shared_ptr<Assistant>> m_buttonToMap;

    const QString m_teacherLogin;

private:
    QTableWidgetItem *addGradeItem(int row,
                                   int column,
                                   const QString &text,
                                   bool isEditable = false);
    void setGradeTableRecord(int index, const Grade &grade);
    static bool matchBySearchField(const QString &searchText, const Grade &grade);
    void setupGradeTableWidget();
    const std::shared_ptr<Grade> getRecordFromTable(int index);
    QLayout *setupTableLayout();

    void setupFormulaTabLayout();

    void setupFormulaTableWidget();
    void addFormulaVariable(int rowIndex,
                            const QString &variableName,
                            const QString &period,
                            const QString &elementOfControl);

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    QList<std::shared_ptr<Teacher>> teacherList;
    QList<std::shared_ptr<Assistant>> assistantsList;
    QList<std::shared_ptr<Grade>> gradeList;
    QList<std::shared_ptr<Fcs>> fcsList;

    std::unique_ptr<IGradeRepository> gradeRepository;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // GRADETAB_H
