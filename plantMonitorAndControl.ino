#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
#include <Wire.h>
#define DS1307_I2C_ADDRESS 0x68

// SENZOR VLAZNOSTI ZEMLJISTA
const byte vlaznost_zemljista = A3;
// MIN I MAX VREDNOSTI KOJE SENZOR VLAZNOSTI ZEMLJISTA IMA
const int map_low = 1000;
const int map_high = 200;

// DEFINISANJE IZLAZA (RELEJI)
const byte svetlo = 5;
const byte fen = 4;
const byte grejac_vazduh = 13;
const byte pumpa_zemlja = 11;
const byte ventilator_ulaz = 10;
const byte ventilator_izlaz = 6;
const byte pumpa_vazduh = 7;
const byte grejac_zemlja = 12;

// SENZOR TEMPERATURE I VLAZNOSTI VAZDUHA
DHT dht(DHTPIN, DHTTYPE);

// SENZOR TEMPERATURE ZEMLJISTA
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// RTC
byte second, minute, hour, dayOfMonth, month, year;
byte decToBcd(byte val)
{
    return ((val / 10 * 16) + (val % 10));
}
byte bcdToDec(byte val)
{
    return ((val / 16 * 10) + (val % 16));
}

// GLOBALNE PROMENJIVE
byte HOUR, MINUTE;
double AIRHUM, AIRTEMP, SOILHUM, SOILTEMP;

void setup()
{
    Serial.begin(9600);
    readRelay();
    relayInitialSet();
    dht.begin();
    Wire.begin();
    sensors.begin();
}

void time()
{
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(decToBcd(0));
    Wire.endTransmission();
    Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
    second = bcdToDec(Wire.read() & 0x7f);
    minute = bcdToDec(Wire.read());
    hour = bcdToDec(Wire.read() & 0x3f);
    // dayOfMonth = bcdToDec(Wire.read());
    // month = bcdToDec(Wire.read());
    // year = bcdToDec(Wire.read());
}

double readDHTHumidity()
{
    return dht.readHumidity();
}

double readDHTTemperature()
{
    return dht.readTemperature();
}

double readSoilTemp()
{
    return sensors.getTempCByIndex(0);
}

int readSoilHumidity()
{
    return map(analogRead(vlaznost_zemljista), map_low, map_high, 0, 100);
}

void readRelay()
{
    // citanje izlaza releja
    pinMode(svetlo, OUTPUT);
    pinMode(fen, OUTPUT);
    pinMode(ventilator_ulaz, OUTPUT);
    pinMode(ventilator_izlaz, OUTPUT);
    pinMode(pumpa_zemlja, OUTPUT);
    pinMode(pumpa_vazduh, OUTPUT);
    pinMode(grejac_zemlja, OUTPUT);
    pinMode(grejac_vazduh, OUTPUT);
}

void relayInitialSet()
{
    digitalWrite(pumpa_vazduh, HIGH);
    digitalWrite(fen, HIGH);
    digitalWrite(ventilator_izlaz, HIGH);
    digitalWrite(ventilator_ulaz, HIGH);
    digitalWrite(pumpa_zemlja, HIGH);
    digitalWrite(svetlo, HIGH);
    digitalWrite(grejac_zemlja, HIGH);
    digitalWrite(grejac_vazduh, HIGH);
}

void turnRelayOutOn(byte n)
{
    digitalWrite(n, LOW);
}

void turnRelayOutOff(byte n)
{
    digitalWrite(n, HIGH);
}

void light(byte hour)
{
    if (hour >= 6 && hour <= 23)
    {
        turnRelayOutOn(svetlo);
        Serial.println("OBAVESTENJE: Svetlo je upaljeno");
    }
    else
    {
        turnRelayOutOff(svetlo);
        Serial.println("OBAVESTENJE: Svetlo je ugaseno");
    }
}

void airHumidity(double value)
{
    if (value > 90)
    {
        turnRelayOutOn(ventilator_ulaz);
        turnRelayOutOn(ventilator_izlaz);
        Serial.println("UPOZORENJE: Vlaznost vazduha je povecana");
    }
    else if (value < 80)
    {
        turnRelayOutOn(ventilator_ulaz);
        turnRelayOutOn(pumpa_vazduh);
        Serial.println("UPOZORENJE: Vlaznost vazduha je smanjena");
    }
    else
    {
        turnRelayOutOff(ventilator_ulaz);
        turnRelayOutOff(ventilator_izlaz);
        Serial.println("OBAVESTENJE: Optimalna vlaznost vazduha");
    }
}

void soilTemperature(double value)
{
    if (value < 25)
    {
        turnRelayOutOn(grejac_zemlja);
        Serial.println("UPOZORENJE: Niska temperatura zemljista");
    }
    else
    {
        turnRelayOutOff(grejac_zemlja);
        Serial.println("OBAVESTENJE: Optimalna temperatura zemljista");
    }
}

void soilHumidity(double value)
{
    if (value < 85)
    {
        turnRelayOutOn(pumpa_zemlja);
        Serial.println("UPOZORENJE: Niska vlaznost zemljista");
    }
    else
    {
        turnRelayOutOff(pumpa_zemlja);
        Serial.println("OBAVESTENJE: Optimalna vlaznost zemljista");
    }
}

void airTemperature(double value)
{
    if (value > 25)
    {
        turnRelayOutOn(ventilator_ulaz);
        turnRelayOutOn(ventilator_izlaz);
        Serial.println("UPOZORENJE: Povecana temperatura vazduha");
    }
    else if (value > 23 && value <= 24)
    {
        turnRelayOutOn(grejac_vazduh);
        Serial.println("UPOZORENJE: Temperatura vazduha ispod optimalne");
    }
    else if (value < 23)
    {
        turnRelayOutOn(fen);
        Serial.println("UPOZORENJE: Smanjena temperatura vazduha");
    }
    else
    {
        turnRelayOutOff(grejac_vazduh);
        turnRelayOutOff(fen);
        turnRelayOutOff(ventilator_ulaz);
        turnRelayOutOff(ventilator_izlaz);
        Serial.println("UPOZORENJE: Optimalna temperatura vazduha");
    }
}

void readAndPrintData()
{
    time();
    HOUR = hour;
    MINUTE = minute;
    AIRHUM = readDHTHumidity();
    AIRTEMP = readDHTTemperature();
    SOILHUM = readSoilHumidity();
    SOILTEMP = readSoilTemp();
    Serial.print("Vreme: ");
    Serial.print(HOUR);
    Serial.print(":");
    Serial.println(MINUTE);
    Serial.print("Vlaznost vazduha: ");
    Serial.println(AIRHUM);
    Serial.print("Temperatura vazduha: ");
    Serial.println(AIRTEMP);
    Serial.print("Vlaznost zemljista: ");
    Serial.println(SOILHUM);
    Serial.print("Temperatura zemljista: ");
    Serial.println(SOILTEMP);
}

void loop()
{
    readAndPrintData();
    light(HOUR);
    airHumidity(AIRHUM);
    soilTemperature(SOILTEMP);
    soilHumidity(SOILHUM);
    airTemperature(AIRTEMP);
}
