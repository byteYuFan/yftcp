//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_FUNCTION_H
#define YFTCP_FUNCTION_H

#include "yftcp.h"

void hz_print_eth_dev_info(const struct rte_eth_dev_info *dev_info);

void hz_print_mac_address(struct rte_ether_addr *mac_addr);

void hz_print_arp_ip_addresses(struct rte_arp_hdr *arp_header);

void hz_print_ether_addr(const char *name, const struct rte_ether_addr *eth_addr);

char *hz_print_ipv4_address(uint32_t addr);

char *format_connection_info(uint32_t sip, uint32_t dip, uint16_t s_port, uint16_t d_port);

char *hz_get_current_time_string(void);

char *hz_generate_ring_name(void);

const char *generate_random_string(void);

const char *hz_generateRandomString(void);

#endif //YFTCP_FUNCTION_H
