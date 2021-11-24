/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QMutexLocker>
#include "logthrottle.h"


LogThrottle::LogThrottle (QString alternateName)
	: QObject(0)
	, m_name(alternateName.isEmpty() ? CLogThrottle : alternateName)
	, m_pWriter(0)
    , m_count(-1)
    , m_timer(this)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    m_timer.setInterval(CRepeatingLogNotificationThresholdMilliseconds);
    m_timer.setSingleShot(true);

    connect(this,SIGNAL(startTimer()),&m_timer,SLOT(start()),Qt::QueuedConnection);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()),Qt::QueuedConnection);
}


LogThrottle::~LogThrottle ()
{
    m_timer.stop();

    QMutexLocker lock(&m_logMutex);

    NotifyRepeats();
}


void LogThrottle::setTime(int milliseconds)
{
	m_timer.setInterval(milliseconds);
}


void LogThrottle::write (LogEntry* pEntry)
{
    QMutexLocker lock(&m_logMutex);

    if ((m_count >= 0)
    		&& (m_logMessage.GetLevel() == pEntry->GetLevel())
            && (m_logMessage.GetContext() == pEntry->GetContext())
            && (m_logMessage.GetMethod().compare(pEntry->GetMethod()) == 0)
            && (m_logMessage.GetEntry().compare(pEntry->GetEntry()) == 0))
	{
		// Increment the count to track the number of duplicates.
		m_count++;
	}
	else
	{
        emit startTimer();

		// Log accumulated repeats before logging the new message.
		NotifyRepeats();

		// Set the count to 0 to allow for the tracking of duplicate messages.
		m_count = 0;
        m_logMessage = *pEntry;

        if (m_pWriter) m_pWriter->write(pEntry);
    }
}


bool LogThrottle::setWriter(AbsLogWriter* pWriter)
{
	m_pWriter = pWriter;
	return true;
}


AbsLogWriter* LogThrottle::getWriterByName(QString name)
{
	if (name == m_name)	return this;

	if (m_pWriter) return m_pWriter->getWriterByName(name);

	return 0;
}


AbsLogWriter* LogThrottle::getWriter()
{
	if (m_pWriter) return m_pWriter;

	return 0;
}


void LogThrottle::timeout ()
{
    QMutexLocker lock(&m_logMutex);

	NotifyRepeats();

	// Reset the value to -1 to let the next log entry (even a duplicate) slip through
	m_count = -1;
}


void LogThrottle::NotifyRepeats ()
{
	if (m_count > 0)
	{
        LogEntry message(m_logMessage.GetContext(),
            m_logMessage.GetServiceName().toStdString().c_str(),
            m_logMessage.GetLevel(),
            m_logMessage.GetMethod().toStdString().c_str(),
            "%s (Repeated %d times.)",
            m_logMessage.GetEntry().toStdString().c_str(),
            m_count);

        if (m_pWriter) m_pWriter->write(&message);
	}
}
