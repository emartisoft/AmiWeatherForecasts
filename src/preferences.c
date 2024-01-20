/* 
AmiWFPrefs by emarti, Murat Ozdemir 

v1.6 Jan 2024

*/

#include "includes.h"
#include "defines.h"
#include "main_locale.h"
#include <funcs.h>
#include <httpgetlib.h>
#include "version.h"
#include "common.h"
#include <announce.h>

BPTR fp;
char fpStr[BUFFER/2];

char location[BUFFER/8];
char unit[10];
char lang[16];
char slang[16];
char apikey[BUFFER/16];
char days[BUFFER];
char months[BUFFER];
char displayIcon[2];
char displayLocation[2];
char displayDescription[2];
char displayDateTime[2];
char autostart[2];
int addX, addY, addTextY, voiceSpeed;
ULONG textColor, backgroundColor, style, voice, announcePeriod;
ULONG sunColor, cloudColor, lcloudColor, dcloudColor;
ULONG styleColor, shadowColor;
char espeakPath[BUFFER/2];
char espeakText[BUFFER/2];
char espeakAnTime[2];
char espeakAnText[2];

char **amonths;
char **adays;
char** wdata; 

#define PREFFILEPATH	str_4
void SavePrefs(void);
void LoadPrefs(void);
void createPreferencesWin(void);
void loadDefaults(void);

#define PREFPPALETTE    str_3
BOOL getColorsFromWBPalette(void);
#define COLORCOUNT      8
UWORD colormap[COLORCOUNT];

#define IPTXT       str_7
#define GETEXTIP    str_8
#define GETEXTIPRUN GETEXTIP str_9 IPTXT
#define MYLOCTXT    str_10

char iptxt[15];
char locationCsv[BUFFER/2];
char ipapiurl[64];
char **locationData;

struct Say sy;

enum 
{
    BAR=0,
    ICONIC,
    DIGITAL,
    INVISIBLE
};

#define CLANGCOUNT  14
STRPTR clanguages[] = {
        str_11,
        str_12,
        str_13,
        str_14,
        str_15,
        str_16,
        str_17,
        str_19,
        str_20,
        str_21,
        str_22,
        str_23,
        str_24,
        str_25,
        NULL
};   

STRPTR hi[] = {
    "Hola",
    "Hej",
    "Hi",
    "Hi",
    "Hi",
    "Hei",
    "Salut",
    "CIAO",
    "Hei",
    "Hoi",
    "oi",
    "Hej",
    "Hola",
    "Merhaba",
    NULL
}; 

STRPTR styles[] = {
    str_26, str_27, str_28, str_149, NULL
};

STRPTR announces[] = {
    str_200,
    str_201,
    str_202,
    NULL
};

STRPTR voicesNames[] = {
    str_300,
    str_301,
    str_302,
    str_303,
    str_304,
    str_305,
    str_306,
    str_307,
    str_308,
    str_309,
    str_310,
    str_311,
    str_312,
    str_313,
    NULL
};

STRPTR voiceSpeedNames[] = {
    "25%", // 44
    "50%", // 88
    "75%", // 132
    str_206, // 175
    "125%", // 218
    "150%", // 262
    "175%", // 306
    NULL
};

struct Locale *currentLocale;

struct Library *CheckBoxBase = NULL;

ULONG getIndexChooser(STRPTR val);
void RemoveLineFromUserStartup(void);
BOOL IsAddedToUserStartup(void);

void loadDefaults(void)
{
    strcpy(location, str_49);
    strcpy(unit,str_50);
    strcpy(lang,str_14);
    strcpy(slang, lang);
    slang[2]='\0';
    strcpy(apikey,str_51);
    strcpy(months,str_52);
    strcpy(days,str_53);
    strcpy(displayIcon,str_54);
    strcpy(displayLocation,str_54);
    strcpy(displayDescription,str_54);
    strcpy(displayDateTime,str_54);
    strcpy(autostart, str_54);
    addX = addY = addTextY = 0;
    textColor = 1;
    backgroundColor = 2;
    sunColor = 7;
    cloudColor = 0;
    lcloudColor = 5;
    dcloudColor = 4;
    style = 0;
    styleColor=3;
    shadowColor=6;
    strcpy(espeakPath, str_150);
    strcpy(espeakText, str_151); 
    strcpy(espeakAnText,str_54);
    strcpy(espeakAnTime,str_54);
    voiceSpeed = 3;
    announcePeriod = 2;
    voice = 0;
}

BOOL getColorsFromWBPalette()
{
    UWORD r,g,b,indx;
    int i;
    FILE *fp;
    long offset;

    offset = 0xb2; 

    // index   R     G     B
    // -----   -----------------
    // 00 00   12 88 67 54 93 fe  (total 8 bytes info for Color0)
    // 00 01   ....
    //
    // R=12 -> 1
    // G=67 -> 6
    // B=93 -> 9
    //
    // rgb4 color -> 0x0169


    if((fp = fopen(PREFPPALETTE, str_67)) != NULL) 
    {
        fseek(fp, offset, SEEK_SET);
        for (i = 0; i < COLORCOUNT; i++)
        {
            fread(&indx, sizeof(UWORD), 1, fp);
            //printf("%04lx: %04lx\n", offset, indx);
            fread(&r, sizeof(UWORD), 1, fp);
            //printf("%04lx: %04lx\n", offset, r);
            fread(&g, sizeof(UWORD), 1, fp);
            //printf("%04lx: %04lx\n", offset, g);
            fread(&b, sizeof(UWORD), 1, fp);
            //printf("%04lx: %04lx\n", offset, b);
            offset +=8;

            colormap[i]= ((r>>12)<<8)|((g>>12)<<4)|(b>>12);
            // printf("color[%d]=%04lx\n", i, colormap[i]);
        }
        fclose(fp);
    }
    return TRUE;
}

void executeApp(STRPTR path)
{                                   
    Execute(path, NULL, NULL);
}


int main(int argc, char **argv)
{
    wdata = (char**)malloc(4*sizeof(char*));
    adays = (char**)malloc(7*sizeof(char*));
    amonths = (char**)malloc(12*sizeof(char*));
    // Pref file
    if(!fileExist(PREFFILEPATH))
	{
        loadDefaults();
		SavePrefs();
	}

    LoadPrefs();

    if ((CheckBoxBase =(struct CheckBoxBase *) OpenLibrary (str_73,0L))==0)
    {
        return SORRY;
    }

    InitStrings();

    currentLocale = OpenLocale(NULL);

    createPreferencesWin();

    CloseLocale(currentLocale);
    CloseLibrary(CheckBoxBase);

    return 0;

}

void LoadPrefs(void)
{
    char tm[BUFFER], td[BUFFER];
	fp = Open(PREFFILEPATH, MODE_OLDFILE);
	if (fp)
	{
        UBYTE buffer[BUFFER/8];

        FGets(fp, buffer, BUFFER/8);
        sprintf(location, str_89, buffer);
        location[strlen(location)-1] = '\0';    // location

        FGets(fp, buffer, BUFFER/8);
        sprintf(unit, str_89, buffer);
        unit[strlen(unit)-1] = '\0';    // unit

        FGets(fp, buffer, BUFFER/8);
        sprintf(lang, str_89, buffer);
        lang[strlen(lang)-1] = '\0';    // lang
        strcpy(slang, lang);
        slang[2]='\0';

        FGets(fp, buffer, BUFFER/8);
        sprintf(apikey, str_89, buffer);
        apikey[strlen(apikey)-1] = '\0';    // apikey

        FGets(fp, buffer, BUFFER/8);
        sprintf(days, str_89, buffer);
        days[strlen(days)-1] = '\0';    // days

        FGets(fp, buffer, BUFFER/8);
        sprintf(months, str_89, buffer);
        months[strlen(months)-1] = '\0';    // months

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayIcon, str_89, buffer);
        displayIcon[strlen(displayIcon)-1] = '\0';    // displayIcon

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayLocation, str_89, buffer);
        displayLocation[strlen(displayLocation)-1] = '\0';    // displayLocation

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayDescription, str_89, buffer);
        displayDescription[strlen(displayDescription)-1] = '\0';    // displayDescription

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayDateTime, str_89, buffer);
        displayDateTime[strlen(displayDateTime)-1] = '\0';    // displayDateTime

        FGets(fp, buffer, BUFFER/8);
        addX = atoi(buffer);    // addX

        FGets(fp, buffer, BUFFER/8);
        addY = atoi(buffer);    // addY

        FGets(fp, buffer, BUFFER/8);
        addTextY = atoi(buffer);    // addTextY

        FGets(fp, buffer, BUFFER/8);
        textColor = atoi(buffer); // text color

        FGets(fp, buffer, BUFFER/8);
        backgroundColor = atoi(buffer); // background color

        FGets(fp, buffer, BUFFER/8);
        sunColor = atoi(buffer); // sun color

        FGets(fp, buffer, BUFFER/8);
        cloudColor = atoi(buffer); // cloud color

        FGets(fp, buffer, BUFFER/8);
        lcloudColor = atoi(buffer); // light cloud color

        FGets(fp, buffer, BUFFER/8);
        dcloudColor = atoi(buffer); // dark cloud color

        FGets(fp, buffer, BUFFER/8);
        style = atoi(buffer);  // style

        FGets(fp, buffer, BUFFER/8);
        styleColor = atoi(buffer); // panel color

        FGets(fp, buffer, BUFFER/8);
        shadowColor = atoi(buffer); // shadow color

        FGets(fp, buffer, BUFFER/8); // autostart
        sprintf(autostart, str_89, buffer);
        autostart[strlen(autostart)-1] = '\0';
    
        FGets(fp, buffer, BUFFER/2);
        sprintf(espeakPath, str_89, buffer);
        espeakPath[strlen(espeakPath)-1] = '\0';    // espeak path

        FGets(fp, buffer, BUFFER/2);
        sprintf(espeakText, str_89, buffer);
        espeakText[strlen(espeakText)-1] = '\0';    // espeak text 
        
        FGets(fp, buffer, BUFFER/8);
        sprintf(espeakAnTime, str_89, buffer);
        espeakAnTime[strlen(espeakAnTime)-1] = '\0';    // espeakAnTime

        FGets(fp, buffer, BUFFER/8);
        sprintf(espeakAnText, str_89, buffer);
        espeakAnText[strlen(espeakAnText)-1] = '\0';    // espeakAnText
        
        FGets(fp, buffer, BUFFER/8);   
        announcePeriod = atoi(buffer);  // announcePeriod

        FGets(fp, buffer, BUFFER/8);
        voice = atoi(buffer);  // voice

        FGets(fp, buffer, BUFFER/8);
        voiceSpeed = atoi(buffer);  // voiceSpeed


        strcpy(tm, months);
        strcpy(td, days);
        amonths= getArray(tm, str_90, 12);
        adays = getArray(td, str_90, 7);
        
		Close(fp);
	}
}

void LoadPrefsForAddXY(void)
{
    int i;
    fp = Open(PREFFILEPATH, MODE_OLDFILE);
	if (fp)
	{
        UBYTE buffer[BUFFER/8];

        for (i = 0; i < 11; i++)
            FGets(fp, buffer, BUFFER/8);
        addX = atoi(buffer);    // addX

        FGets(fp, buffer, BUFFER/8);
        addY = atoi(buffer);    // addY

        Close(fp);
    }
}

// save settings
void SavePrefs(void)
{
    LoadPrefsForAddXY(); // if main window changes position, get new addx and addy variables.
	fp = Open(PREFFILEPATH, MODE_NEWFILE);
	if (fp)
	{
		sprintf(fpStr, str_91, 
                    location, unit, lang, apikey, days, months, displayIcon, displayLocation, displayDescription, 
                    displayDateTime, addX, addY, addTextY, textColor, backgroundColor, sunColor, cloudColor, 
                    lcloudColor, dcloudColor, style, styleColor, shadowColor, autostart, espeakPath, espeakText, espeakAnTime, espeakAnText, announcePeriod, voice, voiceSpeed);
		FPuts(fp, fpStr);
		Close(fp);
        
	}
    putenv(TRIGGER);
}

ULONG getIndexChooser(STRPTR val)
{
    ULONG i;
    for ( i = 0; i < CLANGCOUNT; i++)
    {
        if(STREQUAL(clanguages[i],val)) break;
    }
    return i;
}

// Preferences window
void createPreferencesWin(void)
{
	enum {
		IDFIRST=0x00,
		// gadget id starts
		IDSTRINGLOCATION,
        IDSTRINGLANGUAGE,
        IDINTEGERUNIT,
        IDSTRINGDAYS,
        IDSTRINGMONTHS,
        IDSTRINGAPIKEY,		
        IDCHECKBOXDISPLAYICON,
        IDCHECKBOXDISPLAYLOCATION,
        IDCHECKBOXDISPLAYDESCRIPTION,
        IDCHECKBOXDISPLAYDATETIME,		
        IDINTEGERADDX,
        IDINTEGERADDY,
        IDINTEGERaddTextY,
        IDBSAVE,
        IDBCLOSE,
        IDPALETTETEXT,
        IDPALETTEBACKGROUND,
        IDPALETTESUN,
        IDPALETTECLOUD,
        IDPALETTELCLOUD,
        IDPALETTEDCLOUD,
        IDGETMYLOCATION,
        IDGETMYDAYS,
        IDGETMYMONTHS,
        IDCHECKBOXDAYS,
        IDCHECKBOXMONTHS,
        IDSTYLE,
        IDSETTEXTPOSBUTTON,
        IDPALETTESTYLE,
        IDPALETTESHADOW,
        IDCHECKBOXAUTOSTART,
        IDBDEFAULTS,
        IDGETFILE,
        IDCHECKBOXANTIME,
        IDCHECKBOXANWF,
        IDTEXTFORMAT,
        IDTESTBUTTON,
        IDCOMBOANNOUNCE,
        IDCOMBOVOICE,
        IDVOICESPEED,
		// end of gadget id
		IDLAST
	};
	
	struct Gadget *gad[IDLAST-1];
	struct Gadget *vcreatePreferencesParent = NULL;
	APTR *createPreferencesWinObj = NULL;
	struct Window *createPreferencesWin = NULL;
	ULONG signalcreatePreferences, resultcreatePreferences;
	signed char donecreatePreferences;
	UWORD codecreatePreferences;

    ULONG dIcon, dDescription, dLocation, dDatetime, dAbbDays=TRUE, dAbbMonths=TRUE, dAutostart=TRUE, despeakAnTime, despeakAnText;

    int localDays=0, localMonths=0;
	
	struct List *chooserUnit;
    struct List *tablabels;
	chooserUnit = ChooserLabels( str_50,str_92, NULL );
    tablabels = ClickTabs(str_207, str_93, str_94, str_95, str_96, str_97, str_98, NULL);

    dIcon = STREQUAL(displayIcon, str_54)?TRUE:FALSE;
    dLocation = STREQUAL(displayLocation, str_54)?TRUE:FALSE;
    dDescription = STREQUAL(displayDescription, str_54)?TRUE:FALSE;
    dDatetime = STREQUAL(displayDateTime, str_54)?TRUE:FALSE;
    dAutostart = STREQUAL(autostart, str_54)?TRUE:FALSE;
    despeakAnTime = STREQUAL(espeakAnTime, str_54)?TRUE:FALSE;
    despeakAnText = STREQUAL(espeakAnText, str_54)?TRUE:FALSE;

    createPreferencesWinObj = NewObject
    (   WINDOW_GetClass(),
        NULL,
		//WA_PubScreen,             screen,
        WA_Title,           	  str_99,
        WA_Activate,              TRUE,
        WA_DragBar,               TRUE,
        WA_CloseGadget,           TRUE,
		WA_SizeGadget, 			  FALSE,
        WA_SmartRefresh,          TRUE,
        WA_MinWidth,              400,
        WA_MinHeight,             60,
        WA_MaxWidth,              400,
        WA_MaxHeight,             60,
        WA_InnerWidth,            400,
        WA_InnerHeight,           60,
       // WA_Left,                  0,
       // WA_Top,                   0,
       // WA_Width,                 400,
       // WA_Height,                200,
        WA_SizeBRight,            FALSE,
        WA_SizeBBottom,           FALSE,
       // WA_NewLookMenus,          TRUE,
        WINDOW_Position,          WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,       vcreatePreferencesParent = VGroupObject,
												LAYOUT_SpaceOuter, TRUE,
												LAYOUT_DeferLayout, TRUE,

                                                LAYOUT_AddChild, SpaceObject,
                                                                SPACE_Transparent,TRUE,
                                                                SPACE_BevelStyle, BVS_NONE,
                                                End,

												// LOCATION
                                                LAYOUT_AddChild, HGroupObject,

                                                    LAYOUT_AddChild, gad[IDSTRINGLOCATION-1] = (struct Gadget *) StringObject,
                                                                GA_ID, IDSTRINGLOCATION,
                                                                GA_RelVerify, TRUE,
                                                                STRINGA_TextVal, location,
                                                                STRINGA_MaxChars, 64,
                                                                STRINGA_Justification, GACT_STRINGLEFT,
                                                                STRINGA_HookType, SHK_CUSTOM,
                                                    End,

                                                    LAYOUT_AddChild, gad[IDGETMYLOCATION-1] = ButtonObject,
															GA_Text, str_100,
															GA_RelVerify, TRUE,
															GA_ID, IDGETMYLOCATION,
                                                    End,
                                                End,
                                                CHILD_Label,  LabelObject,
                                                                LABEL_Justification, 0,
                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                LABEL_Text, str_101,
                                                End,

                                                LAYOUT_AddChild, HGroupObject,
                                                    // UNIT
                                                    LAYOUT_AddChild, gad[IDINTEGERUNIT-1] = (struct Gadget *) ChooserObject,
                                                                    GA_ID,  IDINTEGERUNIT,
                                                                    GA_RelVerify, TRUE,
                                                                    CHOOSER_PopUp, TRUE,
                                                                    CHOOSER_MaxLabels, 2,
                                                                    CHOOSER_Offset, 0,
                                                                    CHOOSER_Selected, (STREQUAL(unit,str_50))?0:1,
                                                                    CHOOSER_Labels, chooserUnit,
                                                    End,                                                
                                                    CHILD_Label, LabelObject,
                                                                    LABEL_Justification, 0,
                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                    LABEL_Text, str_102,
                                                    End,

                                                    // LANGUAGE
                                                    LAYOUT_AddChild, gad[IDSTRINGLANGUAGE-1] = (struct Gadget *) ChooserObject,
                                                                    GA_ID, IDSTRINGLANGUAGE,
                                                                    GA_RelVerify, TRUE,
                                                                    CHOOSER_PopUp, TRUE,
                                                                    CHOOSER_MaxLabels, 46,
                                                                    CHOOSER_Offset, 0,
                                                                    CHOOSER_Selected, getIndexChooser(lang),
                                                                    CHOOSER_LabelArray, clanguages,
                                                    End,  
                                                    
                                                    CHILD_Label,  LabelObject,
                                                                    LABEL_Justification, 0,
                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                    LABEL_Text, str_103,
                                                    End,
                                                End,

                                                LAYOUT_AddChild, SpaceObject,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                End,

                                                LAYOUT_AddChild,  ClickTabObject,
                                                                    GA_RelVerify, TRUE,
                                                                    CLICKTAB_Labels, tablabels,

                                                                    CLICKTAB_PageGroup, PageObject,
                                                                    
                                                                        LAYOUT_DeferLayout, TRUE,

                                                                        // Announce
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                        GA_Text, str_208,
                                                                                        BUTTON_BackgroundPen, 4,
                                                                                        BUTTON_TextPen, 1,
                                                                                        GA_ReadOnly, TRUE,
                                                                                End,
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_209,
                                                                                End,

                                                                                LAYOUT_AddChild, HGroupObject,

                                                                                    LAYOUT_AddChild, gad[IDGETFILE-1] = (struct Gadget *) GetFileObject,
                                                                                        GA_ID, IDGETFILE,
                                                                                        GA_RelVerify, TRUE,
                                                                                        GETFILE_TitleText, str_210,
                                                                                        GETFILE_Pattern, str_154,
                                                                                        GETFILE_FullFile, espeakPath, 
                                                                                        GETFILE_DoPatterns, TRUE,
                                                                                        GETFILE_RejectIcons, TRUE,
                                                                                        GETFILE_ReadOnly, TRUE,
                                                                                    End,
                                                                                    
                                                                                    CHILD_Label, LabelObject,
                                                                                        LABEL_Justification, 0,
                                                                                        LABEL_SoftStyle, FSF_BOLD,
                                                                                        LABEL_Text, str_211,
                                                                                    End,

                                                                                    LAYOUT_AddChild, gad[IDTESTBUTTON-1] = ButtonObject,
                                                                                            GA_Text, str_212,
                                                                                            GA_RelVerify, TRUE,
                                                                                            GA_ID, IDTESTBUTTON,
                                                                                    End,
                                                                                End,

                                                                                LAYOUT_AddChild, gad[IDCHECKBOXANTIME-1] = (struct Gadget *) CheckBoxObject,
                                                                                                            GA_ID, IDCHECKBOXANTIME,
                                                                                                            GA_RelVerify, TRUE,
                                                                                                            GA_Selected, despeakAnTime,
                                                                                                            GA_Text, str_213,
                                                                                                            CHECKBOX_TextPen, 1,
                                                                                                            CHECKBOX_FillTextPen, 1,
                                                                                                            CHECKBOX_BackgroundPen, 0,
                                                                                                            CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                End,

                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_Text, str_156,
                                                                                End,

                                                                                LAYOUT_AddChild, gad[IDCHECKBOXANWF-1] = (struct Gadget *) CheckBoxObject,
                                                                                                            GA_ID, IDCHECKBOXANWF,
                                                                                                            GA_RelVerify, TRUE,
                                                                                                            GA_Selected, despeakAnText,
                                                                                                            GA_Text, str_214,
                                                                                                            CHECKBOX_TextPen, 1,
                                                                                                            CHECKBOX_FillTextPen, 1,
                                                                                                            CHECKBOX_BackgroundPen, 0,
                                                                                                            CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                End,

                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_Text, str_156,
                                                                                End,

                                                                                LAYOUT_AddChild, gad[IDTEXTFORMAT-1] = (struct Gadget *) StringObject,
                                                                                                    GA_ID, IDTEXTFORMAT,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    STRINGA_TextVal, espeakText,
                                                                                                    STRINGA_MaxChars, 255,
                                                                                                    STRINGA_Justification, GACT_STRINGLEFT,
                                                                                                    STRINGA_HookType, SHK_CUSTOM,
                                                                                End,
                                                                                
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_215,
                                                                                End,

                                                                                LAYOUT_AddChild, gad[IDCOMBOVOICE-1] = (struct Gadget *) ChooserObject,
                                                                                                GA_ID,  IDCOMBOVOICE,
                                                                                                GA_RelVerify, TRUE,
                                                                                                CHOOSER_PopUp, TRUE,
                                                                                                CHOOSER_MaxLabels, 14,
                                                                                                CHOOSER_Offset, 0,
                                                                                                CHOOSER_Selected, voice,
                                                                                                CHOOSER_LabelArray, voicesNames,
                                                                                End,                                                
                                                                                CHILD_Label, LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_216,
                                                                                End,  

                                                                                LAYOUT_AddChild, gad[IDVOICESPEED-1] = (struct Gadget *) ChooserObject,
                                                                                                                    GA_ID, IDVOICESPEED,
                                                                                                                    GA_RelVerify, TRUE,
                                                                                                                    CHOOSER_PopUp, TRUE,
                                                                                                                    CHOOSER_MaxLabels, 7,
                                                                                                                    CHOOSER_Offset, 0,
                                                                                                                    CHOOSER_Selected, voiceSpeed,
                                                                                                                    CHOOSER_LabelArray, voiceSpeedNames,
                                                                                End,
                                                                                CHILD_Label, LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_217,
                                                                                End,                                                                                  
                                                                                
                                                                                LAYOUT_AddChild, gad[IDCOMBOANNOUNCE-1] = (struct Gadget *) ChooserObject,
                                                                                                GA_ID,  IDCOMBOANNOUNCE,
                                                                                                GA_RelVerify, TRUE,
                                                                                                CHOOSER_PopUp, TRUE,
                                                                                                CHOOSER_MaxLabels, 3,
                                                                                                CHOOSER_Offset, 0,
                                                                                                CHOOSER_Selected, announcePeriod,
                                                                                                CHOOSER_LabelArray, announces,
                                                                                End,                                                
                                                                                CHILD_Label, LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_218,
                                                                                End,

                                                                                
                                                                        
                                                                        End,

                                                                        // Days & Months
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,

                                                                                    // DAYS
                                                                                    LAYOUT_AddChild, gad[IDSTRINGDAYS-1] = (struct Gadget *) StringObject,
                                                                                                    GA_ID, IDSTRINGDAYS,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    STRINGA_TextVal, days,
                                                                                                    STRINGA_MaxChars, 255,
                                                                                                    STRINGA_Justification, GACT_STRINGLEFT,
                                                                                                    STRINGA_HookType, SHK_CUSTOM,
                                                                                    End,
                                                                                    
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_104,
                                                                                    End,

                                                                                    // Get days and months
                                                                                    LAYOUT_AddChild, HGroupObject,
                                                                                        LAYOUT_AddChild, gad[IDGETMYDAYS-1] = ButtonObject,
                                                                                                GA_Text, str_105,
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDGETMYDAYS,
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDAYS-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                    GA_ID, IDCHECKBOXDAYS,
                                                                                                                    GA_RelVerify, TRUE,
                                                                                                                    GA_Selected, TRUE,
                                                                                                                    GA_Text, str_106,
                                                                                                                    CHECKBOX_TextPen, 1,
                                                                                                                    CHECKBOX_FillTextPen, 1,
                                                                                                                    CHECKBOX_BackgroundPen, 0,
                                                                                                                    CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,

                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_Text, str_107,
                                                                                    End,


                                                                                    // MONTHS
                                                                                    LAYOUT_AddChild, gad[IDSTRINGMONTHS-1] = (struct Gadget *) StringObject,
                                                                                                    GA_ID, IDSTRINGMONTHS,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    STRINGA_TextVal, months,
                                                                                                    STRINGA_MaxChars, 255,
                                                                                                    STRINGA_Justification, GACT_STRINGLEFT,
                                                                                                    STRINGA_HookType, SHK_CUSTOM,
                                                                                    End,
                                                                                    
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_108,
                                                                                    End,

                                                                                    

                                                                                    LAYOUT_AddChild, HGroupObject,
                                                                                        LAYOUT_AddChild, gad[IDGETMYMONTHS-1] = ButtonObject,
                                                                                                GA_Text, str_109,
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDGETMYMONTHS,
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXMONTHS-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                    GA_ID, IDCHECKBOXMONTHS,
                                                                                                                    GA_RelVerify, TRUE,
                                                                                                                    GA_Selected, TRUE,
                                                                                                                    GA_Text, str_110,
                                                                                                                    CHECKBOX_TextPen, 1,
                                                                                                                    CHECKBOX_FillTextPen, 1,
                                                                                                                    CHECKBOX_BackgroundPen, 0,
                                                                                                                    CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,

                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_Text, str_107,
                                                                                    End,

                                                                                    LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                    End,
                                                                        End,

                                                                        // Display
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,
                                                                                    LAYOUT_AddChild, HGroupObject,
                                                                                        // Add to Pos Y of the text
                                                                                        LAYOUT_AddChild, gad[IDINTEGERaddTextY-1] = (struct Gadget *) IntegerObject,
                                                                                                                        GA_ID, IDINTEGERaddTextY,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        INTEGER_Number, addTextY,
                                                                                                                        INTEGER_MaxChars, 3,
                                                                                                                        INTEGER_Minimum, -16,
                                                                                                                        INTEGER_Maximum, 16,
                                                                                                                        INTEGER_Arrows, TRUE,
                                                                                                                        STRINGA_Justification, GACT_STRINGRIGHT,
                                                                                        End,
                                                                                        CHILD_Label, LabelObject,
                                                                                                        LABEL_Justification, 0,
                                                                                                        LABEL_SoftStyle, FSF_BOLD,
                                                                                                        LABEL_Text, str_111,
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDSETTEXTPOSBUTTON-1] = ButtonObject,
                                                                                                GA_Text, str_112,
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDSETTEXTPOSBUTTON,
                                                                                        End,
                                                                                    End,

                                                                                    // Displays icon
                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDISPLAYICON-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXDISPLAYICON,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dIcon,
                                                                                                                        GA_Text, str_113,
                                                                                                                        CHECKBOX_TextPen, 1,
                                                                                                                        CHECKBOX_FillTextPen, 1,
                                                                                                                        CHECKBOX_BackgroundPen, 0,
                                                                                                                        CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,
                                                                                    // Displays location
                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDISPLAYLOCATION-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXDISPLAYLOCATION,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dLocation,
                                                                                                                        GA_Text, str_114,
                                                                                                                        CHECKBOX_TextPen, 1,
                                                                                                                        CHECKBOX_FillTextPen, 1,
                                                                                                                        CHECKBOX_BackgroundPen, 0,
                                                                                                                        CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,
                                                                                    
                                                                                            
                                                                                    // Displays description
                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDISPLAYDESCRIPTION-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXDISPLAYDESCRIPTION,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dDescription,
                                                                                                                        GA_Text, str_115,
                                                                                                                        CHECKBOX_TextPen, 1,
                                                                                                                        CHECKBOX_FillTextPen, 1,
                                                                                                                        CHECKBOX_BackgroundPen, 0,
                                                                                                                        CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,

                                                                                    // Displays date & time
                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDISPLAYDATETIME-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXDISPLAYDATETIME,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dDatetime,
                                                                                                                        GA_Text, str_116,
                                                                                                                        CHECKBOX_TextPen, 1,
                                                                                                                        CHECKBOX_FillTextPen, 1,
                                                                                                                        CHECKBOX_BackgroundPen, 0,
                                                                                                                        CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,
                                                                                    
                                                                                    

                                                                                    LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                    End,

                                                                        End,

                                                                        // Colors
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                        GA_Text, str_117,
                                                                                        BUTTON_BackgroundPen, 4,
                                                                                        BUTTON_TextPen, 1,
                                                                                        GA_ReadOnly, TRUE,
                                                                                End,
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_118,
                                                                                End,

                                                                                LAYOUT_AddChild, HGroupObject,
                                                                                    // Text Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTETEXT-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTETEXT,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             textColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_119,
                                                                                    End,

                                                                                    // Background Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTEBACKGROUND-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTEBACKGROUND,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             backgroundColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_120,
                                                                                    End,
                                                                                End,
                                                                                LAYOUT_AddChild, HGroupObject,
                                                                                    // Sun Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTESUN-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTESUN,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             sunColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_121,
                                                                                    End,

                                                                                    // Cloud Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTECLOUD-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTECLOUD,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             cloudColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_122,
                                                                                    End,
                                                                                End,
                                                                                LAYOUT_AddChild, HGroupObject,
                                                                                    // Light Cloud Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTELCLOUD-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTELCLOUD,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             lcloudColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_123,
                                                                                    End,
                                                                                    
                                                                                    // Dark Cloud Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTEDCLOUD-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTEDCLOUD,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             dcloudColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_124,
                                                                                    End,

                                                                                End,
                                                                                LAYOUT_AddChild, HGroupObject,

                                                                                    // Style Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTESTYLE-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTESTYLE,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             styleColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_125,
                                                                                    End,

                                                                                    // Shadow Color
                                                                                    LAYOUT_AddChild, gad[IDPALETTESHADOW-1] = (struct Gadget *) PaletteObject,
                                                                                                                    GA_ReadOnly,                FALSE,
                                                                                                                    GA_ID,                      IDPALETTESHADOW,
                                                                                                                    GA_RelVerify,               TRUE,
                                                                                                                    PALETTE_Colour,             shadowColor,
                                                                                                                    PALETTE_ColourOffset,       0,
                                                                                                                    PALETTE_NumColours,         COLORCOUNT,
                                                                                    End,
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_126,
                                                                                    End,
                                                                                End,

                                                                                
                                                                                LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                End,
                                                                                


                                                                        End,

                                                                        // Position & Style
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                        GA_Text, str_127,
                                                                                        BUTTON_BackgroundPen, 4,
                                                                                        BUTTON_TextPen, 1,
                                                                                        GA_ReadOnly, TRUE,
                                                                                End,
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, str_128,
                                                                                End,

                                                                                LAYOUT_AddChild, HGroupObject,

                                                                                    LAYOUT_AddChild, gad[IDSTYLE-1] = (struct Gadget *) ChooserObject,
                                                                                                    GA_ID,  IDSTYLE,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    CHOOSER_PopUp, TRUE,
                                                                                                    CHOOSER_MaxLabels, 4,
                                                                                                    CHOOSER_Offset, 0,
                                                                                                    CHOOSER_Selected, style,
                                                                                                    CHOOSER_LabelArray, styles,
                                                                                    End,                                                
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_129,
                                                                                    End,
                                                                            
                                                                                End,

                                                                                LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                End,

                                                                        End,

                                                                        // ENV Variables
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,

                                                                            LAYOUT_AddChild, VGroupObject,
                                                                                LAYOUT_AddChild, LabelObject,
                                                                                                        LABEL_Justification, 0,
                                                                                                        LABEL_SoftStyle, FSF_BOLD,
                                                                                                        LABEL_Text, str_61,
                                                                                End,

                                                                                CHILD_Label, LabelObject,
                                                                                                        LABEL_Justification, 0,
                                                                                                        LABEL_SoftStyle, FSF_BOLD,
                                                                                                        LABEL_Text, str_130,
                                                                                End,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, str_131,
                                                                                            BUTTON_BackgroundPen, 7,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                End,                                                                                

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, str_132,
                                                                                            BUTTON_BackgroundPen, 3,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                End,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, str_133,
                                                                                            BUTTON_BackgroundPen, 4,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                End,
                                                                                
                                                                                LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                End,

                                                                            End,
                                                                        
                                                                        End,

                                                                        // API Key
                                                                        PAGE_Add, VGroupObject,
                                                                            LAYOUT_SpaceOuter, TRUE,
                                                                            LAYOUT_SpaceInner, TRUE,
                                                                            
                                                                                    LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, str_134,
                                                                                            BUTTON_BackgroundPen, 4,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_135,
                                                                                    End,
                                                                                    // APIKEY
                                                                                    LAYOUT_AddChild, gad[IDSTRINGAPIKEY-1] = (struct Gadget *) StringObject,
                                                                                                    GA_ID, IDSTRINGAPIKEY,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    STRINGA_TextVal, apikey,
                                                                                                    STRINGA_MaxChars, 64,
                                                                                                    STRINGA_Justification, GACT_STRINGLEFT,
                                                                                                    STRINGA_HookType, SHK_CUSTOM,
                                                                                    End,
                                                                                    
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, str_136,
                                                                                    End,

                                                                                    LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                    End,
                                                                                    CHILD_WeightedHeight, 0,
                                                                        
                                                                        End,
                                                                    End,
                                                End,

                                                LAYOUT_AddChild, gad[IDCHECKBOXAUTOSTART-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXAUTOSTART,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dAutostart,
                                                                                                                        GA_Text, str_219,
                                                                                                                        CHECKBOX_TextPen, 1,
                                                                                                                        CHECKBOX_FillTextPen, 1,
                                                                                                                        CHECKBOX_BackgroundPen, 0,
                                                                                                                        CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                End,
                                                                   
                                                LAYOUT_AddChild, SpaceObject,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                End,

                                                // Buttons
												LAYOUT_AddChild, HGroupObject,

													LAYOUT_AddChild, gad[IDBDEFAULTS-1] = ButtonObject,
															GA_Text, str_220,
															GA_RelVerify, TRUE,
															GA_ID, IDBDEFAULTS,
                                                    End,

                                                    LAYOUT_AddChild, SpaceObject,
                                                            GA_ID, 97,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                    End,

													LAYOUT_AddChild, gad[IDBSAVE-1] = ButtonObject,
															GA_Text, str_138,
															GA_RelVerify, TRUE,
															GA_ID, IDBSAVE,
                                                    End,

                                                    LAYOUT_AddChild, gad[IDBCLOSE-1] = ButtonObject,
															GA_Text, str_137,
															GA_RelVerify, TRUE,
															GA_ID, IDBCLOSE,
                                                    End,
												End,

                                                LAYOUT_AddChild, SpaceObject,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                End,

											End
	);

	createPreferencesWin = RA_OpenWindow(createPreferencesWinObj);
	GetAttr(WINDOW_SigMask, createPreferencesWinObj, &signalcreatePreferences);	

	donecreatePreferences=FALSE;
	
	while(!donecreatePreferences)
    {   
		Wait(signalcreatePreferences | (1 << createPreferencesWin->UserPort->mp_SigBit));

        while ((resultcreatePreferences = DoMethod(createPreferencesWinObj, WM_HANDLEINPUT, &codecreatePreferences)) != WMHI_LASTMSG)
        {   
            STRPTR vLocation, vDays, vMonths, vApikey, vEspeakPath, vEspeakText;
            ULONG iLang, iUnit;
            char tm[BUFFER], td[BUFFER];
            int Control, Count, x;
            char cmd[PATH_MAX];

            switch (resultcreatePreferences & WMHI_CLASSMASK)
            {
                case WMHI_CLOSEWINDOW:
                    donecreatePreferences = TRUE;
                break;
				
				case WMHI_GADGETUP:
                    switch(resultcreatePreferences & WMHI_GADGETMASK)
                    {         
                        case IDBCLOSE:
                            donecreatePreferences = TRUE;
                            break;   
                        case IDBDEFAULTS:
                            if(showMessage(createPreferencesWin, str_221, str_222, str_223)==1)
                            {
                                executeApp("delete "PREFFILEPATH" >NIL:");
                                donecreatePreferences = TRUE;
                            }
                            break;     
                        case IDGETFILE:
							gfRequestFile(gad[IDGETFILE-1], createPreferencesWin);
						break;

                        case IDBSAVE:
                            GetAttr(STRINGA_TextVal, gad[IDSTRINGLOCATION-1], (ULONG*)&vLocation);
                            sprintf(location, str_89, vLocation);

                            GetAttr(CHOOSER_Selected, gad[IDINTEGERUNIT-1], (ULONG*)&iUnit);
                            sprintf(unit, str_89, iUnit==0 ? str_50:str_92);

                            GetAttr(CHOOSER_Selected, gad[IDSTRINGLANGUAGE-1], (ULONG*)&iLang);
                            sprintf(lang, str_89, clanguages[iLang]);
                            strcpy(slang, lang);
                            slang[2]='\0';
					        
                            GetAttr(STRINGA_TextVal, gad[IDSTRINGDAYS-1], (ULONG*)&vDays);
                            sprintf(days, str_89, vDays);

                            GetAttr(STRINGA_TextVal, gad[IDSTRINGMONTHS-1], (ULONG*)&vMonths);
                            sprintf(months, str_89, vMonths);

                            GetAttr(STRINGA_TextVal, gad[IDSTRINGAPIKEY-1], (ULONG*)&vApikey);
                            sprintf(apikey, str_89, vApikey);

                            GetAttr(GETFILE_FullFile, gad[IDGETFILE-1], (ULONG*)&vEspeakPath);
                            sprintf(espeakPath, str_89, vEspeakPath);
                            
                            GetAttr(STRINGA_TextVal, gad[IDTEXTFORMAT-1], (ULONG*)&vEspeakText);
                            sprintf(espeakText, str_89, vEspeakText);

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYICON-1], &dIcon);
                            sprintf(displayIcon, dIcon ? str_54:str_60);

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYLOCATION-1], &dLocation);
                            sprintf(displayLocation, dLocation ? str_54:str_60);

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYDESCRIPTION-1], &dDescription);
                            sprintf(displayDescription, dDescription ? str_54:str_60);

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYDATETIME-1], &dDatetime);
                            sprintf(displayDateTime, dDatetime ? str_54:str_60);

                            GetAttr(INTEGER_Number, gad[IDINTEGERaddTextY-1], &addTextY);

                            GetAttr(PALETTE_Colour, gad[IDPALETTETEXT-1], (ULONG*)&textColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTEBACKGROUND-1], (ULONG*)&backgroundColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTESUN-1], (ULONG*)&sunColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTECLOUD-1], (ULONG*)&cloudColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTELCLOUD-1], (ULONG*)&lcloudColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTEDCLOUD-1], (ULONG*)&dcloudColor);

                            GetAttr(PALETTE_Colour, gad[IDPALETTESTYLE-1], (ULONG*)&styleColor);
                            GetAttr(PALETTE_Colour, gad[IDPALETTESHADOW-1], (ULONG*)&shadowColor);

                            GetAttr(CHOOSER_Selected, gad[IDSTYLE-1], (ULONG*)&style);

                            GetAttr(GA_Selected, gad[IDCHECKBOXAUTOSTART-1], &dAutostart);
                            sprintf(autostart, dAutostart ? str_54:str_60);
                            
                            GetAttr(GA_Selected, gad[IDCHECKBOXANTIME-1], &despeakAnTime);
                            sprintf(espeakAnTime, despeakAnTime ? str_54:str_60);
                            
                            GetAttr(GA_Selected, gad[IDCHECKBOXANWF-1], &despeakAnText);
                            sprintf(espeakAnText, despeakAnText ? str_54:str_60);

                            GetAttr(CHOOSER_Selected, gad[IDCOMBOANNOUNCE-1], (ULONG*)&announcePeriod);
                            GetAttr(CHOOSER_Selected, gad[IDVOICESPEED-1], (ULONG*)&voiceSpeed);
                            GetAttr(CHOOSER_Selected, gad[IDCOMBOVOICE-1], (ULONG*)&voice);

                            // Controlling usage "," for days
                            Count = 0;
                            for (x=0;vDays[x];x++)
                            {
                                if (vDays[x]==',')
                                {
                                   Count++;
                                }

                            }

                            if (Count!=6)
                            {
                                showMessage(createPreferencesWin,  str_78, str_139, str_48);
                                break;
                            }

                            // Controlling usage "," for months
                            Count = 0;
                            for (x=0;vMonths[x];x++)
                            {
                                if (vMonths[x]==',')
                                {
                                   Count++;
                                }

                            }

                            if (Count!=11)
                            {
                                showMessage(createPreferencesWin,   str_78, str_140, str_48);
                                break;
                            }


                            // Different Color Controlling
                            Control = (1<<textColor)|(1<<backgroundColor)|(1<<sunColor)|(1<<cloudColor)|(1<<lcloudColor)|(1<<dcloudColor);
                            Count = 0;

                            for (x=0;x<COLORCOUNT;x++)
                            {
                               if ((Control>>x)&1)
                               {
                                  Count++;
                               }  
                            }


                            if (Count!=6)
                            {      
                                showMessage(createPreferencesWin,  str_78, str_141, str_48);
                                break;
                            }

                            if(dAutostart)
                            {
                                // autostart is checked
                                if (!IsAddedToUserStartup())
                                {
                                    sprintf(cmd, str_81, str_155, APPNAME, USERSTARTUP);
                                    executeApp(cmd);
                                }
                            }
                            else
                            {
                                // autostart is unchecked
                                if(IsAddedToUserStartup())
                                {
                                    RemoveLineFromUserStartup();
                                }
                            }

                            SavePrefs();
                            
                        break;

                        case IDGETMYDAYS:
                            GetAttr(GA_Selected, gad[IDCHECKBOXDAYS-1], &dAbbDays);
                            localDays=(dAbbDays)?0:7;

                            strcpy(td, str_47);
                            for(x=8-localDays;x<15-localDays;x++)
                            {
                                strcat(td, GetLocaleStr(currentLocale, x));
                                if(x!=14-localDays) strcat(td, str_90);
                            }
                            SetGadgetAttrs(gad[IDSTRINGDAYS-1], createPreferencesWin, NULL, STRINGA_TextVal, td, TAG_DONE); 
                        break;

                        case IDGETMYMONTHS:
                            GetAttr(GA_Selected, gad[IDCHECKBOXMONTHS-1], &dAbbMonths);
                            localMonths=(dAbbMonths)?0:12;
                            strcpy(tm, str_47);
                            for(x=27-localMonths;x<39-localMonths;x++)
                            {
                                strcat(tm, GetLocaleStr(currentLocale, x));
                                if(x!=38-localMonths) strcat(tm, str_90);
                            }
                            SetGadgetAttrs(gad[IDSTRINGMONTHS-1], createPreferencesWin, NULL, STRINGA_TextVal, tm, TAG_DONE); 
                        break;
                    
                        case IDGETMYLOCATION:
                            if (fileExist(GETEXTIP))
                            {
                                SetGadgetAttrs(gad[IDGETMYLOCATION-1], createPreferencesWin, NULL, GA_Disabled, TRUE, TAG_DONE);
                                executeApp(GETEXTIPRUN);

                                fp = Open(IPTXT, MODE_OLDFILE);
	
                                if (fp)
                                {
                                    FGets(fp, iptxt, sizeof(iptxt));
                                    iptxt[strlen(iptxt)-1] = '\0';
                                    Close(fp);
                                }

                                if(strlen(iptxt)>6) // ip minumum length 1.1.1.1
                                {

                                    sprintf(ipapiurl, str_142, iptxt);
                                    httpget(ipapiurl, MYLOCTXT);

                                    fp = Open(MYLOCTXT, MODE_OLDFILE);
                                    if(fp)
                                    {
                                        FGets(fp, locationCsv, sizeof(locationCsv));
                                        locationCsv[strlen(locationCsv)-1] = '\0';
                                        Close(fp);
                                    }

                                    locationData = getArray(locationCsv, str_90, 14);

                                    if(STREQUAL(locationData[0], str_143))
                                    {
                                        strcpy(locationCsv, convertToLatin(locationData[5]));
                                        strcat(locationCsv,str_90);
                                        strcat(locationCsv, locationData[2]);
                                        SetGadgetAttrs(gad[IDSTRINGLOCATION-1], createPreferencesWin, NULL, STRINGA_TextVal, locationCsv, TAG_DONE);  
                                    }
                                }
                                
                                SetGadgetAttrs(gad[IDGETMYLOCATION-1], createPreferencesWin, NULL, GA_Disabled, FALSE, TAG_DONE);

                                if (fileExist(IPTXT)||fileExist(MYLOCTXT))
                                {
                                    Execute(str_144,NULL,NULL);
                                }
                            }
                            else
                            {                         
                               showMessage(createPreferencesWin,  str_78, str_145, str_48);
                            }
                            
                        break;

                        
                        case IDSETTEXTPOSBUTTON:
                            if(style==BAR) // if only bar style
                            {
                                GetAttr(INTEGER_Number, gad[IDINTEGERaddTextY-1], &addTextY);
                                SavePrefs();
                            }
                        break;

                        case IDTESTBUTTON:
                            if (fileExist(espeakPath))
                            {
                                GetAttr(CHOOSER_Selected, gad[IDSTRINGLANGUAGE-1], (ULONG*)&iLang);
                                sprintf(lang, str_89, clanguages[iLang]);
                                strcpy(slang, lang);
                                slang[2]='\0';

                                GetAttr(GETFILE_FullFile, gad[IDGETFILE-1], (ULONG*)&vEspeakPath);
                                sprintf(espeakPath, str_89, vEspeakPath);
                                GetAttr(CHOOSER_Selected, gad[IDVOICESPEED-1], (ULONG*)&voiceSpeed);
                                GetAttr(CHOOSER_Selected, gad[IDCOMBOVOICE-1], (ULONG*)&voice);

                                strcpy(sy.espeakPath, espeakPath);
                                strcpy(sy.language, slang);
                                strcpy(sy.text, hi[iLang]);
                                strcat(sy.text, AMIGA);
                                sy.voice = voice;
                                sy.voicespeed = voiceSpeed;
                                executeSay(sy, AN_TEST);
                            }
                            else
                            {
                                showMessage(createPreferencesWin, str_224, str_225, str_48);
                            }
                        break;
                        
						
                        default:
                        break;
                    }
                break;
				
				default:
                break;
			}   
		}   
	}

	RA_CloseWindow(createPreferencesWinObj);
	createPreferencesWin=NULL;
	if (createPreferencesWinObj)
    {   
		DisposeObject(createPreferencesWinObj);
        createPreferencesWinObj = NULL;
    }
}

BOOL IsAddedToUserStartup(void)
{
    char satir[PATH_MAX];
    BOOL added = FALSE;
    fp = Open(USERSTARTUP, MODE_OLDFILE);
	
    if (fp)
	{
        while (FGets(fp, satir, sizeof(satir))) 
        {
            if (strstr(satir, APPNAME) != NULL)
            {
                added = TRUE;
                break;
            }
        }       
        Close(fp);
    }
    return added;
}

void RemoveLineFromUserStartup(void)
{
    BPTR fpn;
    char cmd[PATH_MAX+32];
    char satir[PATH_MAX];
                                                                                                                                  
    sprintf(cmd, str_146, USERSTARTUP);
    Execute(cmd, NULL, NULL);                                

    fp = Open(COPIEDUSERSTARTUP, MODE_OLDFILE);
    fpn = Open(USERSTARTUP, MODE_NEWFILE);

    if (fp && fpn)
	{   
        while (FGets(fp, satir, sizeof(satir))) 
        {
            if (strstr(satir, APPNAME) == NULL)
            {
                FPuts(fpn, satir);
            }
        }
        
        Close(fp);
        Close(fpn);
    }

    if (fileExist(COPIEDUSERSTARTUP))
    {
        Execute(str_147 COPIEDUSERSTARTUP str_57,NULL,NULL);
    }
    
}
