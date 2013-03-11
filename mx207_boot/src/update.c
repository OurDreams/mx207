/**
 ******************************************************************************
 * @file       update.c
 * @version    V0.0.1
 * @brief      升级流程、串口协议实现
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
/* 命令码定义  */
/* 0x20 测试连接  */
#define COMMAND_PING             0x20u
/* 0x21 开始下载  */
#define COMMAND_DOWNLOAD         0x21u
/* 0x23 获取状态  */
#define COMMAND_GET_STATUS       0x23u
/* 0x24 发送数据 */
#define COMMAND_SEND_DATA        0x24u
/*0x25 复位 */
#define COMMAND_RESET            0x25u
/* 应答码 */
/* 0xCC 接收成功 */
#define COMMAND_ACK              0xCCu
/* 0x33 接收失败 */
#define COMMAND_NAK              0x33u

/* 状态码 */
/* 0x40 成功 */
#define COMMAND_RET_SUCCESS      0x40u
/* 0x41 未知命令 */
#define COMMAND_RET_UNKNOWN_CMD  0x41u
/* 0x42 非法命令 */
#define COMMAND_RET_INVALID_CMD  0x42u
/* 0x43 非法参数 */
#define COMMAND_RET_INVALID_PARA 0x43u
/* 0x44 FLASH操作失败 */
#define COMMAND_RET_FLASH_FAIL   0x44u
/* 0x45 缓冲区不足 */
#define COMMAND_RET_BUFFER_FAIL  0x45u

/* 信息标识 */
#define UPDATE_FLAG              "mx"
#define MAX_UPDATE_FILE_NUM      5u
#define FILENAME_UPDATE          "0:update_boot.mx"      /* 升级命令文件 */
//#define FLASH_SECTOR_SIZE        (512u)

/* 升级交换区起始偏移量*/
#define FLASH_UPDATE_OFFSET      (0u)
/* 升级数据内容存储起始地址 */
#define APP_START_ADDR   FLASH_SECTOR_SIZE
/* 升级交换区的大小512K */
#define FLASH_UPDATE_SIZE               (512 * 1024u)

/* 数据缓冲区大小，注意:必须 <= 252 */
#define MAX_BUF_SIZE          252
/* 应用程序起始地址，32K */
#define APP_START_ADDRESS     0x08008000u

/*-----------------------------------------------------------------------------
 Section: Private Type Definitions
 ----------------------------------------------------------------------------*/
/** 升级状态 */
typedef enum
{
    US_IDLE = 0,       /**< 未进行 */
    US_PROCESSING,     /**< 处理中 */
    US_FINISH,         /**< 已完成 */
} update_status_e;

/* 保存原有对齐模式 */
#pragma pack(push)
/* 设定为1字节对齐 */
#pragma pack(1)

/**
 * 升级信息结构定义
 */
typedef struct
{
    uint8_t flag[4]; /**< 如果等于"sxdq", 则表示需要进行升级 */
    uint32_t addr;   /**< 升级内容在DataFlash中的存储地址 */
    uint32_t len;    /**< 升级内容的长度 */
    uint8_t id[16];  /**< 升级文件标识 */
} update_t;

/** 升级文件类型 */
typedef enum
{
    FT_APP = 0,    /**< 应用程序 */
    FT_PARAM,      /**< 参数数据 */
    FT_FONTS,      /**< 字库 */
} file_type_e;

/** 升级文件信息 */
typedef struct
{
    file_type_e file_type;  /**< 升级文件类型 */
    int8_t file_name[24];     /**< 文件名 */
    uint32_t dest_addr;       /**< 目标地址 */
    uint32_t zip_len;         /**< 在命令包中的长度（压缩后） */
    uint32_t raw_len;         /**< 原始长度（压缩前） */
    uint8_t md5[16];          /**< 原始文件md5值（压缩前） */
} file_info_t;

/** 升级命令 */
typedef struct
{
    uint16_t crc;             /**< CRC检验 */
    uint32_t len;             /**< 长度，不包括crc、len */
    uint8_t id[16];           /**< 命令标识 */
    bool_e is_zip;          /**< 是否压缩 */
    bool_e is_reboot;       /**< 是否需要重启 */
    uint8_t dummy;            /**< 备用 */
    uint8_t file_count;       /**< 文件数量 */
    file_info_t file_info[MAX_UPDATE_FILE_NUM]; /**< 文件信息 */
} update_cmd_t;

/* 恢复原有对齐模式  */
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
 * @brief      从串口进行升级
 * @param[in]  None
 * @param[out] None
 * @retval     status: COMMAND_RET_SUCCESS-成功、其它-失败
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

    /* 检测是否要从串口进行升级 */
    if (!esc_key_detect())
    {
        return (OK);
    }

    print("Receiving data ...");

    the_run_status = RS_RUNNING;

    /* 过滤多余按键或串口切换产生的错误字节 */
    while (!send_hello())
    {
    }
    start_address = 0xffffffff;

    s = COMMAND_RET_SUCCESS;
    while (TRUE)
    {
        /* 如果未正确接收报文，放弃处理 */
        if (!receive_packet(&cmd, data, &size))
        {
            send_nak();
            continue;
        }

        /* 根据命令字进行处理 */
        switch (cmd)
        {
        /* 测试连接 */
        case COMMAND_PING:
            s = COMMAND_RET_SUCCESS;

            /* 返回确认包 */
            send_ack();

            break;

            /* 开始下载 */
        case COMMAND_DOWNLOAD:
            s = COMMAND_RET_SUCCESS;
            do
            {
                /* 检查报文长度 */
                if (size != 8u)
                {
                    s = COMMAND_RET_INVALID_CMD;

                    break;
                }

                /* 起始地址 */
                start_address = ((uint32_t) data[0] << 24)
                        | ((uint32_t) data[1] << 16)
                        | ((uint32_t) data[2] << 8)
                        | (uint32_t) data[3];

                /* 起始地址必须为0，由各BootLoader自行决定写入位置 */
                if (start_address != 0u)
                {
                    s = COMMAND_RET_INVALID_CMD;
                    break;
                }
                start_address = APP_START_ADDRESS;

                /* 下载字节数 */
                transfer_size = ((uint32_t) data[4] << 24)
                        | ((uint32_t) data[5]
                        << 16) | ((uint32_t) data[6] << 8)
                        | (uint32_t) data[7];

                /* 初始化Flash */
                iflash_init();

                /* 擦除将要更新的空间 */
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

            /* 确认报文已得到处理 */
            send_ack();

            break;

            /* 获取状态 */
        case COMMAND_GET_STATUS:
            /* 返回确认包 */
            send_ack();

            /* 返回当前状态 */
            send_status(s);

            break;
            /* 数据 */
        case COMMAND_SEND_DATA:
            s = COMMAND_RET_SUCCESS;

            /* 检查是否还有数据未写入 */
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

            /* 返回确认包 */
            send_ack();

            break;
            /* 复位 */
        case COMMAND_RESET:
            /* 返回确认包 */
            send_ack();

            /* 复位 */
            reset();

            /* 死循环 */
            while (TRUE)
            {
            }
            break;
            /* 未知命令 */
        default:
            /* 返回确认包 */
            send_ack();

            /* 状态设为未知命令 */
            s = COMMAND_RET_UNKNOWN_CMD;

            break;
        }
    }
    return OK;
}


/**
 ******************************************************************************
 * @brief      ESC按键检测
 * @param[in]  None
 * @param[out] None
 * @retval     bool_e: TRUE-成功、FALSE-失败
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
        /* 如果超过3秒，则结束等待 */
        if (3u <= get_systime())
        {
            break;
        }

        /* 检测是否已按下"ESC"键 */
        if ((uart_try_receive(&key)) && (27 == key))
        {
            result = TRUE;
            break;
        }
#if 0
        /* 此段代码保留，用于测试串口 */
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
 * @brief      发送接收确认包
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details    注意:只说明数据包合法并已被处理，而不意味着流程正确
 *
 * @note
 ******************************************************************************
 */
static void
send_ack(void)
{
    /* 确认包 */
    const uint8_t packet_ack[] =
    { 0, 0, COMMAND_ACK };
    send_packet(packet_ack, ARRAY_SIZE(packet_ack));
}

/**
 ******************************************************************************
 * @brief      返回接收否认包
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
 * @brief      发送握手报文
 * @param[in]  None
 * @param[out] None
 * @retval     bool_e: TRUE-已建立、 FALSE-未建立
 *
 * @details    建立通道，过滤多余按键或串口切换产生的错误字节
 *
 * @note
 ******************************************************************************
 */
static bool_e
send_hello(void)
{
    uint8_t data = 0u;

    /* 读取帧长度字节 */
    do
    {
        uart_receive(&data, 1u);
    }
    while (3u != data); /* 如果返回帧长度不为3，则放弃 */

    /* 读取帧校验字节 */
    uart_receive(&data, 1u);
    if (data != 0x20)
    {
        return (FALSE);
    }

    /* 读取帧命令字 */
    uart_receive(&data, 1u);
    if (data != 0x20)
    {
        return (FALSE);
    }

    /* 返回确认包 */
    send_ack();

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      接收一个数据包
 * @param[in]  None
 * @param[out] uint8_t *cmd   : 命令字
 * @param[out] uint8_t *data  : 接收缓冲
 * @param[out] uint8_t *size  : data长度
 * @retval     bool_e: TRUE-成功、 FALSE-失败
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

    /* 读取帧长度字节 */
    do
    {
        uart_receive(size, 1u);
    }
    while (*size < 3); /* 帧长度 >= 3 */

    /* 读取帧校验字节 */
    uart_receive(&sum, 1u);

    /* 读取命令字 */
    uart_receive(cmd, 1u);

    /* 扣除长度、命令字及校验字节，还需读取的字节数 */
    *size -= 3;

    if (*size > 0)
    {
        uart_receive(data, *size);
    }

    /* 如果帧校验错误，返回FALSE*/
    if (!check_sum(*cmd, data, *size, sum))
    {
        return (FALSE);
    }

    return (TRUE);
}

/**
 ******************************************************************************
 * @brief      发送状态包
 * @param[in]  uint8_t s: 状态
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

    /* 发送长度字节 */
    data = 3u;
    uart_send(data);

    /* 发送校验字节 */
    uart_send(s);

    /* 发送状态字节 */
    uart_send(s);

    /* 等待返回一个非0字节 */
    do
    {
        uart_receive((uint8_t *) &data, 1u);
    }
    while (0u == data);
}

/**
 ******************************************************************************
 * @brief      发送数据
 * @param[in]  const uint8_t *packet: 待发送数据
 * @param[in]  uint16_t size: 待发送数据长度
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
 * @brief      检查校验值
 * @param[in]  const uint8_t cmd : 命令字
 * @param[in]  const uint8_t *data : 数据缓冲区
 * @param[in]  const uint8_t size : 数据长度
 * @param[in]  const uint8_t sum : 校验值
 * @param[out] None
 * @retval     bool_e: TRUE-正确、flase-错误
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
