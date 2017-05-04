#include <avr/wdt.h> // watchdog
#include <avr/pgmspace.h>

#define pocet_cidel 8
float HODNOTY_PORTU[pocet_cidel];   // datové pole hodnot
float NTC_temp = 99;
const int addrT[151] = {-40,-39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110 };
const float addrV[151] = {190.5562,183.4132,175.6740,167.6467,159.5647,151.5975,143.8624,136.4361,129.3641,122.6678,116.3519,110.4098,104.8272,99.5847,94.6608,90.0326,85.6778,81.5747,77.7031,74.0442,70.5811,67.2987,64.1834,61.2233,58.4080,55.7284,53.1766,50.7456,48.4294,46.2224,44.1201,42.1180,40.2121,38.3988,36.6746,35.0362,33.4802,32.0035,30.6028,29.2750,28.0170,26.8255,25.6972,24.6290,23.6176,22.6597,21.7522,20.8916,20.0749,19.2988,18.5600,18.4818,18.1489,17.6316,16.9917,16.2797,15.5350,14.7867,14.0551,13.3536,12.6900,12.0684,11.4900,10.9539,10.4582,10.0000,9.5762,9.1835,8.8186,8.4784,8.1600,7.8608,7.5785,7.3109,7.0564,6.8133,6.5806,6.3570,6.1418,5.9343,5.7340,5.5405,5.3534,5.1725,4.9976,4.8286,4.6652,4.5073,4.3548,4.2075,4.0650,3.9271,3.7936,3.6639,3.5377,3.4146,3.2939,3.1752,3.0579,3.9414,2.8250,2.7762,2.7179,2.6523,2.5817,2.5076,2.4319,2.3557,2.2803,2.2065,2.1350,2.0661,2.0004,1.9378,1.8785,1.8225,1.7696,1.7197,1.6727,1.6282,1.5860,1.5458,1.5075,1.4707,1.4352,1.4006,1.3669,1.3337,1.3009,1.2684,1.2360,1.2037,1.1714,1.1390,1.1067,1.0744,1.0422,1.0104,0.9789,0.9481,0.9480,0.8889,0.8610,0.8346,0.8099,0.7870,0.7665,0.7485,0.7334,0.7214,0.7130 };
float valueFilt; // * LOW PASS FILTER
#define NTC_PULLUP 10 // PULLUP resistance in kOhm 1% or 0.5% !!!

void CTENI_PORTU(){  
    int hodnota = analogRead(A0);
    ZAPIS_HODNOTU(hodnota, 0);
    hodnota = analogRead(A1);
    ZAPIS_HODNOTU(hodnota, 1);
    hodnota = analogRead(A2);
    ZAPIS_HODNOTU(hodnota, 2);
    hodnota = analogRead(A3);
    ZAPIS_HODNOTU(hodnota, 3);
    hodnota = analogRead(A4);
    ZAPIS_HODNOTU(hodnota, 4);
    hodnota = analogRead(A5);
    ZAPIS_HODNOTU(hodnota, 5);
    hodnota = analogRead(A6);
    ZAPIS_HODNOTU(hodnota, 6);
    hodnota = analogRead(A7);
    ZAPIS_HODNOTU(hodnota, 7);
    
    wdt_reset(); // watchdog OK
}

void ZAPIS_HODNOTU(int hodnota, byte port){  // FILTR A ZAPIS HODNOT POLE
  HODNOTY_PORTU[port] = (1-0.998)*hodnota + 0.998*HODNOTY_PORTU[port];
}

void PREVOD_TEPLOTA(byte port) {  // převod filtrované syrové hodnoty na teplotu
  NTC_temp = getTemp(HODNOTY_PORTU[port]);
}

float getR(float V){    // teploměr
  float result;
  result = -NTC_PULLUP * V / (V - 1024);
  return result;
}
float getTemp(float V){ // teploměr
  float R;
  R = getR(V);
  int i = 0;
  while (addrV[i] > R)
  {
    i++;
  }
  float result;
  result = (R - addrV[i]) * (addrT[i - 1] - addrT[i]) / (addrV[i - 1] - addrV[i]) + addrT[i];
  return result;
}


int freeRam() { // zobrazení volného místa v RAM
  // příklad: Serial.print(freeRam());
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


void setup() {
  wdt_enable(WDTO_8S); //interní watchdog 8 vteřin - hodnoty WDTO_15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S
  Serial.begin(9600);
  analogReference(DEFAULT);    // DEFAULT, INTERNAL, (INTERNAL1V1, INTERNAL2V56 pro MEGA 12xx) , or EXTERNAL
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  delay(10);
  analogRead(A0);
}

void loop() {
  wdt_reset(); // watchdog OK

  for (unsigned int i = 0; i < 1023; i++) {
    CTENI_PORTU();
  }

  for (byte i = 0; i < pocet_cidel; i++) {
    Serial.print(i);
    Serial.print(F(" : "));
    Serial.println(getTemp(HODNOTY_PORTU[i]));
  }
  //Serial.println(freeRam()); // volná paměť RAM
}
