#ifndef VERSION_H
#define VERSION_H
// version
#define MAJOR   "1"
#define MINOR   "2"
#define BUILD   "0510"
#define DATE    "04.11.2023"
#define CODER   "emarti, Murat OZDEMIR"
#define APPNAME "AmiWeatherForecasts"

#define ABOUT APPNAME" "MAJOR"."MINOR" ("DATE")\nCoded by "CODER"\n\nWebSite:\nhttps://github.com/emartisoft/"APPNAME"\n\nLoves from Turkey!\n\nMIT License"
UBYTE *vers = "\0$VER: "APPNAME" "MAJOR"."MINOR" ("DATE") "CODER"\r\n\0";
#endif
