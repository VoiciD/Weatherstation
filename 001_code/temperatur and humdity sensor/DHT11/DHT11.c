#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "lib/dht22/dht22.h"
#include "hardware/pwm.h"



#define DHT22_pin 28
#define led_pin 5

#define diff_bit_time 50 // zero ~ 28us one ~70us -> 50us


#define DHT_TIMEOUT_US 120


int pin_pwm( int gpio, float level){        //PWM function to set a desired level (level) (between 0 and 1) for a pin (gpio). Clock frequency, divider and period can not be altered in this function. Use pin_pwm_adv for this.
    gpio_init(gpio);
    gpio_set_dir(gpio,GPIO_OUT);
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    //determine slice and channel to which the gpio is connected
    int slice = pwm_gpio_to_slice_num(gpio); 
    int channel = pwm_gpio_to_channel(gpio);
    int frequency = 125e6;                       //desired clockfrequency. Needed to calculate the clockdivider
    int cycles = frequency/100e3;                //number of cycles in the PWM signal (Resolution). The clock frequency is divided by the pwm frequency (100 kHz). This is number, the free count will count up to.  
    pwm_set_clkdiv(slice,(125e6/frequency));     //Set clock divider. In this case 1
    pwm_set_wrap(slice,cycles);                  //set the count number (cycles) for the counter. This will result in a 100kHz PWM-Signal.
    pwm_set_chan_level(slice,channel,(cycles*level)); //set the gpio to the desired level. The function will set the gpio high for given number of cycles which match the desired output level.
    pwm_set_enabled(slice,true);
}





int main(void){

    gpio_init(6);
    gpio_init(led_pin);
    gpio_set_dir(6,GPIO_OUT);

    
    stdio_init_all();

    float temperature;
    float humidity;

while (true) {

    if (dht22_get_data(DHT22_pin, &temperature, &humidity)) {
        printf("T = %.1f °C, RH = %.1f %%\n",
               temperature, humidity);
    } 
    else {
        printf("DHT22: Error while reading data\n");
    }

    float duty_cycle = 0.5;
    pin_pwm(led_pin,duty_cycle);
    sleep_ms(1000);
    duty_cycle = 0.9;
    pin_pwm(led_pin,duty_cycle);
    sleep_ms(1000);

}
}