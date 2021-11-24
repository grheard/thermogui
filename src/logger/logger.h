/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QSemaphore>

#include <stdarg.h>

#include "logentry.h"
#include "abslogwriter.h"

#include "logfilter.h"
#include "logthrottle.h"
#include "logwriterfile.h"
#include "logwritersyslog.h"


#define LogTrace(format, ...) Logger::Instance()->write(this,LogEntry::Trace,__PRETTY_FUNCTION__,format, ## __VA_ARGS__)
#define LogDebug(format, ...) Logger::Instance()->write(this,LogEntry::Debug,__PRETTY_FUNCTION__,format, ## __VA_ARGS__)
#define LogInfo(format, ...) Logger::Instance()->write(this,LogEntry::Info,__PRETTY_FUNCTION__,format, ## __VA_ARGS__)
#define LogWarning(format, ...) Logger::Instance()->write(this,LogEntry::Warning,__PRETTY_FUNCTION__,format, ## __VA_ARGS__)
#define LogError(format, ...) Logger::Instance()->write(this,LogEntry::Error,__PRETTY_FUNCTION__,format, ## __VA_ARGS__)


class Logger : public QThread
{
    Q_OBJECT

public:
    static Logger * Instance();

    static void RemoveInstance();

    void setServiceName(QString serviceName);

    void write(void * pContext, LogEntry::LogLevel level, const char * pMethod, const char * pFormat, ...);

    void addWriter(AbsLogWriter* pWriter);

    AbsLogWriter* getWriterByName(QString name);

    void suspend();
    void resume();

    bool isSuspended() { return m_suspend; }

protected:
    Logger ();
    ~Logger ();

    void run () Q_DECL_OVERRIDE;

    static QMutex m_instanceMutex;
    static Logger * m_pInstance;

    bool m_run;

    QString m_serviceName;

	QMutex m_qMutex;
	QSemaphore m_qSem;
    QQueue<LogEntry*> m_queue;

    QMutex m_writerMutex;
    QList<AbsLogWriter*> m_writers;

    bool m_suspend;
};

#endif
