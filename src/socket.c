//
// Created by wangyufan on 2023-11-22.
//

#include "socket.h"
#include "red_black.h"
#include "conn_status.h"
#include "list.h"
#include "udp_info.h"

#define HZ_DEFAULT_FD_NUM 3
#define HZ_MAX_FD_COUNT 1024
static unsigned char hz_fd_table[HZ_MAX_FD_COUNT] = {0};  // 1024*8个fd

extern struct rte_ether_addr global_ethernet_device_mac;

int hz_get_fd_from_bitmap(void) {
    // TODO
    int fd = HZ_DEFAULT_FD_NUM;
    for (; fd < HZ_MAX_FD_COUNT; fd++) {
        if ((hz_fd_table[fd / 8] & (0x1 << (fd % 8))) == 0) {
            hz_fd_table[fd / 8] |= (0x1 << (fd % 8));
            return fd;
        }
    }
    return -1;
}

int hz_reset_fd_about_bitmap(int fd) {
    if (fd >= HZ_MAX_FD_COUNT)return -1;
    hz_fd_table[fd / 8] &= ~(0x1 << (fd % 8));
    return 0;
}

struct hz_connection_info *hz_get_conn_info_by_fd(int fd) {
    struct hz_list_connection_info *conn_info = hz_get_conn_list_info_instance();
    struct hz_connection_info *info = NULL;
    for (info = conn_info->head; info != NULL; info = info->next) {
        if (info->fd == fd) {
            return info;
        }
    }
    return NULL;
}

struct hz_connection_info *
hz_get_conn_info_by_socket(uint32_t sip, uint32_t dip, uint16_t s_port, uint16_t d_port, int protocol) {
    struct hz_list_connection_info *conn_info = hz_get_conn_list_info_instance();
    struct hz_connection_info *info = NULL;
    for (info = conn_info->head; info != NULL; info = info->next) {
        if (protocol == HZ_PROTOCOL_UDP) {
            if (info->socket_info->sip == sip && info->socket_info->src_port == s_port) {
                return info;
            }
        } else if (protocol == HZ_PROTOCOL_TCP) {
            if (info->socket_info->sip == sip && info->socket_info->src_port == s_port &&
                info->socket_info->dst_port == d_port && info->socket_info->dip == dip) {
                return info;
            }
        }

    }
    return NULL;
}

int hz_socket(__attribute__((unused)) int domain, int type, __attribute__((unused))int protocol) {
    int fd = hz_get_fd_from_bitmap();
    if (fd == -1)return -1;
    struct rte_ether_addr addr;
    memset(&addr, 0, RTE_ETHER_ADDR_LEN);
    struct hz_connection_info *conn_info = hz_init_connection_info(fd, type, 0, 0, 0, 0, addr, addr);
    if (conn_info == NULL)return -1;

    INSERT_NODE_HEAD(hz_get_conn_list_info_instance()->head, conn_info);
    hz_get_conn_list_info_instance()->count++;
    return fd;
}

int hz_bind(int socket_fd, const struct sockaddr *addr, __attribute__((unused)) socklen_t addr_len) {
    struct hz_connection_info *conn_info = hz_get_conn_info_by_fd(socket_fd);
    if (conn_info == NULL)return -1;
    const struct sockaddr_in *local_addr = (const struct sockaddr_in *) addr;
    conn_info->socket_info->src_port = local_addr->sin_port;
    rte_memcpy(&conn_info->socket_info->sip, &local_addr->sin_addr, sizeof(uint32_t));
    rte_memcpy(&conn_info->src_mac, &global_ethernet_device_mac, RTE_ETHER_ADDR_LEN);

    return 0;

}

ssize_t hz_receive_from(int socket_fd, void *buf, size_t len, __attribute__((unused)) int flags,
                        struct sockaddr *src_addr, __attribute__((unused)) socklen_t *addr_len) {

    struct hz_connection_info *conn_info = hz_get_conn_info_by_fd(socket_fd);
    if (conn_info == NULL) {
        printf("get conn info err.\n");
        return -1;
    }
    int rt_len = 0;
    struct hz_udp_payload *udp_payload = NULL;
    int receive_num = -1;
    pthread_mutex_lock(&conn_info->mutex);
    if (!conn_info->receive_buffer)return -1;
    while ((receive_num = rte_ring_mc_dequeue(conn_info->receive_buffer, (void **) &udp_payload)) < 0) {
        pthread_cond_wait(&conn_info->cond, &conn_info->mutex);
    }
    pthread_mutex_unlock(&conn_info->mutex);
    if (receive_num < 0)return -1;
    struct sockaddr_in *s_addr = (struct sockaddr_in *) src_addr;
    if (udp_payload) {
        s_addr->sin_port = udp_payload->src_port;
        rte_memcpy(&s_addr->sin_addr, &udp_payload->src_ip, sizeof(uint32_t));
        if (len < udp_payload->data_length) {
            rte_memcpy(buf, udp_payload->data, len);
            unsigned char *ptr = rte_malloc("unsigned char *", udp_payload->data_length - len, 0);
            rte_memcpy(ptr, udp_payload->data + len, udp_payload->data_length - len);
            udp_payload->data_length -= len;
            free(udp_payload->data);
            udp_payload->data = ptr;
            rte_ring_mp_enqueue(conn_info->receive_buffer, udp_payload);
            return (ssize_t) len;
        } else {
            rte_memcpy(buf, udp_payload->data, udp_payload->data_length);
            rt_len = udp_payload->data_length;
            free(udp_payload->data);
            rte_free(udp_payload);
            return (ssize_t) rt_len;
        }
    }
    return -1;
}

ssize_t hz_sendto(int socket_fd, void *buf, size_t len, __attribute__((unused)) int flags,
                  const struct sockaddr *dest_addr, __attribute__((unused)) socklen_t addr_len) {
    struct hz_connection_info *conn_info = hz_get_conn_info_by_fd(socket_fd);
    if (conn_info == NULL) {
        printf("conn_info== NULL\n");
        return -1;
    }
    const struct sockaddr_in *dst_addr = (const struct sockaddr_in *) dest_addr;
    struct hz_udp_payload *udp_payload = hz_create_udp_payload(conn_info->socket_info->sip, dst_addr->sin_addr.s_addr,
                                                               conn_info->socket_info->src_port, dst_addr->sin_port,
                                                               conn_info->socket_info->protocol, (unsigned char *) buf,
                                                               len);
    if (udp_payload == NULL) {
        printf("udp_payload == NULL\n");
        return -1;
    }

    udp_payload->data_length = len;

    if (conn_info->send_buffer) {
        rte_ring_mp_enqueue(conn_info->send_buffer, udp_payload);
    }
    return (ssize_t) len;
}

int hz_close(int socket_fd) {
    struct hz_connection_info *conn_info = hz_get_conn_info_by_fd(socket_fd);
    if (conn_info == NULL)return 0;
    if (conn_info->socket_info->protocol == HZ_PROTOCOL_UDP) {
        DELETE_TARGET_NODE(hz_get_conn_list_info_instance()->head, conn_info);
        hz_reset_fd_about_bitmap(socket_fd);
        rte_free(conn_info);
    }
    return 1;
}
