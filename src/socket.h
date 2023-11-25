//
// Created by wangyufan on 2023-11-22.
//

#ifndef YFTCP_SOCKET_H
#define YFTCP_SOCKET_H

#include "yftcp.h"
#include "conn_status.h"
#include "red_black.h"

int hz_get_fd_from_bitmap(void);

int hz_reset_fd_about_bitmap(int fd);

struct hz_connection_info *hz_get_conn_info_by_fd(int fd);

struct hz_connection_info *
hz_get_conn_info_by_socket(uint32_t sip, uint32_t dip, uint16_t s_port, uint16_t d_port, int protocol);

int hz_socket(int domain, int type, int protocol);

int hz_bind(int socket_fd, const struct sockaddr *addr, socklen_t addr_len);

ssize_t hz_receive_from(int sock_fd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addr_len);

ssize_t
hz_sendto(int socket_fd,  void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addr_len);

int hz_close(int socket_fd);

#endif //YFTCP_SOCKET_H
