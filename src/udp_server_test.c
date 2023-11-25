//
// Created by wangyufan on 2023-11-23.
//

#include "udp_server_test.h"
#include "function.h"
//
// Created by wangyufan on 2023-11-16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket.h"

#define BUFFER_SIZE 1024

int udp_server_enter(__attribute__((unused)) void *arg) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 创建UDP套接字
    if ((sockfd = hz_socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    // 设置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = inet_addr("192.168.182.41");

    // 绑定套接字到地址和端口
    if (hz_bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        hz_close(sockfd);
        //   exit(EXIT_FAILURE);
        return 0;
    }
    printf("UDP server is running on port %d...\n", 9999);
    while (1) {
        // 接收数据
        ssize_t recv_len = hz_receive_from(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr,
                                           &client_len);
        if (recv_len == -1) {
            perror("recvfrom");
            hz_close(sockfd);
            exit(EXIT_FAILURE);
        }
        // 在数据前面加上回显前缀
        const char *echo_prefix = "Echo: ";
        char echo_buffer[BUFFER_SIZE + strlen(echo_prefix)];
        strcpy(echo_buffer, echo_prefix);
        strcat(echo_buffer, buffer);

        // 发送回显数据给客户端
        ssize_t send_len = hz_sendto(sockfd, echo_buffer, strlen(echo_buffer), 0, (struct sockaddr *) &client_addr,
                                     client_len);
        printf("send data:%s\n", echo_buffer);

        if (send_len == -1) {
            perror("sendto");
            hz_close(sockfd);
            return 0;
        }
    }

    // 关闭套接字
    close(sockfd);
    return 0;
}

int udp_server_enter_2(__attribute__((unused)) void *arg) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 创建UDP套接字
    if ((sockfd = hz_socket(AF_INET, SOCK_DGRAM, 1)) == -1) {
        perror("socket");
        return -1;
    }

    // 设置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8898);
    server_addr.sin_addr.s_addr = inet_addr("192.168.182.41");

    // 绑定套接字到地址和端口
    if (hz_bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        hz_close(sockfd);
        //   exit(EXIT_FAILURE);
        return 0;
    }
    printf("UDP server is running on port %d...\n", 8898);
    struct hz_connection_info *conn_info = hz_get_conn_info_by_fd(sockfd);
    if (conn_info) {
        printf("src %s:%d:%d\n", hz_print_ipv4_address(conn_info->socket_info->sip),
               htons(conn_info->socket_info->src_port), sockfd);
        //todo
    }
    while (1) {
        // 接收数据
        ssize_t recv_len = hz_receive_from(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr,
                                           &client_len);
        printf("receive8898");
        if (recv_len == -1) {
            perror("recvfrom");
            hz_close(sockfd);
            exit(EXIT_FAILURE);
        }
        // 在数据前面加上回显前缀
        const char *echo_prefix = "Echo: ";
        char echo_buffer[BUFFER_SIZE + strlen(echo_prefix)];
        strcpy(echo_buffer, echo_prefix);
        strcat(echo_buffer, buffer);

        // 发送回显数据给客户端
        ssize_t send_len = hz_sendto(sockfd, echo_buffer, strlen(echo_buffer), 0, (struct sockaddr *) &client_addr,
                                     client_len);
        printf("send data:%s\n", echo_buffer);
        if (send_len == -1) {
            perror("sendto");
            hz_close(sockfd);
            return 0;
        }
    }

    // 关闭套接字
    close(sockfd);
    return 0;
}