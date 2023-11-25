//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_RED_BLACK_H
#define YFTCP_RED_BLACK_H

#include "yftcp.h"

struct hz_socket_info;
struct hz_connection_info;


typedef enum {
    HZ_COMPARE_FALSE,
    HZ_COMPARE_TRUE,
    HZ_COMPARE_EQUAL,
} COMPARE_RESULT;

struct hz_list_connection_info {
    struct hz_connection_info *head;
    int count;
};


struct hz_list_connection_info *hz_get_conn_list_info_instance(void);


#endif //YFTCP_RED_BLACK_H
