/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <QObject>
#include <QString>
#include <stdarg.h>

class LogEntry
{
public:
    enum LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error
    };

    LogEntry ();
    LogEntry (const LogEntry& log);
    LogEntry (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, ...);
    LogEntry (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, va_list pArgs);
    ~LogEntry ();

    void * GetContext () { return m_pContext; }
    QString GetContextString () { return QString("%1").arg((quint64)m_pContext, 2 * sizeof(void *), 16, QChar('0')); }
    QString GetServiceName () { return m_serviceName; }
    LogLevel GetLevel () { return m_logLevel; }
    QString GetMethod (bool formatted = false);
    const QString& GetEntry () { return m_logEntry; }

    // ToString() - Compose log entry into a string.
    void ToString (QString& logStr);

private:
   void Create (void * pContext, const char * pServiceName, LogLevel level, const char * pMethod, const char * pFormat, va_list pArgs);

   void ScrubLog ();
   void Scrub (QString& str);

   QString LogLevelToString (LogLevel level);

   void * m_pContext;
   LogLevel m_logLevel;
   QString m_serviceName;
   QString m_method;
   QString m_logEntry;
   QString m_dateTime;
};


#endif
