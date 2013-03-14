/**
 ******************************************************************************
 * @file       ring.h
 * @version    V0.0.1
 * @brief      ring bufģ��.
 * @details    �ṩ���λ���������, Modify from djyos.
 *
 ******************************************************************************
 */
#include <string.h>
#include <ring.h>

/**
 ******************************************************************************
 * @brief      �������λ�����
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]  *buf     : ��������ʼ��ַ
 * @param[in]   len     : ����������.��λ���ֽ���
 * @retval     None
 *
 * @details
 *  �������λ���������ʼ����ʹ���������֮ǰ���û�Ӧ�ö��建�����ڴ���
 *  ���������ݽṹ��
 ******************************************************************************
 */
void
ring_init(struct ring_buf *ring, uint8_t *buf, uint32_t len)
{
    ring->buf = buf;
    ring->max_len = len;
    ring->offset_write = 0;
    ring->offset_read = 0;
    ring->len = 0;
}

/**
 ******************************************************************************
 * @brief     ��黺��������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     ���������������ǵ���ring_initʱʹ�õ�len������
 *
 * @details
 * ���ػ���������
 ******************************************************************************
 */
uint32_t
ring_capacity(struct ring_buf *ring)
{
    return ring->max_len;
}

/**
 ******************************************************************************
 * @brief      �����ֽڳص�ַ
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     ���λ��������ֽڳص�ַ
 *
 * @details
 * ��ѯ���������ֽڳص�ַ�������ַ���û�����ring_initʱʹ�õĻ�������ַ��
 * @note
 *      ����������������û�������ַ������
 *      1��Ҫ�ͷŻ������ڴ棬Ӧ�ó������ʹ�õ���ring_initʱʹ�õ�ָ�룬�����
 *         ָ�붪ʧ�����ñ�����ȡ�ظ�ָ�롣
 *      2���û����Լ����ַ��ʸû��������ⲻ��һ�����ǵ�ѡ�����ƻ�ģ�������
 *         �����ɣ���ʱ��ʹ����Ӧ����ȫ�����Լ��ڸ�ʲô!
 ******************************************************************************
 */
uint8_t*
ring_get_buf(struct ring_buf *ring)
{
    return ring->buf;
}

/**
 ******************************************************************************
 * @brief      ���λ�����д��
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]  *buffer  : ��д�������ָ��
 * @param[in]   len     : ��д������ݳ���.��λ���ֽ���
 * @retval     ʵ��д����ֽ���,������������㹻�Ŀռ�,=len
 *
 * @details
 * ���λ�����д�����ɸ��ֽ�,����ʵ��д���������,���ƶ�дָ��,�������
 * ������û���㹻�Ŀռ�,��ʵ��ʣ��ռ�д��
 ******************************************************************************
 */
uint32_t
ring_write(struct ring_buf *ring, uint8_t *buffer, uint32_t len)
{
    uint32_t wr_len;
    uint32_t partial;

    wr_len = ring->max_len - ring->len;
    if (wr_len == 0)
    {
        return 0;
    }
    if (wr_len > len)
    {
        wr_len = len;
    }
    if ((ring->offset_write + wr_len) > ring->max_len)
    { //���ݷ�������
        partial = ring->max_len - ring->offset_write;
        memcpy(&ring->buf[ring->offset_write], buffer, partial); //д��һ����
        memcpy(ring->buf, &buffer[partial], wr_len - partial); //д�ڶ�����
        ring->offset_write = wr_len - partial;
    }
    else
    { //����������
        memcpy(&ring->buf[ring->offset_write], buffer, wr_len);
        ring->offset_write += wr_len;
    }
    ring->len += wr_len;

    return wr_len;
}

/**
 ******************************************************************************
 * @brief      �ӻ��λ�������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]  *buffer  : �������ݵĻ�����ָ��
 * @param[in]   len     : �����������ݳ���.��λ���ֽ���
 * @retval     ʵ�ʶ������ֽ���,������������㹻������,=len
 *
 * @details
 * �ӻ��λ������������ɸ��ֽ�,����ʵ�ʶ�����������,�����ƶ���ָ�����
 * �����������ݲ��㣬��ʵ��������ȡ��
 ******************************************************************************
 */
uint32_t
ring_read(struct ring_buf *ring, uint8_t *buffer, uint32_t len)
{
    uint32_t wr_len;

    wr_len = (ring->len < len) ? ring->len : len;
    if ((ring->offset_read + wr_len) > ring->max_len)
    { //���ݷ�������
        uint32_t partial;
        partial = ring->max_len - ring->offset_read;
        memcpy(buffer, &ring->buf[ring->offset_read], partial); //д��һ����
        memcpy(&buffer[partial], ring->buf, wr_len - partial); //д�ڶ�����
        ring->offset_read = wr_len - partial;
    }
    else
    { //����������
        memcpy(buffer, &ring->buf[ring->offset_read], wr_len);
        ring->offset_read += wr_len;
    }
    ring->len -= wr_len;

    return wr_len;
}

/**
 ******************************************************************************
 * @brief      ��黺����������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     �������е�������
 *
 * @details    ���ָ���Ļ��λ������е�������,�����ֽ���.
 ******************************************************************************
 */
uint32_t
ring_check(struct ring_buf *ring)
{
    return ring->len;
}

/**
 ******************************************************************************
 * @brief      ��黺�����Ƿ��
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     ���򷵻�TRUE,�ǿշ���FALSE
 *
 * @details     ���ָ���Ļ��λ��������Ƿ��Ѿ���.
 ******************************************************************************
 */
bool_e
ring_if_empty(struct ring_buf *ring)
{
    return (ring->len == 0) ? TRUE : FALSE;
}

/**
 ******************************************************************************
 * @brief      ��黺�����Ƿ���
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     ���򷵻�TRUE,��������FALSE
 *
 * @details     ���ָ���Ļ��λ��������Ƿ��Ѿ���.
 *
 * @note
 ******************************************************************************
 */
bool_e
ring_if_full(struct ring_buf *ring)
{
    return (ring->len == ring->max_len) ? TRUE : FALSE;
}

/**
 ******************************************************************************
 * @brief      ��ջ��λ�����
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @retval     None
 *
 * @details     �������������������
 ******************************************************************************
 */
void
ring_flush(struct ring_buf *ring)
{
    ring->len = 0;
    ring->offset_write = 0;
    ring->offset_read = 0;
}

/**
 ******************************************************************************
 * @brief      �ͷ���������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]   len     : �ͷŵ���������
 * @retval     ʵ���ͷŵ�������
 *
 * @details �Ӷ�ָ�뿪ʼ,�ͷŵ�ָ����С������,�൱���ƶ���len���ֽ�
 ******************************************************************************
 */
uint32_t
ring_dumb_read(struct ring_buf *ring, uint32_t len)
{
    uint32_t result;

    result = (ring->len < len) ? ring->len : len;
    if ((ring->offset_read + result) > ring->max_len)
    { //���ݷ�������
        ring->offset_read = result + ring->offset_read - ring->max_len;
    }
    else
    { //����������
        ring->offset_read += result;
    }
    ring->len -= result;
    return result;
}

/**
 ******************************************************************************
 * @brief      �˻���������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]   len     : �˻ص���������
 * @retval     ʵ���˻ص�������
 *
 * @details
 * ��������ring_dumb_read���������෴���ѻ�����ָ���˻�len�ֽڣ�����˻ص�
 * ���ȳ����������Ŀ��г��ȣ���ȡ���������г��ȡ��൱�ڰѻ��������Ѿ�����
 * �����ݷ��ػ�����������û�ж��������ӡ�ringģ�鲢��У���˻صĲ����Ƿ����
 * ԭ�������ݡ�
 ******************************************************************************
 */
uint32_t
ring_recede_read(struct ring_buf *ring, uint32_t len)
{
    uint32_t result;

    if ((ring->max_len - ring->len) > len) //���г��ȴ������˻صĳ���
    {
        result = len;
    }
    else
    {
        result = ring->max_len - ring->len;
    }

    if (ring->offset_read < result)
    { //���ݷ�������
        ring->offset_read = ring->max_len - (result - ring->offset_read);
    }
    else
    { //����������
        ring->offset_read -= result;
    }
    ring->len += result;

    return result;
}

/**
 ******************************************************************************
 * @brief      ȡ����������
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]   len     : �˻ص���������
 * @retval     ʵ���˻ص�������
 *
 * @details  ȡ���Ѿ�д�����Ի��������������ݣ��������û��д��һ����
 ******************************************************************************
 */
uint32_t
ring_skip_tail(struct ring_buf *ring, uint32_t size)
{
    uint32_t result;

    if (ring->len > size)
    {
        result = size;
        if (ring->offset_write < result)
        { //���ݷ�������
            ring->offset_write = ring->max_len - (result - ring->offset_write);
        }
        else
        { //����������
            ring->offset_read -= result;
        }
        ring->len -= result;
    }
    else
    {
        result = ring->len;
        ring_flush(ring);
    }

    return result;
}

/**
 ******************************************************************************
 * @brief      �����ַ�
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]   c       : ����ҵ��ַ�
 * @retval     c���ֵ�λ��,���û�г����򷵻� CN_LIMIT_UINT32
 *
 * @details    ��ring��ǰ��λ�ÿ�ʼ�����ַ�c��λ��
 ******************************************************************************
 */
uint32_t
ring_search_ch(struct ring_buf *ring, char_t c)
{
    uint32_t i;
    uint8_t *buf = ring->buf;

    if (ring->offset_read > ring->offset_write)
    { //�������л���
        for (i = ring->offset_read; i < ring->max_len; i++)
        {
            if (buf[i] == c)
            {
                return (i - ring->offset_read);
            }
        }
        for (i = 0; i < ring->offset_write; i++)
        {
            if (buf[i] == c)
            {
                return (i - ring->offset_read + ring->max_len);
            }
        }
    }
    else
    { //������û�л���
        for (i = ring->offset_read; i < ring->offset_write; i++)
        {
            if (buf[i] == c)
            {
                return (i - ring->offset_read);
            }
        }
    }
    return CN_LIMIT_UINT32;
}

/**
 ******************************************************************************
 * @brief      �����ַ�����
 * @param[in]  *ring    : Ŀ�껷�λ������ṹָ��
 * @param[in]  *string  : ����ҵ��ַ�����
 * @param[in]   str_len : �ַ����г���
 * @retval     string���ֵ�λ�����offset_read��ƫ����,
 *             ���û�г��ַ��� CN_LIMIT_UINT32
 *
 * @details  ��ring��ǰ��λ�ÿ�ʼ�����ַ����е�λ��,�ַ����в���0����,����ָ������
 *
 * @note ������ܿ��ܱȽϳ���,�����ڱ�дʱע�����ٶ��Ż�,��ȴʹ����������.
 ******************************************************************************
 */
uint32_t
ring_search_str(struct ring_buf *ring, char_t *string, uint32_t str_len)
{
    uint32_t i, j;
    bool_e next;
    uint8_t *buf;
    uint32_t end, start;

    if (ring->len < str_len)
    {
        return CN_LIMIT_UINT32;
    }
    buf = ring->buf;
    if (ring->offset_read <= ring->offset_write)
    { //������û�л���
        for (i = ring->offset_read; i <= ring->offset_write - str_len; i++)
        {
            next = FALSE;
            for (j = 0; j < str_len; j++)
            {
                if (buf[i + j] != string[j])
                {
                    next = TRUE;
                }
            }
            if (next == FALSE)
            {
                return (i - ring->offset_read);
            }
        }
    }
    else
    { //�������л���
      //�ȴ������Ʋ���,��������������ĩ��ϲ���
        for (i = ring->offset_read; i <= (ring->max_len - str_len); i++)
        {
            next = FALSE;
            for (j = 0; j < str_len; j++)
            {
                if (buf[i + j] != string[j])
                {
                    next = TRUE; //������ֲ���,���ʾҪ������һ��ѭ��
                }
            }
            if (next == FALSE)
            { //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                return (i - ring->offset_read);
            }
        }
        //�ٴ���������ĩ��ϲ����Լ����Ʋ���
        if (ring->offset_write >= str_len)
        { //�������л��Ʋ��ֵĳ��ȳ������ҳ���,ĩ�˺��׶˷ֱ����
          //�Ȳ��һ�����ĩ�˲���
            for (; i < ring->max_len; i++)
            {
                next = FALSE;
                //string�ֳ�������,end���ַ��ڻ�����ĩ��,start���ַ��ڻ�������
                end = ring->max_len - i;
                start = str_len - end;
                for (j = 0; j < end; j++)
                { //�ȱȽϻ�����ĩ�˲���
                    if (buf[i + j] != string[j])
                    {
                        next = TRUE;
                    }
                }
                if (next == FALSE)
                { //������ĩ�˲���ȫ��ƥ��,��Ҫ���׶��Ƿ�Ҳƥ��
                    for (j = 0; j < start; j++)
                    {
                        if (buf[j] != string[start + j])
                        {
                            next = TRUE;
                        }
                    }
                }
                if (next == FALSE)
                { //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
            //�ٲ����׶˲���
            for (i = 0; i < (ring->offset_write - str_len); i++)
            {
                next = FALSE;
                for (j = 0; j < str_len; j++)
                {
                    if (buf[i + j] != string[j])
                    {
                        next = TRUE; //������ֲ���,���ʾҪ������һ��ѭ��
                    }
                }
                if (next == FALSE)
                { //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
        }
        else
        { //�������л��Ʋ��ֳ���û�г���str_len,ֻ��Ҫ����ĩ�˼���.
            next = FALSE;
            for (; i < (ring->offset_read + ring->len - str_len); i++)
            {
                uint32_t end, start;
                next = FALSE;
                //string�ֳ�������,end���ַ��ڻ�����ĩ��,start���ַ��ڻ�������
                end = ring->max_len - i;
                start = str_len - end;
                for (j = 0; j < end; j++)
                { //�ȱȽϻ�����ĩ�˲���
                    if (buf[i + j] != string[j])
                    {
                        next = TRUE;
                    }
                }
                if (next == FALSE)
                { //������ĩ�˲���ȫ��ƥ��,��Ҫ���׶��Ƿ�Ҳƥ��
                    for (j = 0; j < start; j++)
                    {
                        if (buf[j] != string[start + j])
                        {
                            next = TRUE;
                        }
                    }
                }
                if (next == FALSE)
                { //��ʾ��һ��ѭ�����Ѿ�����ƥ��Ĵ�,
                    return (i - ring->offset_read);
                }
            }
        }
    }
    return CN_LIMIT_UINT32;
}
