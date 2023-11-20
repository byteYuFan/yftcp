//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_RING_H
#define YFTCP_RING_H

#include "yftcp.h"

#define RING_SIZE 32
struct hz_ring_interface {
    struct rte_ring *in;
    struct rte_ring *out;
};
// 单例结构体的指针
extern struct hz_ring_interface *ring_singleton_instance;

// 获取单例实例的函数
struct hz_ring_interface *hz_get_ring_interface_instance(void);

#endif //YFTCP_RING_H
