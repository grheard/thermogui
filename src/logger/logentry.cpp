/*
 * Copyright 2021 grheard@gmail.com
 */

#include "logentry.h"
#include <QDateTime>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


LogEntry::LogEntry ()
    : m_pContext(0),
      m_logLevel(Debug),
      m_serviceName("notset"),
	  m_method("GLogEntry::GLogEntry"),
	  m_logEntry("Default constructor invoked.")
{
    QDateTime date(QDateTime::currentDateTime());

    m_dateTime = date.toString("yyyy-MM-dd HH:mm:ss.zzz");
}

LogEntry::LogEntry (const LogEntry& log)
{
    m_pContext = log.m_pContext;
	m_logLevel = log.m_logLevel;
	m_serviceName = log.m_serviceName;
	m_method = log.m_method;
	m_logEntry = log.m_logEntry;
    m_dateTime = log.m_dateTime;
}


LogEntry::LogEntry (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, ...)
{
    va_list pArgs;
    va_start(pArgs,pFormat);

    Create(pContext,pServiceName,level,pMethod,pFormat,pArgs);

    va_end(pArgs);
}


LogEntry::LogEntry (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, va_list pArgs)
{
    Create(pContext,pServiceName,level,pMethod,pFormat,pArgs);
}


LogEntry::~LogEntry()
{
}


void LogEntry::Create (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, va_list pArgs)
{
    QDateTime date(QDateTime::currentDateTime());

    m_pContext = pContext;

    m_dateTime = date.toString("yyyy-MM-dd HH:mm:ss.zzz");

	if (pServiceName && (strlen(pServiceName) > 0))
	{
        m_serviceName = QString::fromUtf8(pServiceName);
	}

	m_logLevel = level;

	if (pMethod && (strlen(pMethod) > 0))
	{
        m_method = QString::fromUtf8(pMethod);
	}

	if (!pFormat)
	{
		return;
	}

	if (strlen(pFormat) <= 0)
	{
		return;
	}

    m_logEntry.vsprintf(pFormat,pArgs);
}

void LogEntry::ScrubLog ()
{
	Scrub(m_serviceName);
	Scrub(m_method);
	Scrub(m_logEntry);
}


void LogEntry::Scrub (QString& str)
{
    QString::size_type pos(-1);
	do
	{
        pos = str.indexOf("\r\n");
        if (pos == -1)
        {
            pos = str.indexOf("\n");
            if (pos == -1)
            {
                pos = str.indexOf("\t");
            }
        }

        if (pos >= 0)
        {
            QString::size_type back(pos);
            for (;;)
            {
                back--;
                if ((back < 0) || (!str.at(back).isSpace()))
                {
                    back++;
                    break;
                }
            }

            QString::size_type front(pos);
            for (;;)
            {
                front++;
                if ((front >= str.length()) || (!str.at(front).isSpace()))
                {
                    break;
                }
            }

            str = str.replace(back,front - back, " ");
        }
	}
    while (pos != -1);
}

QString LogEntry::LogLevelToString (LogLevel level)
{
    switch (level)
    {
        case Trace:
            return QString("TT");
        case Debug:
            return QString("DD");
        case Info:
            return QString("II");
        case Warning:
            return QString("WW");
        case Error:
            return QString("EE");
        default:
            return QString("??");
    }
}

void LogEntry::ToString (QString& logMessage)
{
    ScrubLog();

	logMessage.clear();

    logMessage += m_dateTime
            + QString(" ") + m_serviceName
            + QString(" ") + LogLevelToString(m_logLevel);

    logMessage += QString(" ")
            + GetContextString();

    QString method(GetMethod(true));
    if (!method.isEmpty())
    {
    	logMessage += " " + method;
    }

    if (!m_logEntry.isEmpty())
	{
		logMessage += " - " + m_logEntry;
	}
}

QString LogEntry::GetMethod(bool formatted)
{
    QString method(m_method);

    if (formatted && !method.isEmpty())
	{

		// Try and identify the method as a __PRETTY_FUNCTION__
        int openparen = method.indexOf('(');
        int closeparen = method.indexOf(')');
        if ((openparen != -1) &&
            (closeparen != -1) &&
			(closeparen > openparen))
		{
			// Looks like a __PRETTY_FUNCTION__, steal the class::method.

			// Look to remove any return type (i.e. void, bool, int, etc.)
			// Look for the last space character before the '('.
            QString::size_type space(0);
            QString::size_type spacetmp(-1);
			do
			{
                spacetmp = method.indexOf(' ',space + 1);
                if ((spacetmp != -1) &&
					(spacetmp < openparen))
				{
					space = spacetmp;
				}
			}
			while (space == spacetmp);

			if (space != 0)
			{
				// If a space was found (non-zero) then the return type needs to be stripped.
                method = method.mid(space + 1,openparen - space - 1);
			}
			else
			{
				// No return type found. Most likely a constructor/destructor.
                method = method.mid(0,openparen);
			}
		}
	}

    return method;
}
