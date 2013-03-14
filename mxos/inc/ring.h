/**
 ******************************************************************************
 * @file       ring.h
 * @version    V0.0.1
 * @brief      ring bufģ��.
 * @details    �ṩ���λ���������, Modify from djyos.
 *
 ******************************************************************************
 */
#ifndef __RING_H__
#define __RING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define CN_LIMIT_UINT32     0xffffffff

struct ring_buf
{
    uint32_t    len;            /**< �������е��ֽ��� */
    uint32_t    offset_read;    /**< ��������ָ��,ָ����һ�ζ���λ�� */
    uint32_t    offset_write;   /**< ������дָ��,ָ����һ��д��λ�� */
    uint32_t    max_len;        /**< ��������󳤶�,Ԫ�ظ���. */
    uint8_t     *buf;           /**< ������ָ�� */
};

extern void     ring_init(struct ring_buf *ring, uint8_t *buf, uint32_t len);
extern uint32_t ring_capacity(struct ring_buf *ring);
extern uint8_t *ring_get_buf(struct ring_buf *ring);
extern uint32_t ring_write(struct ring_buf *ring,uint8_t *buffer, uint32_t len);
extern uint32_t ring_read(struct ring_buf *ring,uint8_t *buffer, uint32_t len);
extern uint32_t ring_check(struct ring_buf *ring);
extern bool_e   ring_if_empty(struct ring_buf *ring);
extern bool_e   ring_if_full(struct ring_buf *ring);
extern void     ring_flush(struct ring_buf *ring);
extern uint32_t ring_dumb_read(struct ring_buf *ring, uint32_t len);
extern uint32_t ring_recede_read(struct ring_buf *ring, uint32_t len);
extern uint32_t ring_skip_tail(struct ring_buf *ring, uint32_t size);
extern uint32_t ring_search_ch(struct ring_buf *ring, char_t c);
extern uint32_t ring_search_str(struct ring_buf *ring, char_t *string, uint32_t str_len);

#ifdef __cplusplus
}
#endif

#endif /* __RING_H__ */
