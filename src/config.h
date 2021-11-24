/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QMutex>
#include <QJsonObject>


class Config : QObject
{
    Q_OBJECT

public:
    static void createInstance(QObject* parent = 0);
    static Config* instance();

    const QString& getLogFilename();

    const QString& getTopicRoot();

    int getMqttDebug();

protected:
    void parseLogger(QJsonObject& object);

    static QMutex m_instanceMutex;
    static Config* m_instance;

    QString m_logFilename;
    QString m_topicRoot;
    int m_mqttDebug;

private:
    Config (QObject* parent = 0);
    ~Config ();

};

#endif
