//
// Created by wangyufan on 2023-11-20.
//

#include "function.h"
#define MAX_TIME_STRING_LENGTH 20

void hz_print_eth_dev_info(const struct rte_eth_dev_info *dev_info) {
    printf("Device Information:\n");
    printf("  Driver Name: %s\n", dev_info->driver_name);
    printf("  Interface Index: %u\n", dev_info->if_index);
    printf("  Minimum MTU: %u\n", dev_info->min_mtu);
    printf("  Maximum MTU: %u\n", dev_info->max_mtu);
    printf("  Maximum RX Queues: %u\n", dev_info->max_rx_queues);
    printf("  Maximum TX Queues: %u\n", dev_info->max_tx_queues);
    printf("  Maximum MAC Addresses: %u\n", dev_info->max_mac_addrs);
    printf("  Maximum VFs: %u\n", dev_info->max_vfs);
    // ... (print other information as needed)

    // Print offload capabilities
    printf("  RX Offload Capabilities: %"
    PRIu64
    "\n", dev_info->rx_offload_capa);
    printf("  TX Offload Capabilities: %"
    PRIu64
    "\n", dev_info->tx_offload_capa);

    // ... (print other offload capabilities and configuration)

    // Print RX and TX descriptors limits
    printf("  RX Descriptors Limits:\n");
    printf("    Max: %u\n", dev_info->rx_desc_lim.nb_max);
    printf("    Min: %u\n", dev_info->rx_desc_lim.nb_min);
    // ... (print other RX descriptor limits)

    printf("  TX Descriptors Limits:\n");
    printf("    Max: %u\n", dev_info->tx_desc_lim.nb_max);
    printf("    Min: %u\n", dev_info->tx_desc_lim.nb_min);
    // ... (print other TX descriptor limits)
}

void hz_print_arp_ip_addresses(struct rte_arp_hdr *arp_header) {
    // 将32位的IPv4地址从网络字节序转换为主机字节序
    uint32_t sender_ip = rte_be_to_cpu_32(arp_header->arp_data.arp_sip);
    uint32_t target_ip = rte_be_to_cpu_32(arp_header->arp_data.arp_tip);

    // 将IPv4地址转换为点分十进制形式
    char sender_ip_str[INET_ADDRSTRLEN];
    char target_ip_str[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &sender_ip, sender_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &target_ip, target_ip_str, INET_ADDRSTRLEN);

    // 打印IPv4地址
    printf("Sender IP Address (SIP): %s\n", sender_ip_str);
    printf("Target IP Address (TIP): %s\n", target_ip_str);
}

void hz_print_mac_address(struct rte_ether_addr *mac_addr) {
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac_addr->addr_bytes[0],
           mac_addr->addr_bytes[1],
           mac_addr->addr_bytes[2],
           mac_addr->addr_bytes[3],
           mac_addr->addr_bytes[4],
           mac_addr->addr_bytes[5]);
}

void
hz_print_ether_addr(const char *name, const struct rte_ether_addr *eth_addr) {
    char buf[RTE_ETHER_ADDR_FMT_SIZE];
    rte_ether_format_addr(buf, RTE_ETHER_ADDR_FMT_SIZE, eth_addr);
    printf("%s%s", name, buf);
}

char *hz_print_ipv4_address(uint32_t addr) {

    struct in_addr ipv4_addr;
    ipv4_addr.s_addr = addr;
    return inet_ntoa(ipv4_addr);
}

char *format_connection_info(uint32_t sip, uint32_t dip, uint16_t s_port, uint16_t d_port) {
    // 分配足够的空间来存储格式化后的字符串
    char *formatted_str = (char *) malloc(256); // 假设最大长度为256，你可以根据实际情况调整

    if (formatted_str) {
        // 使用 snprintf 将连接信息格式化为字符串
        snprintf(formatted_str, 256, "%u:%u-%u:%u", sip, s_port, dip, d_port);
    }

    return formatted_str;
}

// 获取当前时间字符串
char* hz_get_current_time_string(void) {
    // 获取当前时间戳
    time_t timestamp = time(NULL);

    // 将时间戳转换为字符串
    char* buffer = (char*)malloc(MAX_TIME_STRING_LENGTH);
    if (buffer == NULL) {
        // 处理内存分配失败的情况
        exit(EXIT_FAILURE);
    }

    struct tm* timeinfo = localtime(&timestamp);
    strftime(buffer, MAX_TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", timeinfo);

    return buffer;
}

// 生成唯一的环形队列名称
char* hz_generate_ring_name(void) {
    // 使用时间戳作为一部分名称
    time_t current_time = time(NULL);
    char time_buffer[20];
    strftime(time_buffer, sizeof(time_buffer), "%Y%m%d%H%M%S", localtime(&current_time));

    // 为环形队列生成唯一名称
    size_t buffer_size = strlen("ring_") + strlen(time_buffer) + 1;  // 加1是为了容纳字符串结束符 '\0'
    char* ring_name = (char*)malloc(buffer_size);
    if (ring_name == NULL) {
        // 处理内存分配失败的情况
        exit(EXIT_FAILURE);
    }

    // 构造环形队列名称
    snprintf(ring_name, buffer_size, "ring_%s", time_buffer);

    return ring_name;
}

#define STRING_LENGTH 20

// 生成随机字符串
const char* generate_random_string(void) {
    char* random_string = (char*)malloc(STRING_LENGTH + 1); // 加1是为了容纳字符串结束符 '\0'
    if (random_string == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    srand((unsigned int)time(NULL));

    for (int i = 0; i < STRING_LENGTH; ++i) {
        int index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }

    random_string[STRING_LENGTH] = '\0'; // 在字符串末尾添加结束符

    return random_string;
}


const char* hz_generateRandomString(void ) {
    // 定义包含所有可能字符的字符串
    const char* allCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // 获取字符总数
    int totalCharacters = strlen(allCharacters);

    // 设置随机种子
    srand((unsigned int)time(NULL));

    // 分配内存来存储生成的字符串（需要+1用于存储字符串结尾的null字符）
    char* randomString = (char*)malloc(31 * sizeof(char));

    // 生成随机字符
    for (int i = 0; i < 30; ++i) {
        int randomIndex = rand() % totalCharacters;
        randomString[i] = allCharacters[randomIndex];
    }

    // 添加字符串结尾的null字符
    randomString[30] = '\0';

    // 返回const char*类型的指针
    return randomString;
}

