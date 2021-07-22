
/**********
* DEFINES *
**********/
#define LIS2DW12_ADDRESS                  0x18

#define LIS2DW12_OUT_T_L                  0x0D
#define LIS2DW12_OUT_T_H                  0x0E
#define LIS2DW12_WHO_AM_I                 0x0F
#define LIS2DW12_CTRL1                    0x20
#define LIS2DW12_CTRL2                    0x21
#define LIS2DW12_CTRL3                    0x22
#define LIS2DW12_CTRL4_INT1_PAD_CTRL      0x23
#define LIS2DW12_CTRL5_INT2_PAD_CTRL      0x24
#define LIS2DW12_CTRL6                    0x25
#define LIS2DW12_OUT_T                    0x26
#define LIS2DW12_STATUS                   0x27
#define LIS2DW12_OUT_X_L                  0x28
#define LIS2DW12_OUT_X_H                  0x29
#define LIS2DW12_OUT_Y_L                  0x2A
#define LIS2DW12_OUT_Y_H                  0x2B
#define LIS2DW12_OUT_Z_L                  0x2C
#define LIS2DW12_OUT_Z_H                  0x2D
#define LIS2DW12_FIFO_CTRL                0x2E
#define LIS2DW12_FIFO_SAMPLES             0x2F
#define LIS2DW12_TAP_THS_X                0x30
#define LIS2DW12_TAP_THS_Y                0x31
#define LIS2DW12_TAP_THS_Z                0x32
#define LIS2DW12_INT_DUR                  0x33
#define LIS2DW12_WAKE_UP_THS              0x34
#define LIS2DW12_WAKE_UP_DUR              0x35
#define LIS2DW12_FREE_FALL                0x36
#define LIS2DW12_STATUS_DUP               0x37
#define LIS2DW12_WAKE_UP_SRC              0x38
#define LIS2DW12_TAP_SRC                  0x39
#define LIS2DW12_SIXD_SRC                 0x3A
#define LIS2DW12_ALL_INT_SRC              0x3B
#define LIS2DW12_X_OFS_USR                0x3C
#define LIS2DW12_Y_OFS_USR                0x3D
#define LIS2DW12_Z_OFS_USR                0x3E
#define LIS2DW12_CTRL_REG7                0x3F

bool xl_whoAmI(void);
void xl_initialize(void);
int16_t xl_readXAxis(void);
int16_t xl_readYAxis(void);
int16_t xl_readZAxis(void);
