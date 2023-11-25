//
// Created by wangyufan on 2023-11-20.
//

#include "arp_table.h"
#include "list.h"

struct hz_arp_table *hz_get_arp_table_instance(void) {
    if (arp_table_singleton_instance == NULL) {
        arp_table_singleton_instance = rte_malloc("struct hz_arp_table", sizeof(struct hz_arp_table), 0);
        memset(arp_table_singleton_instance, 0, sizeof(struct hz_arp_table));
    }
    return arp_table_singleton_instance;
}

struct rte_ether_addr *hz_get_mac_by_ip(uint32_t ip) {
    struct hz_arp_table_node *arp_node = NULL;
    struct hz_arp_table *arp_table = hz_get_arp_table_instance();
    for (arp_node = arp_table->head; arp_node != NULL; arp_node = arp_node->next) {
        if (arp_node->ip == ip) {
            return &arp_node->mac;
        }
    }
    return NULL;

}

uint32_t hz_get_ip_by_mac(struct rte_ether_addr *mac) {
    struct hz_arp_table_node *arp_node = NULL;
    struct hz_arp_table *arp_table = hz_get_arp_table_instance();
    for (arp_node = arp_table->head; arp_node != NULL; arp_node = arp_node->next) {
        if (arp_node->mac.addr_bytes == mac->addr_bytes) {
            return arp_node->ip;
        }
    }
    return 0;
}

struct hz_arp_table_node *hz_create_arp_table_node(uint32_t ip, struct rte_ether_addr mac, uint8_t type) {
    struct hz_arp_table_node *arp_node = rte_malloc(" struct hz_arp_table_node", sizeof(struct hz_arp_table_node), 0);
    // todo 记得free
    if (arp_node == NULL)return NULL;
    memset(arp_node, 0, sizeof(struct hz_arp_table_node));
    arp_node->ip = ip;
    arp_node->mac = mac;
    arp_node->type = type;
    return arp_node;
}

void hz_update_arp_table(uint32_t ip, struct rte_ether_addr mac) {
    struct hz_arp_table_node *arp_node = NULL;
    struct hz_arp_table *arp_table = hz_get_arp_table_instance();
    for (arp_node = arp_table->head; arp_node != NULL; arp_node = arp_node->next) {
        if (arp_node->ip == ip) {
            memcpy(arp_node->mac.addr_bytes, mac.addr_bytes, RTE_ETHER_ADDR_LEN);
            return;
        }
    }
    INSERT_NODE_HEAD(arp_table->head, hz_create_arp_table_node(ip, mac, ARP_NODE_TYPE_DYNAMIC));
}