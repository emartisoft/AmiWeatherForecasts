#ifndef WEATHER_H
#define WEATHER_H

extern int iconIndex(char *iconstr);
extern char* getWeatherData(char *fname);
extern char* getURL(char* APIKEY, char* location, char* units, char* lang);
extern char* temperatureWithUnit(char* data, char* unit);          

#endif
