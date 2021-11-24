/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QRegularExpressionMatch>
#include "logfilter.h"


LogFilter::LogFilter (QString alternateName)
    : m_name(alternateName.isEmpty() ? CLogFilter : alternateName)
	, m_pWriter(0)
{
}


LogFilter::~LogFilter ()
{
}


void LogFilter::addLogLevelExclusion(LogEntry::LogLevel level)
{
    if (m_levelExclusion.indexOf(level) == -1)
    {
        m_levelExclusion.push_back(level);
    }
}


void LogFilter::removeLogLevelExclusion(LogEntry::LogLevel level)
{
    int index(m_levelExclusion.indexOf(level));
    if (index != -1)
    {
        m_levelExclusion.remove(index);
    }
}


void LogFilter::addContextExclusionPattern(QRegularExpression &regexp)
{
    if (m_contextExclusion.indexOf(regexp) == -1)
    {
        m_contextExclusion.push_back(regexp);
    }
}


void LogFilter::removeContextExclusionPattern(QRegularExpression &regexp)
{
    int index(m_contextExclusion.indexOf(regexp));
    if (index != -1)
    {
        m_contextExclusion.remove(index);
    }
}


void LogFilter::addMethodExclusionPattern(QRegularExpression &regexp)
{
    if (m_methodExclusion.indexOf(regexp) == -1)
    {
        m_methodExclusion.push_back(regexp);
    }
}


void LogFilter::removeMethodExclusionPattern(QRegularExpression &regexp)
{
    int index(m_methodExclusion.indexOf(regexp));
    if (index != -1)
    {
        m_methodExclusion.remove(index);
    }
}


void LogFilter::write (LogEntry* pEntry)
{
    if (m_levelExclusion.indexOf(pEntry->GetLevel()) != -1)
    {
        return;
    }

    for (RegularExpressionVector::iterator it = m_contextExclusion.begin();
         it != m_contextExclusion.end(); ++it)
    {
        if (it->match(pEntry->GetContextString()).hasMatch())
        {
            return;
        }
    }

    for (RegularExpressionVector::iterator it = m_methodExclusion.begin();
         it != m_methodExclusion.end(); ++it)
    {
        if (it->match(pEntry->GetMethod()).hasMatch())
        {
            return;
        }
    }

    if (m_pWriter) m_pWriter->write(pEntry);
}


bool LogFilter::setWriter(AbsLogWriter* pWriter)
{
	m_pWriter = pWriter;
	return true;
}


AbsLogWriter* LogFilter::getWriterByName(QString name)
{
	if (name == m_name)	return this;

	if (m_pWriter) return m_pWriter->getWriterByName(name);

	return 0;
}


AbsLogWriter* LogFilter::getWriter()
{
	if (m_pWriter) return m_pWriter;

	return 0;
}
