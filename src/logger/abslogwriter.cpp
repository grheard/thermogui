/*
 * Copyright 2021 grheard@gmail.com
 */

#include "abslogwriter.h"


AbsLogWriter::AbsLogWriter()
	: m_level(LogEntry::Trace)
{
}


void AbsLogWriter::setLogLevel(LogEntry::LogLevel level)
{
	m_level = level;
}


LogEntry::LogLevel AbsLogWriter::getLogLevel()
{
	return m_level;
}


bool AbsLogWriter::setWriter(AbsLogWriter* pWriter)
{
	return false;
}


AbsLogWriter* AbsLogWriter::getWriterByName(QString name)
{
	return 0;
}


AbsLogWriter* AbsLogWriter::getWriter()
{
	return 0;
}
