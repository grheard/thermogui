/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "logger/logger.h"
#include "mcWpaIf.h"
#include "mqtt.h"
#include "config.h"

// {"command": "STATUS", "result": {"bssid": "10:bf:48:53:83:30", "freq": "2427", "ssid": "rt-n66u-2", "id": "0", "id_str": "rt-n66u-2", "mode": "station", "pairwise_cipher": "CCMP", "group_cipher": "CCMP", "key_mgmt": "WPA2-PSK", "wpa_state": "COMPLETED", "ip_address": "192.168.2.195", "p2p_device_address": "e6:5f:01:50:d4:fc", "address": "e4:5f:01:50:d4:fc", "uuid": "1978ea11-66b4-5548-a287-58f4ca694fbe"}}
// {"command": "SIGNAL_POLL", "result": {"RSSI": "-47", "LINKSPEED": "72", "NOISE": "9999", "FREQUENCY": "2427", "WIDTH": "20 MHz", "CENTER_FRQ1": "2427"}}

const QString CTopicWpaIf(QString::fromUtf8("wpaif"));

const QString CKeyCmd(QString::fromUtf8("command"));
const QString CKeyResult(QString::fromUtf8("result"));

const QString CKeyStatus(QString::fromUtf8("STATUS"));
const QString CKeySignalPoll(QString::fromUtf8("SIGNAL_POLL"));


MCWpaIf::MCWpaIf(QObject* parent)
    : QObject(parent)
    , propertyName(QString::fromUtf8("mcwpaif"))
    , m_topic(CTopicWpaIf)
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


MCWpaIf::~MCWpaIf()
{
}


void MCWpaIf::onMqttConnect(bool result)
{
    if (result)
    {
        LogInfo("Subscribing to '%s'",m_topic.toStdString().c_str());
        Mqtt::instance()->subscribe(m_topic,2);
    }
}


void MCWpaIf::onMqttDisconnect()
{
    LogWarning("Broker disconnected.");
}


void MCWpaIf::onMqttMessage(const QString& topic, const QByteArray& payload)
{
    if (topic != m_topic)
    {
        return;
    }

    // payload must be a json object
    QJsonDocument doc(QJsonDocument::fromJson(payload));
    if (doc.isNull() || doc.isEmpty() || !doc.isObject())
    {
        LogWarning("Payload for topic '%s' is not a valid json object '%s'",m_topic.toStdString().c_str(),QString::fromLocal8Bit(payload).toStdString().c_str());
        return;
    }

    QJsonValue cmd(doc.object()[CKeyCmd]);
    if (!cmd.isString())
    {
        LogWarning("Key '%s' is invalid or missing.",CKeyCmd.toStdString().c_str());
        return;
    }

    QJsonValue result(doc.object()[CKeyResult]);
    if (!result.isObject())
    {
        LogWarning("Key '%s' is invalid or missing.",CKeyResult.toStdString().c_str());
        return;
    }

    if (cmd.toString() == CKeyStatus) emit sigStatus(result.toObject());
    else if (cmd.toString() == CKeySignalPoll) emit sigSignalPoll(result.toObject());
    else
    {
        LogWarning("Unknown command '%s'",cmd.toString().toStdString().c_str());
    }
}
