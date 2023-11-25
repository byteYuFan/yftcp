//
// Created by wangyufan on 2023-11-21.
//

#include "conn_status.h"
#include "function.h"

extern struct hz_ring_name global_ring_name;

struct hz_connection_info *hz_init_connection_info(int fd, uint8_t protocol, uint32_t src_ip, uint32_t dst_ip,
                                                   uint16_t src_port, uint16_t dst_port,
                                                   struct rte_ether_addr src_mac, struct rte_ether_addr dst_mac) {
    struct hz_connection_info *connection_info = (struct hz_connection_info *) malloc(
            sizeof(struct hz_connection_info));
    if (!connection_info)return NULL;
    memset(connection_info, 0, sizeof(struct hz_connection_info));
    //todo 记得free
    connection_info->socket_info = hz_create_socket_info(protocol, src_ip, dst_ip, src_port, dst_port);
    if (connection_info->socket_info == NULL) {
        free(connection_info);
        return NULL;
    }
    connection_info->fd = fd;
    rte_memcpy(&connection_info->src_mac, &src_mac, RTE_ETHER_ADDR_LEN);
    rte_memcpy(&connection_info->dst_mac, &dst_mac, RTE_ETHER_ADDR_LEN);
    pthread_mutex_lock(&global_ring_name.lock);

    connection_info->send_buffer = rte_ring_create(global_ring_name.send_name[global_ring_name.count], RING_SIZE,
                                                   (int) rte_socket_id(),
                                                   RING_F_SP_ENQ | RING_F_SC_DEQ);
    if (connection_info->send_buffer == NULL) {
        free(connection_info->socket_info);
        free(connection_info);
        return NULL;
    }

    connection_info->receive_buffer = rte_ring_create(global_ring_name.receive_name[global_ring_name.count],
                                                      RING_SIZE,
                                                      (int) rte_socket_id(),
                                                      RING_F_SP_ENQ | RING_F_SC_DEQ);
    global_ring_name.count++;
    if (connection_info->receive_buffer == NULL) {
        free(connection_info->socket_info);
        free(connection_info);
        return NULL;
    }
    pthread_mutex_unlock(&global_ring_name.lock);
    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    rte_memcpy(&connection_info->cond, &blank_cond, sizeof(pthread_cond_t));

    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    rte_memcpy(&connection_info->mutex, &blank_mutex, sizeof(pthread_mutex_t));
    if (protocol == HZ_PROTOCOL_UDP) {
        connection_info->special.udp_info.status = HZ_UDP_CLOSED;

    } else if (protocol == HZ_PROTOCOL_TCP) {
        connection_info->special.tcp_info.tcp_status = HZ_TCP_STATUS_CLOSED;
        connection_info->special.tcp_info.send_nxt = 0;
        connection_info->special.tcp_info.rcv_nxt = 0;
    };

    //
    connection_info->next = NULL;
    connection_info->prev = NULL;
    return connection_info;

}


void hz_destroy_connection_info(struct hz_connection_info *connection_info) {
    if (connection_info->send_buffer) {
        rte_free(connection_info->send_buffer);
    }
    if (connection_info->receive_buffer) {
        rte_free(connection_info->receive_buffer);
    }
    if (connection_info->socket_info) {
        rte_free(connection_info->socket_info);
    }
    pthread_cond_destroy(&connection_info->cond);
    pthread_mutex_destroy(&connection_info->mutex);
    rte_free(connection_info);
}

struct hz_socket_info *hz_create_socket_info(uint8_t protocol, uint32_t src_ip, uint32_t dst_ip,
                                             uint16_t src_port, uint16_t dst_port) {

    struct hz_socket_info *socket_info = rte_malloc(" struct hz_socket_info *",
                                                    sizeof(struct hz_socket_info), 0);
    // todo 记得free
    if (!socket_info)return NULL;
    memset(socket_info, 0, sizeof(struct hz_connection_info));
    socket_info->protocol = protocol;
    socket_info->dst_port = dst_port;
    socket_info->src_port = src_port;
    socket_info->sip = src_ip;
    socket_info->dip = dst_ip;

    return socket_info;
}