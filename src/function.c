//
// Created by wangyufan on 2023-11-20.
//

#include "function.h"



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
hz_print_ether_addr(const char *name, const struct rte_ether_addr *eth_addr)
{
    char buf[RTE_ETHER_ADDR_FMT_SIZE];
    rte_ether_format_addr(buf, RTE_ETHER_ADDR_FMT_SIZE, eth_addr);
    printf("%s%s", name, buf);
}