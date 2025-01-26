/***************************************************************************/
/* F r e e C o d e                                                         */
/*                                                                         */
/* THIS CODE IS FREEWARE AND AS SUCH YOU ARE PERMITTED TO DO WHAT YOU WISH */
/* WITH IT. THESE PROGRAMS ARE PROVIDED AS IS WITHOUT ANY WARRANTY,        */
/* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO FITNESS FOR ANY      */
/* PARTICULAR PURPOSE.                                                     */
/*                                                                         */
/* However, we would appriciate if you shared any enhancements to us       */
/* Please send them to www.jmast.se/free/ and we will include them in      */
/* future distributions.                                                   */
/*                                                                         */
/***************************************************************************/
/**************************************************************
                                                        000808
   Include File: STLIBPMC.H
   Description:  Helper functions for OS/2 PM programs.
**************************************************************/

/* WINDOW MANAGEMENT FUNCTIONS
***********************************/
void SetWindowTitle (HWND hwnd, PSZ szNewTitle);
void EnableWindowUpdate(HWND hwnd, int iFlag);
BOOL StartProgram(HWND hwndNotify, char *szTitle, char *szProg, char *szParam, char *szStartDir);
void WinPrint (HWND hwnd, HPS hps, USHORT usRow, USHORT usCol, PSZ szMessage);
void WinCenterWindow(HWND hwnd);
void WinSetDlgLen(HWND hwnd, int iItem, int iLen);
void WinSelDlgEntry(HWND hwnd, int iItem);
LHANDLE WinAddToTaskList(HWND hwndFrame, char *szTitle);
void WinRemoveFromTaskList(HWND hwndFrame);
void WinEmphaseDrag (HWND hwnd, BOOL bFlag);
void WinMaximizeWindow (HWND hwnd);
void WinMinimizeWindow (HWND hwnd);
void WinRestoreWindow (HWND hwnd);
HWND WinQueryWarpCenterHwnd(HWND hwnd);
void WinDraw3D (HPS hps, RECTL rcl, PRECTL prclNew, BOOL bHighLight);


/* SIMPLIFIED MESSAGEBOXES
***********************************/
#define MSGBOX_OK       0
#define MSGBOX_YES      1
#define MSGBOX_NO       2

BOOL MsgBox (HWND hwnd, USHORT usBoxType, PSZ szTitle, PSZ szMessage);
void DEBUG (PSZ szMessage);
void EXTDEBUG (HAB hab, ULONG ulRetCode, PSZ szMessage);


/* BITMAPS
************************************/
HBITMAP LoadBitmapFromFile(HWND hwnd, char *szFileName);
HBITMAP WinLoadBitmap(HMODULE hMod, int iId, int iWidth, int iHeight);
int WinQueryBitmapX(HBITMAP hbm);
int WinQueryBitmapY(HBITMAP hbm);

/* HELP FOR MENUS
***********************************/
HWND MenuHwnd(HWND hwnd);
void MenuEnableItem (HWND hwndMenu, USHORT usMenuId, BOOL bEnabled);
void MenuCheckItem (HWND hwndMenu, USHORT usMenuId, BOOL bCheck);
void MenuSetItemText (HWND hwndMenu, USHORT usMenuId, PSZ szItemText);
PSZ MenuGetItemText (HWND hwndMenu, USHORT usMenuId);
void MenuInsertItem(HWND hwndMenu, USHORT usTopMenuId, USHORT usPosition, USHORT usNewMenuId, PSZ szItemText);
void MenuRemoveItem(HWND hwndMenu, USHORT usMenuId);
BOOL ShowPopupMenu (HWND hwndClient, ULONG ulMenuId);


/* POINTERS (ICONS/CURSORS)
***********************************/
void IconSize (PULONG pxIcon, PULONG pyIcon);
void PointerNormal(void);
void PointerWait(void);


/* CLIPBOARD (TEXT AND BITMAPS)
***********************************/
BOOL ClipQueryText(HWND hwnd);
char* ClipToText(HWND hwnd);
void TextToClip(HWND hwnd, PSZ szText);
BOOL ClipQueryBitmap(HAB hab);
BOOL BitmapToClip(HAB hab, HBITMAP hbm);
HBITMAP ClipToBitmap(HAB hab);


/* FONTs
***********************************/
void QueryFontSize(HWND hwnd, PULONG pulFontWidth, PULONG pulFontHeight);
ULONG SelectFont(BOOL bFlag, HPS hps, char *szFontName, int iFontSize);
BOOL ScaleFont (HPS hpsPrinter, USHORT usFontSize);


/* MLE FUNCTIONS
***********************************/
BOOL MLEImport(HWND hwndMLE, char *pszText, ULONG ulTextLength);
ULONG MLEExportLength(HWND hwndMLE);
BOOL MLEExport(HWND hwndMLE, char *pszText, ULONG ulTextLength);


/* PROFILE FUNCTIONS
***********************************/
BOOL ReadProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize);
BOOL WriteProfile (HAB hab, char* szFile, char* szApp, char* szKey, char* szItem, ULONG iItemSize);


/* STANDARD FOR ALL "BARS"
***********************************/
#define STBORDERSPACE                   5
#define STBORDERWIDTH                   1

#define ST_FONT_DEFAULT                 "8.Helv"
#define ST_FONT_HOVER                   ST_FONT_DEFAULT

#define ST_COLOR_BACKGROUND             CLR_PALEGRAY
#define ST_COLOR_BORDERDARK             CLR_DARKGRAY
#define ST_COLOR_BORDERLIGHT            CLR_WHITE
#define ST_COLOR_BORDERBLACK            CLR_BLACK
#define ST_COLOR_TEXT                   CLR_BLACK

#define ST_COLOR_HOVERFOREGROUND        CLR_BLACK
#define ST_COLOR_HOVERBACKGROUND        CLR_YELLOW



/* HOVERBOX
***********************************/
#define HOVER_LEFT      0
#define HOVER_CENTER    1
#define HOVER_RIGHT     2

#define HOVER_ABOVE     TRUE
#define HOVER_BELOW     FALSE

typedef struct _STHOVERBOX    /* hoverboxitem */
        {
        HWND            hwndParent;
        HWND            hwndHoverBox;
        HWND            hwndHoverBoxChild;

        char            szText[128];
        char            szFont[65];
        int             iColorText;
        int             iColorBack;
        int             iAlign;
        int             iAlignInt;
        int             ySize;
        int             xSize;
        BOOL            bMotion;

        ULONG           idTimer;
        HWND            hwndHit;
        int             iHitId;
        int             (*fnUserTest)(void);
        BOOL            bShow;
        BOOL            bFirstShow;
        BOOL            bPosition;
        } STHOVERBOX;

typedef STHOVERBOX *PSTHOVERBOX;

HWND CreateHoverBox (HWND hwndParent, PSTHOVERBOX phb);
BOOL DestroyHoverBox (PSTHOVERBOX phb);

void SetHoverBoxPosition(PSTHOVERBOX phb, BOOL bPosition);
void SetHoverBoxFont(PSTHOVERBOX phb, char* szFont);
void SetHoverBoxColor (PSTHOVERBOX phb, int iColorText, int iColorBack);
void SetHoverBoxAlignment (PSTHOVERBOX phb, int iAlign);
void SetHoverBoxText (PSTHOVERBOX phb, char* szText);
void SetHoverBoxMotion (PSTHOVERBOX phb, BOOL bMotion);
void SetHoverCallBack (PSTHOVERBOX phb, int (*fnUserTest)(void));
void ShowHoverBox(PSTHOVERBOX phb, HWND hwndHit, int iHitId, BOOL bShow);


/* BUTTON
***********************************/
typedef struct _STBTN    /* stbtn */
        {
        HWND            hwndOwner;
        HWND            hwndClient;
        HWND            hwndButton;
        int             iBtnId;
        int             iBtnType;

        HBITMAP         hbm;
        int             iMethod;

        char            szButtonText[128];
        char            szFont[64];
        long            lColorFore;
        long            lColorBack;

        BOOL            bShow;
        int             iBtnState;
        int             iFrameStyle;

        BOOL            bUseMenu;
        HMODULE         hMenuMod;
        int             iMenuResId;

        BOOL            bUseHover;
        BOOL            bHoverPos;
        char            szHoverText[128];

        STHOVERBOX      hb;
        } STBTN;
typedef STBTN *PSTBTN;


#define STBTN_BITMAP      0x002
#define STBTN_TEXT        0x007
#define STBTN_DATETIME    0x009

#define STBTN_ENABLED     0
#define STBTN_HIGHLITED   1
#define STBTN_DISABLED    2

#define STBTN_ASIS        1
#define STBTN_STRECH      2
#define STBTN_CENTER      3

#define STBTN_FRAMENONE   0
#define STBTN_FRAMESIMPLE 1
#define STBTN_FRAME3D     2

#define WM_STBUTTONCLICK  WM_USER + 1110L

HWND STButtonCreate(HWND hwndOwner, PSTBTN pBtn, int iBtnId, int iBtnType, BOOL bUseHover);
BOOL STButtonDestroy(PSTBTN pBtn);
void STButtonReSize(PSTBTN pBtn, RECTL rclNewSize);
BOOL STButtonShow(PSTBTN pBtn, BOOL bShow);
BOOL STButtonSetHover(PSTBTN pBtn, char *szHoverText, BOOL bPosition);

BOOL STButtonSetPopupMenu(PSTBTN pBtn, BOOL bUseMenu, HMODULE hMenuMod, int iMenuResId);
BOOL STButtonSetColor(PSTBTN pBtn, long lColorFore, long lColorBack);
BOOL STButtonSetText(PSTBTN pBtn, char *szText, char *szFont, long lColorFore, long lColorBack);
BOOL STButtonSetBitmap(PSTBTN pBtn, HBITMAP hbm, int iMethod);
int STButtonSetFrameStyle(PSTBTN pBtn, int iStyle);
int STButtonSetState(PSTBTN pBtn, int iState);
int STButtonGetState(PSTBTN pBtn);

/* CONTROL WINDOW
***********************************/

typedef struct _STCONTROLITEM    /* stcontrolitem */
        {
        int             iItemId;                /* Identification */
        int             iItemType;              /* Subtype */

        HMODULE         hMod;
        ULONG           ulResourceId;           /* Generic variables */
        int             hResHandle;
        char            szText[128];
        int             iForeColor;
        int             iBackColor;

        char            szFlyOverText[64];      /* Fly over text */

        } STCONTROLITEM;

typedef STCONTROLITEM *PSTCONTROLITEM;


typedef struct _STCONTROL    /* stcontrol */
        {
        HWND            hwndOwner;              /* Window to recieve messages */
        HWND            hwndControl;            /* Window around or control */
        HWND            hwndIntControl;         /* Control window inside */

        int             iItemId;                /* Identification */
        int             iControlId;             /* PM control id, MUST be unique */

        BOOL            bShow;                  /* Show control (around) window) */

        int             iType;                  /* Type of control */
        int             iCreateFlags;           /* Control create flags */

        int             iItemCount;             /* Items array size for multi controls */


        int             iResourceId1;            /* Generic variables */
        int             iResourceId2;
        ULONG           hResHandle1;
        ULONG           hResHandle2;
        char            szText1[128];
        char            szText2[128];
        int             iCurrentItem;
        int             iForeColor;
        int             iBackColor;
        char            szFont[33];
        int             iEffect;

        BOOL            bUseHover;              /* Fly over text */
        STHOVERBOX      sthb;
        char            szFlyOverText1[64];
        char            szFlyOverText2[64];

        int             iTimerId;               /* Animation */
        int             iTimerTicks;

        int             iCurrent;               /* Progress bar */
        int             iMaximum;
        BOOL            bShowText;

        MPARAM          mp1;                    /* mp from wndproc */
        MPARAM          mp2;

        HPS             hpsPaint;               /* hps from WM_PAINT */

        PSTCONTROLITEM  pStCi;                  /* Multiitem control settings */
        } STCONTROL;

typedef STCONTROL *PSTCONTROL;


#define WM_CTRLMESSAGE           WM_USER + 1110L

#define IDSTCTRL_NONE             0
#define IDSTCTRL_TEXT            10
#define IDSTCTRL_BUTTON          11
#define IDSTCTRL_ENTRYFIELD      12
#define IDSTCTRL_CHECK           13
#define IDSTCTRL_RADIO           14
#define IDSTCTRL_COMBO           15
#define IDSTCTRL_BITMAP          30
#define IDSTCTRL_STATE           31
#define IDSTCTRL_MANYSTATE       32
#define IDSTCTRL_ANIMATE         50
#define IDSTCTRL_PROGRESS        51
#define IDSTCTRL_DATETIME        52
#define IDSTCTRL_PRINT           53
#define IDSTCTRL_OWNERDRAW       99

#define IDSTCTRL_EFFECT_2D        0
#define IDSTCTRL_EFFECT_RAISED    1
#define IDSTCTRL_EFFECT_SUNKEN    2

#define IDSTCTRL_SUB_NONE         0
#define IDSTCTRL_SUB_TEXT         1
#define IDSTCTRL_SUB_BITMAP       2
#define IDSTCTRL_SUB_HBITMAP      3
#define IDSTCTRL_SUB_COLOR        4
#define IDSTCTRL_SUB_OWNERDRAW   99


HWND CreateControl(HWND hwndOwner, PSTCONTROL pStCtrl, int iType, int iItemId, int iCtrlId, int iCreateFlags, BOOL bUseHover, int iItemCount);
BOOL DestroyControl(PSTCONTROL pStCtrl);

void ControlItemRealloc(PSTCONTROL pStCtrl, int iItemCount);

BOOL ControlSetItem(PSTCONTROL pStCtrl, int iItem, int iItemId, int iItemType, HMODULE hMod, ULONG ulResId, char *szItemText, int iForeColor, int iBackColor, char *szFlyOverText);

void ControlSetColor(PSTCONTROL pStCtrl, int iForeColor, int iBackColor);
void ControlSetFont(PSTCONTROL pStCtrl, char *szFont);
void ControlSetFlyOverText(PSTCONTROL pStCtrl, char *szFlyOverText1, char *szFlyOverText2);
void ControlSetEffect(PSTCONTROL pStCtrl, int iEffect);
int ControlResize(PSTCONTROL pStCtrl, int yPos, int xPos, int ySize, int xSize);
BOOL ControlShow(PSTCONTROL pStCtrl, BOOL bShow);

void ControlSetText(PSTCONTROL pStCtrl, char *szText1, char *szText2);
char *ControlGetText(PSTCONTROL pStCtrl);
void ControlClear(PSTCONTROL pStCtrl);

/* Animation */
void AnimateSetTimerTicks(PSTCONTROL pStCtrl, int iTimer);
void AnimateSetTimerId(PSTCONTROL pStCtrl, int iTimerId);

BOOL AnimateStart(PSTCONTROL pStCtrl);
void AnimateStop(PSTCONTROL pStCtrl);


/* SPLITBAR
***********************************/
typedef struct _SPLITBAR    /* splitbaritem */
        {
        HWND            hwndParent;
        HWND            hwndSplitBar;
        HWND            hwndSplitBarChild;
        BOOL            bDirection;
        BOOL            bShow;
        int             iBorderBL;
        int             iBorderTR;
        } SPLITBAR;

typedef SPLITBAR *PSPLITBAR;

#define SBP_HORZ                FALSE
#define SBP_VERT                TRUE
#define WM_SPLITBARMOVED        WM_USER + 1101L

HWND CreateSplitBar (HWND hwndParent, BOOL bDirection, PSPLITBAR psb);
BOOL DestroySplitBar (PSPLITBAR psb);
ULONG PositionSplitBar (PSPLITBAR psb, int iBorderBL, int iBorderTR, ULONG ulSplitBarPos);
BOOL ShowSplitBar(PSPLITBAR psb, BOOL bShow);


/* BUTTONBAR
***********************************/
typedef struct _STBTNBARITEM    /* stbtnbaritem */
        {
        VOID            *pstOwner;
        int             iItemId;
        int             iItemType;
        USHORT          usState;
        HMODULE         hMod;
        ULONG           ulResId1;
        int             sizeIco;
        HWND            hwndButton;
        int             hResHandle1;
        char            szFlyoverText1[32];
        ULONG           ulResId2;
        int             hResHandle2;
        char            szFlyoverText2[32];
        } STBTNBARITEM;
typedef STBTNBARITEM FAR *PSTBTNBARITEM;

typedef struct _STBTNBAR    /* stbtnbar */
        {
        HWND            hwndOwner;
        HWND            hwndButtonBar;
        USHORT          usItemCount;
        ULONG           ulBarType;
        BOOL            bShow;
        int             yIco;
        int             xIco;
        HWND            hwndBtnCurrent;
        PSTBTNBARITEM   pBtnItems;
        } STBTNBAR;
typedef STBTNBAR *PSTBTNBAR;


#define BBP_LEFT        0
#define BBP_TOP         1

#define BBT_BITMAP      0x002
#define BBT_SPACE       0x007

#define BBS_ENABLED     0
#define BBS_HIGHLITED   1
#define BBS_DISABLED    2

#define WM_BUTTONBARSELECT  WM_USER + 1100L

HWND CreateButtonBar(HWND hwndOwner, PSTBTNBAR pBtnBar, int iBarType, USHORT usItems, int yIco, int xIco);
BOOL DestroyButtonBar(PSTBTNBAR pBtnBar);

BOOL SetButtonBarItem(PSTBTNBAR pBtnBar, int iItem, int iItemId, int iItemType, HMODULE hMod, ULONG ulResId1, ULONG ulResId2, ULONG ulSizeIco, USHORT usState, char *szFlyoverText1, char *szFlyoverText2);
void DrawButtonBar(PSTBTNBAR pBtnBar, int ySize);

ULONG SizeButtonBar(PSTBTNBAR pBtnBar, ULONG ulBtnLen);

BOOL ShowButtonBar(PSTBTNBAR pBtnBar, BOOL bShow);

BOOL HiliteButton(PSTBTNBAR pBtnBar, int iItem, BOOL bHilite);
BOOL EnableButton(PSTBTNBAR pBtnBar, int iItem, BOOL bEnable);
BOOL IsButtonHilited(PSTBTNBAR pBtnBar, int iItem);
BOOL IsButtonEnabled(PSTBTNBAR pBtnBar, int iItem);


/* STATUS BAR
***********************************/
#define WM_STATUSREDRAW        WM_USER + 1022

#define STATUS_ONE      0
#define STATUS_TWO      1

#define STATUS_LEFT     0
#define STATUS_CENTER   1
#define STATUS_RIGHT    2

#define STATUS_TEXT     1
#define STATUS_OWNER    2


typedef struct _STSTATUSBAR    /* ststatusbar */
        {
        int             iStatusId;
        int             iType;
        BOOL            bIsReg;
        HWND            hwndOwner;
        HWND            hwndStatus;
        HWND            hwndChild1;
        HWND            hwndChild2;
        char            szFont[40];
        int             iType1;
        int             iType2;
        char            szText1[128];
        char            szText2[128];
        int             iColor1;
        int             iColor2;
        int             iAlign1;
        int             iAlign2;
        int             iAlignInt1;
        int             iAlignInt2;
        int             iTwoSplit;
        int             iItemToRedraw;
        BOOL            bShow;
        HWND            hwndRedraw;
        HPS             hpsRedraw;
        } STSTATUSBAR;

typedef STSTATUSBAR *PSTSTATUSBAR;

HWND CreateStatusBar (HWND hwnd, PSTSTATUSBAR pstsb, int iType, int iItem1Type, int iItem2Type);
BOOL DestroyStatusBar (PSTSTATUSBAR pstsb);
void UpdateStatusBarItem(PSTSTATUSBAR pstsb, int iItem);
int SizeStatusBar (PSTSTATUSBAR pstsb, int iMaxLen);
BOOL ShowStatusBar (PSTSTATUSBAR pstsb, BOOL bShow);
void SetStatusBarSplit(PSTSTATUSBAR pstsb, int iSplit);
void SetStatusBarText (PSTSTATUSBAR pstsb, char* szText1, char* szText2);
void SetStatusBarColor (PSTSTATUSBAR pstsb, int iColor1, int iColor2);
void SetStatusBarBackColor (PSTSTATUSBAR pstsb, int iColor1, int iColor2);
void SetStatusBarFont (PSTSTATUSBAR pstsb, char* szFont);
void SetStatusBarAlignment (PSTSTATUSBAR pstsb, int iAlign1, int iAlign2);


/* CONTROL BAR
***********************************/
#define WM_CONTROLBARMSG        WM_USER + 1020

typedef struct _STCONTROLBAR    /* stcontrolbar */
        {
        HWND            hwndParent;
        HWND            hwndControlBar;
        HWND            hwndControl;
        BOOL            bShow;
        int             iControlId;
        int             iControlHeight;
        int             iControlWidth;
        int             iControlDecend;
        } STCONTROLBAR;

typedef STCONTROLBAR *PSTCONTROLBAR;

HWND CreateControlBar (HWND hwnd, PSTCONTROLBAR pcb, char* szType, int iStyles, int iId);
BOOL DestroyControlBar (PSTCONTROLBAR pcb);
int SizeControlBar(PSTCONTROLBAR pcb, int iVertTop, int iMaxLen);
BOOL ShowControlBar(PSTCONTROLBAR pcb, BOOL bShow);
void EnableControlBar(PSTCONTROLBAR pcb, BOOL bEnable);
void SizeCtrlControlBar(PSTCONTROLBAR pcb, int iControlHeight, int iControlWidth, int iDecend);


/* CONTAINER WINDOW
***********************************/
#define WM_CONTAINERMSG         WM_USER + 1021

#define IDSTCNR_HBMFOLDER       0
#define IDSTCNR_HBMFILE         1
#define IDSTCNR_HBMOWNER1       2
#define IDSTCNR_HBMOWNER2       3
#define IDSTCNR_HBMOWNER3       4
#define IDSTCNR_HBMOWNER4       5

typedef struct _STCONTAINER
        {
        HWND                    hwndOwner;
        HWND                    hwndClient;
        HWND                    hwndCnr;
        int                     iIdCnr;
        HMODULE                 hMod;
        HBITMAP                 hbmFolder;
        HBITMAP                 hbmFile;
        HBITMAP                 hbmXtra1;
        HBITMAP                 hbmXtra2;
        HBITMAP                 hbmXtra3;
        HBITMAP                 hbmXtra4;
        HBITMAP                 hbmPlus;
        HBITMAP                 hbmMinus;
        int                     iColorFore;
        int                     iColorBack;
        char                    szFont[64];
        int                     iItems;
        int                     iCount;
        int                     iExtra;
        PVOID                   prcNext;
        PVOID                   pList;
        } STCONTAINER;

typedef STCONTAINER *PSTCONTAINER;

HWND CreateContainer(HWND hwndOwner, PSTCONTAINER pCnr, int iIdCnr, int iExtra, int iItems, int iCreateFlags);
void DestroyContainer(PSTCONTAINER pCnr);
BOOL ContainerExpandTree(PSTCONTAINER pCnr, PVOID pItem);
BOOL ContainerSelect(PSTCONTAINER pCnr, PVOID pItem);
PVOID QueryContainerSelected(PSTCONTAINER pCnr);
BOOL QueryContainerFolder(PVOID prc);
PVOID ContainerQueryRecord(PSTCONTAINER pCnr, int iCommand, PVOID prcItem, PVOID prcParent);
PVOID InsertContainerFolder(PSTCONTAINER pCnr, PVOID prcAfter, PVOID prcAskedParent);
PVOID InsertContainerFile(PSTCONTAINER pCnr, PVOID prcAfter, PVOID prcAskedParent);
BOOL DeleteContainerItem(PSTCONTAINER pCnr, PVOID pItem);
BOOL EmptyContainer(PSTCONTAINER pCnr);
BOOL SetContainerItem(PSTCONTAINER pCnr, PVOID pItem, ULONG flRecordAttr, HMODULE hMod, HBITMAP hbmShow, char *szItemText);
void ContainerOpenEdit(PSTCONTAINER pCnr, PVOID pItem);

void QueryContainerColor(PSTCONTAINER pCnr, int *iColorFore, int *iColorBack);
void SetContainerColor(PSTCONTAINER pCnr, int iColorFore, int iColorBack);
void QueryContainerFont(PSTCONTAINER pCnr, char* szFont);
void SetContainerFont(PSTCONTAINER pCnr, char* szFont);
void SetContainerBitmap(PSTCONTAINER pCnr, HMODULE hMod, ULONG ulFolderRes, ULONG ulFileRes, ULONG ulXtra1, ULONG ulXtra2, ULONG ulXtra3, ULONG ulXtra4);
void SetContainerBitmapSize(PSTCONTAINER pCnr, int iSizeX, int iSizeY);
void SetContainerTreeBitmap(PSTCONTAINER pCnr, HMODULE hMod, ULONG ulPlusRes, ULONG ulMinusRes);
void SetContainerTreeBitmapSize(PSTCONTAINER pCnr, int iSizeX, int iSizeY);
void SetContainerWindowAttr(PSTCONTAINER pCnr, ULONG flWindowAttr);
int QueryContainerTreeLineLen(PSTCONTAINER pCnr);
void SetContainerTreeLineLen(PSTCONTAINER pCnr, int iTreeLineLen);


/* PM-HELP
***********************************/
typedef struct _STHELP    /* sthelp */
        {
        HWND            hwndOwner;
        HWND            hwndHelpInstance;

        char            szHelpLibrary[256];
        char            szHelpWindowTitle[128];
        char            szTutorial[256];

        } STHELP;

typedef STHELP *PSTHELP;


HWND CreateHelp(PSTHELP psth, HWND hwndFrame, int iMainHelpPanelId, char *szWindowTitle, char *szLibraryName, char *szTutorial);
BOOL DestroyHelp(PSTHELP psth);
BOOL DisplayHelp(PSTHELP psth, int iHelpId);
BOOL DisplayHelpIndex(PSTHELP psth);
BOOL DisplayHelpContents(PSTHELP psth);
BOOL SizeHelpWindow(PSTHELP psth, LONG xLeft, LONG yBottom, LONG xRight, LONG yTop);

/* PM-PRINTING
***********************************/
typedef struct _PMPRINTINFO    /* pmprintinfo */
{
   char         szLogAddress[256];
   char         szPort[256];
   char         szDriver[256];
   char         szDriverName[256];
   HDC          hdcPrinter;
   HPS          hpsPrinter;
   char         szFormName[32];
   long         lFormWidth;
   long         lFormHeight;
   long         xLeftClip;
   long         yBottomClip;
   long         xRightClip;
   long         yTopClip;
   long         lWidthPels;
   long         lHeightPels;

} PMPRINTINFO;
typedef PMPRINTINFO FAR *PPMPRINTINFO;

typedef struct _PMQUEUEINFO    /* pmqueueinfo */
{
   char         szName[256];
   char         szComment[256];
   char         szDriverName[256];
   char         szPrinters[256];
   char         szDrivData[1024];
   int          iDrivDataLen;
   BOOL         bDefault;
   char         szQueueProc[256];
   char         szQueueParms[256];
} PMQUEUEINFO;
typedef PMQUEUEINFO FAR *PPMQUEUEINFO;

typedef struct _PMSELPRTINFO    /* pmselprt */
{
   int          lType;
   char         szToken[256];
   int          lCount;
   char         szLogAddress[256];
   char         szDriverName[256];
   char         szDrivData[1024];
   int          iDrivDataLen;
   char         szDataType[256];
   char         szComment[256];
   char         szQueueProcName[256];
   char         szQueueProcParams[256];
   char         szSpoolerParams[256];
   char         szNetworkParams[256];
   char         szPrinters[256];
} PMSELPRTINFO;
typedef PMSELPRTINFO FAR *PPMSELPRTINFO;


BOOL PrtStart(HAB hab, PSZ szDocInfo, ULONG ulCopies, ULONG ulPsUnit, PPMPRINTINFO pprt, PPMSELPRTINFO ppSpi);
USHORT PrtEnd(PPMPRINTINFO pprt);
BOOL PrtForm(PPMPRINTINFO pprt);
BOOL PrtInfoOpen(HAB hab, ULONG ulPsUnit, PPMPRINTINFO pprt, PPMSELPRTINFO ppSpi);
USHORT PrtInfoClose(PPMPRINTINFO pprt);
int PrtQueryQueues(void);
BOOL PrtGetQueues(PPMQUEUEINFO pPmQueueArray, int iAntal);
BOOL PrtQueryJobProperties(HAB hab, PPMQUEUEINFO pPmQueue);
BOOL PrtGetSelPrt(PPMQUEUEINFO pPmQueue, PPMSELPRTINFO ppSpi);
BOOL PrtSetSelPrt(PPMSELPRTINFO ppSpi, PPMQUEUEINFO pPmQueue);
void PrtNewPage(PPMPRINTINFO pprt);
void PrtCharPos(PPMPRINTINFO pprt, ULONG ulyPos, ULONG ulxPos, PSZ szText);
void PrtCharAt(PPMPRINTINFO pprt, unsigned short usRow, unsigned short usCol, PSZ szText);
void PrtLine(PPMPRINTINFO pprt, int y1, int x1, int y2, int x2, int iWidth);
void PrtBox(PPMPRINTINFO pprt, int y1, int x1, int y2, int x2, int iWidth, int iRound, BOOL bFill);

