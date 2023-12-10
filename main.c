/* 
AmiWeatherForecasts by emarti, Murat Ozdemir 

v1.5 Dec 2023
v1.4 Nov 2023
V1.3 Nov 2023
V1.2 Nov 2023
V1.1 Oct 2023
V1.0 Feb 2022 (not published)
*/

#include "includes.h"
#include <weather.h>
#include <httpgetlib.h>
#include <funcs.h>
#include "version.h"
#include "images.h"
#include "digital.h"

#define USERSTARTUP "SYS:S/User-Startup"
#define COPIEDUSERSTARTUP "RAM:T/User-Startup"

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
int addX, addY, addTextY;
ULONG textColor, backgroundColor, style;
ULONG sunColor, cloudColor, lcloudColor, dcloudColor;
ULONG styleColor, shadowColor;

char **amonths;
char **adays;
    
struct GfxBase       *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Window        *Window;
struct RastPort      *RP;
struct Screen        *screen;
struct Library       *CheckBoxBase      = NULL;
struct VisualInfo*    VisualInfoPtr     = NULL;
struct Menu*          amiMenu           = NULL;

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
    WFLG_WBENCHWINDOW|WFLG_ACTIVATE|WFLG_REPORTMOUSE,
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
    DIGITAL
};

time_t t;
BOOL first;
char screenText[BUFFER/2];
char strdatetime[0xFF];
struct tm *dt;
unsigned short curMin;
int machineGun;

char txt[BUFFER/4];
int indx;
int newWidth;
int barHeight;
int newWindowWidth;
char** wdata;    

char wf[50] = "RAM:T/weather.json";
char weatherText[BUFFER] = "";
char weatherURL[BUFFER/2] = ""; 
int refresh;

void writeInfo(void);
void update(void);
void beforeClosing(void);
void changeWindowSizeAndPosition(void);

char appPath[PATH_MAX+NAME_MAX];
char curPath[PATH_MAX];
char manualPath[PATH_MAX+NAME_MAX];

#define PREFPPALETTE    "SYS:Prefs/Env-Archive/Sys/Palette.prefs"
BOOL getColorsFromWBPalette(void);
#define COLORCOUNT      8
UWORD colormap[COLORCOUNT];

#define PREFFILEPATH	"SYS:Prefs/Env-Archive/AmiWeatherForecasts.prefs"
void SavePrefs(void);
void LoadPrefs(void);
void createPreferencesWin(void);
void loadDefaults(void);

BOOL IsAddedToUserStartup(void);
void RemoveLineFromUserStartup(void);

#define PREFTIME    "SYS:Prefs/Time"
#define SNTP        "SYS:C/sntp pool.ntp.org >NIL:"
#define IPTXT       "RAM:T/ip.txt"
#define GETEXTIP    "SYS:C/GetExtIP"
#define GETEXTIPRUN GETEXTIP" > "IPTXT
#define MYLOCTXT    "RAM:T/mylocation.txt"

char iptxt[15];
char locationCsv[BUFFER/2];
char ipapiurl[64];
char **locationData;

#define CLANGCOUNT  15
STRPTR clanguages[] = {
        "ca, Catalan",
        "da, Danish",
        "de, German",
        "en, English",
        "eu, Basque",
        "fi, Finnish",
        "fr, French",
        "gl, Galician",
        "it, Italian",
        "no, Norwegian",
        "nl, Dutch",
        "pt, Portuguese",
        "sv, Swedish",
        "sp, Spanish",
        "tr, Turkish",
        NULL};    

STRPTR styles[] = {
    "Bar", "Iconic", "Digital", NULL
};

struct Locale *currentLocale;

void exportToEnv(STRPTR val, STRPTR var);

ULONG getIndexChooser(STRPTR val);

#define MABOUT              "About" 
#define MQUIT               "Quit"
#define MUPDATEWF           "Update Weather Forecast"
#define MSETDTM             "Set Date and Time Manually"
#define MSYNCSTUS           "Sync System Time using SNTP"
#define MPREF               "Preferences"
#define MAU                 "Add to User-Startup"
#define MRU                 "Remove from User-Startup"
#define MMANUAL             "Manual"

struct NewMenu amiMenuNewMenu[] =
{
    NM_TITLE, (STRPTR)"AmiWeatherForecasts",  NULL , 0, NULL, (APTR)~0,
    NM_ITEM , (STRPTR)MABOUT               		    ,  "A" , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                          	,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MQUIT                      	,  "Q" , 0, 0L, (APTR)~0,
	
    NM_TITLE, (STRPTR)"Tools"     	       ,  NULL , 0, NULL, (APTR)~0,
	NM_ITEM , (STRPTR)MUPDATEWF                     ,  "U" , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                          	,  NULL , 0, 0L, (APTR)~0,
	NM_ITEM , (STRPTR)MSETDTM                       ,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MSYNCSTUS                     ,  NULL , 0, 0L, (APTR)~0,

    NM_TITLE, (STRPTR)"Settings"           ,  NULL , 0, NULL, (APTR)~0,
    NM_ITEM , (STRPTR)MPREF                         ,  "P" , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                          	,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MAU                           ,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , NM_BARLABEL                           ,  NULL , 0, 0L, (APTR)~0,
    NM_ITEM , (STRPTR)MRU                           ,  NULL , 0, 0L, (APTR)~0,
    
    NM_TITLE, (STRPTR)"Help"               ,  NULL , 0, NULL, (APTR)~0,
    NM_ITEM , (STRPTR)MMANUAL                       ,  "M" , 0, 0L, (APTR)~0,
	
    NM_END  , NULL                                  ,  NULL , 0, 0L, (APTR)~0
};

ULONG amiMenuTags[] =
{
    (GT_TagBase+67), TRUE,
    (TAG_DONE)
};

int makeMenu(APTR MenuVisualInfo)
{
    if (NULL == (amiMenu = CreateMenusA( amiMenuNewMenu, NULL))) return( 1L );
    LayoutMenusA( amiMenu, MenuVisualInfo, (struct TagItem *)(&amiMenuTags[0]));
    return( 0L );
}

struct EasyStruct aboutReq =
{
    sizeof(struct EasyStruct),
    0,
    "About "APPNAME,
    ABOUT,
    "Ok"
};

struct EasyStruct addedUserStartup =
{
    sizeof(struct EasyStruct),
    0,
    "Info - "APPNAME,
    "Each time your Amiga is booted, this application is executed.",
    "Ok"
};

struct EasyStruct addedUserStartupPreviously =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "It was previously added command line to the User-Startup file\nto execute this application when startup your Amiga.",
    "Ok"
};

struct EasyStruct RemoveFromUserStartup =
{
    sizeof(struct EasyStruct),
    0,
    "Info - "APPNAME,
    "It was removed command line from the User-Startup file\nto execute this application when startup your Amiga.",
    "Ok"
};

struct EasyStruct RemoveFromUserStartupW =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "The User-Startup file not include command line that execute this application.",
    "Ok"
};

struct EasyStruct SNTPMessage =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "SNTP not found in SYS:C drawer. Download it from Aminet.net.\nSystem time could not be synchronized.",
    "Ok"
};

struct EasyStruct GetExtIPMessage =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "GetExtIP not found in SYS:C drawer. Download it from Aminet.net.",
    "Ok"
};

struct EasyStruct ColorControl =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "Your color choices must be different from each other.\nPlease, try again.",
    "Ok"
};

struct EasyStruct DayControl =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "There must be 6 comma characters between the days.\nPlease, try again.",
    "Ok"
};

struct EasyStruct MonthControl =
{
    sizeof(struct EasyStruct),
    0,
    "Warning - "APPNAME,
    "There must be 11 comma characters between the months.\nPlease, try again.",
    "Ok"
};

void loadDefaults(void)
{
    strcpy(location, "istanbul,tr");
    strcpy(unit,"metric");
    strcpy(lang,"en, English");
    strcpy(slang, lang);
    slang[2]='\0';
    strcpy(apikey,"e45eeea40d4a9754ef176588dd068f18");
    strcpy(months,"Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec");
    strcpy(days,"Sun,Mon,Tue,Wed,Thu,Fri,Sat");
    strcpy(displayIcon,"1");
    strcpy(displayLocation,"1");
    strcpy(displayDescription,"1");
    strcpy(displayDateTime,"1");
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
    const char setenv[8] = "SetEnv ";
    strcpy(envvar, setenv);
    strcat(envvar, var);
    strcat(envvar, "=");
    strcat(envvar, val);
    strcat(envvar, " >NIL:");
    Execute(envvar, NULL, NULL);
}

void prepareAndWriteInfo(void)
{
    time(&t);
    dt=localtime(&t);
    refresh=dt->tm_sec*2;
    curMin=dt->tm_min;
    sprintf(strdatetime, " %s %d, %s %02d:%02d ", amonths[dt->tm_mon], dt->tm_mday, adays[dt->tm_wday], dt->tm_hour, dt->tm_min);
    sprintf(screenText, " %s ", wdata[2]);
    if(!STREQUAL(displayLocation, "0")) { strcat(screenText, wdata[1]); strcat(screenText, " ");}
    if(!STREQUAL(displayDescription, "0")) { strcat(screenText, "["); strcat(screenText, wdata[0]); strcat(screenText, "] ");}
    if(!STREQUAL(displayDateTime, "0")) { strcat(screenText, strdatetime);}

    newWidth = 2 + strlen(screenText);

    // Export of weather data to ENV variable
    // $currenttemperature
    exportToEnv(wdata[2], "currenttemperature");
    // $weatherdescription
    exportToEnv(wdata[0], "weatherdescription");
    // $location
    exportToEnv(wdata[1], "location");
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
    
    if((fp = fopen(PREFPPALETTE, "r")) == NULL) 
    {
        Printf("Palette.prefs file not found\n");
        return FALSE;
	}
    else
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

int main(int argc, char **argv)
{
    register BOOL loop = TRUE;
    register struct IntuiMessage *msg;

    // Pref file
    if(!fileExist(PREFFILEPATH))
	{
        loadDefaults();
		SavePrefs();
	}
		
    GetCurrentDirName(curPath, PATH_MAX);
	strcpy(appPath, curPath);
	strcat(appPath, "/");
    strcat(appPath, APPNAME);
	
	strcpy(manualPath, "SYS:Utilities/MultiView ");
	strcat(manualPath, appPath);
	strcat(manualPath, ".guide");
    
    wdata = (char**)malloc(4*sizeof(char*));
    adays = (char**)malloc(7*sizeof(char*));
    amonths = (char**)malloc(12*sizeof(char*));
    
    LoadPrefs();

    if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0L))==0)
        return SORRY;

    if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0L))==0)
    {
        CloseLibrary(IntuitionBase);
        return SORRY;
    }

    if ((CheckBoxBase =(struct CheckBoxBase *) OpenLibrary ("gadgets/checkbox.gadget",0L))==0)
    {
        CloseLibrary(IntuitionBase);
        CloseLibrary(GfxBase);
        return SORRY;
    }

    screen = LockPubScreen(NULL);

    if ((screen==NULL) || ((Window=(struct Window*) OpenWindow(&NewWindow))==0L))
    {
        UnlockPubScreen(NULL, screen);
        CloseLibrary(IntuitionBase);
        CloseLibrary(GfxBase);
        CloseLibrary(CheckBoxBase);
        return SORRY;
    }
 
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

    if (fileExist("SYS:C/sntp"))
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
              Execute("SYS:C/delete RAM:T/weather.json >NIL:",NULL,NULL);
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
            char cmd[PATH_MAX];

            switch (msg->Class)
            {
                case IDCMP_MOUSEBUTTONS:
                    if (code == SELECTDOWN)
                    {
                        dragging = TRUE;
                        Delay(20);
                    }
                    else if (code == SELECTUP)
                    {
                        dragging = FALSE;
                        SavePrefs();
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
                        ReplyMsg(msg);
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
                            EasyRequest(Window, &aboutReq, NULL, NULL);
                            SetMenuStrip(Window, amiMenu);
                        }
                        else if STREQUAL(text->IText, MQUIT) 
                                    loop=FALSE;
                        else if STREQUAL(text->IText, MUPDATEWF)
                                    update();
                        else if STREQUAL(text->IText, MMANUAL) 
                                    executeApp(manualPath);
                        else if STREQUAL(text->IText, MPREF) 
                        { 
                            createPreferencesWin();
                            prepareAndWriteInfo();
                            writeInfo(); 
                            changeWindowSizeAndPosition();
                        }
                        else if STREQUAL(text->IText, MSETDTM)
                        {
                            executeApp(PREFTIME);
                            update();
                        }
                        else if STREQUAL(text->IText, MSYNCSTUS)
                        {
                            if (fileExist("SYS:C/sntp"))
                            {
                            executeApp(SNTP);
                            update();
                            }
                            else
                            {
                            ClearMenuStrip(Window);
                            EasyRequest(Window, &SNTPMessage, NULL, NULL);
                            SetMenuStrip(Window, amiMenu);
                            }
                        }
                        else if STREQUAL(text->IText, MAU)
                                {
                                    if (IsAddedToUserStartup())
                                    {
                                    ClearMenuStrip(Window);
                                    EasyRequest(Window, &addedUserStartupPreviously, NULL, NULL);
                                    SetMenuStrip(Window, amiMenu);
                                    }
                                    else
                                    {
                                        sprintf(cmd, "echo \"*N*NRun >NIL: %s ; %s\" >> %s >NIL:", appPath, APPNAME, USERSTARTUP);      
                                        executeApp(cmd);
                                        ClearMenuStrip(Window);
                                        EasyRequest(Window, &addedUserStartup, NULL, NULL);
                                        SetMenuStrip(Window, amiMenu);
                                    }
                                }
                        else if STREQUAL(text->IText, MRU)
                                {
                                    if(IsAddedToUserStartup())
                                    {
                                        RemoveLineFromUserStartup();
                                        ClearMenuStrip(Window);
                                        EasyRequest(Window, &RemoveFromUserStartup, NULL, NULL);
                                        SetMenuStrip(Window, amiMenu);
                                    }
                                    else
                                    {
                                        ClearMenuStrip(Window);
                                        EasyRequest(Window, &RemoveFromUserStartupW, NULL, NULL);
                                        SetMenuStrip(Window, amiMenu);
                                    }
                                }
                                    
                        code = item->NextSelect;
                    }
                break;
            
                default:
                break;
            }

            ReplyMsg(msg);
            
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
        case 0: // Bar
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - newWindowWidth - ICONWIDTH + addX, 1 + addY, (addY<screen->BarHeight) ? newWindowWidth : newWindowWidth+2, (addY<screen->BarHeight) ? barHeight : ((screenFont->tf_YSize>ICONHEIGHT) ? screenFont->tf_YSize + 4 : ICONHEIGHT + 4 ));
            break;
        case 1: // ICONIC
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - ICONWIDTH + addX - 4, 1 + addY, ICONWIDTH+4, ICONHEIGHT+4);
            break;
        case 2: // Digital
            ChangeWindowBox(Window, IntuitionBase->ActiveScreen->Width - PANELWIDTH + 1 + addX, 1 + addY, PANELWIDTH-1, PANELHEIGHT);
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

    // bar style
    if(style==BAR)
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
        
    }

    // ICONIC style
    if(style==ICONIC)
    {
        SetAPen(RP, textColor);
        RectFill(RP, 0, 0, ICONWIDTH+3, ICONHEIGHT+3);
        SetAPen(RP, styleColor);
        RectFill(RP, 1, 1, ICONWIDTH+2, ICONHEIGHT+2);

    }

    if(style==DIGITAL)
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

        strcpy(strdeg, "X");
        // write deg.
        for (hane = strlen(wdata[2])-3, iii=2; hane >= 0; hane--, iii--)
        {
            strdeg[0] = wdata[2][hane];

            deg = (STREQUAL(strdeg, "-")) ? 10 : atoi(strdeg);

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
        mode = (STREQUAL(unit,"metric"))?0:1;

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
        
    }

    // for testing => indx=?;
    // draw weather icon
    if((!STREQUAL(displayIcon, "0"))||(style!=BAR))
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
    
    wdata = getArray(weatherText, "|", 4);
    
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
    CloseLibrary(IntuitionBase);
    CloseLibrary(GfxBase);
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
        sprintf(location, "%s", buffer);
        location[strlen(location)-1] = '\0';    // location

        FGets(fp, buffer, BUFFER/8);
        sprintf(unit, "%s", buffer);
        unit[strlen(unit)-1] = '\0';    // unit

        FGets(fp, buffer, BUFFER/8);
        sprintf(lang, "%s", buffer);
        lang[strlen(lang)-1] = '\0';    // lang
        strcpy(slang, lang);
        slang[2]='\0';

        FGets(fp, buffer, BUFFER/8);
        sprintf(apikey, "%s", buffer);
        apikey[strlen(apikey)-1] = '\0';    // apikey

        FGets(fp, buffer, BUFFER/8);
        sprintf(days, "%s", buffer);
        days[strlen(days)-1] = '\0';    // days

        FGets(fp, buffer, BUFFER/8);
        sprintf(months, "%s", buffer);
        months[strlen(months)-1] = '\0';    // months

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayIcon, "%s", buffer);
        displayIcon[strlen(displayIcon)-1] = '\0';    // displayIcon

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayLocation, "%s", buffer);
        displayLocation[strlen(displayLocation)-1] = '\0';    // displayLocation

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayDescription, "%s", buffer);
        displayDescription[strlen(displayDescription)-1] = '\0';    // displayDescription

        FGets(fp, buffer, BUFFER/8);
        sprintf(displayDateTime, "%s", buffer);
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


        strcpy(tm, months);
        strcpy(td, days);
        amonths= getArray(tm, ",", 12);
        adays = getArray(td, ",", 7);
        
		Close(fp);
	}
}

// save settings
void SavePrefs(void)
{
	fp = Open(PREFFILEPATH, MODE_NEWFILE);
	if (fp)
	{
		sprintf(fpStr, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", location, unit, lang, apikey, days, months, displayIcon, displayLocation, displayDescription, displayDateTime, addX, addY, addTextY, textColor, backgroundColor, sunColor, cloudColor, lcloudColor, dcloudColor, style, styleColor, shadowColor);
		FPuts(fp, fpStr);
		Close(fp);
	}
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
		IDBCANCEL,
        IDBSAVEANDCLOSE,
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
		// end of gadget id
		IDLAST
	};
	
	struct Gadget *gad[IDLAST-1];
	struct Gadget *vcreatePreferencesParent = NULL;
	Object *createPreferencesWinObj = NULL;
	struct Window *createPreferencesWin = NULL;
	ULONG signalcreatePreferences, resultcreatePreferences;
	signed char donecreatePreferences;
	UWORD codecreatePreferences;

    ULONG dIcon, dDescription, dLocation, dDatetime, dAbbDays=TRUE, dAbbMonths=TRUE;

    int localDays=0, localMonths=0;
	
	struct List *chooserUnit;
    struct List *tablabels;
	chooserUnit = ChooserLabels( "metric","imperial", NULL );
    tablabels = ClickTabs("Days & Months", "Display", "Colors", "Position & Style", "ENV Variables", "API Key", NULL);

    dIcon = STREQUAL(displayIcon, "1")?TRUE:FALSE;
    dLocation = STREQUAL(displayLocation, "1")?TRUE:FALSE;
    dDescription = STREQUAL(displayDescription, "1")?TRUE:FALSE;
    dDatetime = STREQUAL(displayDateTime, "1")?TRUE:FALSE;

    ClearMenuStrip(Window);
	
	if (!(createPreferencesWinObj = NewObject
    (   WINDOW_GetClass(),
        NULL,
		WA_PubScreen,             screen,
        WA_Title,           	  "Preferences - AmiWeatherForecasts",
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
        WA_Left,                  0,
        WA_Top,                   0,
        WA_SizeBRight,            FALSE,
        WA_SizeBBottom,           FALSE,
        WA_NewLookMenus,          TRUE,
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
															GA_Text, "_Get My Location",
															GA_RelVerify, TRUE,
															GA_ID, IDGETMYLOCATION,
                                                    End,
                                                End,
                                                CHILD_Label,  LabelObject,
                                                                LABEL_Justification, 0,
                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                LABEL_Text, "  Location ",
                                                End,

                                                LAYOUT_AddChild, HGroupObject,
                                                    // UNIT
                                                    LAYOUT_AddChild, gad[IDINTEGERUNIT-1] = (struct Gadget *) ChooserObject,
                                                                    GA_ID,  IDINTEGERUNIT,
                                                                    GA_RelVerify, TRUE,
                                                                    CHOOSER_PopUp, TRUE,
                                                                    CHOOSER_MaxLabels, 2,
                                                                    CHOOSER_Offset, 0,
                                                                    CHOOSER_Selected, (STREQUAL(unit,"metric"))?0:1,
                                                                    CHOOSER_Labels, chooserUnit,
                                                    End,                                                
                                                    CHILD_Label, LabelObject,
                                                                    LABEL_Justification, 0,
                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                    LABEL_Text, "      Unit ",
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
                                                                    LABEL_Text, "  Language ",
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
                                                                                                    LABEL_Text, "      Days ",
                                                                                    End,

                                                                                    // Get days and months
                                                                                    LAYOUT_AddChild, HGroupObject,
                                                                                        LAYOUT_AddChild, gad[IDGETMYDAYS-1] = ButtonObject,
                                                                                                GA_Text, "Get _Days",
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDGETMYDAYS,
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDAYS-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                    GA_ID, IDCHECKBOXDAYS,
                                                                                                                    GA_RelVerify, TRUE,
                                                                                                                    GA_Selected, TRUE,
                                                                                                                    GA_Text, "Use long abbreviations for days     ",
                                                                                                                    CHECKBOX_TextPen, 1,
                                                                                                                    CHECKBOX_FillTextPen, 1,
                                                                                                                    CHECKBOX_BackgroundPen, 0,
                                                                                                                    CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,

                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_Text, "           ",
                                                                                    End,

                                                                                    LAYOUT_AddChild, SpaceObject,
                                                                                                    SPACE_Transparent,TRUE,
                                                                                                    SPACE_BevelStyle, BVS_NONE,
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
                                                                                                    LABEL_Text, "    Months ",
                                                                                    End,

                                                                                    

                                                                                    LAYOUT_AddChild, HGroupObject,
                                                                                        LAYOUT_AddChild, gad[IDGETMYMONTHS-1] = ButtonObject,
                                                                                                GA_Text, "Get _Months",
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDGETMYMONTHS,
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXMONTHS-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                    GA_ID, IDCHECKBOXMONTHS,
                                                                                                                    GA_RelVerify, TRUE,
                                                                                                                    GA_Selected, TRUE,
                                                                                                                    GA_Text, "Use months of the year abbreviations",
                                                                                                                    CHECKBOX_TextPen, 1,
                                                                                                                    CHECKBOX_FillTextPen, 1,
                                                                                                                    CHECKBOX_BackgroundPen, 0,
                                                                                                                    CHECKBOX_TextPlace, PLACETEXT_RIGHT,
                                                                                        End,

                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_Text, "           ",
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
                                                                                                        LABEL_Text, " Position Y of the text ",
                                                                                        End,

                                                                                        LAYOUT_AddChild, gad[IDSETTEXTPOSBUTTON-1] = ButtonObject,
                                                                                                GA_Text, " Set Position ",
                                                                                                GA_RelVerify, TRUE,
                                                                                                GA_ID, IDSETTEXTPOSBUTTON,
                                                                                        End,
                                                                                    End,

                                                                                    // Displays icon
                                                                                        LAYOUT_AddChild, gad[IDCHECKBOXDISPLAYICON-1] = (struct Gadget *) CheckBoxObject,
                                                                                                                        GA_ID, IDCHECKBOXDISPLAYICON,
                                                                                                                        GA_RelVerify, TRUE,
                                                                                                                        GA_Selected, dIcon,
                                                                                                                        GA_Text, "Displays icon       ",
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
                                                                                                                        GA_Text, "Displays location   ",
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
                                                                                                                        GA_Text, "Displays description",
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
                                                                                                                        GA_Text, "Displays date & time",
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
                                                                                        GA_Text, "If desired to change colors, use Prefs/Palette.",
                                                                                        BUTTON_BackgroundPen, 4,
                                                                                        BUTTON_TextPen, 1,
                                                                                        GA_ReadOnly, TRUE,
                                                                                End,
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, "       Note ",
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
                                                                                                    LABEL_Text, "       Text ",
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
                                                                                                    LABEL_Text, " Background ",
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
                                                                                                    LABEL_Text, "        Sun ",
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
                                                                                                    LABEL_Text, "      Cloud ",
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
                                                                                                    LABEL_Text, "Light Cloud ",
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
                                                                                                    LABEL_Text, " Dark Cloud ",
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
                                                                                                    LABEL_Text, "      Style ",
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
                                                                                                    LABEL_Text, "     Shadow ",
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
                                                                                        GA_Text, "Select the window and drag to the new position",
                                                                                        BUTTON_BackgroundPen, 4,
                                                                                        BUTTON_TextPen, 1,
                                                                                        GA_ReadOnly, TRUE,
                                                                                End,
                                                                                CHILD_Label,  LabelObject,
                                                                                                LABEL_Justification, 0,
                                                                                                LABEL_SoftStyle, FSF_BOLD,
                                                                                                LABEL_Text, " Position ",
                                                                                End,

                                                                                LAYOUT_AddChild, HGroupObject,

                                                                                    LAYOUT_AddChild, gad[IDSTYLE-1] = (struct Gadget *) ChooserObject,
                                                                                                    GA_ID,  IDSTYLE,
                                                                                                    GA_RelVerify, TRUE,
                                                                                                    CHOOSER_PopUp, TRUE,
                                                                                                    CHOOSER_MaxLabels, 3,
                                                                                                    CHOOSER_Offset, 0,
                                                                                                    CHOOSER_Selected, style,
                                                                                                    CHOOSER_LabelArray, styles,
                                                                                    End,                                                
                                                                                    CHILD_Label, LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, "    Style ",
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
                                                                                                        LABEL_Text, " ",
                                                                                End,

                                                                                CHILD_Label, LabelObject,
                                                                                                        LABEL_Justification, 0,
                                                                                                        LABEL_SoftStyle, FSF_BOLD,
                                                                                                        LABEL_Text, "Environment variables for weather forecast data",
                                                                                End,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, " CurrentTemperature ",
                                                                                            BUTTON_BackgroundPen, 7,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                End,                                                                                

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, " WeatherDescription ",
                                                                                            BUTTON_BackgroundPen, 3,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                End,

                                                                                LAYOUT_AddChild, ButtonObject,
                                                                                            GA_Text, " Location ",
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
                                                                                            GA_Text, "Use your own OpenWeather API key",
                                                                                            BUTTON_BackgroundPen, 4,
                                                                                            BUTTON_TextPen, 1,
                                                                                            GA_ReadOnly, TRUE,
                                                                                    End,
                                                                                    CHILD_Label,  LabelObject,
                                                                                                    LABEL_Justification, 0,
                                                                                                    LABEL_SoftStyle, FSF_BOLD,
                                                                                                    LABEL_Text, "      Note ",
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
                                                                                                    LABEL_Text, "    APIKEY ",
                                                                                    End,

                                                                                    LAYOUT_AddChild, SpaceObject,
                                                                                            SPACE_Transparent,TRUE,
                                                                                            SPACE_BevelStyle, BVS_NONE,
                                                                                    End,
                                                                                    CHILD_WeightedHeight, 0,
                                                                        
                                                                        End,
                                                                    End,
                                                End,
                                                                   
                                                LAYOUT_AddChild, SpaceObject,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                End,

                                                // Buttons
												LAYOUT_AddChild, HGroupObject,
													
                                                    LAYOUT_AddChild, gad[IDBCANCEL-1] = ButtonObject,
															GA_Text, "_Cancel",
															GA_RelVerify, TRUE,
															GA_ID, IDBCANCEL,
                                                    End,

                                                    LAYOUT_AddChild, SpaceObject,
                                                            GA_ID, 97,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                    End,

													LAYOUT_AddChild, gad[IDBSAVEANDCLOSE-1] = ButtonObject,
															GA_Text, "_Save & Close",
															GA_RelVerify, TRUE,
															GA_ID, IDBSAVEANDCLOSE,
                                                    End,
												End,

                                                LAYOUT_AddChild, SpaceObject,
                                                            SPACE_Transparent,TRUE,
                                                            SPACE_BevelStyle, BVS_NONE,
                                                End,

											End
	)))
	{
		donecreatePreferences = TRUE;
	}
		
	createPreferencesWin = RA_OpenWindow(createPreferencesWinObj);
	GetAttr(WINDOW_SigMask, createPreferencesWinObj, &signalcreatePreferences);	
	
	donecreatePreferences=FALSE;
	
	while(!donecreatePreferences)
    {   
		Wait(signalcreatePreferences | (1 << createPreferencesWin->UserPort->mp_SigBit));

        while ((resultcreatePreferences = DoMethod(createPreferencesWinObj, WM_HANDLEINPUT, &codecreatePreferences)) != WMHI_LASTMSG)
        {   
            STRPTR vLocation, vDays, vMonths, vApikey;
            ULONG iLang, iUnit;
            char tm[BUFFER], td[BUFFER];
            int Control, Count, x;

            switch (resultcreatePreferences & WMHI_CLASSMASK)
            {
                case WMHI_CLOSEWINDOW:
                    donecreatePreferences = TRUE;
                break;
				
				case WMHI_GADGETUP:
                    switch(resultcreatePreferences & WMHI_GADGETMASK)
                    {                     
                        case IDBSAVEANDCLOSE:

                            GetAttr(STRINGA_TextVal, gad[IDSTRINGLOCATION-1], (ULONG*)&vLocation);
                            sprintf(location, "%s", vLocation);

                            GetAttr(CHOOSER_Selected, gad[IDINTEGERUNIT-1], (ULONG*)&iUnit);
                            sprintf(unit, "%s", iUnit==0 ? "metric":"imperial");

                            GetAttr(CHOOSER_Selected, gad[IDSTRINGLANGUAGE-1], (ULONG*)&iLang);
                            sprintf(lang, "%s", clanguages[iLang]);
                            strcpy(slang, lang);
                            slang[2]='\0';
					        
                            GetAttr(STRINGA_TextVal, gad[IDSTRINGDAYS-1], (ULONG*)&vDays);
                            sprintf(days, "%s", vDays);

                            GetAttr(STRINGA_TextVal, gad[IDSTRINGMONTHS-1], (ULONG*)&vMonths);
                            sprintf(months, "%s", vMonths);

                            GetAttr(STRINGA_TextVal, gad[IDSTRINGAPIKEY-1], (ULONG*)&vApikey);
                            sprintf(apikey, "%s", vApikey);

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYICON-1], &dIcon);
                            sprintf(displayIcon, dIcon ? "1":"0");

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYLOCATION-1], &dLocation);
                            sprintf(displayLocation, dLocation ? "1":"0");

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYDESCRIPTION-1], &dDescription);
                            sprintf(displayDescription, dDescription ? "1":"0");

                            GetAttr(GA_Selected, gad[IDCHECKBOXDISPLAYDATETIME-1], &dDatetime);
                            sprintf(displayDateTime, dDatetime ? "1":"0");

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
                                EasyRequest(Window, &DayControl, NULL, NULL);
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
                                EasyRequest(Window, &MonthControl, NULL, NULL);
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
                                EasyRequest(Window, &ColorControl, NULL, NULL);
                                break;
                            }

                            SavePrefs();
                            
                            strcpy(tm, months);
                            strcpy(td, days);

                            amonths= getArray(tm, ",", 12);
                            adays = getArray(td, ",", 7);

                            // get weather URL
                            strcpy(weatherURL, getURL(apikey, convertToUTF8(location), unit, slang));
                            update();

                            donecreatePreferences = TRUE;
                            
                        break;
						
						case IDBCANCEL:
                            donecreatePreferences = TRUE;
                        break;

                        case IDGETMYDAYS:
                            GetAttr(GA_Selected, gad[IDCHECKBOXDAYS-1], &dAbbDays);
                            localDays=(dAbbDays)?0:7;

                            strcpy(td, "");
                            for(x=8-localDays;x<15-localDays;x++)
                            {
                                strcat(td, GetLocaleStr(currentLocale, x));
                                if(x!=14-localDays) strcat(td, ",");
                            }
                            SetGadgetAttrs(gad[IDSTRINGDAYS-1], createPreferencesWin, NULL, STRINGA_TextVal, td, TAG_DONE); 
                        break;

                        case IDGETMYMONTHS:
                            GetAttr(GA_Selected, gad[IDCHECKBOXMONTHS-1], &dAbbMonths);
                            localMonths=(dAbbMonths)?0:12;
                            strcpy(tm, "");
                            for(x=27-localMonths;x<39-localMonths;x++)
                            {
                                strcat(tm, GetLocaleStr(currentLocale, x));
                                if(x!=38-localMonths) strcat(tm, ",");
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

                                    sprintf(ipapiurl, "http://ip-api.com/csv/%s", iptxt);
                                    httpget(ipapiurl, MYLOCTXT);

                                    fp = Open(MYLOCTXT, MODE_OLDFILE);
                                    if(fp)
                                    {
                                        FGets(fp, locationCsv, sizeof(locationCsv));
                                        locationCsv[strlen(locationCsv)-1] = '\0';
                                        Close(fp);
                                    }

                                    locationData = getArray(locationCsv, ",", 14);

                                    if(STREQUAL(locationData[0], "success"))
                                    {
                                        strcpy(locationCsv, convertToLatin(locationData[5]));
                                        strcat(locationCsv,",");
                                        strcat(locationCsv, locationData[2]);
                                        SetGadgetAttrs(gad[IDSTRINGLOCATION-1], createPreferencesWin, NULL, STRINGA_TextVal, locationCsv, TAG_DONE);  
                                    }
                                }
                                
                                SetGadgetAttrs(gad[IDGETMYLOCATION-1], createPreferencesWin, NULL, GA_Disabled, FALSE, TAG_DONE);

                                if (fileExist(IPTXT)||fileExist(MYLOCTXT))
                                {
                                    Execute("SYS:C/delete RAM:T/#?.txt >NIL:",NULL,NULL);
                                }
                                

                            }
                            else
                            {                         
                               EasyRequest(Window, &GetExtIPMessage, NULL, NULL);
                            }
                            
                        break;

                        case IDSETTEXTPOSBUTTON:
                            if(style==BAR) // if only bar style
                            {
                                GetAttr(INTEGER_Number, gad[IDINTEGERaddTextY-1], &addTextY);
                                writeInfo();
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
    SetMenuStrip(Window, amiMenu);
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
                                                                                                                                  
    sprintf(cmd, "copy %s to RAM:T/ QUIET >NIL:", USERSTARTUP);
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
        Execute("SYS:C/delete "COPIEDUSERSTARTUP" >NIL:",NULL,NULL);
    }
    
}
