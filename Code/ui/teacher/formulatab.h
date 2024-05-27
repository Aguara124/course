#ifndef FORMULATAB_H
#define FORMULATAB_H

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

#include "fcsrepository.h"
#include "formuladto.h"

class FormulaTab : public QWidget
{
    Q_OBJECT
public:
    explicit FormulaTab(const QString &teacherLogin, QWidget *parent = nullptr);
    void setupTabLayout();

protected slots:
    void onSubjectChanged(QString subjectText, int formulaIndex);
signals:

protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    void setupTableWidget();
    void setRow(int rowIndex,
                const QString &variableName,
                const QString &period,
                const QString &elementOfControl);
    void addTableItem(int rowIndex, int columnIndex, const QString &itemText, bool isEditable);

    QWidget *setupStageBox();
    QWidget *setupVariableBox();
    QWidget *setupFormulaBox();

private:
    QHBoxLayout *m_pFormulaLayout;
    QtMaterialTextField *formulaText;
    QLabel *formulaLabel;
    QTableWidget *m_pTableWidget;

    QtMaterialSelectBox *m_pSubjects;
    QtMaterialRadioButton *m_pFormula1Button;
    QtMaterialRadioButton *m_pFormula2Button;
    QtMaterialRadioButton *m_pFormula3Button;
    QtMaterialTextField *m_pFormulaText;
    QLabel *m_pResultLabel;
    QtMaterialTextField *m_pVarName;
    QtMaterialSelectBox *m_pPeriodBox;
    QtMaterialSelectBox *m_pControlBox;
    QtMaterialRaisedButton *m_pAddButton;

    QtMaterialRaisedButton *m_pTestButton;
    QtMaterialRaisedButton *m_pSaveButton;

    std::unique_ptr<FormulaDto> formula1;
    std::unique_ptr<FormulaDto> formula2;
    std::unique_ptr<FormulaDto> formula3;

    QGroupBox *groupVarBox;

    const QString teacherLogin;
    QList<std::shared_ptr<Teacher>> teacherList;
    QList<std::shared_ptr<Fcs>> fcsList;

    std::shared_ptr<QBrush> oddColor;
    std::shared_ptr<QBrush> evenColor;

    std::unique_ptr<IFcsRepository> fcsRepository;
};

#endif // FORMULATAB_H
