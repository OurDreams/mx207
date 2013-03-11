/**
 ******************************************************************************
 * @file       update.c
 * @version    V0.0.1
 * @brief      �������̡�����Э��ʵ��
 * @details    
 * @copyright
 *
 ******************************************************************************
 */

/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <update.h>
#include <types.h>
#include <target.h>
/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
/* �����붨��  */
/* 0x20 ��������  */
#define COMMAND_PING             0x20u
/* 0x21 ��ʼ����  */
#define COMMAND_DOWNLOAD         0x21u
/* 0x23 ��ȡ״̬  */
#define COMMAND_GET_STATUS       0x23u
/* 0x24 �������� */
#define COMMAND_SEND_DATA        0x24u
/*0x25 ��λ */
#define COMMAND_RESET            0x25u
/* Ӧ���� */
/* 0xCC ���ճɹ� */
#define COMMAND_ACK              0xCCu
/* 0x33 ����ʧ�� */
#define COMMAND_NAK              0x33u

/* ״̬�� */
/* 0x40 �ɹ� */
#define COMMAND_RET_SUCCESS      0x40u
/* 0x41 δ֪���� */
#define COMMAND_RET_UNKNOWN_CMD  0x41u
/* 0x42 �Ƿ����� */
#define COMMAND_RET_INVALID_CMD  0x42u
/* 0x43 �Ƿ����� */
#define COMMAND_RET_INVALID_PARA 0x43u
/* 0x44 FLASH����ʧ�� */
#define COMMAND_RET_FLASH_FAIL   0x44u
/* 0x45 ���������� */
#define COMMAND_RET_BUFFER_FAIL  0x45u

/* ��Ϣ��ʶ */
#define UPDATE_FLAG              "mx"
#define MAX_UPDATE_FILE_NUM      5u
#define FILENAME_UPDATE          "0:update_boot.mx"      /* ���������ļ� */
//#define FLASH_SECTOR_SIZE        (512u)

/* ������������ʼƫ����*/
#define FLASH_UPDATE_OFFSET      (0u)
/* �����������ݴ洢��ʼ��ַ */
#define APP_START_ADDR   FLASH_SECTOR_SIZE
/* �����������Ĵ�С512K */
#define FLASH_UPDATE_SIZE               (512 * 1024u)

/* ���ݻ�������С��ע��:���� <= 252 */
#define MAX_BUF_SIZE          252
/* Ӧ�ó�����ʼ��ַ��32K */
#define APP_START_ADDRESS     0x08008000u

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
/** ����״̬ */
typedef enum
{
    US_IDLE = 0,       /**< δ���� */
    US_PROCESSING,     /**< ������ */
    US_FINISH,         /**< ����� */
} update_status_e;

/* ����ԭ�ж���ģʽ */
#pragma pack(push)
/* �趨Ϊ1�ֽڶ��� */
#pragma pack(1)

/**
 * ������Ϣ�ṹ����
 */
typedef struct
{
    uint8_t flag[4]; /**< �������"sxdq", ���ʾ��Ҫ�������� */
    uint32_t addr;   /**< ����������DataFlash�еĴ洢��ַ */
    uint32_t len;    /**< �������ݵĳ��� */
    uint8_t id[16];  /**< �����ļ���ʶ */
} update_t;

/** �����ļ����� */
typedef enum
{
    FT_APP = 0,    /**< Ӧ�ó��� */
    FT_PARAM,      /**< �������� */
    FT_FONTS,      /**< �ֿ� */
} file_type_e;

/** �����ļ���Ϣ */
typedef struct
{
    file_type_e file_type;  /**< �����ļ����� */
    int8_t file_name[24];     /**< �ļ��� */
    uint32_t dest_addr;       /**< Ŀ���ַ */
    uint32_t zip_len;         /**< ��������еĳ��ȣ�ѹ���� */
    uint32_t raw_len;         /**< ԭʼ���ȣ�ѹ��ǰ�� */
    uint8_t md5[16];          /**< ԭʼ�ļ�md5ֵ��ѹ��ǰ�� */
} file_info_t;

/** �������� */
typedef struct
{
    uint16_t crc;             /**< CRC���� */
    uint32_t len;             /**< ���ȣ�������crc��len */
    uint8_t id[16];           /**< �����ʶ */
    bool_e is_zip;          /**< �Ƿ�ѹ�� */
    bool_e is_reboot;       /**< �Ƿ���Ҫ���� */
    uint8_t dummy;            /**< ���� */
    uint8_t file_count;       /**< �ļ����� */
    file_info_t file_info[MAX_UPDATE_FILE_NUM]; /**< �ļ���Ϣ */
} update_cmd_t;

/* �ָ�ԭ�ж���ģʽ  */
#pragma pack(pop)

/*-----------------------------------------------------------------------------
 Section: Private Function Prototypes
 ----------------------------------------------------------------------------*/
extern uint32_t
get_systime(void);

static bool_e
esc_key_detect(void);

static bool_e
send_hello(void);

static bool_e
receive_packet(uint8_t *cmd,
		uint8_t *data,
        uint8_t *size);

static void
send_status(uint8_t s);

static void
send_ack(void);

static void
send_nak(void);

static bool_e
check_sum(const uint8_t cmd,
        const uint8_t *data,
        const uint8_t size,
        const uint8_t sum);

static void
send_packet(const uint8_t *packet,
        uint16_t size);




/*-----------------------------------------------------------------------------
 Section: Private Variables
 ----------------------------------------------------------------------------*/
static volatile uint32_t the_run_status = RS_IDLE;


/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief      �Ӵ��ڽ�������
 * @param[in]  None
 * @param[out] None
 * @retval     status: COMMAND_RET_SUCCESS-�ɹ�������-ʧ��
 *
 * @details    
 *
 * @note 
 ******************************************************************************
 */
status_t
uart_update(void)
{
    uint32_t transfer_size = 0u;
    uint32_t start_address;
    uint8_t size = 0u;
    uint8_t s;
    uint8_t cmd = 0u;
    uint8_t data[MAX_BUF_SIZE];

    print("Press ESC to upgrade from serial port ...\r\n");

    /* ����Ƿ�Ҫ�Ӵ��ڽ������� */
    if (!esc_key_detect())
    {
        return (OK);
    }

    print("Receiving data ...");

    the_run_status = RS_RUNNING;

    /* ���˶��ఴ���򴮿��л������Ĵ����ֽ� */
    while (!send_hello())
    {
    }
    start_address = 0xffffffff;

    s = COMMAND_RET_SUCCESS;
    while (TRUE)
    {
        /* ���δ��ȷ���ձ��ģ��������� */
        if (!receive_packet(&cmd, data, &size))
        {
            send_nak();
            continue;
        }

        /* ���������ֽ��д��� */
        switch (cmd)
        {
        /* �������� */
        case COMMAND_PING:
            s = COMMAND_RET_SUCCESS;

            /* ����ȷ�ϰ� */
            send_ack();

            break;

            /* ��ʼ���� */
        case COMMAND_DOWNLOAD:
            s = COMMAND_RET_SUCCESS;
            do
            {
                /* ��鱨�ĳ��� */
                if (size != 8u)
                {
                    s = COMMAND_RET_INVALID_CMD;

                    break;
                }

                /* ��ʼ��ַ */
                start_address = ((uint32_t) data[0] << 24)
                        | ((uint32_t) data[1] << 16)
                        | ((uint32_t) data[2] << 8)
                        | (uint32_t) data[3];

                /* ��ʼ��ַ����Ϊ0���ɸ�BootLoader���о���д��λ�� */
                if (start_address != 0u)
                {
                    s = COMMAND_RET_INVALID_CMD;
                    break;
                }
                start_address = APP_START_ADDRESS;

                /* �����ֽ��� */
                transfer_size = ((uint32_t) data[4] << 24)
                        | ((uint32_t) data[5]
                        << 16) | ((uint32_t) data[6] << 8)
                        | (uint32_t) data[7];

                /* ��ʼ��Flash */
                iflash_init();

                /* ������Ҫ���µĿռ� */
                if (!iflash_erase(start_address, transfer_size))
                {
                    s = COMMAND_RET_FLASH_FAIL;
                }

            }
            while (FALSE);

            if (s != COMMAND_RET_SUCCESS)
            {
                transfer_size = 0u;
            }

            /* ȷ�ϱ����ѵõ����� */
            send_ack();

            break;

            /* ��ȡ״̬ */
        case COMMAND_GET_STATUS:
            /* ����ȷ�ϰ� */
            send_ack();

            /* ���ص�ǰ״̬ */
            send_status(s);

            break;
            /* ���� */
        case COMMAND_SEND_DATA:
            s = COMMAND_RET_SUCCESS;

            /* ����Ƿ�������δд�� */
            if (transfer_size >= size)
            {
                if (!iflash_write(data, start_address, (uint32_t)size))
                {
                    s = COMMAND_RET_FLASH_FAIL;
                }
                else
                {
                    transfer_size -= size;
                    start_address += size;
                }
            }
            else
            {
                s = COMMAND_RET_INVALID_PARA;
            }

            /* ����ȷ�ϰ� */
            send_ack();

            break;
            /* ��λ */
        case COMMAND_RESET:
            /* ����ȷ�ϰ� */
            send_ack();

            /* ��λ */
            reset();

            /* ��ѭ�� */
            while (TRUE)
            {
            }
            break;
            /* δ֪���� */
        default:
            /* ����ȷ�ϰ� */
            send_ack();

            /* ״̬��Ϊδ֪���� */
            s = COMMAND_RET_UNKNOWN_CMD;

            break;
        }
    }
    return OK;
}


/**
 ******************************************************************************
 * @brief      ESC�������
 * @param[in]  None
 * @param[out] None
 * @retval     bool_e: TRUE-�ɹ���FALSE-ʧ��
 *
 * @details    
 *
 * @note
 ******************************************************************************
 */
static bool_e
esc_key_detect(void)
{
    uint8_t key;
    bool_e result = FALSE;

    while (TRUE)
    {
        /* �������3�룬������ȴ� */
        if (3u <= get_systime())
        {
            break;
        }

        /* ����Ƿ��Ѱ���"ESC"�� */
        if ((uart_try_receive(&key)) && (27 == key))
        {
            result = TRUE;
            break;
        }
#if 0
        /* �˶δ��뱣�������ڲ��Դ��� */
        if (uart_try_receive(&key))
        {
            uart_send(key);
            break;
        }
#endif
    }
    print("\r\n");

    return (result);
}

/**
 ******************************************************************************
 * @brief      ���ͽ���ȷ�ϰ�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    ע��:ֻ˵�����ݰ��Ϸ����ѱ�����������ζ��������ȷ
 *
 * @note
 ******************************************************************************
 */
static void
send_ack(void)
{
    /* ȷ�ϰ� */
    const uint8_t packet_ack[] =
    { 0, 0, COMMAND_ACK };
    send_packet(packet_ack, ARRAY_SIZE(packet_ack));
}

/**
 ******************************************************************************
 * @brief      ���ؽ��շ��ϰ�
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    
 *
 * @note
 ******************************************************************************
 */
static void
send_nak(void)
{
    const uint8_t packet_nak[] =
    { 0, 0, COMMAND_NAK };
    send_packet(packet_nak, ARRAY_SIZE(packet_nak));
}

/**
 ******************************************************************************
 * @brief      �������ֱ���
 * @param[in]  None
 * @param[out] None
 * @retval     bool_e: TRUE-�ѽ����� FALSE-δ����
 *
 * @details    ����ͨ�������˶��ఴ���򴮿��л������Ĵ����ֽ�
 *
 * @note
 ******************************************************************************
 */
static bool_e
send_hello(void)
{
    uint8_t data = 0u;

    /* ��ȡ֡�����ֽ� */
    do
    {
        uart_receive(&data, 1u);
    }
    while (3u != data); /* �������֡���Ȳ�Ϊ3������� */

    /* ��ȡ֡У���ֽ� */
    uart_receive(&data, 1u);
    if (data != 0x20)
    {
        return (FALSE);
    }

    /* ��ȡ֡������ */
    uart_receive(&data, 1u);
    if (data != 0x20)
    {
        return (FALSE);
    }

    /* ����ȷ�ϰ� */
    send_ack();

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      ����һ�����ݰ�
 * @param[in]  None
 * @param[out] uint8_t *cmd   : ������
 * @param[out] uint8_t *data  : ���ջ���
 * @param[out] uint8_t *size  : data����
 * @retval     bool_e: TRUE-�ɹ��� FALSE-ʧ��
 *
 * @details    
 *
 * @note
 ******************************************************************************
 */
static bool_e
receive_packet(uint8_t *cmd,
        uint8_t *data,
        uint8_t *size)
{
    uint8_t sum = 0u;

    /* ��ȡ֡�����ֽ� */
    do
    {
        uart_receive(size, 1u);
    }
    while (*size < 3); /* ֡���� >= 3 */

    /* ��ȡ֡У���ֽ� */
    uart_receive(&sum, 1u);

    /* ��ȡ������ */
    uart_receive(cmd, 1u);

    /* �۳����ȡ������ּ�У���ֽڣ������ȡ���ֽ��� */
    *size -= 3;

    if (*size > 0)
    {
        uart_receive(data, *size);
    }

    /* ���֡У����󣬷���FALSE*/
    if (!check_sum(*cmd, data, *size, sum))
    {
        return (FALSE);
    }

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      ����״̬��
 * @param[in]  uint8_t s: ״̬
 * @param[out] None
 * @retval     None
 *
 * @details    
 *
 * @note
 ******************************************************************************
 */
static void
send_status(uint8_t s)
{
    uint8_t data;

    /* ���ͳ����ֽ� */
    data = 3u;
    uart_send(data);

    /* ����У���ֽ� */
    uart_send(s);

    /* ����״̬�ֽ� */
    uart_send(s);

    /* �ȴ�����һ����0�ֽ� */
    do
    {
        uart_receive((uint8_t *) &data, 1u);
    }
    while (0u == data);
}

/**
 ******************************************************************************
 * @brief      ��������
 * @param[in]  const uint8_t *packet: ����������
 * @param[in]  uint16_t size: ���������ݳ���
 * @param[out] None
 * @retval     None
 *
 * @details            
 *
 * @note
 ******************************************************************************
 */
static void
send_packet(const uint8_t *packet,
        uint16_t size)
{
    uint16_t i;

    delay(10);
    for (i = 0u; i < size; i++)
    {
        uart_send(packet[i]);
    }
}

/**
 ******************************************************************************
 * @brief      ���У��ֵ
 * @param[in]  const uint8_t cmd : ������
 * @param[in]  const uint8_t *data : ���ݻ�����
 * @param[in]  const uint8_t size : ���ݳ���
 * @param[in]  const uint8_t sum : У��ֵ
 * @param[out] None
 * @retval     bool_e: TRUE-��ȷ��flase-����
 *
 * @details            
 *
 * @note
 ******************************************************************************
 */
static bool_e
check_sum(const uint8_t cmd,
        const uint8_t *data,
        const uint8_t size,
        const uint8_t sum)
{
    uint8_t s = cmd;
    uint8_t i = size;

    while (i > 0u)
    {
        s += *data;
        data++;
        i--;
    }

    return (((s & 0xFFu) == sum) ? TRUE : FALSE);
}

#if 0
uint32_t
get_run_status(void)
{
	return the_run_status;
}
#endif
/*------------------------------End of update.c------------------------------*/
