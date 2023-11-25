//
// Created by wangyufan on 2023-11-21.
//

#include "deal_udp.h"

void
hz_build_udp_request(uint8_t *msg, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                     uint32_t dip, uint16_t src_port, uint16_t dst_port, unsigned char *data, uint16_t total_length) {

    // 以太网头部
    struct rte_ether_hdr *ether_header = (struct rte_ether_hdr *) msg;
    memcpy(ether_header->s_addr.addr_bytes, src_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    memcpy(ether_header->d_addr.addr_bytes, dst_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    ether_header->ether_type = htons(RTE_ETHER_TYPE_IPV4);
    // IP头部信息
    struct rte_ipv4_hdr *ipv4_header = (struct rte_ipv4_hdr *) (ether_header + 1);

    ipv4_header->version_ihl = 0x45;
    ipv4_header->type_of_service = 0x00;
    ipv4_header->total_length = htons(total_length - sizeof(struct rte_ether_hdr));
    ipv4_header->packet_id = 0;
    ipv4_header->fragment_offset = 0;
    ipv4_header->time_to_live = 64;
    ipv4_header->next_proto_id = IPPROTO_UDP;
    ipv4_header->src_addr = sip;
    ipv4_header->dst_addr = dip;

    ipv4_header->hdr_checksum = 0;
    ipv4_header->hdr_checksum = rte_ipv4_cksum(ipv4_header);

    // UDP
    struct rte_udp_hdr *udp_header = (struct rte_udp_hdr *) (ipv4_header + 1);
    udp_header->src_port = src_port;
    udp_header->dst_port = dst_port;
    uint16_t udp_length = total_length - sizeof(struct rte_ether_hdr) - sizeof(struct rte_ipv4_hdr);
    udp_header->dgram_len = htons(udp_length);

    rte_memcpy((uint8_t *) (udp_header + 1), data, udp_length);
    udp_header->dgram_cksum = 0;
    udp_header->dgram_cksum = rte_ipv4_udptcp_cksum(ipv4_header, udp_header);

}

struct rte_mbuf *
hz_send_udp(struct rte_mempool *memory_buffer_pool, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac,
            uint32_t sip,
            uint32_t dip, uint16_t src_port, uint16_t dst_port, unsigned char *data, uint16_t udp_data_length) {
    const unsigned total_length =
            udp_data_length + sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_udp_hdr);
    struct rte_mbuf *udp_buffer = rte_pktmbuf_alloc(memory_buffer_pool);
    if (!udp_buffer) {
        rte_exit(EXIT_FAILURE, "udp_buffer.\n");
    }
    udp_buffer->pkt_len = total_length;
    udp_buffer->data_len = total_length;
    uint8_t *udp_data = rte_pktmbuf_mtod(udp_buffer, uint8_t*);
    hz_build_udp_request(udp_data, src_mac, dst_mac, sip, dip, src_port, dst_port, data, total_length);
    return udp_buffer;
}