#include "stdafx.h"
#include "Log.h"
#include <Shlwapi.h>

CLog m_Log;
CLog::CLog()
{
    m_fout=NULL;
    m_bLog=FALSE;
    m_szLogFile[0]=0;
}

CLog::~CLog()
{
    if (m_fout)
    {
        fclose(m_fout);
        m_fout=NULL;
    }
}




void CLog::InitLog(const char *lpFileName)
{
    
      m_Log.SetLogName(lpFileName);
      m_Log.Start();
}

void CLog::SetLogName(LPCSTR lpName)
{
    if (lpName)
    {
        strcpy(m_szLogFile,lpName);
    }
}

void CLog::Start()
{
    if (m_fout==NULL)
    {
        if (*m_szLogFile)
        {
			m_fout = fopen(m_szLogFile, "w+");
        }
    }

    m_bLog = m_fout!=NULL;
}

void CLog::Stop()
{
    m_bLog=FALSE;
}

void CLog::printf(LPCSTR lpszFormat, ...)
{
    if (m_bLog)
    {
	    va_list args;
	    va_start(args, lpszFormat);
        vprintf(lpszFormat,args);
		SYSTEMTIME sys; 
		GetLocalTime( &sys ); 
		fprintf(m_fout,"%02d:%02d:%02d ",sys.wHour,sys.wMinute, sys.wSecond);

	    vfprintf(m_fout, lpszFormat, args);
        fflush(m_fout);

	    va_end(args);
    }
}

void CLog::justprintf(LPCSTR lpszFormat, ...)
{
	if (m_bLog)
	{
		va_list args;
		va_start(args, lpszFormat);
		vprintf(lpszFormat, args);
		vfprintf(m_fout, lpszFormat, args);
		fflush(m_fout);

		va_end(args);
	}
}
void CLog::dump_mem(const void * lpData, unsigned int len)
{

    if (m_bLog)
    {
        len=lpData==NULL?0:len;

        fprintf(m_fout,"[Data-Len]: %d (0x%04X)\n",len,len);

        if (lpData)
        {
            char szLine[17*3*2];
            UINT i;
            UINT j=0;
            for (i=0;i<len;i++)
            {
                sprintf(&szLine[j],"%02X ",((unsigned char *)lpData)[i]);
                if (i%16==15 || i==len-1)
                {
                    fprintf(m_fout,szLine);
                    fprintf(m_fout,"\n");
                    j=0;
                }else{
                    j+=3;
                }
            }
        }

        fprintf(m_fout,"\n");

	    fflush(m_fout);
    }
}

void CLog::dump_mem(LPCSTR lpTagName, const void * lpData, unsigned int len)
{
    if (m_bLog)
    {
        if (lpTagName)
        {
            fprintf(m_fout,"[Data-Tag]: %s\n",lpTagName);
        }
        dump_mem(lpData,len);
    }
}