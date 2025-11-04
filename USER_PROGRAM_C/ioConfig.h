#ifndef  IOCONFIG_H
#define  IOCONFIG_H
#include <bs86d12c.h>
#include <stdint.h>

// ===== Relay A: PC0 =====
#define RELAYA_MODE      _pcc0
#define RELAYA_DATA      _pc0
#define RELAYA_OUTPUT()  (RELAYA_MODE = 0)
#define RELAYA_ON()      (RELAYA_DATA = 1)   // active-HIGH
#define RELAYA_OFF()     (RELAYA_DATA = 0)

// ===== Relay D: PC1 =====
#define RELAYD_MODE      _pcc1
#define RELAYD_DATA      _pc1
#define RELAYD_OUTPUT()  (RELAYD_MODE = 0)
#define RELAYD_ON()      (RELAYD_DATA = 1)
#define RELAYD_OFF()     (RELAYD_DATA = 0)

// ===== Triac B: PC2 =====
#define TRIACB_MODE      _pcc2
#define TRIACB_DATA      _pc2
#define TRIACB_OUTPUT()  (TRIACB_MODE = 0)
#define TRIACB_ON()      (TRIACB_DATA = 1)
#define TRIACB_OFF()     (TRIACB_DATA = 0)

// ===== Triac C: PC3 =====
#define TRIACC_MODE      _pcc3
#define TRIACC_DATA      _pc3
#define TRIACC_OUTPUT()  (TRIACC_MODE = 0)
#define TRIACC_ON()      (TRIACC_DATA = 1)
#define TRIACC_OFF()     (TRIACC_DATA = 0)

// ===== Buzzer: PD5 =====
#define BUZZ_MODE        _pdc5
#define BUZZ_DATA        _pd5
#define BUZZ_OUTPUT()    (BUZZ_MODE = 0)
#define BUZZ_ON()        (BUZZ_DATA = 1)
#define BUZZ_OFF()       (BUZZ_DATA = 0)

// ===== GRID11: PA4  =====
#define GRID_MODE        _pac4
#define GRID_DATA        _pa4
#define GRID_OUTPUT()    (GRID_MODE = 0)
#define GRID_ON()        (GRID_DATA = 1)
#define GRID_OFF()       (GRID_DATA = 0)

// ===== DIN TM1640: PD3  =====
#define DIN_MODE         _pdc3
#define DIN_DATA         _pd3
#define DIN_OUTPUT()     (DIN_MODE = 0)
#define DIN_HIGH()       (DIN_DATA = 1)
#define DIN_LOW()        (DIN_DATA = 0)

// ===== CLK TM1640: PD2  =====
#define CLK_MODE         _pdc2  
#define CLK_DATA         _pd2
#define CLK_OUTPUT()     (CLK_MODE = 0)
#define CLK_HIGH()       (CLK_DATA = 1)
#define CLK_LOW()        (CLK_DATA = 0)


// ===== Function Prototypes =====
void io_init_outputs(void);
void touch_init(void);
uint16_t touch_read_key_M0(uint8_t key_idx_1_to_4);
uint16_t touch_read_key_M1(uint8_t key_idx_5_to_8);
#endif  // IOCONFIG_H