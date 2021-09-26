/* -----------------------------------------------------------------------------
 Include
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "I2cCtl.h"

/* -----------------------------------------------------------------------------
 Define
------------------------------------------------------------------------------*/
#define I2C_DEVICE ("/dev/i2c-1")
#define FD_INIT_VAL (0xFFFFFFFF)

/* -----------------------------------------------------------------------------
 Global
------------------------------------------------------------------------------*/
static S32 fd = FD_INIT_VAL;

/* -----------------------------------------------------------------------------
 Function   : I2C Init
 Memo       : I2Cデバイスのオープンを行う。プロセス終了するまでオープンしたままにする
 Date       : 2021.08.28
------------------------------------------------------------------------------*/
BOOL I2cCtl_Init()
{
    BOOL status = OK;

    /* デバイスオープン */
    if (fd == (S32)FD_INIT_VAL)
    {
        fd = open(I2C_DEVICE, O_RDWR);
        if (fd < 0)
        {
            perror(I2C_DEVICE);
            return NG;
        }
    }

    return status;
}

/* -----------------------------------------------------------------------------
 Function   : I2C Write
 Memo       : メッセージの先頭に書き込み先レジスタを入れて1メッセージとする
 Date       : 2021.08.28
------------------------------------------------------------------------------*/
BOOL I2cCtl_Write(U08 dev_adr, void *buf, U32 buf_length)
{
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packets;
    S32 ret;
    BOOL status = OK;

    /* メッセージ作成 */
    msg.addr = dev_adr;
    msg.flags = 0;
    msg.len = buf_length;
    msg.buf = buf;

    /* Send */
    packets.msgs = &msg;
    packets.nmsgs = 1; //msgのサイズ
    ret = ioctl(fd, I2C_RDWR, &packets);
    if(ret < 0)
    {
        perror(I2C_DEVICE);
        return NG;
    }

    return status;
}

/* -----------------------------------------------------------------------------
 Function   : I2C Read
 Memo       : メッセージの先頭に書き込み先レジスタを入れて1メッセージとする
 Date       : 2021.08.28
------------------------------------------------------------------------------*/
BOOL I2cCtl_Read(U08 dev_adr, void *buf, U32 buf_length)
{
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data packets;
    S32 ret;
    BOOL status = OK;

     /* メッセージ作成(Data Read) */
    msg.addr = dev_adr;
    msg.flags = I2C_M_RD;
    msg.len = buf_length;
    msg.buf = buf;

    packets.msgs = &msg;
    packets.nmsgs = 1; //msgのサイズ
    ret = ioctl(fd, I2C_RDWR, &packets);
    if(ret < 0)
    {
        perror(I2C_DEVICE);
        return NG;
    }

    return status;
}