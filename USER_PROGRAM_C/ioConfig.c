#include "ioConfig.h"

void io_init_outputs(void) {
    RELAYA_OUTPUT(); RELAYA_OFF();
    RELAYD_OUTPUT(); RELAYD_OFF();
    TRIACB_OUTPUT(); TRIACB_OFF();
    TRIACC_OUTPUT(); TRIACC_OFF();
    BUZZ_OUTPUT();   BUZZ_OFF();
    // GRID_OUTPUT();   GRID_OFF();  
    DIN_OUTPUT();    DIN_LOW();
    CLK_OUTPUT();    CLK_LOW();
}

void touch_init(void)
{
    _tkc1 = (_tkc1 & ~0x03) | 0x01;      // TKFS1..0 = 01 => ~3 MHz 
    _tkc0 = (_tkc0 & ~0x03) | 0x02;      // TK16S1..0 = 10 => fSYS/4 
    // M0 (KEY1..KEY4 trên PB0..PB3)
    _tkm0c1 = (1<<3) /*MnKOEN*/ | (1<<2) /*MnROEN*/ 
            | (1<<0) /*MnK1IO KEY1*/ | (1<<1) /*MnK2IO KEY2*/ | (1<<2) /*MnK3IO KEY3*/ | (1<<3) /*MnK4IO KEY4*/;

    // M1 (KEY5..KEY8 trên PB4..PB7)
    _tkm1c1 = (1<<3) /*MnKOEN*/ | (1<<2) /*MnROEN*/
            | (1<<0) /*K1IO KEY5*/ | (1<<1) /*K2IO KEY6*/ | (1<<2) /*K3IO KEY7*/ | (1<<3) /*K4IO KEY8*/;

    /*This register pair is used to store the touch key module n reference oscillator capacitor value.
    The reference oscillator internal capacitor value=(TKMnRO[9:0]×50pF)/1024*/
    _tkm0rol = 0x20; _tkm0roh = 0x00;    // ~ (0x20 * 50pF)/1024 ≈ 0.98nF
    _tkm1rol = 0x20; _tkm1roh = 0x00;

    _tkc0 &= ~(1<<5);    // TKST=0 clear counters
    _tkc0 |=  (1<<5);    // TKST=1 start scanning
}

/// @brief  Đọc giá trị C/F counter của touch key module M0, M1 (KEY1..KEY8)
/// @param key_idx_1_to_8 (1..8)
/// @return  Giá trị C/F counter (0..65535)
uint16_t touch_read_key(uint8_t key_idx_1_to_8)
{
    uint8_t k = (key_idx_1_to_8 - 1) & 0x07;
    uint8_t sel = k & 0x03; // 0..3 trong module
    if(k<4)
    {
        // Module 0: KEY1..4
        _tkm0c0 = (_tkm0c0 & ~0xC0) | (sel << 6);   // MnMXS1..0 ở bit 7..6
    }
    else
    {
        _tkm1c0 = (_tkm1c0 & ~0xC0) | (sel << 6);   // MnMXS1..0 ở bit 7..6
    }
    _tkrcov &= ~(1<<6);  // TKRCOV CLR
    _tkst |= (1<<5);    // TKST SET
    while (!(_tkc0 & (1<<6))) { /* wait TKRCOV=1 */ }  // TKRCOV set khi tràn time slot
    _tkc0 &= ~(1<<6);

    if(k<4)
    {
        return ((uint16_t)_tkm016dh << 8) | _tkm016dl; 
    }
    else
    {
        return ((uint16_t)_tkm116dh << 8) | _tkm116dl; 
    }

}

/// @brief  Đọc giá trị C/F counter của touch key module M1 (KEY5..KEY8)
/// @param key_idx_5_to_8 (5..8)
/// @return  Giá trị C/F counter (0..65535)
uint16_t touch_read_key_M1(uint8_t key_idx_5_to_8)
{
    // Map KEY5..8 -> MnMXS = 00,01,10,11 theo bảng
    uint8_t sel = (key_idx_5_to_8 - 1) & 0x03;
    _tkm1c0 = (_tkm1c0 & ~0xC0) | (sel << 6);   // MnMXS1..0 ở bit 7..6

    while (!(_tkc0 & (1<<6))) { /* wait TKRCOV=1 */ }  // TKRCOV set khi tràn time slot
    _tkc0 &= ~(1<<6);  

    uint16_t v = ((uint16_t)_tkm116dh << 8) | _tkm116dl; // C/F counter của module M1
    return v;
}



#define N_KEYS         8
#define CALI_SAMPLES   16
#define TH_PCT         10  
#define DEBOUNCE_N     2     // cần 2 mẫu liên tiếp để xác nhận

static unsigned int baseline[N_KEYS];
static unsigned char pressed[N_KEYS];   // 0/1
static unsigned char stable_cnt[N_KEYS];

void touch_calibrate_idle(void)
{
	uint8_t i,n,k;
    for (i = 0;i<N_KEYS;i++) baseline[i]=0;
    for (n =0 ;n<CALI_SAMPLES;n++) {
        for (k=1;k<=N_KEYS;k++) {
            baseline[k-1] += touch_read_key(k);
        }
    }
    
    for (k=1;k<=N_KEYS;k++) baseline[k-1] /= CALI_SAMPLES;
}

static inline unsigned int absu(int x){ return x<0 ? -x : x; }

void touch_scan_update(void)
{
	uint8_t k=1;
    for (k = 1;k<=N_KEYS;k++) {
        unsigned int cur = touch_read_key(k);
        int delta = (int)cur - (int)baseline[k-1];

        unsigned int adelta = absu(delta);
        unsigned int th = (baseline[k-1] * TH_PCT) / 100;
        unsigned char now_on = (adelta > th);

        if (now_on == pressed[k-1]) {
            stable_cnt[k-1] = 0;         
        } else {
            if (++stable_cnt[k-1] >= DEBOUNCE_N) {
                pressed[k-1] = now_on;     
                stable_cnt[k-1] = 0;
            }
        }
    }
}

// API check key state
unsigned char key_is_pressed(uint8_t key1_to_8)
{
    return pressed[(key1_to_8-1)&0x07];
}
