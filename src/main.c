//
// Created by wangyufan on 2023-11-20.
//
#include "yftcp.h"
#include "function.h"
#include "config.h"
#include "arp.h"
#include "ring.h"
#include "deal_packet.h"
#include "arp_table.h"
#include "timer.h"

uint32_t global_local_ipv4 = MAKE_IPV4_ADDR(192, 168, 182, 41);
unsigned int global_ethernet_device_id = 0;
struct rte_ether_addr global_ethernet_device_mac;
static const struct rte_eth_conf port_conf_default = {
        .rxmode = {.max_rx_pkt_len = RTE_ETHER_MAX_LEN}
};
struct rte_ether_addr global_default_arp_mac = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

static struct rte_mempool *init_dpdk_environment(int argc, char *argv[]);

static void yf_init_port(struct rte_mempool *memory_buffer_pool);

static void enable_mul_thread_deal_pkt(struct rte_mempool *memory_buffer_pool);

static void init(void);

static void enable_arp_timer(struct rte_mempool *memory_buffer_pool, struct rte_timer *, rte_timer_cb_t function);

struct hz_ring_interface *ring_singleton_instance = NULL;
struct hz_arp_table *arp_table_singleton_instance = NULL;

int main(int argc, char *argv[]) {
    struct rte_mempool *memory_buffer_pool = init_dpdk_environment(argc, argv);
    init();
    enable_mul_thread_deal_pkt(memory_buffer_pool);
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    rte_timer_subsystem_init();
    struct rte_timer arp_timer;
    enable_arp_timer(memory_buffer_pool, &arp_timer,arp_request_timer_cb);

    printf("*******************DPDK STARTING****************************\n");
    while (1) {

        struct rte_mbuf *rx[BURST_SIZE];
        uint16_t rx_num_received = rte_eth_rx_burst(global_ethernet_device_id, 0, rx, BURST_SIZE);
        if (rx_num_received > BURST_SIZE) {
            rte_exit(EXIT_FAILURE, "Error receiving from eth\n");
        } else if (rx_num_received > 0) {
            rte_ring_sp_enqueue_burst(ring->in, (void **) rx, rx_num_received, NULL);
        }

        struct rte_mbuf *tx[BURST_SIZE];
        uint16_t tx_num_received = rte_ring_sc_dequeue_burst(ring->out, (void **) tx, BURST_SIZE, NULL);
        if (tx_num_received > 0) {
            rte_eth_tx_burst(global_ethernet_device_id, 0, tx, tx_num_received);
            for (uint16_t i = 0; i < tx_num_received; i++) {
                rte_pktmbuf_free(tx[i]);
            }
        }

        static uint64_t prev_tsc = 0, cur_tsc;
        uint64_t diff_tsc;

        cur_tsc = rte_rdtsc();
        diff_tsc = cur_tsc - prev_tsc;
        if (diff_tsc > TIMER_RESOLUTION_CYCLES) {
            rte_timer_manage();
            prev_tsc = cur_tsc;
        }
    }
}


static struct rte_mempool *init_dpdk_environment(int argc, char *argv[]) {
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL init\n");

    }

    //创建内存池
    struct rte_mempool *memory_buffer_pool = rte_pktmbuf_pool_create("memory_buffer_pool",
                                                                     NUMBER_OF_ELEMENTS_INT_THE_MEMORY_BUFFER_POOL,
                                                                     0, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                                                     (int) rte_socket_id());
    if (memory_buffer_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Could not create memory_buffer_pool pool\n");
    }
    //初始化以太网设备
    rte_eth_macaddr_get(global_ethernet_device_id, &global_ethernet_device_mac);
    yf_init_port(memory_buffer_pool);
    return memory_buffer_pool;
}

static void yf_init_port(struct rte_mempool *memory_buffer_pool) {

    //返回可用的 Ethernet 设备数量
    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Supported eth found.\n");
    }

    //获取指定 Ethernet 设备的设备信息
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(global_ethernet_device_id, &dev_info); //

    //配置以太网设备
    const int num_rx_queues = 1;    //接收队列数量
    const int num_tx_queues = 1;    //发送队列数量
    struct rte_eth_conf port_conf = port_conf_default;    //struct rte_eth_conf 结构体包含了各种网卡相关的配置参数和属性
    rte_eth_dev_configure(global_ethernet_device_id, num_rx_queues, num_tx_queues, &port_conf);

    //分配并设置以太网设备的接收队列
    if (rte_eth_rx_queue_setup(global_ethernet_device_id, 0, 1024,
                               rte_eth_dev_socket_id(global_ethernet_device_id), NULL, memory_buffer_pool) < 0) {

        rte_exit(EXIT_FAILURE, "Could not setup RX queue\n");
    }
    struct rte_eth_txconf txq_conf = dev_info.default_txconf;
    txq_conf.offloads = port_conf.rxmode.offloads;

    //分配并设置以太网设备的传输队列
    if (rte_eth_tx_queue_setup(global_ethernet_device_id, 0, 1024,
                               rte_eth_dev_socket_id(global_ethernet_device_id), &txq_conf) < 0) {

        rte_exit(EXIT_FAILURE, "Could not setup TX queue\n");
    }
    //启动以太网设备。
    if (rte_eth_dev_start(global_ethernet_device_id) < 0) {
        rte_exit(EXIT_FAILURE, "Could not start\n");
    }
//    print_eth_dev_info(&dev_info);
}

static void enable_mul_thread_deal_pkt(struct rte_mempool *memory_buffer_pool) {
    unsigned lcore_id = rte_lcore_id();
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(hz_pkt_process, memory_buffer_pool, lcore_id);
}

static void init(void) {
    ring_singleton_instance = hz_get_ring_interface_instance();
    arp_table_singleton_instance = hz_get_arp_table_instance();
}

static void
enable_arp_timer(struct rte_mempool *memory_buffer_pool, struct rte_timer *arp_timer, rte_timer_cb_t function) {
    rte_timer_init(arp_timer);
    /* load timer0, every second, on master lcore, reloaded automatically */
    unsigned hz = rte_get_timer_hz();
    unsigned lcore_id = rte_lcore_id();
    rte_timer_reset(arp_timer, hz, PERIODICAL, lcore_id, function, memory_buffer_pool);
}
