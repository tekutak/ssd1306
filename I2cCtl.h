#ifndef _I2C_CTL_H_
#define _I2C_CTL_H_
/* -----------------------------------------------------------------------------
 Include
------------------------------------------------------------------------------*/
#include "common.h"

/* -----------------------------------------------------------------------------
 Extern
------------------------------------------------------------------------------*/
extern BOOL I2cCtl_Init();
extern BOOL I2cCtl_Write(U08 dev_adr, void *buf, U32 buf_length);
extern BOOL I2cCtl_Read(U08 dev_adr, void *buf, U32 buf_length);

#endif /* _I2C_CTL_H_ */