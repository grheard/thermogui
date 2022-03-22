/*
 * Copyright 2021 grheard@gmail.com
 */

#include "config.h"

#include <QCoreApplication>
#include <QMutexLocker>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>

#include "logger/logger.h"


const QString CCommon(QString::fromUtf8("common"));
const QString CTopicRoot(QString::fromUtf8("topic-root"));

const QString CGui(QString::fromUtf8("gui"));
const QString CMqttDebug(QString::fromUtf8("mqtt-debug"));

const QString CLogger(QString::fromUtf8("logger"));
const QString CHandlers(QString::fromUtf8("handlers"));
const QString CFile(QString::fromUtf8("file"));
const QString CFilename(QString::fromUtf8("filename"));


QMutex Config::m_instanceMutex;
Config* Config::m_instance(0);


Config::Config (QObject* parent)
    : QObject(parent)
    , m_mqttDebug(0)
{
    QCommandLineParser parser;

    QCommandLineOption config(QString::fromUtf8("config"),QString::fromUtf8("configuration"),QString::fromUtf8("config"));
    parser.addOption(config);

    parser.parse(QCoreApplication::arguments());

    if (!parser.isSet(config))
    {
        LogWarning("Config command line option is missing.");
        return;
    }

    // First, try to parse the config option directly.
    QJsonDocument doc(QJsonDocument::fromJson(parser.value(config).toUtf8()));
    if (doc.isNull())
    {
        LogInfo("'%s' is not a valid JSON object, it will be attempted as a file.",parser.value(config).toStdString().c_str());

        QFile file(parser.value(config));

        if (!file.open(QIODevice::ReadOnly))
        {
            LogWarning("'%s' is neither JSON or a file.");
            return;
        }

        QByteArray data(file.readAll());
        doc = QJsonDocument::fromJson(data);
        if (doc.isNull())
        {
            LogWarning("File '%s' does not contain a valid JSON object.",parser.value(config).toStdString().c_str());
            return;
        }
    }

    QJsonObject common(doc.object()[CCommon].toObject());
    if (common.isEmpty())
    {
        LogWarning("The '%s' object is either missing or empty.",CCommon.toStdString().c_str());
    }
    else
    {
        parseLogger(common);

        QJsonValue topicroot(common[CTopicRoot]);
        if (!topicroot.isString())
        {
            LogWarning("The '%s' object is either missing or not a string.",CTopicRoot.toStdString().c_str());
        }
        else
        {
            m_topicRoot = topicroot.toString();
        }
    }

    QJsonObject gui(doc.object()[CGui].toObject());
    if (common.isEmpty())
    {
        LogWarning("The '%s' object is either missing or empty.",CGui.toStdString().c_str());
    }
    else
    {
        parseLogger(gui);

        QJsonValue mqttDebug(gui[CMqttDebug]);
        if (mqttDebug.isUndefined() || mqttDebug.toInt(-1) == -1)
        {
            LogDebug("The '%s' object is missing or not an integer.",CMqttDebug.toStdString().c_str());
        }
        else
        {
            m_mqttDebug = mqttDebug.toInt();
        }
    }

    LogInfo("Log file set to '%s'",m_logFilename.toStdString().c_str());
    LogInfo("Topic root set to '%s'",m_topicRoot.toStdString().c_str());
    LogInfo("Mqtt debug is %d",m_mqttDebug);
}


Config::~Config ()
{
    m_instance = 0;
}


void Config::createInstance(QObject* parent)
{
    QMutexLocker lock(&m_instanceMutex);

    if (m_instance == 0)
    {
        m_instance = new Config(parent);
    }
}


Config* Config::instance()
{
    return m_instance;
}


void Config::parseLogger(QJsonObject& object)
{
    QJsonObject logger(object[CLogger].toObject());
    if (logger.isEmpty()) return;

    QJsonObject handlers(logger[CHandlers].toObject());
    if (handlers.isEmpty()) return;

    QJsonObject file(handlers[CFile].toObject());
    if (file.isEmpty()) return;

    QString filename(file[CFilename].toString());
    if (filename.isEmpty()) return;

    m_logFilename = filename;
}


const QString& Config::getLogFilename()
{
    return m_logFilename;
}


const QString& Config::getTopicRoot()
{
    return m_topicRoot;
}


int Config::getMqttDebug()
{
    return m_mqttDebug;
}
