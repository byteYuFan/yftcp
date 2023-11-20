//
// Created by wangyufan on 2023-11-20.
//

#include "ring.h"
#include "string.h"

struct hz_ring_interface *hz_get_ring_interface_instance(void) {
    if (ring_singleton_instance == NULL) {
        // 如果实例不存在，则创建一个新的实例
        ring_singleton_instance = rte_malloc("struct hz_ring_interface",sizeof(struct hz_ring_interface),0);
        memset(ring_singleton_instance, 0, sizeof(struct hz_ring_interface));
        // 初始化实例中的成员
        // 请根据你的需求初始化 in 和 out
        ring_singleton_instance->in = rte_ring_create("in_ring", RING_SIZE, (int) rte_socket_id(),
                                                      RING_F_SP_ENQ | RING_F_SC_DEQ);
        ring_singleton_instance->out = rte_ring_create("out_ring", RING_SIZE, (int) rte_socket_id(),
                                                       RING_F_SP_ENQ | RING_F_SC_DEQ);
    }

    return ring_singleton_instance;
}