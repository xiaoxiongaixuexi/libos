#include "libos.h"
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#ifdef __linux__
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/syscall.h>
#else
#include <windows.h>
#include <io.h>
#endif

#define OS_LOG_DATE_MAX_LEN 64       // ��־ʱ�䳤�� 
#define OS_LOG_FILE_MAX_LEN 256      // �ļ�����󳤶�Ϊ255
#define OS_LOG_MSG_MAX_BUFF 3072     // ��־��Ϣ��󳤶�
#define OS_LOG_LINE_MAX_LEN 4096     // ��־ÿ�д�ӡ������ֽ���

typedef struct _os_log_t
{
    int line;                        // ��־��������
    long thr_id;                     // �߳�id
    LOG_MSG_LEVEL level;             // ��־����
    char date[OS_LOG_DATE_MAX_LEN];  // ��ӡ��־ʱ��
    char file[OS_LOG_FILE_MAX_LEN];  // ��־��Ϣ�����ļ�
    char func[OS_LOG_FILE_MAX_LEN];  // ��־��Ϣ���ں���
    char msg[OS_LOG_MSG_MAX_BUFF];   // ��־��Ϣ
} os_log_t;

// �Ƿ��Ѿ���ʼ��
static bool g_is_init = false;
// д���ļ�����־����
static LOG_MSG_LEVEL g_log_level = LOG_LEVEL_INFO;
// �ļ����
static FILE * g_log_fp = NULL;
// �ļ�·��
static char g_log_path[OS_PATH_MAX_LEN] = { 0 };
// ��־�����ַ���
static const char * g_log_str[] = { "INFO", "WARN", "ERROR", "FATAL" };

// ������־����
static int log_msg_doit(LOG_MSG_LEVEL level, const char * file, const int line, const char * func, const char * fmt, va_list vl);

bool log_msg_init(const char * file, const LOG_MSG_LEVEL level)
{
    if (g_is_init)
    {
        log_msg_warn("log has already initialized!");
        return false;
    }

    assert(level <= LOG_LEVEL_FATAL && level >= LOG_LEVEL_INFO);
    
    if (level<LOG_LEVEL_INFO || level>LOG_LEVEL_FATAL)
    {
        fputs("log level is out of range!\n", stderr);
        return false;
    }
    g_log_level = level;
    g_is_init = true;

    if (file)
    {
        strncpy(g_log_path, file, OS_PATH_MAX_LEN - 1);
        // TODO ��ȡ����·��
        g_log_fp = fopen(g_log_path, "a+");
        if (NULL == g_log_fp)
        {
            log_msg_error("fopen %s failed, err:%s", g_log_path, strerror(errno));
            g_is_init = false;
            return false;
        }
    }

    return true;
}

void log_msg_uninit()
{
    if (!g_is_init)
        return;

    if (g_log_fp)
    {
        fclose(g_log_fp);
        g_log_fp = NULL;
    }

    g_is_init = false;
}

int log_msg(const LOG_MSG_LEVEL level, const char * file, const int line, const char * func, const char * fmt, ...)
{
    va_list vl;

    va_start(vl, fmt);
    int len = log_msg_doit(level, file, line, func, fmt, vl);
    va_end(vl);

    return len;
}

int log_msg_doit(LOG_MSG_LEVEL level, const char * file, const int line, const char * func, const char * fmt, va_list vl)
{
    os_log_t node = { 0 };

#ifdef __linux__
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm * sys_tv = localtime(&tv.tv_sec);
#else
    SYSTEMTIME sys_tv;
    GetLocalTime(&sys_tv);
#endif

    // ��������
    if (level >= LOG_LEVEL_INFO && level <= LOG_LEVEL_FATAL)
        node.level = level;
    else
        node.level = LOG_LEVEL_FATAL;
    node.line = line;

#if __linux__
    if (file)
    {
        char * file_name = basename((char *)file);
        strncpy(node.file, file_name, OS_LOG_FILE_MAX_LEN);
    }
    snprintf(node.date, OS_LOG_DATE_MAX_LEN, "%04d/%02d/%02d %02d:%02d:%02d.%03d", sys_tv->tm_year + 1900, sys_tv->tm_mon + 1,
        sys_tv->tm_mday, sys_tv->tm_hour, sys_tv->tm_min, sys_tv->tm_sec, tv.tv_usec / 1000);
    node.thr_id = syscall(SYS_gettid);
#else
    // ��ӡ��־ʱ��
    snprintf(node.date, OS_LOG_DATE_MAX_LEN, "%04d/%02d/%02d %02d:%02d:%02d.%03d",
        sys_tv.wYear, sys_tv.wMonth, sys_tv.wDay, sys_tv.wHour,
        sys_tv.wMinute, sys_tv.wSecond, sys_tv.wMilliseconds);
    node.thr_id = (long)GetCurrentThreadId();
    if (file)
    {
        char tmp[OS_LOG_FILE_MAX_LEN] = { 0 };
        char ext[OS_LOG_DATE_MAX_LEN] = { 0 };
        _splitpath(file, NULL, NULL, tmp, ext);
        snprintf(node.file, OS_LOG_FILE_MAX_LEN, "%s%s", tmp, ext);
    }
#endif

    // ���ں���
    if (func)
        strncpy(node.func, func, OS_LOG_FILE_MAX_LEN - 1);

    // ��ʽ����־
    int len = vsnprintf(node.msg, sizeof(node.msg), fmt, vl);
    char log_buf[OS_LOG_LINE_MAX_LEN] = { 0 };
    snprintf(log_buf, OS_LOG_LINE_MAX_LEN, "[%s][%s][%ld][%s:%d][%s] %s\n",
        node.date, g_log_str[node.level], node.thr_id, node.file,
        node.line, node.func, node.msg);

    fflush(stdout);
    fputs(log_buf, stderr);
    fflush(stderr);

    if (node.level >= g_log_level && g_log_fp)
    {
        bool exist = false;
#ifdef __linux__
        exist = (0 == access(g_log_path, F_OK));
#else
        exist = (0 == _access(g_log_path, 0));
#endif
        if (!exist)
        {
            g_log_fp = freopen(g_log_path, "a+", g_log_fp);
        }

        if (g_log_fp)
        {
            fputs(log_buf, g_log_fp);
            fflush(g_log_fp);
        }
    }

    // ���ݼ����ж��Ƿ��˳�
    if (level == LOG_LEVEL_FATAL)
    {
        log_msg_uninit();  //��־����ʼ����ɱ����ǰ����
        exit(EXIT_FAILURE);
    }
    return len;
}