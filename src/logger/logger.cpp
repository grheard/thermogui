/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QCoreApplication>
#include <QMutexLocker>
#include <QTimer>

#include "logger.h"


QMutex Logger::m_instanceMutex(QMutex::NonRecursive);
Logger * Logger::m_pInstance(0);


Logger::Logger ()
	: QThread(0)
	, m_run(true)
	, m_suspend(false)
{
	start();
}


Logger::~Logger ()
{
	m_run = false;
	m_qSem.release();
	wait();
}


Logger * Logger::Instance ()
{
    QMutexLocker lock(&m_instanceMutex);

    if (m_pInstance == 0)
    {
        m_pInstance = new Logger();
    }

    return m_pInstance;
}


void Logger::RemoveInstance()
{
    QMutexLocker lock(&m_instanceMutex);

    delete m_pInstance;
    m_pInstance = 0;
}


void Logger::setServiceName (QString serviceName)
{
    m_serviceName = serviceName;
}


void Logger::addWriter(AbsLogWriter* pWriter)
{
	if (pWriter)
	{
		QMutexLocker lock(&m_writerMutex);

		m_writers.append(pWriter);
	}
}


AbsLogWriter* Logger::getWriterByName(QString name)
{
	QMutexLocker lock(&m_writerMutex);

	for (int i = 0; i < m_writers.count(); i++)
	{
		AbsLogWriter* pWriter = m_writers[i]->getWriterByName(name);
		if (pWriter)
		{
			return pWriter;
		}
	}

	return 0;
}


void Logger::write (void *pContext, LogEntry::LogLevel level, const char *pMethod, const char *pFormat, ...)
{
    va_list pArgs;
    va_start(pArgs,pFormat);

    LogEntry* pEntry = new LogEntry(pContext,m_serviceName.toStdString().c_str(),level,pMethod,pFormat,pArgs);

    va_end(pArgs);

    m_qMutex.lock();

    m_queue.enqueue(pEntry);

    m_qMutex.unlock();

    m_qSem.release();
}


void Logger::suspend()
{
	m_suspend = true;
}


void Logger::resume()
{
	m_suspend = false;
	m_qSem.release();
}


void Logger::run()
{

	while(m_run)
	{
		m_qSem.acquire();

		m_qMutex.lock();
		while(!m_queue.isEmpty() && !m_suspend)
		{
			LogEntry* pEntry = m_queue.dequeue();
			m_qMutex.unlock();
			if (pEntry)
			{
				QMutexLocker lock(&m_writerMutex);
				for (int i = 0; i < m_writers.count(); i++)
				{
					m_writers[i]->write(pEntry);
				}

				delete pEntry;
			}
			m_qMutex.lock();
		}
		m_qMutex.unlock();
	}
}
