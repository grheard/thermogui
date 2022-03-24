/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "logger/logger.h"
#include "mcThermostat.h"
#include "mqtt.h"
#include "config.h"

// {settings.MODE: mode, TEMPERATURE: f'{temp:2.3f}' if not temp is None else 'NA', HUMIDITY: f'{humid:2.1f}' if not humid is None else 'NA', STATE: state, OUTPUT: output, settings.FAN: fan, FAN_STATE: fan_state}


const QString CTopicThermostat(QString::fromUtf8("thermostat"));

const QString COutOfService(QString::fromUtf8("out-of-service"));

const QString CKeyMode(QString::fromUtf8("mode"));
const QString CKeyTemperature(QString::fromUtf8("temperature"));
const QString CKeyHumidity(QString::fromUtf8("humidity"));
const QString CKeyState(QString::fromUtf8("state"));
const QString CKeyOutput(QString::fromUtf8("output"));
const QString CKeyFan(QString::fromUtf8("fan"));
const QString CKeyFanState(QString::fromUtf8("fan-state"));

const QString CKeyCmd(QString::fromUtf8("cmd"));
const QString CCmdGetSettings(QString::fromUtf8("get-settings"));
const QString CCmdPutSettings(QString::fromUtf8("put-settings"));
const QString CCmdGetFan(QString::fromUtf8("get-fan"));
const QString CCmdPutFan(QString::fromUtf8("put-fan"));

const QString CKeyResult(QString::fromUtf8("result"));

const QString CHeat(QString::fromUtf8("heat"));
const QString CCool(QString::fromUtf8("cool"));

const QString COff(QString::fromUtf8("off"));
const QString CAuto(QString::fromUtf8("auto"));
const QString COn(QString::fromUtf8("on"));


MCThermostat::MCThermostat(QObject* parent)
    : QObject(parent)
    , propertyName(QString::fromUtf8("mcthermostat"))
    , m_inService(false)
    , m_topic(CTopicThermostat)
{
    QString topicRoot(Config::instance()->getTopicRoot());
    if (!topicRoot.isEmpty())
    {
        m_topic = topicRoot + QString::fromUtf8("/") + m_topic;
    }

    connect(Mqtt::instance(),SIGNAL(onConnect(bool)),this,SLOT(onMqttConnect(bool)));
    connect(Mqtt::instance(),SIGNAL(onDisconnect()),this,SLOT(onMqttDisconnect()));
    connect(Mqtt::instance(),SIGNAL(onMessage (const QString&, const QByteArray&)),this,SLOT(onMqttMessage (const QString&, const QByteArray&)));
}


MCThermostat::~MCThermostat()
{
}


void MCThermostat::onMqttConnect(bool result)
{
    if (result)
    {
        LogInfo("Subscribing to '%s'",m_topic.toStdString().c_str());
        Mqtt::instance()->subscribe(m_topic,2);
    }
}


void MCThermostat::onMqttDisconnect()
{
    LogWarning("Broker disconnected.");
    m_inService = false;
    emit sigOutOfService();
}


void MCThermostat::onMqttMessage(const QString& topic, const QByteArray& payload)
{
    if (topic != m_topic)
    {
        return;
    }

    if (QString(payload) == COutOfService)
    {
        m_inService = false;
        LogInfo("Thermostat is going out of service.");
        emit sigOutOfService();
        return;
    }

    // payload must be a json object
    QJsonDocument doc(QJsonDocument::fromJson(payload));
    if (doc.isNull() || doc.isEmpty() || !doc.isObject())
    {
        LogWarning("Payload for topic '%s' is not a valid json object '%s'",m_topic.toStdString().c_str(),QString::fromLocal8Bit(payload).toStdString().c_str());
        return;
    }

    if (!m_inService)
    {
        m_inService = true;
        LogInfo("Thermostat is in service.");
        publishGetSettings();
        publishGetFan();
        emit sigInService();
    }

    if (doc.object().find(CKeyTemperature) != doc.object().end()) parseStatus(doc);
    else if (doc.object().find(CKeyCmd) != doc.object().end()) parseAction(doc);
    else
    {
        LogWarning("Unknown topic '%s' payload '%s'",topic.toStdString().c_str(),payload.toStdString().c_str());
    }
}


void MCThermostat::parseStatus(const QJsonDocument& doc)
{
    QString str;

    str = doc.object().find(CKeyState).value().toString();
    if (str.isEmpty())
    {
        LogWarning("Result key '%s' is invalid or missing.",CKeyState.toStdString().c_str());
    }
    else emit sigState(str);

    str = doc.object().find(CKeyOutput).value().toString();
    if (str.isEmpty())
    {
        LogWarning("Result key '%s' is invalid or missing.",CKeyOutput.toStdString().c_str());
    }
    else emit sigOutput(str);

    str = doc.object().find(CKeyFan).value().toString();
    if (str.isEmpty())
    {
        LogWarning("Result key '%s' is invalid or missing.",CKeyFan.toStdString().c_str());
    }
    else
    {
        m_fan = str;
        emit sigFan(str);
    }

    str = doc.object().find(CKeyFanState).value().toString();
    if (str.isEmpty())
    {
        LogWarning("Result key '%s' is invalid or missing.",CKeyFanState.toStdString().c_str());
    }
    else emit sigFanState(str);

    QJsonValue value(doc.object().find(CKeyTemperature).value());
    if (!value.isDouble())
    {
        LogWarning("Payload key '%s' is invalid or missing.",CKeyTemperature.toStdString().c_str());
    }
    else
    {
        // TODO: perform temperature conversion
        // thermostat operature in celcius.
        double temp(ConvertFromThermostatUnits(value.toDouble()));

        emit sigTemperature(QString::fromUtf8("%1").arg(temp,3,'f',1,' ').trimmed());
    }

    value = doc.object().find(CKeyHumidity).value();
    if (!value.isDouble())
    {
        LogWarning("Payload key '%s' is invalid or missing.",CKeyHumidity.toStdString().c_str());
    }
    else
    {
        emit sigHumidity(QString::fromUtf8("%1").arg(value.toDouble(),2,'f',1,' ').trimmed());
    }
}


void MCThermostat::parseAction(const QJsonDocument& doc)
{
    QString cmd(doc.object().find(CKeyCmd).value().toString());
    if (cmd.isEmpty())
    {
        LogWarning("Payload key '%s' is invalid or missing.",CKeyCmd.toStdString().c_str());
        return;
    }

    QJsonValue vresult(doc.object().find(CKeyResult).value());
    if (vresult.isNull() or vresult.isUndefined())
    {
        LogWarning("Payload key '%s' is missing.",CKeyResult.toStdString().c_str());
        return;
    }

    if (cmd == CCmdGetSettings)
    {
        if (!vresult.isObject())
        {
            LogWarning("Cmd '%s' payload key '%s' is invalid.",cmd.toStdString().c_str(),CKeyResult.toStdString().c_str());
            return;
        }

        QJsonObject result(vresult.toObject());

        QString mode(result.find(CKeyMode).value().toString());
        if (mode.isEmpty())
        {
            LogWarning("Cmd '%s' result key '%s' is invalid or missing.",cmd.toStdString().c_str(),CKeyMode.toStdString().c_str());
        }
        else
        {
            m_settingMode = mode;
            emit sigSettingsMode(mode);
        }

        QJsonValue vHeat(result.find(CHeat).value());
        if (!vHeat.isDouble())
        {
            LogWarning("Cmd '%s' setpoint key '%s' is invalid or missing.",cmd.toStdString().c_str(),CHeat.toStdString().c_str());
        }
        else
        {
            // TODO: perform temperature conversion
            // thermostat operature in celcius.
            QString heat(QString::fromUtf8("%1").arg(ConvertFromThermostatUnits(vHeat.toDouble()),3,'f',0));

            emit sigSettingsHeat(heat.trimmed());
        }

        QJsonValue vCool(result.find(CCool).value());
        if (!vCool.isDouble())
        {
            LogWarning("Cmd '%s' setpoint key '%s' is invalid or missing.",cmd.toStdString().c_str(),CCool.toStdString().c_str());
        }
        else
        {
            // TODO: perform temperature conversion
            // thermostat operature in celcius.
            QString cool(QString::fromUtf8("%1").arg(ConvertFromThermostatUnits(vCool.toDouble()),3,'f',0));

            emit sigSettingsCool(cool.trimmed());
        }
    }
    else if (cmd == CCmdGetFan)
    {
        if (!vresult.isString())
        {
            LogWarning("Cmd '%s' payload key '%s' is invalid.",cmd.toStdString().c_str(),CKeyResult.toStdString().c_str());
            return;
        }

        m_fan = vresult.toString();

        emit sigSettingsFan(m_fan);
    }
    else if (cmd == CCmdPutSettings)
    {
        publishGetSettings();
    }
    else if (cmd == CCmdPutFan)
    {
        publishGetFan();
    }
    else
    {
        LogWarning("Unknown cmd '%s'",cmd.toStdString().c_str());
    }
}


void MCThermostat::slotSettingsChange(const QString& setting, const int& value)
{
    LogDebug("%s -> %d",setting.toStdString().c_str(),value);

    // TODO: perform temperature conversion
    // thermostat operature in celcius.
    double set(ConvertToThermostatUnits(value));

    QJsonObject setpoint;

    setpoint.insert(setting,QJsonValue(set));

    QJsonObject root;

    root.insert(CKeyCmd,CCmdPutSettings);
    root.insert(CKeyResult,QJsonValue(setpoint));

    QJsonDocument doc;
    doc.setObject(root);

    publishPutSettings(doc.toJson(QJsonDocument::Compact));
}


void MCThermostat::slotModeChange(const QString& mode)
{
    if (mode == COff) m_settingMode = CAuto;
    else if (mode == CAuto) m_settingMode = CCool;
    else if (mode == CCool) m_settingMode = CHeat;
    else if (mode == CHeat) m_settingMode = COff;

    QJsonObject setting;

    setting.insert(CKeyMode,QJsonValue(m_settingMode));

    QJsonObject root;

    root.insert(CKeyCmd,CCmdPutSettings);
    root.insert(CKeyResult,QJsonValue(setting));

    QJsonDocument doc;
    doc.setObject(root);

    publishPutSettings(doc.toJson(QJsonDocument::Compact));
}


void MCThermostat::slotFanChange(const QString& fan)
{
    if (m_fan == CAuto) m_fan = COn;
    else m_fan = CAuto;

    publishPutFan();
}

QByteArray MCThermostat::assembleFan()
{
    QJsonObject result;

    result.insert(CKeyFan,m_fan);

    QJsonObject root;

    root.insert(CKeyCmd,CCmdPutFan);
    root.insert(CKeyResult,result);

    QJsonDocument doc;
    doc.setObject(root);

    return doc.toJson(QJsonDocument::Compact);
}


void MCThermostat::publishPutSettings(const QByteArray& payload)
{
    Mqtt::instance()->publish(QString::fromUtf8("%1/action").arg(m_topic),payload,2);
}


void MCThermostat::publishGetSettings()
{
    Mqtt::instance()->publish(QString::fromUtf8("%1/action").arg(m_topic),QByteArray("{\"cmd\": \"get-settings\"}"),2);
}


void MCThermostat::publishPutFan()
{
    Mqtt::instance()->publish(QString::fromUtf8("%1/action").arg(m_topic),assembleFan(),2);
}


void MCThermostat::publishGetFan()
{
    Mqtt::instance()->publish(QString::fromUtf8("%1/action").arg(m_topic),QByteArray("{\"cmd\": \"get-fan\"}"),2);
}


double MCThermostat::ConvertFromThermostatUnits(double value)
{
    return (value * 9 / 5) + 32;
}


double MCThermostat::ConvertToThermostatUnits(double value)
{
    return (value - 32) * 5.0 / 9.0;
}
