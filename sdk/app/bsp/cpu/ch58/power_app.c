#include "asm/power_interface.h"
#include "app_config.h"
#include "audio/audio.h"
#include "adc_drv.h"
#include "gpio.h"
#include "vm_api.h"

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[PMU]"
#include "log.h"

/*用于调试低功耗异常时使用，可开启库的调试信息*/
const char debug_timeout = 0;           //打印蓝牙和系统分别可进入低功耗的时间(msec)
const char debug_is_idle = 0;           //打印当前哪些模块处于busy,用于蓝牙已经进入sniff但系统无法进入低功耗的情况，如果usr_timer处于busy则会打印对应的func地址

void sys_softoff()
{
    vm_pre_erase();
    audio_off();
    adc_uninit();
    power_set_soft_poweroff();
}

extern volatile u8 sys_low_power_request;
extern volatile u32 lowpower_usec;

void sys_power_down(u32 usec)
{
    if (sys_low_power_request ==  0) {
        lowpower_usec = usec;
        low_power_sys_request(NULL);
    }
}

void mclr_reset_sw(u8 en)
{
    p33_fast_access(P3_PR_PWR, BIT(3), en);
}

static enum LOW_POWER_LEVEL power_app_level(void)
{
    return LOW_POWER_MODE_SLEEP;
}

static u8 power_app_idle(void)
{
    return 1;
}

REGISTER_LP_TARGET(power_app_lp_target) = {
    .name = "power_app",
    .level = power_app_level,
    .is_idle = power_app_idle,
};
