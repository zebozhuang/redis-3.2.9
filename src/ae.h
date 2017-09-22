/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__

#include <time.h>

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

/* 声明一个事件对象，预先声明，下面的定义的数据结构需要使用aeEventLoop,
  但aeEventLoop内部结构也需要他们 */
struct aeEventLoop;

/* Types and data structures */
/* 文件事件处理 */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
/* 时间事件处理 */
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);

typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
/* 文件事件驱动对象 */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) 可读 | 可写*/  
    aeFileProc *rfileProc;  /* 可读 */
    aeFileProc *wfileProc;  /* 可写 */
    void *clientData;       /* 客户端数据 */
} aeFileEvent;

/* Time event structure */
/* 时间时间驱动对象 */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */  /* 事件id */
    long when_sec; /* seconds */                /* 时间（秒） */
    long when_ms; /* milliseconds */            /* 时间（毫秒）*/
    aeTimeProc *timeProc;
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    struct aeTimeEvent *next;
} aeTimeEvent;

/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
/* 定义了aeEventLoop 时间结构 */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */ /* 当前注册的最大文件句柄 */
    int setsize; /* max number of file descriptors tracked */       /* 设置最多可以被追踪的文件句柄数 */
    long long timeEventNextId;                                      /* 下个时间事件Id */
    time_t lastTime;     /* Used to detect system clock skew */     /* 用来测试时间偏移(https://en.wikipedia.org/wiki/Clock_skew)*/
    aeFileEvent *events; /* Registered events */        /* 注册的文件事件 */
    aeFiredEvent *fired; /* Fired events */             /* 已经没有用到的事件 */
    aeTimeEvent *timeEventHead;                         /* 时间事件 */
    int stop;
    void *apidata; /* This is used for polling API specific data */ /* 给Polling API数据 */
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

/* Prototypes */
/* 接口原型 */
/* 创建事件EventLoop */
aeEventLoop *aeCreateEventLoop(int setsize);
/* 删除事件EventLoop */
void aeDeleteEventLoop(aeEventLoop *eventLoop);
/* 停止事件 */
void aeStop(aeEventLoop *eventLoop);
/* 创建文件事件 */
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
/* 删除文件事件 */
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
/* 获取文件事件 */
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
/* 创建事件事件 */
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
/* 删除事件事件 */
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
/* 处理事件 */
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
/* 等待 */
int aeWait(int fd, int mask, long long milliseconds);
/* ??? */
void aeMain(aeEventLoop *eventLoop);
/*获取API名字 */
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
/* 获取集合大小*/
int aeGetSetSize(aeEventLoop *eventLoop);
/* 重置集合大小 */
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
