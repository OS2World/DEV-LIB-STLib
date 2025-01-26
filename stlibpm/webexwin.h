/******************************************************************************/
/*  IBM Web Explorer View Window API                                          */
/*                                                                            */
/*  (C) Copyright IBM Corp. 1995                                              */
/*  Most Rights Reserved.                                                     */
/*                                                                            */
/*  Last update: 1998-02-01                                                   */ 
/******************************************************************************/

#ifndef WEBXWIN__H
#define WEBXWIN__H

#define WC_WXVIEW    "WXViewWC"

#define WXSMALLSTRING   32
#define WXMEDSTRING    256
#define WXLARGESTRING 1024

#define WXMAXMSG       256

/*
** Error codes
*/
#define WX_ERROR      (APIRET)-1
#define WX_SUCCESS     0
#define WX_CANCELLED   1

#define WXENONE        0
#define WXEINVAL       1
#define WXEINPROGRESS  2
#define WXEFILE        3
#define WXENODOC       4
#define WXEBUFSIZE     5
#define WXENOPROXY     6
#define WXENOSOCKS     7
#define WXEMIME        8
#define WXENOTFOUND    9
#define WXEINTR        10
#define WXENOANCHOR    11

/*
** WX_* window messages
*/
#define WX_MOUSEMOVE            (WM_USER + 1000)
#define WX_BUTTONCLICK          (WM_USER + 1001)
#define WX_LOADRESULT           (WM_USER + 1004)
#define WX_LOADSTATUS           (WM_USER + 1005)
#define WX_PRINTRESULT          (WM_USER + 1006)
#define WX_MSGERROR             (WM_USER + 1007)
#define WX_MSGINFO              (WM_USER + 1008)
#define WX_MSGSTATUS            (WM_USER + 1009)
#define WX_DISPLAYDIALOG        (WM_USER + 1010)
#define WX_MSGSECURITY          (WM_USER + 1011)
#define WX_CHANGEEXPIREDPASSWD  (WM_USER + 1012)
#define WX_SELNEWPRIVKEY        (WM_USER + 1013)

/*
** internal window messages
*/
#define WX_QUERYDOC      WM_USER + 2000

 typedef ULONG WXFLAGS ;
 typedef WXFLAGS *PWXFLAGS ;
 typedef PVOID PWXOPAQUE ;

#define WXLOAD_ANCHOR       0
#define WXLOAD_FORCERELOAD  1
#define WXLOAD_IMGMAP       2
#define WXLOAD_FORM         4

 typedef PVOID WXANCHORDATA ;
 typedef WXANCHORDATA *PWXANCHORDATA ;

 typedef struct _wxINITDATA {
    USHORT usLen ;                          /* length of this structure */
    USHORT usThreads ;                      /* number of img loading threads */
    BOOL   bPaletteMgmt ;                   /* palette mgmt on or off? */
 } WXINITDATA ;
 typedef WXINITDATA *PWXINITDATA ;

/*
** Cache options structures
*/
 typedef struct _wxCACHEOPTS {
    BOOL bEnabled ;                        /* enable, disable cacheing */
    BOOL bMemoryImageCacheing;             /* keep cached images in memory */
    LONG lDocLimit ;                       /* number of docs to cache */
    LONG lImageLimit ;                     /* number of images to cache */
    CHAR szCacheDir[WXMEDSTRING];          /* directory for cacheing to disk */
 } WXCACHEOPTS ;
 typedef WXCACHEOPTS *PWXCACHEOPTS ;

/*
** Display options structures
*/
typedef ULONG WXFONTSIZE ;
#define WXFONT_SMALL  0
#define WXFONT_NORMAL 1
#define WXFONT_LARGE  2
#define WXFONT_XLARGE 3

 typedef struct _wxDISPLAYOPTS {
    CHAR szFontName[ WXSMALLSTRING ] ;
    WXFONTSIZE wxFontSize ;
    BOOL bUnderlineAnchors ;               /* use underline text for links */

    LONG lClrText ;                        /* text color */
    LONG lClrAnchor ;                      /* link color */
    LONG lClrVisitedAnchor ;               /* seen link color */
    LONG lClrBackground ;                  /* bg view color */

    BOOL bInlineGraphics ;                 /* show inline images? */
    BOOL bFastLoad ;                       /* load document ahead of images? */
    BOOL bStreamingGraphics ;              /* scanline-at-a-time redraw? */
    BOOL bFancyInterlace ;                 /* blur the ilaced GIF? */

    BOOL bInternalViewer ;                 /* view standalone imgs in view? */
    BOOL bIgnoreImgErrors ;                /* toggle display of red "X" on error */
 } WXDISPLAYOPTS ;
 typedef WXDISPLAYOPTS *PWXDISPLAYOPTS ;

/*
** Network and Gateway options
*/
 typedef ULONG WXGATEWAY ;
#define WXGATEWAY_NONE          0
#define WXGATEWAY_PROXY         1          /* gateway flags can be OR'ed */
#define WXGATEWAY_SOCKS         2

 typedef struct _wxNETWORKOPTS {
    CHAR szEmailAddress[ WXLARGESTRING ] ;     /* user@host smtp style address */
    CHAR szNewsServer[ WXLARGESTRING ] ;       /* news hostname/ip address */
    WXGATEWAY wxGateway ;                      /* proxy or socks server state */
    CHAR szHTTPProxyServer[ WXLARGESTRING ] ;  /* proxy gw (http://..) */
    CHAR szSocksServer[ WXLARGESTRING ] ;      /* socks gw hostname/ip address */
 } WXNETWORKOPTS ;
 typedef WXNETWORKOPTS *PWXNETWORKOPTS ;

/*
** MIME type mapping to external viewers
*/
 typedef struct _wxVIEWER {
    CHAR  szMIMEType[ WXMEDSTRING ] ;       /* MIME type to be viewed */
    CHAR  szProgram[ WXLARGESTRING ] ;      /* program to use as a viewer */
 } WXVIEWER ;
 typedef WXVIEWER *PWXVIEWER ;

/*
** extension mapping to MIME types
*/
 typedef ULONG WXDATAFORMAT ;
#define  WXFORMAT_BINARY 0
#define  WXFORMAT_7BIT   1
#define  WXFORMAT_8BIT   2

 typedef struct _wxEXTMAP {
    CHAR szExtension[ WXMEDSTRING ] ;      /* extension to be mapped */
    CHAR szMIMEType[ WXMEDSTRING ] ;       /* MIME type to assign to this ext */
    WXDATAFORMAT wxFormat ;                /* binary, 7bit, etc. */
 } WXEXTMAP ;
 typedef WXEXTMAP *PWXEXTMAP ;

/*
** Miscellaneous flags
*/
#define WXANCHOR_DEFAULT 0                 /* can be OR'ed together */
#define WXANCHOR_VISITED 1

 typedef ULONG WXSEARCHOPTS ;
#define  WXSEARCH_FORWARD  0
#define  WXSEARCH_BACKWARD 1

 typedef ULONG WXPOINTERTYPE ;
#define  WXPOINTER_NORMAL  0
#define  WXPOINTER_WAIT    1
#define  WXPOINTER_ANCHOR  2
#define  WXPOINTER_UNKNOWN 3

/*
** Internal web explorer position indicator
*/
 typedef struct _wxPOS {
    ULONG  ulComponent ;
    USHORT x ;
    USHORT y ;
 } WXPOS ;
 typedef WXPOS *PWXPOS ;

#define WXPOS_NONE     0
#define WXPOS_ANCHOR   1
#define WXPOS_IMGMAP   2
#define WXPOS_FORM     4

 typedef struct _wxMOUSEDATA {
    USHORT x ;
    USHORT y ;
    USHORT uswHitTest ;
    USHORT fsFlags ;
    WXFLAGS wxflPosType ;
 } WXMOUSEDATA ;
 typedef WXMOUSEDATA *PWXMOUSEDATA ;

 typedef struct _wxBUTTONDATA {
    USHORT usButtonNum ;
    USHORT usClickNum ;
    SHORT  x ;
    SHORT  y ;
    USHORT fsHitTestRes ;
    USHORT fsFlags ;
    WXFLAGS wxflPosType ;
 } WXBUTTONDATA ;
 typedef WXBUTTONDATA *PWXBUTTONDATA ;

/*
** Printer information
*/
 typedef struct _wxPRINTINFO {
    PDRIVDATA pDriverData ;
    CHAR szDriverName[WXMEDSTRING] ;
    CHAR szQueueName[WXMEDSTRING] ;
    CHAR szQueueDesc[WXMEDSTRING] ;
    UINT nCopies ;
 } WXPRINTINFO ;
 typedef WXPRINTINFO *PWXPRINTINFO ;

/*
** Thread data structure for updating progress
*/
#define WXTHREAD_IDLE     0
#define WXTHREAD_CONNECT  1
#define WXTHREAD_WAIT     2
#define WXTHREAD_DATA     3
#define WXTHREAD_RENDER   4
#define WXTHREAD_BEGINDOC 5
#define WXTHREAD_ENDDOC   6

#define WXTHDIDX_DOC  0
#define WXTHDIDX_FMT  1
#define WXTHDIDX_IMG1 2
#define WXTHDIDX_IMG2 3
#define WXTHDIDX_IMG3 4
#define WXTHDIDX_IMG4 5
#define WXTHDIDX_IMG5 6
#define WXTHDIDX_IMG6 7
#define WXTHDIDX_IMG7 8
#define WXTHDIDX_IMG8 9
#define WXTHDIDX_NUM  10

 typedef struct _wxTHREADATA {
    WXFLAGS wxflState ;
    ULONG ulProgressCount ;
    ULONG ulProgressLimit ;
 } WXTHREADDATA ;
 typedef WXTHREADDATA *PWXTHREADDATA ;

 typedef struct _wxMSGDATA {
    ULONG     ulMsgID ;
    PWXOPAQUE pwxMsgData ;
    PWXOPAQUE pwxRetData;
 } WXMSGDATA ;
 typedef WXMSGDATA *PWXMSGDATA ;

/*
** Main viewer API entry points
*/
 USHORT APIENTRY WXViewQueryVersion( VOID ) ;
 APIRET APIENTRY WXViewLoad( HWND, PCSZ, WXFLAGS, PWXANCHORDATA ) ;
 BOOL   APIENTRY WXViewIsLoading( HWND ) ;
 APIRET APIENTRY WXViewLoadToFile( HWND, PCSZ, PCSZ, WXFLAGS, PWXANCHORDATA ) ;
 APIRET APIENTRY WXViewCancelLoad( HWND ) ;
 APIRET APIENTRY WXViewPrint( HWND, PWXPRINTINFO, USHORT ) ;
 APIRET APIENTRY WXViewSearch( HWND, WXSEARCHOPTS, PWXPOS, PWXPOS, PCSZ ) ;
 APIRET APIENTRY WXViewQueryLastError( HWND ) ;
 APIRET APIENTRY WXViewQueryLastLoadError( HWND ) ;

/*
** Document manipulation functions
*/
 APIRET APIENTRY WXViewQueryPos( HWND, USHORT, USHORT, PWXPOS ) ;
 APIRET APIENTRY WXViewQueryAnchor( HWND, PWXPOS, PSZ, USHORT) ;
 USHORT APIENTRY WXViewQueryAnchorLen( HWND, PWXPOS ) ;
 APIRET APIENTRY WXViewQueryAnchorData( HWND, PWXPOS, PWXANCHORDATA, USHORT ) ;
 USHORT APIENTRY WXViewQueryAnchorDataLen( HWND, PWXPOS ) ;

 APIRET APIENTRY WXViewQueryDocAnchor( HWND, PSZ, USHORT ) ;
 USHORT APIENTRY WXViewQueryDocAnchorLen( HWND ) ;

 APIRET APIENTRY WXViewQueryDocTitle( HWND, PSZ, USHORT ) ;
 USHORT APIENTRY WXViewQueryDocTitleLen( HWND ) ;

 APIRET APIENTRY WXViewQueryAnchorState( HWND, PCSZ, PWXFLAGS ) ;
 APIRET APIENTRY WXViewSetAnchorState( HWND, PCSZ, WXFLAGS ) ;

/*
** Configuration API
*/
 HPOINTER APIENTRY WXViewQueryPointer( HWND, WXPOINTERTYPE ) ;
 APIRET APIENTRY WXViewSetPointer( HWND, WXPOINTERTYPE, HPOINTER ) ;

 APIRET APIENTRY WXViewQueryCacheOpts( HWND, PWXCACHEOPTS, USHORT ) ;
 APIRET APIENTRY WXViewSetCacheOpts( HWND, PWXCACHEOPTS, USHORT ) ;

 APIRET APIENTRY WXViewQueryDisplayOpts( HWND, PWXDISPLAYOPTS, USHORT ) ;
 APIRET APIENTRY WXViewSetDisplayOpts( HWND, PWXDISPLAYOPTS, USHORT ) ;

 /* These were changed since 1.1h wants another structure */

 /* APIRET APIENTRY WXViewQueryNetworkOpts( HWND, PWXNETWORKOPTS, USHORT ) ; */
 /* APIRET APIENTRY WXViewSetNetworkOpts( HWND, PWXNETWORKOPTS, USHORT ) ; */
 APIRET APIENTRY WXViewQueryNetworkOpts( HWND, PVOID, USHORT ) ;
 APIRET APIENTRY WXViewSetNetworkOpts( HWND, PVOID, USHORT ) ;
 APIRET APIENTRY WXViewEnableGateway( HWND, WXGATEWAY ) ;

 APIRET APIENTRY WXViewQueryViewer( HWND, PCSZ, PWXVIEWER, USHORT ) ;
 APIRET APIENTRY WXViewSetViewer( HWND, PWXVIEWER, USHORT ) ;
 APIRET APIENTRY WXViewQueryExtMap( HWND, PCSZ, PWXEXTMAP, USHORT ) ;
 APIRET APIENTRY WXViewSetExtMap( HWND, PWXEXTMAP, USHORT ) ;

/*
** Miscellaneous API functions
*/
 APIRET APIENTRY WXViewQueryMsgStr( HWND, PWXMSGDATA, PSZ, USHORT ) ;


/*
** Undocumented API functions
*/

/********************************************************************************* 
Strings must be declared atleast WXMEDSTRING in wxViewQueryNewsOpts.
   szNewsGrp = Newsgroup file (news.grp)
   szNewsSig = Signature file (news.sig)
   szNewsSig = Configuration file (news.cfg)
NOTE: You cannot query news (wxViewQueryNewsOpts) for settings. Settings MUST
      be set (wxViewSetNewsOpts) at start of program.
*********************************************************************************/
APIRET APIENTRY wxViewSetNewsOpts(PCSZ szNewsGrp, PCSZ szNewsSig, PCSZ szNewsCfg);
APIRET APIENTRY wxViewQueryNewsOpts(PCSZ szNewsGrp, PCSZ szNewsSig, PCSZ szNewsCfg);


/********************************************************************************* 
This message is send when the Enter key is pressed and is used to submit pages
that has a default SUBMIT button (like mailto).

You shall call WXViewQueryAnchorLen, WXViewQueryAnchor, WXViewQueryAnchorDataLen
WXViewQueryAnchorData, WXViewLoad the normal way but with the following 
difference:

- use (PWXPOS) 0xFFFFFFFF instead of a pointer to a WXPOS structure
- use WXLOAD_FORCERELOAD when calling WXViewLoad
iLen = WXViewQueryAnchorDataLen(app.hwndWX, (PWXPOS) 0xFFFFFFFF);
*********************************************************************************/
#define WX_ENTERPRESSED        (WM_USER + 1014) /* 13F6 */


/********************************************************************************* 
This message is send when animated bitmaps are to be loaded. You must allocate
a array of hbm to use with the WX_ANIMATELOAD message.

- mp1 is the window that send the message
- mp2 is the number of bitmaps
*********************************************************************************/
#define WX_ANIMATEALLOC       (WM_USER + 1507) /* 15E3 */


/********************************************************************************* 
This message is send when the the animated bitmaps are loaded. You must put
the hbm in your array allocated in the 

- mp1 is the window that send the message
- mp2 is the bitmap handle
*********************************************************************************/
#define WX_ANIMATELOAD        (WM_USER + 1508) /* 15E4 */


/********************************************************************************* 
In WebExplorer 1.1h the NETWORKSOPTS structure is changed. It contains an
extra field BETWEEN!! proxy and socks.

- You need to find a way to see if the user is using WebExp 1.1h
*********************************************************************************/
 typedef struct _wxNETWORKOPTS11H {
    CHAR szEmailAddress[ WXLARGESTRING ] ;     /* user@host smtp style address */
    CHAR szNewsServer[ WXLARGESTRING ] ;       /* news hostname/ip address */
    WXGATEWAY wxGateway ;                      /* proxy or socks server state */
    CHAR szHTTPProxyServer[ WXLARGESTRING ] ;  /* proxy gw (http://..) */
    CHAR szHTTPNoProxyFor[ WXLARGESTRING ] ;   /* no proxy for (http://..) */
    CHAR szSocksServer[ WXLARGESTRING ] ;      /* socks gw hostname/ip address */
 } WXNETWORKOPTS11H ;
 typedef WXNETWORKOPTS11H *PWXNETWORKOPTS11H ;


/********************************************************************************* 
Its there but I dont think it does anything...
*********************************************************************************/
APIRET APIENTRY wxViewInit(void);




/********************************************************************************* 
This is the struct that is used in WXViewQueryAnchorData, WXViewLoad and
wxViewQueryDocPos (the PWXANCHORDATA thing).
Most things are undocumented but the following is known:

pszFormText        Text returned from a form (and mailto)
                   mail layout is:
                   mailaddr=adr@host.com&mailsubj=Subjectstring      (no enter!)
                   &mailbody=line1%0D%0Aline2%0D%0A..'\0'

iEmailOrCgi        1=mailto form, 0=other form

iScrollX           Coordinates for wxViewQueryDocPos/WXViewLoad
iScroolY           save before loading a new page using wxViewQueryDocPos
                   and add to a (otherwise empty WXANCHORDATASTRUCT)
                   when calling WXViewLoad (last parameter)
*********************************************************************************/
typedef struct _WXANCHORDATASTRUCT
        {
        int             *iPtrUnknown1;
        int             *iPtrUnknown2;
        char            *pszFormText;
        int             *iPtrUnknown3;
        int             iEmailOrCgi;
        char            szWhoKnows[268];
        int             iScrollX;
        int             iScrollY;
        } WXANCHORDATASTRUCT;
typedef WXANCHORDATASTRUCT *PWXANCHORDATASTRUCT;



/********************************************************************************* 
Use this function to get the current position of the page (scrollposition).

NOTE: This function is NOT avail in older WebExplorer thuns you MUST load 
      it from WEBEXWIN.DLL yourself.

DosLoadModule(NULL, 0, "WEBEXWIN.DLL", &hmod);
DosQueryProcAddr(hmod, 0, "wxViewQueryDocPos", (PFN *) &wxViewQueryDocPos)
DosFreeModule(hmod);
*********************************************************************************/
APIRET (* EXPENTRY wxViewQueryDocPos) (HWND, PWXANCHORDATA);


#endif /* WEBXWIN__H */
