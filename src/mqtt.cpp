/*
 * Copyright 2021 grheard@gmail.com
 */

#include <errno.h>

#include "logger/logger.h"

#include <QMetaType>
#include <QStringList>

#include "mqtt.h"

Mqtt* Mqtt::m_instance(0);
QMutex Mqtt::m_instanceMutex;


Mqtt::Mqtt (QObject * parent)
	: QObject(parent),
	m_pMosquitto(0),
	m_keepalive(60),
	m_connected(false),
	m_debug(0)
{
	mosquitto_lib_init();

	int major;
	int minor;
	int rev;

	mosquitto_lib_version(&major,&minor,&rev);
	LogInfo("libmosquitto version %d.%d.%d",major,minor,rev);
}


Mqtt::~Mqtt ()
{
	if (m_pMosquitto)
	{
		mosquitto_disconnect(m_pMosquitto);

		int rc = mosquitto_loop_stop(m_pMosquitto,true);
		if (rc != MOSQ_ERR_SUCCESS)
		{
			LogError("Error stopping mosquitto loop %d  '%s'",rc,mosquitto_strerror(rc));
		}

		mosquitto_destroy(m_pMosquitto);
	}

	mosquitto_lib_cleanup();

	m_instance = 0;
}


void Mqtt::createInstance (QObject* parent)
{
	QMutexLocker lock(&m_instanceMutex);

	if (m_instance == 0)
	{
		m_instance = new Mqtt(parent);
	}
}


void Mqtt::removeInstance ()
{
	QMutexLocker lock(&m_instanceMutex);

	delete m_instance;
}


Mqtt* Mqtt::instance ()
{
	QMutexLocker lock(&m_instanceMutex);

	return m_instance;
}


bool Mqtt::initialize (QString id, bool cleanSession)
{
	LogTrace(0);
	if (m_pMosquitto)
	{
		mosquitto_destroy(m_pMosquitto);
		m_pMosquitto = 0;
	}

	m_pMosquitto = mosquitto_new((id.isEmpty()) ? 0 : id.toStdString().c_str(),(id.isEmpty()) ? true : cleanSession,this);
	if (m_pMosquitto == 0)
	{
		LogError("mosquitto_new() = %d: '%s'",errno,strerror(errno));
		return false;
	}

	mosquitto_log_callback_set(m_pMosquitto,wrapper_mosquitto_log);
	mosquitto_connect_callback_set(m_pMosquitto,wrapper_mosquitto_onconnect);
	mosquitto_disconnect_callback_set(m_pMosquitto,wrapper_mosquitto_ondisconnect);
	mosquitto_publish_callback_set(m_pMosquitto,wrapper_mosquitto_onpublish);
	mosquitto_subscribe_callback_set(m_pMosquitto,wrapper_mosquitto_onsubscribe);
	mosquitto_message_callback_set(m_pMosquitto,wrapper_mosquitto_onmessage);

	int rc = mosquitto_loop_start(m_pMosquitto);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		LogError("Error starting mosquitto loop %d  '%s'",rc,mosquitto_strerror(rc));
		mosquitto_destroy(m_pMosquitto);
		m_pMosquitto = 0;
		return false;
	}

	return true;
}


bool Mqtt::connect ()
{
	return connect(QString::fromUtf8("127.0.0.1"));
}


bool Mqtt::connect (QString host, int port)
{
	LogTrace(0);
	int rc = mosquitto_connect_async(m_pMosquitto,host.toStdString().c_str(),port,m_keepalive);
	switch (rc)
	{
		case MOSQ_ERR_INVAL:
			LogError("Invalid arguments.");
			return false;
		case MOSQ_ERR_ERRNO:
			LogError("Error %d '%s'",errno,strerror(errno));
			return false;
		case MOSQ_ERR_SUCCESS:
			LogInfo("Connection to %s in progress.",host.toStdString().c_str());
			return true;
	}

	LogWarning("Unhandled return value %d",rc);
	return false;
}


// bool Mqtt::reconnect ()
// {
// 	LogTrace(0);
// 	int rc = mosquitto_reconnect_async(m_pMosquitto);
// 	switch (rc)
// 	{
// 		case MOSQ_ERR_INVAL:
// 			LogError("Invalid arguments.");
// 			return false;
// 		case MOSQ_ERR_NOMEM:
// 			LogError("No mem.");
// 			return false;
// 		case MOSQ_ERR_ERRNO:
// 			LogError("Error %d '%s'",errno,strerror(errno));
// 			return false;
// 		case MOSQ_ERR_SUCCESS:
// 			LogInfo("Reconnect in progress.");
// 			return true;
// 	}

// 	LogWarning("Unhandled return value %d",rc);
// 	return false;
// }


bool Mqtt::disconnect ()
{
	int rc = mosquitto_disconnect(m_pMosquitto);
	switch (rc)
	{
		case MOSQ_ERR_INVAL:
			LogError("Invalid arguments.");
			return false;
		case MOSQ_ERR_NO_CONN:
			LogInfo("Not connected.");
			return true;
		case MOSQ_ERR_SUCCESS:
			LogInfo("Disconnected.");
			return true;
	}

	LogWarning("Unhandled return value %d",rc);
	return false;
}


bool Mqtt::isConnected ()
{
	return m_connected;
}


bool Mqtt::setWill (const QString& topic, const QByteArray& payload, int qos, bool retain)
{
	int rc = mosquitto_will_set(m_pMosquitto,
								topic.toStdString().c_str(),
								payload.size(),
								payload.data(),
								qos,
								retain);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		LogError("%d  '%s'",rc,mosquitto_strerror(rc));
		return false;
	}

	return true;
}


bool Mqtt::clearWill ()
{
	int rc = mosquitto_will_clear(m_pMosquitto);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		LogError("%d  '%s'",rc,mosquitto_strerror(rc));
		return false;
	}

	return true;
}


void Mqtt::publish (const QString& topic, const QByteArray& payload, int qos, bool retain)
{
	int mid(0);

	int rc = mosquitto_publish(m_pMosquitto,
								&mid,
								topic.toStdString().c_str(),
								payload.size(),
								payload.data(),
								qos,
								retain);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		LogError("%d  '%s'",rc,mosquitto_strerror(rc));
	}
	else
	{
		if (m_debug >= 3) LogDebug("Queued publish mid %d",mid);
	}
}


bool Mqtt::subscribe (const QString& topic, int qos)
{
	int mid(0);

	int rc = mosquitto_subscribe(m_pMosquitto,
								&mid,
								topic.toStdString().c_str(),
								qos);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		LogError("%d  '%s'",rc,mosquitto_strerror(rc));
		return false;
	}

	if (m_debug >= 3) LogDebug("Queued subscription mid %d",mid);
	return true;
}


void Mqtt::setKeepAlive (int seconds)
{
	m_keepalive = seconds;
}


void Mqtt::setDebug (int level)
{
	m_debug = level;
}


void Mqtt::wrapper_mosquitto_log (struct mosquitto* pMosquitto, void* pObject, int logLevel, const char* errorString)
{
	((class Mqtt *)pObject)->mosquitto_log(logLevel,errorString);
}


void Mqtt::mosquitto_log (int logLevel, const char* errorString)
{
	switch (logLevel)
	{
		case MOSQ_LOG_DEBUG:
			if (m_debug >= 3) LogDebug("%s",errorString);
			break;
		case MOSQ_LOG_INFO:
		case MOSQ_LOG_NOTICE:
			if (m_debug >= 2) LogInfo("%s",errorString);
			break;
		case MOSQ_LOG_WARNING:
			if (m_debug >= 1) LogWarning("%s",errorString);
			break;
		case MOSQ_LOG_ERR:
			LogError("%s",errorString);
			break;
		default:
			LogError("Unknown %d  %s",logLevel,errorString);
			break;
	}
}


void Mqtt::wrapper_mosquitto_onconnect (struct mosquitto* pMosquitto, void* pObject, int result)
{
	((class Mqtt *)pObject)->mosquitto_onconnect(result);
}


void Mqtt::mosquitto_onconnect (int result)
{
	LogInfo("%d %s",result,mosquitto_connack_string(result));
	m_connected = (result == MOSQ_ERR_SUCCESS);
	emit onConnect(m_connected);
}


void Mqtt::wrapper_mosquitto_ondisconnect (struct mosquitto* pMosquitto, void* pObject, int result)
{
	((class Mqtt *)pObject)->mosquitto_ondisconnect(result);
}


void Mqtt::mosquitto_ondisconnect (int result)
{
	m_connected = false;
	if (result != 0)
	{
		LogWarning("%d",result);
		emit onDisconnect();
	}
}


void Mqtt::wrapper_mosquitto_onpublish (struct mosquitto* pMosquitto, void* pObject, int mid)
{
	((class Mqtt *)pObject)->mosquitto_onpublish(mid);
}


void Mqtt::mosquitto_onpublish (int mid)
{
	if (m_debug >= 3) LogDebug("Sent publish mid %d",mid);
}


void Mqtt::wrapper_mosquitto_onsubscribe (struct mosquitto* pMosquitto, void* pObject, int mid, int qosCount, const int* qosGranted)
{
	((class Mqtt *)pObject)->mosquitto_onsubscribe(mid,qosCount,qosGranted);
}


void Mqtt::mosquitto_onsubscribe (int mid, int qosCount, const int* qosGranted)
{
	QStringList qosList;
	for (int i = 0; i < qosCount; i++)
	{
		qosList.append(QString("%1").arg(qosGranted[i]));
	}

	if (m_debug >= 3) LogDebug("Sent subscription mid %d qosCount=%d qosGranted=%s",mid,qosCount,qosList.join(" ").toStdString().c_str());
}


void Mqtt::wrapper_mosquitto_onmessage (struct mosquitto* pMosquitto, void* pObject, const struct mosquitto_message* pMessage)
{
	((class Mqtt *)pObject)->mosquitto_onmessage(pMessage);
}


void Mqtt::mosquitto_onmessage (const struct mosquitto_message* pMessage)
{
	QByteArray payload;
	if (pMessage->payloadlen != 0)
	{
		payload.append((const char *)pMessage->payload,pMessage->payloadlen);
	}
	emit onMessage(QString(pMessage->topic),QByteArray((const char*)pMessage->payload,pMessage->payloadlen));
}
