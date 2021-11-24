/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QCoreApplication>

#include <syslog.h>

#include "logwritersyslog.h"


LogWriterSyslog::LogWriterSyslog (QString alternateName)
    : m_name(alternateName.isEmpty() ? CLogWriterSyslog : alternateName)
	, m_opened(false)
{
}


LogWriterSyslog::~LogWriterSyslog ()
{
	closelog();
}


void LogWriterSyslog::write (LogEntry* pEntry)
{
	if (!pEntry) return;

	if (pEntry->GetLevel() < m_level) return;

	if (!m_opened)
	{
		m_opened = true;
		// Make a local copy of the service name. openlog()
        // copies the pointer not the string pointed to.
		m_serviceName = pEntry->GetServiceName().toStdString();
		openlog(m_serviceName.c_str(),LOG_NDELAY,LOG_USER);
	}

	int priority;
	QString log;
	formatEntry(pEntry,priority,log);

	syslog(priority,"%s",log.toStdString().c_str());
}


AbsLogWriter* LogWriterSyslog::getWriterByName(QString name)
{
	if (name == m_name)
	{
		return this;
	}

	return 0;
}


void LogWriterSyslog::formatEntry(LogEntry* pLogEntry, int& priority, QString& logEntry)
{
	switch(pLogEntry->GetLevel())
	{
		case LogEntry::Error:
			priority = LOG_ERR;
			break;
		case LogEntry::Warning:
			priority = LOG_WARNING;
			break;
		case LogEntry::Info:
			priority = LOG_INFO;
			break;
		case LogEntry::Debug:
		case LogEntry::Trace:
		default:
			priority = LOG_DEBUG;
			break;
	}

	logEntry = pLogEntry->GetMethod(true);

	if (!pLogEntry->GetEntry().isEmpty())
	{
		logEntry += " - " + pLogEntry->GetEntry();
	}
}
