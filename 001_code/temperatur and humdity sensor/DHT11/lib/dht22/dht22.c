#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"




#define diff_bit_time 50 // zero ~ 28us one ~70us -> 50us


#define DHT_TIMEOUT_US 120




int measure_pulse(int gpio, bool level, int64_t *bit_time){
while (gpio_get(gpio) != level) {
        // blockiert hier, bis der Pegel erreicht ist
    }

    absolute_time_t t_start = get_absolute_time();
    //printf("Start time: %ld\n", t_start);
    // Warten, bis der Pegel wieder endet.
    while (gpio_get(gpio) == level) {
        // blockiert hier, solange der Puls aktiv ist
    }

    absolute_time_t t_stop = get_absolute_time();
    //printf("Stop time: %ld\n", t_stop);

    *bit_time = absolute_time_diff_us(t_start, t_stop);

    return true;
}



bool dht22_get_data(uint gpio, float *temperature_c, float *humidity_rh){
    uint8_t data[5] = {0};

    int64_t ack_low_us;
    int64_t ack_high_us;

    gpio_init(gpio);

    // Pico erzeugt das Startsignal.
    gpio_set_dir(gpio, GPIO_OUT);
    gpio_put(gpio, 0);
    sleep_ms(2);

    // Leitung loslassen. Externer Pull-up nach 3V3 vorausgesetzt.
    gpio_set_dir(gpio, GPIO_IN);
    gpio_disable_pulls(gpio);
    sleep_us(30);

    // DHT22-Antwort erfassen.
    measure_pulse(gpio, 0, &ack_low_us);
    measure_pulse(gpio, 1, &ack_high_us);

    if (ack_low_us < 60 || ack_low_us > 100 ||
        ack_high_us < 60 || ack_high_us > 100) {
        return false;
    }

    // Nach ACK-HIGH folgt direkt der LOW-Anteil des ersten Datenbits.
    for (uint8_t byte_count = 0; byte_count < 5; ++byte_count) {
        for (uint8_t bit_count = 0; bit_count < 8; ++bit_count) {
            int64_t low_us;
            int64_t high_us;

            measure_pulse(gpio, 0, &low_us);
            measure_pulse(gpio, 1, &high_us);

            data[byte_count] <<= 1;

            if (high_us > 50) {
                data[byte_count] |= 1;
            }
        }
    }

    uint8_t checksum = (uint8_t)(
        data[0] + data[1] + data[2] + data[3]
    );

    if (checksum != data[4]) {
        return false;
    }

    uint16_t humidity_raw = ((uint16_t)data[0] << 8) | data[1];
    uint16_t temperature_raw = ((uint16_t)(data[2] & 0x7F) << 8) | data[3];

    *humidity_rh = humidity_raw / 10.0f;
    *temperature_c = temperature_raw / 10.0f;

    if (data[2] & 0x80) {
        *temperature_c = -*temperature_c;
    }

    return true;
}