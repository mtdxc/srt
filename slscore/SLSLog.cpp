
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Edward.Wu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <stdio.h>
#include <string.h>
#include <stddef.h>
#ifndef _WIN32
#include <strings.h>
#endif

#include "SLSLog.hpp"

static char const * LOG_LEVEL_NAME[] = {
    "FATAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};

static const char APP_NAME[] = "SLS";

CSLSLog* CSLSLog::m_pInstance = NULL;

CSLSLog::CSLSLog()
{
	m_level = SLS_LOG_INFO;
	m_log_file   = NULL;
    log_filename[0] = 0;
}

CSLSLog::~CSLSLog()
{
    if (m_log_file) {
        fclose(m_log_file);
        m_log_file = NULL;
    }
}

int CSLSLog::create_instance()
{
    if (!m_pInstance) {
    	m_pInstance = new CSLSLog();
    	m_pInstance->m_level = SLS_LOG_ERROR;//default
		return 0;
    }
	return -1;
}

int CSLSLog::destory_instance()
{
	SAFE_DELETE(m_pInstance);
	return 0;
}

void CSLSLog::log(int level, const char *fmt, ...)
{
    if (!m_pInstance)
    	m_pInstance = new CSLSLog();
    if (level > m_pInstance->m_level)
        return;

	va_list vl;
    va_start(vl, fmt);
	m_pInstance->print_log(level, fmt, vl);
	va_end (vl);
}


void CSLSLog::print_log(int level, const char *fmt, va_list vl)
{
    CSLSLock lock(&m_mutex);
    char buf[4096] = {0};
    char cur_time[STR_DATE_TIME_LEN] = {0};
    int64_t cur_time_msec = sls_gettime_ms();
    int64_t cur_time_sec = cur_time_msec / 1000;
    cur_time_msec = cur_time_msec % 1000;
    sls_gettime_fmt(cur_time, cur_time_sec, "%Y-%m-%d %H:%M:%S");
    int pos = sprintf(buf, "%s:%03d %s %s: " , cur_time, (int)cur_time_msec, APP_NAME, LOG_LEVEL_NAME[level]);
    pos += vsnprintf(buf + pos, 4095 - pos, fmt, vl);
    buf[pos++] = '\n';
    printf(buf);

    if (m_log_file) {
        fwrite(buf, pos, 1, m_log_file);
    }
}

void CSLSLog::set_log_level(char * level)
{
    if (!m_pInstance)
    	m_pInstance = new CSLSLog();

    level = sls_strupper(level);//to upper
    int n = sizeof(LOG_LEVEL_NAME)/sizeof(char*);
    for (int i = 0; i < n; i ++) {
        if (strcmp(level, LOG_LEVEL_NAME[i]) == 0) {
            m_pInstance->m_level = i;
            printf("set log level='%s'.\n" , level, LOG_LEVEL_NAME[i]);
            return ;
        }
    }
    printf("!!!wrong log level '%s', set default '%s'.\n" , level, LOG_LEVEL_NAME[m_pInstance->m_level]);
}

void CSLSLog::set_log_file(char * file_name)
{
    if (!m_pInstance)
        m_pInstance = new CSLSLog();

    if (m_pInstance->log_filename[0] == 0) {
        strcpy(m_pInstance->log_filename, file_name);
        m_pInstance->m_log_file = fopen(m_pInstance->log_filename, "a");
    }
}

