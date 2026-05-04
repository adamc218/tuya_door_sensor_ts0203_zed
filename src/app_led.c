#include "app_main.h"

static app_light_t light = {
    .timerLedEvt = NULL,
    .oriSta = false,
};

bool led_status() {
    return light.ledStatus;
}

void led_on(uint32_t pin)
{
    if (device->device_en) {
        drv_gpio_write(pin, LED_ON(device->led_on));
        light.ledStatus = true;
    }
}

void led_off(uint32_t pin)
{
    if (device->device_en) {
        drv_gpio_write(pin, LED_OFF(device->led_off));
        light.ledStatus = false;
    }
}

void light_on(void)
{
    if (device->device_en) led_on(device->led_gpio.gpio);
}

void light_off(void)
{
    if (device->device_en) led_off(device->led_gpio.gpio);
}

void light_init(void)
{
    if (device->device_en) led_off(device->led_gpio.gpio);

}

int32_t zclLightTimerCb(void *arg)
{
    uint32_t interval = 0;

    if(light.sta == light.oriSta){
        light.times--;
        if(light.times <= 0){
            light.timerLedEvt = NULL;
            return -1;
        }
    }

    light.sta = !light.sta;
    if(light.sta){
        light_on();
        interval = light.ledOnTime;
    }else{
        light_off();
        interval = light.ledOffTime;
    }

    return interval;
}

void light_blink_start(uint8_t times, uint16_t ledOnTime, uint16_t ledOffTime)
{
    uint32_t interval = 0;
    light.times = times;

    if(!light.timerLedEvt){
        if(light.oriSta){
            light_off();
            light.sta = 0;
            interval = ledOffTime;
        }else{
            light_on();
            light.sta = 1;
            interval = ledOnTime;
        }
        light.ledOnTime = ledOnTime;
        light.ledOffTime = ledOffTime;

        light.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
    }
}

void light_blink_stop(void)
{
    if(light.timerLedEvt){
        TL_ZB_TIMER_CANCEL(&light.timerLedEvt);

        light.times = 0;
        if(light.oriSta){
            light_on();
        }else{
            light_off();
        }
    }
}
