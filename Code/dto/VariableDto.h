#ifndef VARIABLEDTO_H
#define VARIABLEDTO_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <dto.h>

class VariableDto : public IDto
{
private:
    double testValue;
    QString varName;
    QString period;
    QString elementOfControl;

    void initFromJsonObject(const QJsonObject &object)
    {
        testValue = object.value("testValue").toDouble();
        varName = object.value("varName").toString();
        period = object.value("period").toString();
        elementOfControl = object.value("elementOfControl").toString();
    }

public:
    VariableDto() {}

    VariableDto(const QByteArray &array)
    {
        if (!array.isNull() && !array.isEmpty()) {
            fromJson(array);
        }
    }

    VariableDto(const QJsonObject &object) { initFromJsonObject(object); }

public:
    void fromJson(const QByteArray &content) override
    {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);

        if (jsonError.error != QJsonParseError::ParseError::NoError) {
            qDebug() << jsonError.errorString() << " error: " << jsonError.error;
            return;
        };

        QJsonObject obj = doc.object();
        initFromJsonObject(obj);
    }
    const QByteArray toJson() const override
    {
        QJsonObject obj;

        obj["testValue"] = getTestValue();
        obj["varName"] = getVarName();
        obj["period"] = getPeriod();
        obj["elementOfControl"] = getElementOfControl();

        QJsonDocument jsonDoc(obj);
        return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact);
    }

    double getTestValue() const { return testValue; }
    void setTestValue(double newTestValue) { testValue = newTestValue; }
    QString getVarName() const { return varName; }
    void setVarName(const QString &newVarName) { varName = newVarName; }
    QString getPeriod() const { return period; }
    void setPeriod(const QString &newPeriod) { period = newPeriod; }
    QString getElementOfControl() const { return elementOfControl; }
    void setElementOfControl(const QString &newElementOfControl)
    {
        elementOfControl = newElementOfControl;
    }
};

#endif // VARIABLEDTO_H
