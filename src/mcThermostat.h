/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef MCTHERMOSTAT_H
#define MCTHERMOSTAT_H


#include <QObject>
#include <QJsonDocument>


class MCThermostat : public QObject
{
    Q_OBJECT

public:
    MCThermostat(QObject* parent = 0);
    ~MCThermostat();

    const QString propertyName;

    void processMessage(const QString& topic, const QByteArray& payload);

    Q_SIGNAL void sigInService();
    Q_SIGNAL void sigOutOfService();
    Q_SIGNAL void sigTemperature(const QString& value);
    Q_SIGNAL void sigHumidity(const QString& value);
    Q_SIGNAL void sigState(const QString& value);
    Q_SIGNAL void sigOutput(const QString& value);
    Q_SIGNAL void sigFan(const QString& value);
    Q_SIGNAL void sigFanState(const QString& value);

    Q_SIGNAL void sigSettingsMode(const QString& value);
    Q_SIGNAL void sigSettingsHeat(const QString& value);
    Q_SIGNAL void sigSettingsCool(const QString& value);
    Q_SIGNAL void sigSettingsFan(const QString& value);

    Q_SLOT void slotSettingsChange(const QString& setting, const int& value);
    Q_SLOT void slotModeChange(const QString& mode);
    Q_SLOT void slotFanChange(const QString& fan);

    Q_SLOT void onMqttConnect(bool result);
    Q_SLOT void onMqttDisconnect();
    Q_SLOT void onMqttMessage(const QString& topic, const QByteArray& payload);

protected:
    void parseStatus(const QJsonDocument& doc);
    void parseAction(const QJsonDocument& doc);
    QByteArray assembleSettings();
    QByteArray assembleFan();
    void publishPutSettings();
    void publishGetSettings();
    void publishPutFan();
    void publishGetFan();

    bool m_inService;

    double ConvertFromThermostatUnits(double value);
    double ConvertToThermostatUnits(double value);

    QString m_topic;

    QString m_settingMode;
    double m_settingHeat;
    double m_settingCool;

    QString m_fan;
};

#endif
