#ifndef __LIBOS_H__
#define __LIBOS_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __linux__
#define OS_PATH_MAX_LEN 4096 // ·����󳤶�
#else
#define OS_PATH_MAX_LEN 260  // ·����󳤶�
#endif

typedef enum _LOG_MSG_LEVEL  // ��־����
{
    LOG_LEVEL_INFO = 0,      // ��ͨ��ӡ
    LOG_LEVEL_WARN,          // ���棬������Լ�������
    LOG_LEVEL_ERROR,         // ����ϵͳ��������
    LOG_LEVEL_FATAL          // �����޷���������ֱ���˳�
} LOG_MSG_LEVEL;

typedef struct _os_mutex_t os_mutex_t;
typedef struct _os_dlist_node_t os_dlist_node_t;
typedef struct _os_dlist_t os_dlist_t;
typedef struct _os_queue_node_t os_queue_node_t;
typedef struct _os_queue_t os_queue_t;

#ifndef __linux__
#define OS_API __declspec(dllexport)
#else
#define OS_API
#endif

#ifdef __cplusplus
extern "C"{
#endif

/*
* os_mutex_create
* @brief  ������
* @return ��ָ�����NULL
*/
OS_API os_mutex_t * os_mutex_create();

/*
* os_mutex_destroy
* @brief  ������
* @param  mtx ָ����ָ���ָ��
*/
OS_API void os_mutex_destroy(os_mutex_t ** mtx);

/*
* os_mutex_lock
* @brief  ����
* @param  mtx  ��ָ��
* @return true--�ɹ� false--ʧ��
*/
OS_API bool os_mutex_lock(os_mutex_t * mtx);

/*
* os_mutex_unlock
* @brief  ����
* @param  mtx  ��ָ��
* @return true--�ɹ� false--ʧ��
*/
OS_API bool os_mutex_unlock(os_mutex_t * mtx);

/*
* os_dlist_create
* @brief  ��������
* @param  node_size  �ڵ��С
* @return ����ָ�����ΪNULL
*/
OS_API os_dlist_t * os_dlist_create(const size_t node_size);

/*
* os_dlist_destroy
* @brief  ��������
* @param  os_lst  ָ������ָ���ָ��
*/
void os_dlist_destroy(os_dlist_t ** os_lst);

/*
* os_dlist_clear
* @brief  �������
* @param  os_lst  ����ָ��
*/
void os_dlist_clear(os_dlist_t * os_lst);

/*
* os_dlist_empty
* @brief  �ж������Ƿ�Ϊ��
* @param  os_lst  ����ָ��
* @return true--�ɹ� false--ʧ��
*/
bool os_dlist_empty(const os_dlist_t * os_lst);

/*
* os_dlist_size
* @brief  ��ȡ������
* @param  os_lst  ����ָ��
* @return ������
*/
size_t os_dlist_size(const os_dlist_t * os_lst);

/*
* os_dlist_add
* @brief  ���������������
* @param  os_lst   ����ָ��
* @param  os_node  �ڵ�ָ�� ��Ϊ�գ���Ϊ������ӵ��ýڵ�֮��
* @param  data     ����
* @return true--�ɹ� false--ʧ��
*/
bool os_dlist_add(os_dlist_t * os_lst, os_dlist_node_t * os_node, void * data);

/*
* os_dlist_delete
* @brief  ��������ɾ������
* @param  os_lst   ����ָ��
* @param  os_node  Ҫɾ���Ľڵ�ָ��
* @return true--�ɹ� false--ʧ��
*/
bool os_dlist_delete(os_dlist_t * os_lst, os_dlist_node_t * os_node);

/*
* os_dlist_head
* @brief  ��ȡ����ͷ�ڵ�
* @param  os_lst   ����ָ��
* @return ͷ�ڵ�ָ�����NULL
*/
OS_API os_dlist_node_t * os_dlist_head(const os_dlist_t * os_lst);

/*
* os_dlist_tail
* @brief  ��ȡ����β�ڵ�
* @param  os_lst   ����ָ��
* @return β�ڵ�ָ�����NULL
*/
OS_API os_dlist_node_t * os_dlist_tail(const os_dlist_t * os_lst);

/*
* os_dlist_next
* @brief  ��ȡ�ڵ����һ���ڵ�
* @param  os_node   �ڵ�ָ��
* @return �ڵ���һ���ڵ�ָ�����NULL
*/
OS_API os_dlist_node_t * os_dlist_next(const os_dlist_node_t * os_node);

/*
* os_dlist_prev
* @brief  ��ȡ�ڵ����һ���ڵ�
* @param  os_node   �ڵ�ָ��
* @return �ڵ���һ���ڵ�ָ�����NULL
*/
OS_API os_dlist_node_t * os_dlist_prev(const os_dlist_node_t * os_node);

/*
* os_dlist_getdata
* @brief  ��ȡ�ڵ������ָ��
* @param  os_node   �ڵ�ָ��
* @return �ڵ�����ָ�����NULL
*/
OS_API void * os_dlist_getdata(const os_dlist_node_t * node);

/*
* os_queue_create
* @brief  ��������
* @param  node_size  �ڵ��С
* @return ����ָ�����ΪNULL 
*/
OS_API os_queue_t * os_queue_create(uint32_t node_size);

/*
* os_queue_destroy
* @brief  ���ٶ���
* @param  os_queue  ָ�����ָ���ָ��
*/
OS_API void os_queue_destroy(os_queue_t ** os_queue);

/*
* os_queue_clear
* @brief  ��ն���
* @param  os_queue ����ָ��
*/
OS_API void os_queue_clear(os_queue_t * os_queue);

/*
* os_queue_empty
* @brief  �ж϶����Ƿ�Ϊ��
* @param  os_queue  ����ָ��
* @return true--�ɹ� false--ʧ��
*/
OS_API bool os_queue_empty(os_queue_t * os_queue);

/*
* os_queue_size
* @brief  ��ȡ���г���
* @param  os_queue  ����ָ��
* @return ���г���
*/
OS_API size_t os_queue_size(const os_queue_t * os_queue);

/*
* os_queue_push
* @brief  ��������
* @param  os_queue  ����ָ��
* @param  data  ����ָ��
* @return true--�ɹ� false--ʧ��
*/
OS_API bool os_queue_push(os_queue_t * os_queue, void * data);

/*
* os_queue_pop
* @brief  ɾ������ͷ
* @param  os_queue  ����ָ��
* @return true--�ɹ� false--ʧ��
*/
OS_API bool os_queue_pop(os_queue_t * os_queue);

/*
* os_queue_front
* @brief  ��ȡ����ͷ
* @param  os_queue  ����ָ��
* @return ͷ���ָ�����NULL
*/
OS_API os_queue_node_t * os_queue_front(os_queue_t * os_queue);

/*
* os_queue_getdata
* @brief  ��ȡ�ڵ�����
* @param  node    �ڵ�ָ��
* @return ����ָ��
*/
OS_API void * os_queue_getdata(const os_queue_node_t * node);

/*
* log_msg_init
* @brief  ��־��ʼ��
* @param  file    ��־�ļ�
* @param  level   д���ļ�����־����
* @return true--�ɹ� false--ʧ��
*/
OS_API bool log_msg_init(const char * file, LOG_MSG_LEVEL level);

/*
* log_msg_uninit
* @brief  ��־����
*/
OS_API void log_msg_uninit();

/*
* log_msg
* @brief  ��ӡ��־
* @param  level  ��־����
* @param  file   ��־��Ϣ�����ļ�
* @param  line   ��־��Ϣ��������
* @param  func   ��־��Ϣ���ں���
* @param  fmt    ��־��ʽ
*/
OS_API int log_msg(LOG_MSG_LEVEL level, const char * file, int line, const char * func, const char * fmt, ...);

#define log_msg_info(...) log_msg(LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_msg_warn(...) log_msg(LOG_LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_msg_error(...) log_msg(LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define log_msg_fatal(...) log_msg(LOG_LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
