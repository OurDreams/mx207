/**
 ******************************************************************************
 * @file      syscli.c
 * @brief     C Source file of syscli.c.
 * @details   This file including all API functions's 
 *            implement of syscli.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <types.h>
#include <maths.h>
#include <shell.h>
#include <dmnLib.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/

extern void
bsp_reboot(void) __attribute__ (( weak ));

/* Shell cmd for task information */
extern uint32_t vTaskInfo();
uint32_t do_task_info (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    return vTaskInfo();
}

SHELL_CMD(
    i,  CFG_MAXARGS,        do_task_info,
    "Summary of tasks' TCBs\r\n"
);

/*SHELL CMD FOR NET*/
uint32_t do_reboot(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    bsp_reboot();
    return 1;
}

SHELL_CMD(
    reboot, CFG_MAXARGS,        do_reboot,
    "reboot \n"
);
/**
 *******************************************************************************
 * @brief      display the information of system memory pool.
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *      This routine is a shell command for display system memory
 *      pool's usage information.
 * @note
 *******************************************************************************
 */
extern void showMenInfo(void);
uint32_t do_memshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    showMenInfo();
    return 0;
}

SHELL_CMD(
    memshow,   CFG_MAXARGS,        do_memshow,
    "Print heap memory infomation \n"
);

/**
 ******************************************************************************
 * @brief      Show devLib info
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
extern void devlib_show_info(void);
uint32_t  do_devshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    devlib_show_info();
    return 0;
}

SHELL_CMD(
    devshow,   CFG_MAXARGS,        do_devshow,
    "Print devlib infomation \n"
);

uint32_t  do_dmnshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    dmn_info(TRUE);
    return 0;
}

SHELL_CMD(
    dmnshow,   CFG_MAXARGS,        do_dmnshow,
    "Print dmn infomation \n"
);

extern void tty_show_info(void);
uint32_t  do_ttyshow(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    tty_show_info();
    return 0;
}

SHELL_CMD(
    ttyshow,   CFG_MAXARGS,        do_ttyshow,
    "Print tty infomation \n"
);

uint32_t do_time_info (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    struct tm daytime;
    time_t stime;

    stime = time(NULL);
    daytime = *localtime(&stime);
    printf("DT: %04d-%02d-%02d  %02d:%02d:%02d\r\n",
        daytime.tm_year+ 1900, daytime.tm_mon + 1, daytime.tm_mday,
        daytime.tm_hour, daytime.tm_min, daytime.tm_sec);
    return 1;
}


SHELL_CMD(
    time,  CFG_MAXARGS,        do_time_info,
    "Print current date and time\r\n"
);

#if 0
uint32_t do_mem_md (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t i;
    uint8_t j;
    uint32_t adr, nunits, width;
    uint8_t linebytes, cnt, bytes[16];

    // 参数数量=2~4
    if ((argc < 2) || (argc > 4)) {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    // 取起始地址
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // 取显示数量
    if (argc > 2) {
        if (0 == sscanf((char_t *)argv[2], "%i", &nunits)) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
        // 至少显示1个
        nunits = MAX(nunits, 1);
    } else nunits = 16 * 8; // 默认为16 * 8

    // 取访问宽度
    if (argc > 3) {
        if ((0 == sscanf((char_t *)argv[3], "%i", &width))
            || ((width != 1) && (width != 2) && (width != 4))) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
    } else width = 2; // 默认为半字

    linebytes = 0;
    for (i = 0; i < nunits;  i++) {
        // 输出地址
        if (0 == linebytes)
           printf("%08X: ", adr);
        // 检查当前地址是否允许读取
#if 0
        if (!sysMemIsReadable(adr, width)) {
            printf("\r\nMemory Access Exception\r\n");
            return 1;
        }
#endif
        printf(" ");
        // 读取并打印hex值
        switch (width) {
        case 1: // 按字节读取
            bytes[linebytes] = (*((uint8_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            adr++;
            break;

        case 2: // 按半字读取
            (*((uint16_t *)&bytes[linebytes])) = (*((uint16_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 2;
            break;

        case 4: // 按字读取
            (*((uint32_t *)&bytes[linebytes])) = (*((uint32_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 4;
            break;

        }

        // 打印字符
        if (16 == linebytes) {
            printf("   *");
            for (j = 0; j < 16; j++) {
                printf("%c", isprint(bytes[j]) ? bytes[j] : '.');
            }
            printf("*\r\n");
            linebytes = 0;
        }
    }
    // 打印剩余字符
    if (0 != linebytes) {
        cnt = 16 - linebytes;
        cnt = (cnt << 1) + (cnt >> (width == 1 ? 0 : width / 2));
        for (i = 0; i < cnt; i++)
            printf(" ");
        printf("   *");
        for (i = 0; i < linebytes; i++) {
           printf("%c", isprint(bytes[i]) ? bytes[i] : '.');
        }
        printf("*\r\n");
    }

    return 1;
}

SHELL_CMD(
    d,  CFG_MAXARGS,        do_mem_md,
    "\r\t adr[nunits[width]] \r\t\t\t\t Display memory\n"
);


uint32_t do_mem_mm (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t adr, value, width;

    // 参数数量=4
    if (argc != 4) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // 取起始地址
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // 取写入宽度
    if ((0 == sscanf((char_t *)argv[2], "%i", &width))
        || ((width != 1) && (width != 2) && (width != 4))) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // 取值
    if (0 == sscanf((char_t *)argv[3], "%i", &value)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }
#if 0
    // 检查是否可以修改
    if (!sysMemIsWriteable(adr, width)) {
        printf("\r\nMemory Access Exception\r\n");
        return 1;
    }
#endif
    switch (width) {
    case 1: // 按字节写入
        (*((uint8_t *)(adr))) = value; break;
    case 2: // 按半字写入
        (*((uint16_t *)(adr))) = value; break;
    case 4: // 按字写入
        (*((uint32_t *)(adr))) = value; break;
    }

    return 0;
}

SHELL_CMD(
    m,  CFG_MAXARGS,        do_mem_mm,
    "Modify memory \r\t adr width value\n"

);



#endif
/*----------------------------syscli.c--------------------------------*/
