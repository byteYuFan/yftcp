//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_LIST_H
#define YFTCP_LIST_H

// 插入节点到链表头部
#define INSERT_NODE_HEAD(list, node)            \
    do {                                             \
        node->prev=NULL;                        \
        node->next=list;                        \
        if (list == node) list = node->next;         \
        list=node;      \
    } while (0)\

// 删除链表中的指定节点
#define DELETE_TARGET_NODE(list, node)                             \
    do {                                                          \
          if (node->prev != NULL) node->prev->next = node->next; \
          if (node->next != NULL) node->next->prev = node->prev; \
          if (list == node) list = node->next; \
          node->prev = node->next = NULL; \
    } while (0)


#endif //YFTCP_LIST_H
