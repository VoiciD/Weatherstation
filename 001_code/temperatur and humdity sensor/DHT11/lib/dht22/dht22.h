#ifndef DHT22_H
#define DHT22_H

#include <stdbool.h>
#include <stdint.h>


/*
 * Liest einen DHT22 aus.
 *
 * gpio          GPIO-Nummer, z. B. 28 für GP28
 * temperature_c Zielvariable für Temperatur in °C
 * humidity_rh   Zielvariable für relative Feuchte in %RH
 *
 * Rückgabe:
 * true  -> Daten erfolgreich gelesen, Checksum stimmt
 * false -> Antwort oder Checksum fehlerhaft
 */
bool dht22_get_data(uint gpio, float *temperature_c, float *humidity_rh);

#endif