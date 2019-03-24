//=============================================================================
// Projet : WebSiteBlocker
// Fichier : resources.h
//
//=============================================================================

#include <windows.h>


//=============================================================================
//                               Constantes
//=============================================================================

#define NOM_APP                       "WebSiteBlocker"

#define APP_VERSION                   "v 0.2"

#define SITE_WEB_MAX_LENGTH           48

#define FICHIER_HOST                  "C:\\WINDOWS\\system32\\drivers\\etc\\hosts" // "C:\\Windows\\System32\\drivers\\etc\\hosts" /* Ne fonctionne pas sous windows XP */

#define IDI_ICONE                     100
#define IDI_ICONE2                    101

/* DialogBox principale */
#define LISTE_SITES                   102
#define BT_BLOQUER                    103
#define BT_DEBLOQUER                  104
#define BT_APROPOS                    105
#define BT_QUITTER                    106

/* DialogBox bloquer site */
#define IDE_SITE_WEB                  107
#define BT_OK                         108

//=============================================================================
//                           Variables globales
//=============================================================================

HINSTANCE hInst;

HWND hPrincipal;

//=============================================================================
//                              Prototypes
//=============================================================================

BOOL APIENTRY DlgPrcProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY DlgBloquerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void addToList(char *buffer);
BOOL saveList(HWND hwnd);
