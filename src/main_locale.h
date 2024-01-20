#ifndef MAIN_LOCALE
#define MAIN_LOCALE

struct LocalString {
  struct LocalString *Next;
  int CatalogID;
  char Str[1];
};

struct {void *Previous;int ID;char Str[(10)+1];} LS0 = {0,0,"About"
};
struct {void *Previous;int ID;char Str[(8)+1];} LS1 = {&LS0,1,"Quit"
};
struct {void *Previous;int ID;char Str[(46)+1];} LS2 = {&LS1,2,"Update Weather Forecast"
};
struct {void *Previous;int ID;char Str[(52)+1];} LS3 = {&LS2,3,"Set Date and Time Manually"
};
struct {void *Previous;int ID;char Str[(54)+1];} LS4 = {&LS3,4,"Sync System Time using SNTP"
};
struct {void *Previous;int ID;char Str[(12)+1];} LS8 = {&LS4,8,"Manual"
};
struct {void *Previous;int ID;char Str[(2)+1];} LS9 = {&LS8,9,"A"
};
struct {void *Previous;int ID;char Str[(2)+1];} LS10 = {&LS9,10,"Q"
};
struct {void *Previous;int ID;char Str[(10)+1];} LS11 = {&LS10,11,"Tools"
};
struct {void *Previous;int ID;char Str[(2)+1];} LS12 = {&LS11,12,"U"
};
struct {void *Previous;int ID;char Str[(8)+1];} LS15 = {&LS12,15,"Help"
};
struct {void *Previous;int ID;char Str[(2)+1];} LS16 = {&LS15,16,"M"
};
struct {void *Previous;int ID;char Str[(4)+1];} LS17 = {&LS16,17,"Ok"
};
struct {void *Previous;int ID;char Str[(32)+1];} LS18 = {&LS17,18,"About"
};
struct {void *Previous;int ID;char Str[(58)+1];} LS19 = {&LS18,19,"Warning - AmiWeatherForecasts"
};
struct {void *Previous;int ID;char Str[(200)+1];} LS20 = {&LS19,20,"SNTP not found in SYS:C drawer. Download it from Aminet.net."
"\nSystem time could not be synchronized."
};
struct {void *Previous;int ID;char Str[(52)+1];} LS22 = {&LS20,22,"Info - AmiWeatherForecasts"
};
struct {void *Previous;int ID;char Str[(26)+1];} LS26 = {&LS22,26,"Days & Months"
};
struct {void *Previous;int ID;char Str[(14)+1];} LS27 = {&LS26,27,"Display"
};
struct {void *Previous;int ID;char Str[(12)+1];} LS28 = {&LS27,28,"Colors"
};
struct {void *Previous;int ID;char Str[(32)+1];} LS29 = {&LS28,29,"Position & Style"
};
struct {void *Previous;int ID;char Str[(26)+1];} LS30 = {&LS29,30,"ENV Variables"
};
struct {void *Previous;int ID;char Str[(14)+1];} LS31 = {&LS30,31,"API Key"
};
struct {void *Previous;int ID;char Str[(66)+1];} LS32 = {&LS31,32,"Preferences - AmiWeatherForecasts"
};
struct {void *Previous;int ID;char Str[(32)+1];} LS33 = {&LS32,33,"_Get My Location"
};
struct {void *Previous;int ID;char Str[(22)+1];} LS34 = {&LS33,34,"  Location "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS35 = {&LS34,35,"      Unit "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS36 = {&LS35,36,"  Language "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS37 = {&LS36,37,"      Days "
};
struct {void *Previous;int ID;char Str[(18)+1];} LS38 = {&LS37,38,"Get _Days"
};
struct {void *Previous;int ID;char Str[(72)+1];} LS39 = {&LS38,39,"Use long abbreviations for days     "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS40 = {&LS39,40,"    Months "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS41 = {&LS40,41,"Get _Months"
};
struct {void *Previous;int ID;char Str[(72)+1];} LS42 = {&LS41,42,"Use months of the year abbreviations"
};
struct {void *Previous;int ID;char Str[(48)+1];} LS43 = {&LS42,43," Position Y of the text "
};
struct {void *Previous;int ID;char Str[(28)+1];} LS44 = {&LS43,44," Set Position "
};
struct {void *Previous;int ID;char Str[(40)+1];} LS45 = {&LS44,45,"Displays icon       "
};
struct {void *Previous;int ID;char Str[(40)+1];} LS46 = {&LS45,46,"Displays location   "
};
struct {void *Previous;int ID;char Str[(40)+1];} LS47 = {&LS46,47,"Displays description"
};
struct {void *Previous;int ID;char Str[(40)+1];} LS48 = {&LS47,48,"Displays date & time"
};
struct {void *Previous;int ID;char Str[(94)+1];} LS49 = {&LS48,49,"If desired to change colors, use Prefs/Palette."
};
struct {void *Previous;int ID;char Str[(24)+1];} LS50 = {&LS49,50,"       Note "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS51 = {&LS50,51,"       Text "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS52 = {&LS51,52," Background "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS53 = {&LS52,53,"        Sun "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS54 = {&LS53,54,"      Cloud "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS55 = {&LS54,55,"Light Cloud "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS56 = {&LS55,56," Dark Cloud "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS57 = {&LS56,57,"      Style "
};
struct {void *Previous;int ID;char Str[(24)+1];} LS58 = {&LS57,58,"     Shadow "
};
struct {void *Previous;int ID;char Str[(92)+1];} LS59 = {&LS58,59,"Select the window and drag to the new position"
};
struct {void *Previous;int ID;char Str[(20)+1];} LS60 = {&LS59,60," Position "
};
struct {void *Previous;int ID;char Str[(20)+1];} LS61 = {&LS60,61,"    Style "
};
struct {void *Previous;int ID;char Str[(94)+1];} LS62 = {&LS61,62,"Environment variables for weather forecast data"
};
struct {void *Previous;int ID;char Str[(64)+1];} LS63 = {&LS62,63,"Use your own OpenWeather API key"
};
struct {void *Previous;int ID;char Str[(22)+1];} LS64 = {&LS63,64,"      Note "
};
struct {void *Previous;int ID;char Str[(22)+1];} LS65 = {&LS64,65,"    APIKEY "
};
struct {void *Previous;int ID;char Str[(14)+1];} LS66 = {&LS65,66,"_Close"
};
struct {void *Previous;int ID;char Str[(26)+1];} LS67 = {&LS66,67,"_Save"
};
struct {void *Previous;int ID;char Str[(140)+1];} LS68 = {&LS67,68,"There must be 6 comma characters between the days.\nPlease, "
"try again."
};
struct {void *Previous;int ID;char Str[(146)+1];} LS69 = {&LS68,69,"There must be 11 comma characters between the months.\nPleas"
"e, try again."
};
struct {void *Previous;int ID;char Str[(146)+1];} LS70 = {&LS69,70,"Except Style and Shadow, Your color choices must be different from each other.\nPleas"
"e, try again."
};
struct {void *Previous;int ID;char Str[(128)+1];} LS71 = {&LS70,71,"GetExtIP not found in SYS:C drawer. Download it from Aminet."
"net."
};
struct {void *Previous;int ID;char Str[(22)+1];} LS72 = {&LS71,72,"On the hour"};
struct {void *Previous;int ID;char Str[(32)+1];} LS73 = {&LS72,73,"On the half hour"};
struct {void *Previous;int ID;char Str[(38)+1];} LS74 = {&LS73,74,"On the quarter hour"};
struct {void *Previous;int ID;char Str[(12)+1];} LS78 = {&LS74,78,"Normal"};
struct {void *Previous;int ID;char Str[(16)+1];} LS79 = {&LS78,79,"Announce"};
struct {void *Previous;int ID;char Str[(112)+1];} LS80 = {&LS79,80,"This app is used amiga-port of espeak speech synthesizer"};
struct {void *Previous;int ID;char Str[(26)+1];} LS81 = {&LS80,81,"        Info "};
struct {void *Previous;int ID;char Str[(58)+1];} LS82 = {&LS81,82,"Select espeak application ..."};
struct {void *Previous;int ID;char Str[(26)+1];} LS83 = {&LS82,83,"  espeak App "};
struct {void *Previous;int ID;char Str[(10)+1];} LS84 = {&LS83,84,"Test"};
struct {void *Previous;int ID;char Str[(148)+1];} LS85 = {&LS84,85,"Announce the time (available for turkish, german, french and english only)"};
struct {void *Previous;int ID;char Str[(118)+1];} LS86 = {&LS85,86,"Announce the weather forecast in the following text format"};
struct {void *Previous;int ID;char Str[(26)+1];} LS87 = {&LS86,87," Text Format "};
struct {void *Previous;int ID;char Str[(26)+1];} LS88 = {&LS87,88,"      Voices "};
struct {void *Previous;int ID;char Str[(26)+1];} LS89 = {&LS88,89,"       Speed "};
struct {void *Previous;int ID;char Str[(26)+1];} LS90 = {&LS89,90,"Announce Time"};
struct {void *Previous;int ID;char Str[(80)+1];} LS91 = {&LS90,91,"Run automatically at startup"};
struct {void *Previous;int ID;char Str[(34)+1];} LS92 = {&LS91,92,"_Restore Defaults"};
struct {void *Previous;int ID;char Str[(26)+1];} LS93 = {&LS92,93,"Are you sure?"};
struct {void *Previous;int ID;char Str[(66)+1];} LS94 = {&LS93,94,"Are you sure to restore defaults?"};
struct {void *Previous;int ID;char Str[(14)+1];} LS95 = {&LS94,95,"Yes|No"};
struct {void *Previous;int ID;char Str[(14)+1];} LS96 = {&LS95,96,"Error"};
struct {void *Previous;int ID;char Str[(44)+1];} LS97 = {&LS96,97,"eSpeak app not found"};
struct {void *Previous;int ID;char Str[(18)+1];} LS100 = {&LS97,100,"Male 1"};
struct {void *Previous;int ID;char Str[(18)+1];} LS101 = {&LS100,101,"Male 2"};
struct {void *Previous;int ID;char Str[(18)+1];} LS102 = {&LS101,102,"Male 3"};
struct {void *Previous;int ID;char Str[(18)+1];} LS103 = {&LS102,103,"Male 4"};
struct {void *Previous;int ID;char Str[(18)+1];} LS104 = {&LS103,104,"Male 5"};
struct {void *Previous;int ID;char Str[(18)+1];} LS105 = {&LS104,105,"Male 6"};
struct {void *Previous;int ID;char Str[(18)+1];} LS106 = {&LS105,106,"Male 7"};
struct {void *Previous;int ID;char Str[(20)+1];} LS107 = {&LS106,107,"Female 1"};
struct {void *Previous;int ID;char Str[(20)+1];} LS108 = {&LS107,108,"Female 2"};
struct {void *Previous;int ID;char Str[(20)+1];} LS109 = {&LS108,109,"Female 3"};
struct {void *Previous;int ID;char Str[(20)+1];} LS110 = {&LS109,110,"Female 4"};
struct {void *Previous;int ID;char Str[(20)+1];} LS111 = {&LS110,111,"Female 5"};
struct {void *Previous;int ID;char Str[(52)+1];} LS112 = {&LS111,112,"Whisper + Male"};
struct {void *Previous;int ID;char Str[(52)+1];} LS113 = {&LS112,113,"Whisper + Female"};

#define STR_BEGIN_ENTRY &LS113

#define str_29 LS0.Str
#define str_30 LS1.Str
#define str_31 LS2.Str
#define str_32 LS3.Str
#define str_33 LS4.Str
#define str_37 LS8.Str
#define str_39 LS9.Str
#define str_40 LS10.Str
#define str_41 LS11.Str
#define str_42 LS12.Str
#define str_45 LS15.Str
#define str_46 LS16.Str
#define str_48 LS17.Str
#define str_77 LS18.Str
#define str_78 LS19.Str
#define str_79 LS20.Str
#define str_82 LS22.Str
#define str_93 LS26.Str
#define str_94 LS27.Str
#define str_95 LS28.Str
#define str_96 LS29.Str
#define str_97 LS30.Str
#define str_98 LS31.Str
#define str_99 LS32.Str
#define str_100 LS33.Str
#define str_101 LS34.Str
#define str_102 LS35.Str
#define str_103 LS36.Str
#define str_104 LS37.Str
#define str_105 LS38.Str
#define str_106 LS39.Str
#define str_108 LS40.Str
#define str_109 LS41.Str
#define str_110 LS42.Str
#define str_111 LS43.Str
#define str_112 LS44.Str
#define str_113 LS45.Str
#define str_114 LS46.Str
#define str_115 LS47.Str
#define str_116 LS48.Str
#define str_117 LS49.Str
#define str_118 LS50.Str
#define str_119 LS51.Str
#define str_120 LS52.Str
#define str_121 LS53.Str
#define str_122 LS54.Str
#define str_123 LS55.Str
#define str_124 LS56.Str
#define str_125 LS57.Str
#define str_126 LS58.Str
#define str_127 LS59.Str
#define str_128 LS60.Str
#define str_129 LS61.Str
#define str_130 LS62.Str
#define str_134 LS63.Str
#define str_135 LS64.Str
#define str_136 LS65.Str
#define str_137 LS66.Str
#define str_138 LS67.Str
#define str_139 LS68.Str
#define str_140 LS69.Str
#define str_141 LS70.Str
#define str_145 LS71.Str
#define str_200 LS72.Str
#define str_201 LS73.Str
#define str_202 LS74.Str
#define str_206 LS78.Str
#define str_207 LS79.Str
#define str_208 LS80.Str
#define str_209 LS81.Str
#define str_210 LS82.Str
#define str_211 LS83.Str
#define str_212 LS84.Str
#define str_213 LS85.Str
#define str_214 LS86.Str
#define str_215 LS87.Str
#define str_216 LS88.Str
#define str_217 LS89.Str
#define str_218 LS90.Str
#define str_219 LS91.Str
#define str_220 LS92.Str
#define str_221 LS93.Str
#define str_222 LS94.Str
#define str_223 LS95.Str
#define str_224 LS96.Str
#define str_225 LS97.Str
#define str_300 LS100.Str
#define str_301 LS101.Str
#define str_302 LS102.Str
#define str_303 LS103.Str
#define str_304 LS104.Str
#define str_305 LS105.Str
#define str_306 LS106.Str
#define str_307 LS107.Str
#define str_308 LS108.Str
#define str_309 LS109.Str
#define str_310 LS110.Str
#define str_311 LS111.Str
#define str_312 LS112.Str
#define str_313 LS113.Str


//struct Library *LocaleBase;
void InitStrings(void)
{
   APTR catalog;
   struct LocalString *lstr=(struct LocalString *)STR_BEGIN_ENTRY;

   //if (LocaleBase = OpenLibrary("locale.library",38)) {
      catalog  = OpenCatalog(NULL,"AmiWeatherForecasts.catalog",
                             OC_BuiltInLanguage,"english",
                             OC_Version,1,
                             TAG_DONE);

      while(lstr) {
         char *str;
         str=GetCatalogStr(catalog, lstr->CatalogID,NULL);
         if(str) strcpy(lstr->Str,str);
         lstr=lstr->Next;
      }
      CloseCatalog(catalog);
      //CloseLibrary(LocaleBase);
   //}
}

#endif
