/**
 ******************************************************************************
 * @file      memCmd.c
 * @brief     C Source file of memCmd.c.
 * @details   This file including all API functions's 
 *            implement of memCmd.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <maths.h>
#include <shell.h>

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
#define in_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isascii(c)  ((unsigned) (c) <= 0177)
#define toascii(c)  ((c) & 0177)

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
uint32_t do_mem_md (cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    uint32_t i;
    uint8_t j;
    uint32_t adr, nunits, width;
    uint8_t linebytes, cnt, bytes[16];

    // ��������=2~4
    if ((argc < 2) || (argc > 4)) {
        printf("Usage:\r\n");
        printf("%s\r\n", cmdtp->usage);
        return 1;
    }
    // ȡ��ʼ��ַ
    adr = strtoul((const char_t *)argv[1], 0, 16);

#if 0
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }
#endif
    // ȡ��ʾ����
    if (argc > 2) {
        nunits = strtoul((const char_t *)argv[2], 0, 10);
#if 0
        if (0 == sscanf((char_t *)argv[2], "%i", &nunits)) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
#endif
        // ������ʾ1��
        nunits = MAX(nunits, 1);
    } else nunits = 16 * 8; // Ĭ��Ϊ16 * 8

    // ȡ���ʿ��
    if (argc > 3) {
        width = strtoul((const char_t *)argv[3], 0, 10);
        if (((width != 1) && (width != 2) && (width != 4))) {
            printf ("Usage:\r\n");
            printf ("%s\r\n", cmdtp->usage);
            return 1;
        }
    } else width = 2; // Ĭ��Ϊ����

    linebytes = 0;
    for (i = 0; i < nunits;  i++) {
        // �����ַ
        if (0 == linebytes)
           printf("%08X: ", adr);
        // ��鵱ǰ��ַ�Ƿ������ȡ
#if 0
        if (!sysMemIsReadable(adr, width)) {
            printf("\r\nMemory Access Exception\r\n");
            return 1;
        }
#endif
        printf(" ");
        // ��ȡ����ӡhexֵ
        switch (width) {
        case 1: // ���ֽڶ�ȡ
            bytes[linebytes] = (*((uint8_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            adr++;
            break;

        case 2: // �����ֶ�ȡ
            (*((uint16_t *)&bytes[linebytes])) = (*((uint16_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 2;
            break;

        case 4: // ���ֶ�ȡ
            (*((uint32_t *)&bytes[linebytes])) = (*((uint32_t *)(adr)));
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            printf("%02X", bytes[linebytes++]);
            adr += 4;
            break;

        }

        // ��ӡ�ַ�
        if (16 == linebytes) {
            printf("   *");
            for (j = 0; j < 16; j++) {
                printf("%c", isprint(bytes[j]) ? bytes[j] : '.');
            }
            printf("*\r\n");
            linebytes = 0;
        }
    }
    // ��ӡʣ���ַ�
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

    // ��������=4
    if (argc != 4) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡ��ʼ��ַ
    adr = strtoul((const char_t *)argv[1], 0, 16);
#if 0
    if (0 == sscanf((char_t *)argv[1], "%i", &adr)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }
#endif
    // ȡд����
    width = strtoul((const char_t *)argv[2], 0, 10);
    if (((width != 1) && (width != 2) && (width != 4))) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    // ȡֵ
    value = strtoul((const char_t *)argv[3], 0, 10);
#if 0
    if (0 == sscanf((char_t *)argv[3], "%i", &value)) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }
#endif
#if 0
    // ����Ƿ�����޸�
    if (!sysMemIsWriteable(adr, width)) {
        printf("\r\nMemory Access Exception\r\n");
        return 1;
    }
#endif
    switch (width) {
    case 1: // ���ֽ�д��
        (*((uint8_t *)(adr))) = value; break;
    case 2: // ������д��
        (*((uint16_t *)(adr))) = value; break;
    case 4: // ����д��
        (*((uint32_t *)(adr))) = value; break;
    }

    return 0;
}

SHELL_CMD(
    m,  CFG_MAXARGS,        do_mem_mm,
    "Modify memory \r\t adr width value\n"

);

/*----------------------------memCmd.c--------------------------------*/
