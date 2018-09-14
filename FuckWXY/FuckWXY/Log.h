#ifndef _H_LOG_INCLUDE_
#define _H_LOG_INCLUDE_
#include "windows.h"
#include "stdio.h"

class CLog
{
public:
    CLog();
    ~CLog();
	void InitLog(const char *lpFileName);
    void SetLogName(LPCSTR lpName);
    void Start();
    void Stop();
    void printf(LPCSTR lpszFormat, ...);
	void justprintf(LPCSTR lpszFormat, ...);
    void dump_mem(const void * start, unsigned int len);
    void dump_mem(LPCSTR lpTagName, const void * start, unsigned int len);

private:
	
    FILE * m_fout;
    BOOL   m_bLog;
    char   m_szLogFile[1024];
};


extern CLog m_Log;
#define Print m_Log.printf

#endif //(_H_LOG_INCLUDE_)