//
// Created by wangyufan on 2023-11-20.
//

#include "yftcp.h"
#include "ring.h"
#include "icmp.h"
#include "config.h"

extern struct rte_ether_addr global_ethernet_device_mac;

void hz_process_icmp(struct rte_mempool *memory_buffer_pool, struct rte_ether_hdr *ether_header,
                     struct rte_ipv4_hdr *ipv4_header) {
    struct rte_icmp_hdr *icmp_header = (struct rte_icmp_hdr *) (ipv4_header + 1);
    struct rte_mbuf *icmp_buffer = hz_send_icmp(memory_buffer_pool, &global_ethernet_device_mac, &ether_header->s_addr,
                                                ipv4_header->dst_addr,
                                                ipv4_header->src_addr, icmp_header->icmp_ident,
                                                icmp_header->icmp_seq_nb);
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    rte_ring_mp_enqueue_burst(ring->out, (void **) &icmp_buffer, 1, NULL);
}

void
hz_build_icmp_request(uint8_t *msg, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                      uint32_t dip, uint16_t id,
                      uint16_t seq_num) {
    // 以太网头部
    struct rte_ether_hdr *ether_header = (struct rte_ether_hdr *) msg;
    memcpy(ether_header->s_addr.addr_bytes, src_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    memcpy(ether_header->d_addr.addr_bytes, dst_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    ether_header->ether_type = htons(RTE_ETHER_TYPE_IPV4);
    // IP头部信息
    struct rte_ipv4_hdr *ipv4_header = (struct rte_ipv4_hdr *) (ether_header + 1);

    ipv4_header->version_ihl = 0x45;
    ipv4_header->type_of_service = 0x00;
    ipv4_header->total_length = htons(sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_icmp_hdr));
    ipv4_header->packet_id = 0;
    ipv4_header->fragment_offset = 0;
    ipv4_header->time_to_live = 64;
    ipv4_header->next_proto_id = IPPROTO_ICMP;
    ipv4_header->src_addr = sip;
    ipv4_header->dst_addr = dip;

    ipv4_header->hdr_checksum = 0;
    ipv4_header->hdr_checksum = rte_ipv4_cksum(ipv4_header);

    //ICMP header
    struct rte_icmp_hdr *icmp_header = (struct rte_icmp_hdr *) (ipv4_header + 1);
    icmp_header->icmp_ident = id;
    icmp_header->icmp_seq_nb =seq_num;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_type = RTE_IP_ICMP_ECHO_REPLY;
    icmp_header->icmp_cksum = 0;
    icmp_header->icmp_cksum = hz_checksum((uint16_t *) icmp_header, sizeof(struct rte_icmp_hdr));
#if ICMP_DEBUG
    const uint8_t *byte = (uint8_t *) icmp_header;
    size_t size = sizeof(struct rte_icmp_hdr);
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", byte[i]);
    }
    printf("\nchecksum= %x\n", icmp_header->icmp_cksum);

#endif
}

struct rte_mbuf *
hz_send_icmp(struct rte_mempool *memory_buffer_pool, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac,
             uint32_t sip,
             uint32_t dip, uint16_t id,
             uint16_t seq_num) {
    const unsigned total_length =
            sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_icmp_hdr);
    struct rte_mbuf *icmp_buffer = rte_pktmbuf_alloc(memory_buffer_pool);
    if (!icmp_buffer) {
        rte_exit(EXIT_FAILURE, "icmp_buffer.\n");
    }
    icmp_buffer->pkt_len = total_length;
    icmp_buffer->data_len = total_length;

    uint8_t *icmp_data = rte_pktmbuf_mtod(icmp_buffer, uint8_t*);
    hz_build_icmp_request(icmp_data, src_mac, dst_mac, sip, dip, id, seq_num);
    return icmp_buffer;

}

uint16_t hz_checksum(uint16_t *icmp_header_addr, int count) {
    register long sum = 0;
    while (count > 1) {
        sum += *(unsigned short *) icmp_header_addr++;
        count -= 2;
    }
    if (count > 0) {
        sum += *(unsigned char *) icmp_header_addr;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}