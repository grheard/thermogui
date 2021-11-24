/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef MCWPAIF_H
#define MCWPAIF_H


#include <QObject>
#include <QJsonDocument>


class MCWpaIf : public QObject
{
    Q_OBJECT

public:
    MCWpaIf(QObject* parent = 0);
    ~MCWpaIf();

    const QString propertyName;

    void processMessage(const QString& topic, const QByteArray& payload);

    Q_SLOT void onMqttConnect(bool result);
    Q_SLOT void onMqttDisconnect();
    Q_SLOT void onMqttMessage(const QString& topic, const QByteArray& payload);

    Q_SIGNAL void sigStatus(const QJsonObject& status);
    Q_SIGNAL void sigSignalPoll(const QJsonObject& signalPoll);

protected:
    QString m_topic;

};

#endif
