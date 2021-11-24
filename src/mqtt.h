/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef MQTT_H
#define MQTT_H


#include <QObject>
#include <QMutex>

#include <mosquitto.h>


class Mqtt : public QObject
{
	Q_OBJECT

public:
	static void createInstance (QObject* parent = 0);
	static void removeInstance ();
	static Mqtt* instance ();

	bool initialize (QString id, bool cleanSession = true);
	bool connect ();
	bool connect (QString host, int port = 1883);
	// bool reconnect ();
	bool disconnect ();

	bool isConnected ();

	// setWill, if used, must be called prior to connect.
	bool setWill (const QString& topic, const QByteArray& payload, int qos, bool retain = false);
	bool clearWill ();

	bool subscribe (const QString& topic, int qos);

	void setKeepAlive (int seconds);

	void setDebug(int level);

signals:
	void onConnect (bool result);
	void onDisconnect ();
	void onMessage (const QString& topic, const QByteArray& payload);

public slots:
	void publish (const QString& topic, const QByteArray& payload, int qos, bool retain = false);

private:
	Mqtt (QObject* parent = 0);
	~Mqtt ();
	// Disable copy/assignment
	// Mqtt(const Mqtt& copy);
	// Mqtt& operator=(const Mqtt& right);

	static Mqtt* m_instance;
	static QMutex m_instanceMutex;

	static void wrapper_mosquitto_log (struct mosquitto* pMosquitto, void* pObject, int logLevel, const char* errorString);
	void mosquitto_log (int logLevel, const char* errorString);

	static void wrapper_mosquitto_onconnect (struct mosquitto* pMosquitto, void* pObject, int result);
	void mosquitto_onconnect (int result);

	static void wrapper_mosquitto_ondisconnect (struct mosquitto* pMosquitto, void* pObject, int result);
	void mosquitto_ondisconnect (int result);

	static void wrapper_mosquitto_onpublish (struct mosquitto* pMosquitto, void* pObject, int mid);
	void mosquitto_onpublish (int mid);

	static void wrapper_mosquitto_onsubscribe (struct mosquitto* pMosquitto, void* pObject, int mid, int qosCount, const int* qosGranted);
	void mosquitto_onsubscribe (int mid, int qosCount, const int* qosGranted);

	static void wrapper_mosquitto_onmessage (struct mosquitto* pMosquitto, void* pObject, const struct mosquitto_message* pMessage);
	void mosquitto_onmessage (const struct mosquitto_message* pMessage);

	struct mosquitto* m_pMosquitto;

	int m_keepalive;

	bool m_connected;

	int m_debug;
};

#endif
