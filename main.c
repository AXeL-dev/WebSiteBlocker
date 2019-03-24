//=============================================================================
// Projet : WebSiteBlocker
// Version : 0.2
// Fichier : main.c
// Auteur : AXeL
// Date de création : 22/12/2013
// Date de la dernière modification : 22/12/2013
// Lacunes : L'autohscroll dans la listbox ne fonctionne pas
// Idées d'amélioration : - Gérer l'autohscroll de la listbox
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                           Fonction principale
//=============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecedente, LPSTR lpCmdLine, int nCmdShow)
{
   hInst = hInstance;
   DialogBox(hInstance, "DLGPRC", NULL, (DLGPROC)DlgPrcProc);
    
   return 0;    
}

//=============================================================================
//                           DialogBox principale
//=============================================================================

BOOL APIENTRY DlgPrcProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            hPrincipal = hwnd;
            /* Affichage de l'icone */
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Affichage de l'icone du button BLOQUER */
            SendDlgItemMessage(hwnd, BT_BLOQUER, BM_SETIMAGE, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Affichage de l'icone du button DEBLOQUER */
            SendDlgItemMessage(hwnd, BT_DEBLOQUER, BM_SETIMAGE, (WPARAM)ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE2)));
            /* On remplit la listbox (si possible) */
            HANDLE hFichier;
            DWORD FileSize, nbcharRead;
            CHAR *buffer;
            /* Lecture du fichier */
            hFichier = CreateFile(FICHIER_HOST, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFichier != INVALID_HANDLE_VALUE)
            {
               FileSize = GetFileSize(hFichier, NULL);
               buffer = (PCHAR)LocalAlloc(LMEM_FIXED, FileSize+1);
               ReadFile(hFichier, buffer, FileSize, &nbcharRead, NULL) ;
               buffer[FileSize] = 0;
               /* Détection des sites web déjà mentionnés dans le fichier et ajout à la listbox */
               int i;
               for (i = 0; i < FileSize; i++)
               {
                  if (buffer[i] == '1' && buffer[i-1] == '\n') /* Si '1' de "127.0.0.1" après un saut de ligne */
                  {
                     char siteWeb[FileSize];
                     int j = 0;
                     while (buffer[i] != '\n') /* tant que ce n'est pas encore la fin de la ligne '\n' */
                     {
                        siteWeb[j] = buffer[i]; /* On recopie le site web */
                        i++;
                        j++;
                     }
                     siteWeb[j] = '\0'; /* Fin de la chaine siteWeb */
                     addToList(siteWeb); /* On affiche le site web sur la listbox */
                  }
               }
               LocalFree(buffer); /* Vidage de buffer */
               CloseHandle(hFichier); /* Fermeture du fichier */
            }
            break;
       case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                     switch (LOWORD(wParam))
                     {
                         case BT_BLOQUER:
                              DialogBox(hInst, "DLGBLOQUER", hwnd, (DLGPROC)DlgBloquerProc);
                              break;
                         case BT_DEBLOQUER:
                         {
                              int selection = SendDlgItemMessage(hwnd, LISTE_SITES, LB_GETCURSEL, 0, 0);
                              if (selection != LB_ERR) /* Si un élément de la listeBox est séléctionné */
                              {
                                 SendDlgItemMessage(hwnd, LISTE_SITES, LB_DELETESTRING, selection, 0); /* Suppression */
                                 SendDlgItemMessage(hwnd, LISTE_SITES, LB_SETCURSEL, SendDlgItemMessage(hwnd, LISTE_SITES, LB_GETCOUNT, 0, 0)-1, 0); /* Séléction */
                                 saveList(hwnd); /* sauvegarde */
                              }
                              else
                                 MessageBox(hwnd, "Veuillez sélectionner un site web !", "Erreur", MB_OK | MB_ICONWARNING);
                              break;
                         }
                         case BT_APROPOS:
                              MessageBox(hwnd, NOM_APP " " APP_VERSION
                              "\nPar : AXeL"
                              "\nCompatibilité : Windows XP/7"
                              "\nLibre de droit et d'utilisations."
                              , "A Propos ?", MB_OK | MB_ICONINFORMATION);
                              break;
                         case BT_QUITTER:
                         case IDCANCEL:
                              EndDialog(hwnd, 0);
                              break;      
                     }
                     break;
            }
            break;   
   }
   
   return 0;
}

//=============================================================================
//                           DialogBox bloquer site
//=============================================================================

BOOL APIENTRY DlgBloquerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Limite le nombre de caractères dans l'edit box */
            PostMessage(GetDlgItem(hwnd, IDE_SITE_WEB), EM_LIMITTEXT, SITE_WEB_MAX_LENGTH, 0);
            /* Affichage de "127.0.0.1 " dans l'edit box */
            SetDlgItemText(hwnd, IDE_SITE_WEB, "127.0.0.1 ");
            break;
       case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                     switch (LOWORD(wParam))
                     {
                         case BT_OK:
                         {
                            int length = GetWindowTextLength(GetDlgItem(hwnd, IDE_SITE_WEB));
                            if (length) /* S'il y'a du texte */
                            {
                               char buffer[length+1];
                               GetDlgItemText(hwnd, IDE_SITE_WEB, buffer, length+1); /* Récupération du contenu de l'edit */
                               addToList(buffer);
                               if (!saveList(hwnd))
                               {
                                  int selection = SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_GETCURSEL, 0, 0);
                                  SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_DELETESTRING, selection, 0); /* On efface le site web qu'on vien d'ajouter */
                               }
                            }
                            else
                            {
                               MessageBox(hwnd, "Veuillez saisir un site web !", "Erreur", MB_OK | MB_ICONSTOP);
                               break;
                            }
                         }
                         case IDCANCEL:
                              EndDialog(hwnd, 0);
                              break;      
                     }
                     break;
            }
            break;   
   }
   
   return 0;
}

//=============================================================================
//    Procédure qui ajoute le texte spécifié à la listbox et le sélectionne
//=============================================================================

void addToList(char *buffer)
{
   SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_ADDSTRING, 0, (LONG)buffer); /* Ajout à la listeBox */
   SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_SETCURSEL, SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_GETCOUNT, 0, 0)-1, 0); /* Séléction */
}

//=============================================================================
//   Fonction qui sauvegarde le contenu de la listbox dans le fichier hosts
//=============================================================================

BOOL saveList(HWND hwnd)
{
   /* Fichier hosts texte */
   char hostText[] = "# Copyright (c) 1993-2009 Microsoft Corp.\n#\n"
                     "# This is a sample HOSTS file used by Microsoft TCP/IP for Windows.\n#\n"
                     "# This file contains the mappings of IP addresses to host names. Each\n"
                     "# entry should be kept on an individual line. The IP address should\n"
                     "# be placed in the first column followed by the corresponding host name.\n"
                     "# The IP address and the host name should be separated by at least one\n"
                     "# space.\n#\n"
                     "# Additionally, comments (such as these) may be inserted on individual\n"
                     "# lines or following the machine name denoted by a '#' symbol.\n#\n"
                     "# For example:\n\n"
                     "#      102.54.94.97     rhino.acme.com          # source server\n"
                     "#       38.25.63.10     x.acme.com              # x client host\n\n"
                     "# localhost name resolution is handled within DNS itself.\n"
                     "#	127.0.0.1       localhost\n"
                     "#	::1             localhost\n\n";
   /* Ouverture du fichier */
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   char *buffer, *tmp;
   int count, listeCount = SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_GETCOUNT, 0, 0);
   FileSize = lstrlen(hostText)+(SITE_WEB_MAX_LENGTH*listeCount)+1;
   buffer = (char*)LocalAlloc(LMEM_FIXED, FileSize+1);
   tmp = (char*)LocalAlloc(LMEM_FIXED, SITE_WEB_MAX_LENGTH+1);
   ZeroMemory(buffer, FileSize+1);
   ZeroMemory(tmp, SITE_WEB_MAX_LENGTH+1);
   lstrcat(buffer, hostText); /* On colle le hostText dans buffer */
   for (count = 0; count < listeCount; count++) /* On parcoure la listBox */
   {
      SendDlgItemMessage(hPrincipal, LISTE_SITES, LB_GETTEXT, count, (LONG)tmp); /* On récupère chaque ligne */
      lstrcat(tmp, "\n"); /* Retour à la ligne */
      lstrcat(buffer, tmp); /* On colle la ligne dans buffer */
   }
   LocalFree(tmp); /* Vidage de tmp */
   lstrcat(buffer, "\0");
   //buffer[lstrlen(buffer)] = '\0';
   hFichier = CreateFile(FICHIER_HOST, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFichier != INVALID_HANDLE_VALUE)
   {
      WriteFile(hFichier, buffer, lstrlen(buffer), &nbcharRead, NULL); /* Ecriture */
      CloseHandle(hFichier); /* Fermeture */
      LocalFree(buffer); /* Vidage de buffer */
      return TRUE;
   }
   else
   {
      LocalFree(buffer); /* Vidage de buffer */
      MessageBox(hwnd, "Veuillez relancer le programme en mode administrateur !", "Erreur", MB_OK | MB_ICONSTOP);
      return FALSE;
   }
}
