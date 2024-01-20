/* 
AmiWeatherForecasts by emarti, Murat Ozdemir 

v1.6 Jan 2024
v1.5 Dec 2023
v1.4 Nov 2023
V1.3 Nov 2023
V1.2 Nov 2023
V1.1 Oct 2023
V1.0 Feb 2022 (not published)

*/

#include "includes.h"
#include "defines.h"
#include "main_locale.h"
#include <weather.h>
#include <httpgetlib.h>
#include <funcs.h>
#include "version.h"
#include "images.h"
#include "digital.h"
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
    
struct GfxBase       *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Window        *Window;
struct RastPort      *RP;
struct Screen        *screen;
struct Library       *CheckBoxBase      = NULL;
struct VisualInfo    *VisualInfoPtr     = NULL;
struct Menu          *amiMenu           = NULL;

struct NewWindow NewWindow =
{
    0x00, 0x00,
    1, 0x0A,
    0,1,
    IDCMP_VANILLAKEY|
    IDCMP_MENUPICK|
    IDCMP_MOUSEMOVE|
    IDCMP_MOUSEBUTTONS,
    WFLG_BORDERLESS|
    WFLG_NEWLOOKMENUS|
    WFLG_WBENCHWINDOW|
    WFLG_ACTIVATE|
    WFLG_REPORTMOUSE,
    NULL,NULL,NULL,NULL,NULL,
    0,0,0,0,
    WBENCHSCREEN
};

struct TextFont *screenFont;
struct DrawInfo *drawinfo;
struct TextAttr *textattr;
BOOL dragging;

enum 
{
    BAR=0,
    ICONIC,
    DIGITAL,
    INVISIBLE
};

time_t t;
BOOL first;
char screenText[BUFFER/2];
char strdatetime[0xFF];
struct tm *dt;
unsigned short curMin, curHour;
int machineGun;

char txt[BUFFER/4];
int indx;
int newWidth;
int barHeight;
int newWindowWidth;
char** wdata;    

char *wf = str_2;
char weatherText[BUFFER];
char weatherURL[BUFFER/2]; 
int refresh;

void writeInfo(void);
void update(void);
void beforeClosing(void);
void changeWindowSizeAndPosition(void);

char appPath[PATH_MAX+NAME_MAX];
char curPath[PATH_MAX];
char manualPath[PATH_MAX+NAME_MAX];

#define PREFPPALETTE    str_3
BOOL getColorsFromWBPalette(void);
#define COLORCOUNT      8
UWORD colormap[COLORCOUNT];

#define PREFFILEPATH	str_4
void SavePrefs(void);
void LoadPrefs(void);
void loadDefaults(void);

BOOL IsAddedToUserStartup(void);
void RemoveLineFromUserStartup(void);

#define PREFTIME    str_5
#define SNTP        str_6
#define IPTXT       str_7

struct Locale *currentLocale;

void exportToEnv(STRPTR val, STRPTR var);
void updateFromPrefs(void);
ULONG getIndexChooser(STRPTR val);

#define MABOUT              str_29 
#define MQUIT               str_30
#define MUPDATEWF           str_31
#define MSETDTM             str_32
#define MSYNCSTUS           str_33
#define MMANUAL             str_37

struct NewMenu amiMenuNewMenu[] =
{
    NM_TITLE, (STRPTR)str_38,  NULL , 0, NULL, (APTR)~0,
    NM_ITEM , (STRPTR)MABOUT               		    ,  str_39/*Shorcut key of About*/ , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                          	,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MQUIT                      	,  str_40 /*Shorcut key of Quit*/ , 0, 0L, (APTR)~0,
	
    NM_TITLE, (STRPTR)str_41     	       ,  NULL , 0, NULL, (APTR)~0,
	NM_ITEM , (STRPTR)MUPDATEWF                     ,  str_42/*Shorcut key of Update*/ , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                          	,  NULL , 0, 0L, (APTR)~0,
	NM_ITEM , (STRPTR)MSETDTM                       ,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MSYNCSTUS                     ,  NULL , 0, 0L, (APTR)~0,
    
    NM_TITLE, (STRPTR)str_45               ,  NULL , 0, NULL, (APTR)~0,
    NM_ITEM , (STRPTR)MMANUAL                       ,  str_46/*Shorcut key of Manual*/ , 0, 0L, (APTR)~0,
	
    NM_END  , NULL                                  ,  NULL , 0, 0L, (APTR)~0
};

ULONG amiMenuTags[] =
{
    (GT_TagBase+67), TRUE,
    (TAG_DONE)
};

struct Say sy;
void AnnounceTheTime();
void AnnounceTheWeatherForecast();

int makeMenu(APTR MenuVisualInfo)
{
    if (NULL == (amiMenu = CreateMenusA( amiMenuNewMenu, NULL))) return( 1L );
    LayoutMenusA( amiMenu, MenuVisualInfo, (struct TagItem *)(&amiMenuTags[0]));
    return( 0L );
}

void executeApp(STRPTR path)
{                                   
    ClearMenuStrip(Window);
    Execute(path, NULL, NULL);
    SetMenuStrip(Window, amiMenu);
}

void exportToEnv(STRPTR val, STRPTR var)
{
    char envvar[BUFFER/4];
    const char setenv[8] = str_55;
    strcpy(envvar, setenv);
    strcat(envvar, var);
    strcat(envvar, str_56);
    strcat(envvar, val);
    strcat(envvar, str_57);
    Execute(envvar, NULL, NULL);
}

void prepareAndWriteInfo(void)
{
    time(&t);
    dt=localtime(&t);
    refresh=dt->tm_sec*2;
    curMin=dt->tm_min;
    curHour=dt->tm_hour;
    sprintf(strdatetime, str_58, amonths[dt->tm_mon], dt->tm_mday, adays[dt->tm_wday], dt->tm_hour, dt->tm_min);
    sprintf(screenText, str_59, wdata[2]);
    if(!STREQUAL(displayLocation, str_60)) { strcat(screenText, wdata[1]); strcat(screenText, str_61);}
    if(!STREQUAL(displayDescription, str_60)) { strcat(screenText, str_62); strcat(screenText, wdata[0]); strcat(screenText, str_63);}
    if(!STREQUAL(displayDateTime, str_60)) { strcat(screenText, strdatetime);}

    newWidth = 2 + strlen(screenText);

    // Export of weather data to ENV variable
    // $currenttemperature
    exportToEnv(wdata[2], str_64);
    // $weatherdescription
    exportToEnv(wdata[0], str_65);
    // $location
    exportToEnv(wdata[1], str_66);
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

int main(int argc, char **argv)
{
    register BOOL loop = TRUE;
    register struct IntuiMessage *msg;
    char *trigger;

    // Pref file
    if(!fileExist(PREFFILEPATH))
	{
        loadDefaults();
		SavePrefs();
	}
		
    GetCurrentDirName(curPath, PATH_MAX);
	strcpy(appPath, curPath);
	strcat(appPath, str_68);
    strcat(appPath, APPNAME);
	
	strcpy(manualPath, str_69);
	strcat(manualPath, appPath);
	strcat(manualPath, str_70);
    
    wdata = (char**)malloc(4*sizeof(char*));
    adays = (char**)malloc(7*sizeof(char*));
    amonths = (char**)malloc(12*sizeof(char*));
    
    LoadPrefs();

    if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary(str_71,0L))==0)
        return SORRY;

    if ((GfxBase=(struct GfxBase *)OpenLibrary(str_72,0L))==0)
    {
        //CloseLibrary(IntuitionBase);
        return SORRY;
    }

    if ((CheckBoxBase =(struct CheckBoxBase *) OpenLibrary (str_73,0L))==0)
    {
        return SORRY;
    }

    if(!fileExist(PREFPPALETTE)) executeApp(str_74);

    screen = LockPubScreen(NULL);

    if ((screen==NULL) || ((Window=(struct Window*) OpenWindow(&NewWindow))==0L))
    {
        UnlockPubScreen(NULL, screen);
        CloseLibrary(CheckBoxBase);
        return SORRY;
    }
 
    InitStrings();
    currentLocale = OpenLocale(NULL);

    RP=(struct RastPort*)Window->RPort;

    // get weather URL
    strcpy(weatherURL, getURL(apikey, convertToUTF8(location), unit, slang));

    // get weather info
    update();

    barHeight = screen->BarHeight-1;

    VisualInfoPtr = GetVisualInfo(screen, NULL);

    makeMenu(VisualInfoPtr);
    SetMenuStrip(Window, amiMenu); 

    if (fileExist(str_75))
    {
         executeApp(SNTP);
         update();
    }

    if (textattr = AllocMem(sizeof(struct TextAttr), MEMF_CLEAR)) 
    {
        if (textattr->ta_Name = AllocMem(48, MEMF_CLEAR)) 
        {
            drawinfo = GetScreenDrawInfo(screen);
            screenFont = drawinfo->dri_Font;
            strcpy(textattr->ta_Name, screenFont->tf_Message.mn_Node.ln_Name);
            textattr->ta_YSize = screenFont->tf_YSize;
            textattr->ta_Style = screenFont->tf_Style;
            textattr->ta_Flags = screenFont->tf_Flags;
        }
    }

    screenFont = OpenDiskFont(textattr);

    prepareAndWriteInfo();
    writeInfo();
    changeWindowSizeAndPosition();
    first=FALSE;
    machineGun=0;
    dragging = FALSE;

    while(loop)
    {
        // update periodically
        if((curMin%PERIOD4UPDATE==0)&&((refresh/2)%60==15))
        {
            update();  
            prepareAndWriteInfo();
            switch (announcePeriod)
            {
                case 0:
                    if(curMin==0) {
                        AnnounceTheTime();
                        AnnounceTheWeatherForecast();
                    }
                    break;

                case 1:
                if((curMin==00)||(curMin==30) ){
                        AnnounceTheTime();
                        AnnounceTheWeatherForecast();
                    }
                    break;

                case 2:
                    AnnounceTheTime();
                    AnnounceTheWeatherForecast();
                    break;

                default:
                    break;
            }
                                          
        }

        if((((refresh/2)%10)==0)&&!dragging){
                trigger = getenv(str_152);
                if(STREQUAL(trigger, str_54)) {
                    putenv(str_153);
                    updateFromPrefs();
                }
        }

        if ((((refresh/2)%60)==0)||first)
        {
            first=FALSE;
            prepareAndWriteInfo();
            writeInfo();
            changeWindowSizeAndPosition();
            Delay(WAITONESECOND);
            
            if (fileExist(wf))
            {
              Execute(str_76,NULL,NULL);
            }
            
            refresh+=2;                          
        }

        if(machineGun<0x04)
        {
            writeInfo();
            changeWindowSizeAndPosition();              
        }
        machineGun++;
         
        while ((msg = (struct IntuiMessage*)GetMsg( Window->UserPort)))
        {
            UWORD code = msg->Code;

            switch (msg->Class)
            {
                case IDCMP_MOUSEBUTTONS:
                    if (code == SELECTDOWN)
                    {
                        dragging = TRUE;
                        Delay(10);
                    }
                    else if (code == SELECTUP)
                    {
                        dragging = FALSE;
                        SavePrefs();
                        LoadPrefs();
                        machineGun=0;
                    }
                break;

                case IDCMP_MOUSEMOVE:
                    if (dragging)
                    {
                        MoveWindow(Window, msg->MouseX,  msg->MouseY);
                        if(Window->LeftEdge<=0) 
                            Window->LeftEdge = 0;
                        switch (style)
                        {
                            case BAR:
                                if(Window->LeftEdge>IntuitionBase->ActiveScreen->Width - newWindowWidth - ICONWIDTH) 
                                    Window->LeftEdge = IntuitionBase->ActiveScreen->Width - newWindowWidth - ICONWIDTH;
                                addX = Window->LeftEdge - (IntuitionBase->ActiveScreen->Width - newWindowWidth - ICONWIDTH);
                                break;
                            case ICONIC:
                                if(Window->LeftEdge>IntuitionBase->ActiveScreen->Width - 4 - ICONWIDTH) 
                                    Window->LeftEdge = IntuitionBase->ActiveScreen->Width - 4 - ICONWIDTH;
                                addX = Window->LeftEdge - (IntuitionBase->ActiveScreen->Width - ICONWIDTH - 4);
                                break;
                            case DIGITAL:
                                if(Window->LeftEdge>IntuitionBase->ActiveScreen->Width + 1 - PANELWIDTH) 
                                    Window->LeftEdge = IntuitionBase->ActiveScreen->Width + 1 - PANELWIDTH;
                                addX = Window->LeftEdge - (IntuitionBase->ActiveScreen->Width + 1 - PANELWIDTH);
                                break;
                            default:
                                break;
                        }
                        if(Window->TopEdge>IntuitionBase->ActiveScreen->Height-Window->Height) 
                            Window->TopEdge = IntuitionBase->ActiveScreen->Height-Window->Height;
                        if(Window->TopEdge<1) 
                            Window->TopEdge = 1;
                        addY = Window->TopEdge - 1;
                        ReplyMsg((struct Message*)msg);
                        continue;
                    }
                break;

                case IDCMP_VANILLAKEY:
                    switch(code)
                    {
                        case 0x1B: // press ESC to exit
                            loop=FALSE;
                            break;
                        default:
                            break;
                    }
                break;

                case IDCMP_MENUPICK: 
                    while (code != MENUNULL) { 
                        struct MenuItem *item = ItemAddress(amiMenu, code); 
                        struct IntuiText *text = item->ItemFill;
                        if STREQUAL(text->IText, MABOUT)
                        {
                            ClearMenuStrip(Window);
                            showMessage(Window, str_77, ABOUT, str_48);
                            SetMenuStrip(Window, amiMenu);
                        }
                        else if STREQUAL(text->IText, MQUIT) 
                        {
                            loop=FALSE;
                        }
                        else if STREQUAL(text->IText, MUPDATEWF)
                        {
                            updateFromPrefs();
                        }
                        else if STREQUAL(text->IText, MMANUAL)
                        {
                            executeApp(manualPath);
                        }
                        else if STREQUAL(text->IText, MSETDTM)
                        {
                            executeApp(PREFTIME);
                            update();
                        }
                        else if STREQUAL(text->IText, MSYNCSTUS)
                        {
                            if (fileExist(str_75))
                            {
                                executeApp(SNTP);
                                update();
                            }
                            else
                            {
                                ClearMenuStrip(Window);
                                showMessage(Window,  str_78, str_79, str_48);
                                SetMenuStrip(Window, amiMenu);
                            }
                        }
                        code = item->NextSelect;
                    }
                break;
            
                default:
                break;
            }

            ReplyMsg((struct Message*)msg);
        }

        if(!dragging)
        {
            Delay(WAITONESECOND/2); 
            refresh++;
        }
    }

    beforeClosing();
    return 0x00;
}

void changeWindowSizeAndPosition()
{
    switch (style)
    {
        case BAR:
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - newWindowWidth - ICONWIDTH + addX, 1 + addY, (addY<screen->BarHeight) ? newWindowWidth : newWindowWidth+2, (addY<screen->BarHeight) ? barHeight : ((screenFont->tf_YSize>ICONHEIGHT) ? screenFont->tf_YSize + 4 : ICONHEIGHT + 4 ));
            break;
        case ICONIC:
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - ICONWIDTH + addX - 4, 1 + addY, ICONWIDTH+4, ICONHEIGHT+4);
            break;
        case DIGITAL:
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - PANELWIDTH + 1 + addX, 1 + addY, PANELWIDTH-1, PANELHEIGHT);
            break;
        case INVISIBLE:
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - PANELWIDTH + 1 + addX, 1 + addY, 0, 0);
            break;
        default:
            break;
    }
    WindowToBack(Window);
}

void writeInfo(void)
{
    int i,ii,iii;     
    struct ViewPort *vp;
    char strdeg[2];
    int deg, hane, mode;
    
    BOOL freeBarStyle;
    int border = 0;
    getColorsFromWBPalette();
    vp = (struct ViewPort*) ViewPortAddress(Window);
    LoadRGB4(vp, colormap, COLORCOUNT);

    SetFont(RP, screenFont);
    newWindowWidth = TextLength(RP, screenText, newWidth) + ICONWIDTH;
    Move(RP,0,0);

    switch (style)
    {
        // bar style
        case BAR:
        {
            freeBarStyle = addY>screen->BarHeight;
            border = (freeBarStyle) ? 1 : 0;
            if(freeBarStyle)
            {
                SetAPen(RP, textColor);
                RectFill(RP, 0, 0, newWindowWidth+1, Window->Height-1);
            }
            SetAPen(RP, backgroundColor);
            RectFill(RP, 0+border, 0+border, newWindowWidth-1, Window->Height-1-border);
            SetAPen(RP, textColor);
            SetBPen(RP, backgroundColor);
            
            // write text
            Move(RP, ICONWIDTH+border+1, 6+border + addTextY + (Window->Height - screenFont->tf_YSize)/2 - 1);
            Text(RP, screenText, strlen(screenText));
            break;
        }

        // ICONIC style
        case ICONIC:
        {
            SetAPen(RP, textColor);
            RectFill(RP, 0, 0, ICONWIDTH+3, ICONHEIGHT+3);
            SetAPen(RP, styleColor);
            RectFill(RP, 1, 1, ICONWIDTH+2, ICONHEIGHT+2);
            break;
        }

        case DIGITAL:
        {
            // setup panel
            for (i = 0; i < PANELHEIGHT; i++)
            {
                for (ii = 0; ii < PANELWIDTH-1; ii++)
                {
                    switch(digitalPanel[i][ii])
                    {
                        case '*': SetAPen(RP, textColor); break;
                        case 'W': SetAPen(RP, lcloudColor); break;
                        case 'B': SetAPen(RP, styleColor); break;
                        case ' ': SetAPen(RP, backgroundColor); break;
                    }
                    WritePixel(RP, ii, i%PANELHEIGHT);
                }
                
            }

            strcpy(strdeg, str_86);
            // write deg.
            for (hane = strlen(wdata[2])-3, iii=2; hane >= 0; hane--, iii--)
            {
                strdeg[0] = wdata[2][hane];

                deg = (STREQUAL(strdeg, str_87)) ? 10 : atoi(strdeg);

                for (i = FONTHEIGHT*deg; i < FONTHEIGHT*deg+FONTHEIGHT; i++)
                {
                    for (ii = 0; ii < FONTWIDTH-1; ii++)
                    {
                        switch(numberFont[i][ii])
                        {
                            case '*': SetAPen(RP, textColor); break;
                            case 'B': SetAPen(RP, shadowColor); break;
                            case ' ': SetAPen(RP, backgroundColor); break;
                        }
                        WritePixel(RP, ii+30+iii*FONTWIDTH, i%FONTHEIGHT + 16);
                    }
                }
            }

            // write mode
            mode = (STREQUAL(unit,str_50))?0:1;

            for (i = mode*15; i < 15+mode*15; i++)
            {
                for (ii = 0; ii < 18; ii++)
                {
                    switch(degMode[i][ii])
                    {
                        case '*': SetAPen(RP, textColor); break;
                        case 'B': SetAPen(RP, shadowColor); break;
                        case ' ': SetAPen(RP, backgroundColor); break;
                    }
                    WritePixel(RP, ii+81, i%15+16);
                }
                
            }
            break;
        }

        default:
            break;
    }

    // for testing => indx=?;
    // draw weather icon
    if((!STREQUAL(displayIcon, str_60))||(style!=BAR))
    {
        for(i=ICONHEIGHT*indx;i<ICONHEIGHT*indx+ICONHEIGHT;i++)
        {
            for(ii=0;ii<ICONWIDTH-1;ii++)
            {
                switch(images[i][ii])
                {
                    case '*': SetAPen(RP, textColor); break;
                    case '+': SetAPen(RP, sunColor); break;
                    case '-': SetAPen(RP, cloudColor); break;
                    case '=': SetAPen(RP, lcloudColor); break;
                    case '#': SetAPen(RP, dcloudColor); break;
                    case ' ': SetAPen(RP, (style==ICONIC)?styleColor : backgroundColor); break;
                }
                if(style==DIGITAL)
                    WritePixel(RP, ii+5, i%ICONHEIGHT + 13);
                else
                    WritePixel(RP, ii+border+1, i%ICONHEIGHT + (Window->Height - screenFont->tf_YSize )/2 + ((style==ICONIC)?1:0));
            }
        } 
    }

}

void update(void)
{
    httpget(weatherURL, wf);
    
    strcpy(weatherText, getWeatherData(wf));      
    strcpy(weatherText, convertToLatin(weatherText));     
    
    wdata = getArray(weatherText, str_88, 4);
    
    indx = iconIndex(wdata[3]);
    strcpy(wdata[2], temperatureWithUnit(wdata[2], unit));
                 
    first=TRUE;
}

void beforeClosing()
{
    CloseFont(screenFont);
    FreeMem(textattr->ta_Name, 48);
    FreeMem(textattr, sizeof(struct TextAttr));
    UnlockPubScreen(NULL, screen);
    ClearMenuStrip(Window);
    FreeVisualInfo(VisualInfoPtr);
    CloseLocale(currentLocale);
    CloseWindow(Window);
    CloseLibrary(CheckBoxBase);
}

/*
### SYS:Prefs/Env-Archive/AmiWeatherForecasts.prefs ###
*/
// load settings from pref file
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

        FGets(fp, buffer, BUFFER/8); 
        sprintf(autostart, str_89, buffer); // autostart
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

// save settings
void SavePrefs(void)
{
    int newAddX, newAddY, newAddTextY;
    newAddX = addX;
    newAddY = addY;
    newAddTextY = addTextY;
    LoadPrefs();
	fp = Open(PREFFILEPATH, MODE_NEWFILE);
	if (fp)
	{
		sprintf(fpStr, str_91, 
                    location, unit, lang, apikey, days, months, displayIcon, displayLocation, displayDescription, 
                    displayDateTime, newAddX, newAddY, newAddTextY, textColor, backgroundColor, sunColor, cloudColor, 
                    lcloudColor, dcloudColor, style, styleColor, shadowColor, autostart, espeakPath, espeakText, espeakAnTime, espeakAnText, announcePeriod, voice, voiceSpeed);
		FPuts(fp, fpStr);
		Close(fp);
	}
}

void updateFromPrefs(void)
{
    LoadPrefs();  // get weather URL
    strcpy(weatherURL, getURL(apikey, convertToUTF8(location), unit, slang)); 
    update();
    changeWindowSizeAndPosition();
    prepareAndWriteInfo();
    writeInfo();
    machineGun = 0;
}

void AnnounceTheTime()
{
    if(STREQUAL(espeakAnTime, str_60)) return; 
    strcpy(sy.language, slang);
    strcpy(sy.espeakPath, espeakPath);
    sy.hour = curHour;
    sy.minute = curMin;
    sy.voice = voice;
    sy.voicespeed = voiceSpeed;
    executeSay(sy, AN_TIME);
}

void AnnounceTheWeatherForecast()
{
    if(STREQUAL(espeakAnText, str_60)) return; 
    strcpy(sy.language, slang);
    strcpy(sy.espeakPath, espeakPath);
    strcpy(sy.text, espeakText);
    sy.voice = voice;
    sy.voicespeed = voiceSpeed;
    executeSay(sy, AN_TEXT);
}