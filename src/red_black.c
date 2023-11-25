//
// Created by wangyufan on 2023-11-21.
//

#include "red_black.h"
#include "conn_status.h"


extern struct hz_list_connection_info *conn_info_singleton_instance;

struct hz_list_connection_info *hz_get_conn_list_info_instance(void) {
    if (conn_info_singleton_instance == NULL) {
        // 如果实例不存在，则创建一个新的实例
        conn_info_singleton_instance = rte_malloc("struct hz_list_connection_info *",
                                                  sizeof(struct hz_list_connection_info),
                                                  0);
        // todo 记得free
        memset(conn_info_singleton_instance, 0, sizeof(struct hz_list_connection_info));
        conn_info_singleton_instance->head = NULL;
        conn_info_singleton_instance->count = 0;

    }

    return conn_info_singleton_instance;
}



