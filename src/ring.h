//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_RING_H
#define YFTCP_RING_H

#include "yftcp.h"

#define RING_SIZE 1024
struct hz_ring_interface {
    struct rte_ring *in;
    struct rte_ring *out;
};

// 定义结构体
struct hz_ring_name{
    const char* send_name[64];
    const char* receive_name[64];
    int count;
    pthread_mutex_t lock;
};

// 获取单例实例的函数
struct hz_ring_interface *hz_get_ring_interface_instance(void);

#endif //YFTCP_RING_H
