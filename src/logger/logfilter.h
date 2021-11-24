/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGFILTER_H
#define LOGFILTER_H

#include <QObject>
#include <QRegularExpression>
#include <QVector>

#include "abslogwriter.h"


const QString CLogFilter("filter");


class LogFilter : public AbsLogWriter
{
public:
    LogFilter (QString alternateName = QString());
    ~LogFilter ();

    void addLogLevelExclusion (LogEntry::LogLevel level);
    void removeLogLevelExclusion (LogEntry::LogLevel level);

    void addContextExclusionPattern (QRegularExpression& regexp);
    void removeContextExclusionPattern (QRegularExpression& regexp);

    void addMethodExclusionPattern (QRegularExpression& regexp);
    void removeMethodExclusionPattern (QRegularExpression& regexp);

    void write(LogEntry* pEntry);

    bool setWriter(AbsLogWriter* pWriter);
    AbsLogWriter* getWriterByName(QString name);
    AbsLogWriter* getWriter();

protected:
    QString m_name;

    AbsLogWriter* m_pWriter;

    typedef QVector<LogEntry::LogLevel> LogLevelVector;
    typedef QVector<QRegularExpression> RegularExpressionVector;

    LogLevelVector m_levelExclusion;
    RegularExpressionVector m_methodExclusion;
    RegularExpressionVector m_contextExclusion;

private:
	// Prohibit copy and assignment.
    LogFilter & operator=(const LogFilter &right);
    LogFilter (const LogFilter &right);
};

#endif
