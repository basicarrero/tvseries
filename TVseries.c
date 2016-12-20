#define UNICODE
#define _WIN32_WINNT 0x0501 //WIN_XP
#define _WIN32_IE 0x0600 	//IE_6
#define IDM_LSTVIEW 100
#define IDM_TOOL 101
#define IDM_ADD 102
#define IDM_EDIT 103
#define IDM_DEL 104
#define IDM_FIND 105
#define STR_LEN 40
#define MAX_BUFF 100

#include <windows.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <commctrl.h>
#include <resource.h>

typedef struct{
       char titulo[STR_LEN];
       char genero[STR_LEN];
       char director[STR_LEN];
       char prota[STR_LEN];
       long int ano;
}serie;

serie buff[MAX_BUFF]; //Buffer Principal
int leidos, pos, DlgUserModeEdit;

// Procedimientos de ventanas
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgUser(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgFind(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int readFile(serie *s);
int writeFile(serie *s, int n, const char *mode);
void formatStr(char *s);
int validLong(char *str);
int equalSerie(serie *a, serie *b);
int compareSerie(const void *_a, const void *_b);
void wtxtoserie(serie *sre, WCHAR wtx[][STR_LEN]);
void serietowtx(WCHAR wtx[][STR_LEN], serie *sre);
void setCols(HWND hwnd);
int loadLv(HWND hList);
void loadItem(HWND hwnd, int idx, serie *s);
int find(int results[MAX_BUFF], serie *store, char *key, int searchBy);


int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil){
	
	FILE *fp;
	if((fp=fopen("datos", "ab")) == NULL){
		perror("Error al crear la base de datos");
		exit(1);
	}else fclose(fp);
	
	HWND hwnd;
    MSG mensaje;             // Mensajes recibidos por la aplicación
    WNDCLASSEX wincl;        // Estructura de datos para la clase de ventana
	
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);

    // Estructura de la ventana
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = TEXT("MAINW");
    wincl.lpfnWndProc = WindowProcedure;      // Esta función es invocada por Windows
    wincl.style = CS_DBLCLKS;                 // Captura los doble-clicks
    wincl.cbSize = sizeof (WNDCLASSEX);

    // Usar icono y puntero por defecto
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_APPICON));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(IDI_APPICON));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;				    // Menú por defecto
    wincl.cbClsExtra = 0;                       // Sin información adicional para la clase o la ventana
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_WINDOW; // Usar el color de fondo por defecto para la ventana

    if(!RegisterClassEx(&wincl)) return 0; // Registrar la clase de ventana

    // Crear la ventana
    hwnd = CreateWindowEx(
           0,                   // Posibilidades de variación
           TEXT("MAINW"),		// Nombre de la clase
           TEXT("Series"),		// Texto del título
           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Tipo por defecto
           CW_USEDEFAULT,       // Windows decide la posición
           CW_USEDEFAULT,       // donde se coloca la ventana
           630,                 // Ancho
           370,                 // Alto en pixels
           HWND_DESKTOP,        // La ventana es hija del escritorio
           NULL,                // Sin menú
           hThisInstance,       // Manipulador de instancia
           NULL                 // No hay datos de creación de ventana
    );

    ShowWindow(hwnd, SW_SHOWDEFAULT); //Mostrar la ventana
	
    // Bucle de mensajes, se ejecuta hasta que haya error o GetMessage devuelva FALSE
    while(TRUE == GetMessage(&mensaje, NULL, 0, 0)) {
       if(!IsDialogMessage(hwnd, &mensaje) ) {
		   TranslateMessage(&mensaje);	// Traducir mensajes de teclas virtuales a mensajes de caracteres
		   DispatchMessage(&mensaje);	// Enviar mensaje al procedimiento de ventana
	   }
    }
    return mensaje.wParam;
}

//  Esta función es invocada por la función DispatchMessage()
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    static HINSTANCE hInstance;
	static HWND hList, hTool;
	
    switch (msg){
        case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		   
			hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
					hwnd, (HMENU)IDM_TOOL, GetModuleHandle(NULL), NULL);
			
			TBBUTTON tbButtons[] = {
				{10, 0, 0, BTNS_SEP},
				{STD_FILENEW, IDM_ADD,  TBSTATE_ENABLED, 0, {0}, 0, (INT_PTR)TEXT("Nuevo")},
				{STD_PROPERTIES, IDM_EDIT, 0, 0, {0}, 0, (INT_PTR)TEXT("Editar")},
				{STD_DELETE, IDM_DEL, 0, 0, {0}, 0, (INT_PTR)TEXT("Eliminar")},
				{20, 0, 0, BTNS_SEP},
				{STD_FIND, IDM_FIND, TBSTATE_ENABLED, 0, {0}, 0, (INT_PTR)TEXT("Buscar")}
			};
			
			SendMessage(hTool, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);
			SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
			SendMessage(hTool, TB_ADDBUTTONS, (WPARAM)6, (LPARAM)&tbButtons);
			SendMessage(hTool, TB_AUTOSIZE, 0, 0); 
					  
			hList = CreateWindowEx(
					  WS_EX_CLIENTEDGE,
					  WC_LISTVIEW,
					  NULL,
					  WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | LVS_REPORT,
					  9, 42,
					  607, 295,
					  hwnd,
					  (HMENU)IDM_LSTVIEW,
					  hInstance,
					  NULL);
			
			setCols(hList);
			SendMessage(hList,LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			loadLv(hList);
			return 0;
			break;
		case WM_NOTIFY:
			if(((NMHDR *)lParam)->code == NM_CLICK){
				pos = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
				if(pos>=0){
					SendMessage(hTool, TB_SETSTATE, IDM_EDIT, TBSTATE_ENABLED);
					SendMessage(hTool, TB_SETSTATE, IDM_DEL, TBSTATE_ENABLED);
				}else{
					SendMessage(hTool, TB_SETSTATE, IDM_EDIT, TBSTATE_INDETERMINATE);
					SendMessage(hTool, TB_SETSTATE, IDM_DEL, TBSTATE_INDETERMINATE);
				}
			}
			break;
		case WM_COMMAND:
		    switch(LOWORD(wParam)){
				case IDM_ADD:
					DlgUserModeEdit = FALSE;
					pos = leidos;
					if(TRUE == DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_USER), hwnd, DlgUser)){
						writeFile(buff, leidos + 1, "wb");
						loadLv(hList);
						printf("Added new Item\n");
					}
					break;
				case IDM_EDIT:
					if(pos>=0){
						DlgUserModeEdit = TRUE;
						if(TRUE == DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_USER), hwnd, DlgUser)){
							writeFile(buff, leidos, "wb");
							loadLv(hList);
							printf("Edited Item %d\n",pos);
						}
					}
					break;
				case IDM_DEL:
					if(pos>=0){
						writeFile(buff, pos, "wb");
						writeFile(&buff[pos + 1], leidos -1 -pos, "ab");
						loadLv(hList);
						printf("Deleted Item %d\n",pos);
					}
					break;
				case IDM_FIND:
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_FIND), hwnd, DlgFind);
					break;
				default:
					return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		   break;
        case WM_DESTROY:
           PostQuitMessage(0);    // envía un mensaje WM_QUIT a la cola de mensajes
           break;
        default:                  // para los mensajes de los que no nos ocupamos
           return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK DlgUser(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	static WCHAR auxData[5][STR_LEN];
	
    switch (msg){
        case WM_INITDIALOG:
			if(DlgUserModeEdit){
				SetWindowText(hDlg, TEXT("Modificar Registro"));
				serietowtx(auxData, &buff[pos]);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_TITULO), auxData[0]);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_DIRECTOR), auxData[1]);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_ANO), auxData[2]);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_GENERO), auxData[3]);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PROTA), auxData[4]);
			}else SetWindowText(hDlg, TEXT("Nuevo Registro"));
			return FALSE;
        case WM_COMMAND:
           switch(LOWORD(wParam)){
              case IDOK:
				 GetWindowText(GetDlgItem(hDlg, IDC_EDIT_TITULO), auxData[0], STR_LEN);
				 GetWindowText(GetDlgItem(hDlg, IDC_EDIT_DIRECTOR), auxData[1], STR_LEN);
				 GetWindowText(GetDlgItem(hDlg, IDC_EDIT_ANO), auxData[2], STR_LEN);
				 GetWindowText(GetDlgItem(hDlg, IDC_EDIT_GENERO), auxData[3], STR_LEN);
				 GetWindowText(GetDlgItem(hDlg, IDC_EDIT_PROTA), auxData[4], STR_LEN);
				 wtxtoserie(&buff[pos], auxData);
				 formatStr(buff[pos].titulo);
				 formatStr(buff[pos].director);
				 formatStr(buff[pos].genero);
				 formatStr(buff[pos].prota);
                 EndDialog(hDlg, TRUE);
                 break;
              case IDCANCEL:
                 EndDialog(hDlg, FALSE);
                 break;
           }
           return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK DlgFind(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	static int idxMatches[MAX_BUFF];
	static WCHAR auxK[STR_LEN];
	static char key[STR_LEN];
	static int i, curBox, nMatches;
	
    switch (msg){
		case WM_NOTIFY:
			if(((NMHDR *)lParam)->code == NM_CLICK)
				pos = ListView_GetNextItem(GetDlgItem(hDlg, IDF_FINDLIST), -1, LVNI_SELECTED);
			break;
        case WM_INITDIALOG:
			SendDlgItemMessage(hDlg, IDF_FINDLIST, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
			setCols(GetDlgItem(hDlg, IDF_FINDLIST));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_ADDSTRING, 0, (LPARAM)TEXT("Título"));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_ADDSTRING, 0, (LPARAM)TEXT("Director"));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_ADDSTRING, 0, (LPARAM)TEXT("Año"));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_ADDSTRING, 0, (LPARAM)TEXT("Género"));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_ADDSTRING, 0, (LPARAM)TEXT("Protagonista"));
			SendDlgItemMessage(hDlg, IDF_COMBO, CB_SETCURSEL, 0, 0);
			return FALSE;
        case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDF_FIND:
					memset(idxMatches, 0, sizeof(int));
					memset(auxK, 0, sizeof(WCHAR));
					memset(key, 0, sizeof(char));
					curBox = (int)SendDlgItemMessage(hDlg, IDF_COMBO, CB_GETCURSEL, 0, 0);
					GetWindowText(GetDlgItem(hDlg, IDF_EDIT_KEYW), auxK, STR_LEN);
					wcstombs(key, auxK, wcstombs(NULL, auxK, 0) + 1);
					formatStr(key);
					nMatches = find(idxMatches, buff, key, curBox);
					SendMessage(GetDlgItem(hDlg, IDF_FINDLIST), LVM_DELETEALLITEMS, 0, 0);
					printf("Search Marches (SearchByidx:%d, KEY: \"%s\"):", curBox, key);
					for(i=0; i<nMatches; i++){
						printf("%d ", idxMatches[i]);
						loadItem(GetDlgItem(hDlg, IDF_FINDLIST), i, &buff[idxMatches[i]]);
					} 
					printf("\n");
					break;
				case IDF_EDIT:
					if (nMatches>0 && pos>=0){
						pos = idxMatches[pos];
						DlgUserModeEdit = TRUE;
						if(TRUE == DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_USER), hDlg, DlgUser)){
							writeFile(buff, leidos, "wb");
							printf("Edited Item %d\n", pos);
							leidos = readFile(buff);
							nMatches = find(idxMatches, buff, key, curBox);
							SendMessage(GetDlgItem(hDlg, IDF_FINDLIST), LVM_DELETEALLITEMS, 0, 0);
							for(i=0; i<nMatches; i++)
								loadItem(GetDlgItem(hDlg, IDF_FINDLIST), i, &buff[idxMatches[i]]);
						}
					}
					break;
				case IDF_DEL:
					if (nMatches>0 && pos>=0){
						pos = idxMatches[pos];
						writeFile(buff, pos, "wb");
						writeFile(&buff[pos + 1], leidos -1 -pos, "ab");
						printf("Deleted Item %d\n", pos);
						leidos = readFile(buff);
						nMatches = find(idxMatches, buff, key, curBox);
						SendMessage(GetDlgItem(hDlg, IDF_FINDLIST), LVM_DELETEALLITEMS, 0, 0);
						for(i=0; i<nMatches; i++)
							loadItem(GetDlgItem(hDlg, IDF_FINDLIST), i, &buff[idxMatches[i]]);
					}
					break;
				case IDCANCEL:
					loadLv(GetDlgItem(GetParent(hDlg), IDM_LSTVIEW));
					EndDialog(hDlg, FALSE);
					break;
			}
			return TRUE;
    }
    return FALSE;
}

void formatStr(char *s){
     int i;
     int len = strlen(s);
     s[0] = toupper(s[0]);
     for(i=1; i<len; i++) s[i] = tolower(s[i]);
}

// Rellena una estructura "serie" en con un arreglo de cadenas WCHAR
void wtxtoserie(serie *sre, WCHAR wtx[][STR_LEN]){
	memset(sre, 0, sizeof(serie));
	size_t requiredSize;
	requiredSize = wcstombs(NULL, wtx[0], 0);
	wcstombs(sre->titulo, wtx[0], requiredSize + 1);
	requiredSize = wcstombs(NULL, wtx[1], 0);
	wcstombs(sre->director, wtx[1], requiredSize + 1);
	requiredSize = wcstombs(NULL, wtx[3], 0);
	wcstombs(sre->genero, wtx[3], requiredSize + 1);
	requiredSize = wcstombs(NULL, wtx[4], 0);
	wcstombs(sre->prota, wtx[4], requiredSize + 1);
	swscanf(wtx[2], L"%ld", &sre->ano);
}

// Convierte los campos de la estructura "serie" en un arreglo de cadenas WCHAR
void serietowtx(WCHAR wtx[][STR_LEN], serie *sre){
	size_t requiredSize;
	int i;
	for (i=0; i<5; i++) memset(wtx[i], 0, STR_LEN * sizeof(WCHAR));
	requiredSize = mbstowcs(NULL, sre->titulo, 0);
	mbstowcs(wtx[0], sre->titulo, requiredSize + 1);
	requiredSize = mbstowcs(NULL, sre->director, 0);
	mbstowcs(wtx[1], sre->director, requiredSize + 1);
	requiredSize = mbstowcs(NULL, sre->genero, 0);
	mbstowcs(wtx[3], sre->genero, requiredSize + 1);
	requiredSize = mbstowcs(NULL, sre->prota, 0);
	mbstowcs(wtx[4], sre->prota, requiredSize + 1);
	swprintf(wtx[2], L"%ld", sre->ano);
}

int compareSerie(const void *_a, const void *_b){
		return strcmp(((serie *)_a)->titulo, ((serie *)_b)->titulo);
}

int readFile(serie *s){
	int i = 0;
    FILE *fp;
    if((fp=fopen("datos", "rb")) == NULL){
         perror("Error al abrir la base de datos");
         exit(1);
    }
    while(fread(&s[i], sizeof(serie), 1, fp)) i++;
    fclose(fp);
	qsort(buff, i, sizeof(serie), &compareSerie);
    return i;
}

int writeFile(serie *s, int n, const char *mode){
	int i;
    FILE *fp;
	if((fp=fopen("datos", mode)) == NULL){
		perror("Error al escribir la base de datos");
		exit(1);
	}
	for(i=0; i<n; i++) fwrite(&s[i], sizeof(serie), 1, fp);
	fclose(fp);
    return i;
}

int validLong(char *str){
	int sign = 0;
	if ((str[0] == '-') | (str[0] == '+')) sign++;
	int numchk = 0;
	long num;
	while (isdigit(str[numchk + sign])) numchk++;
	if (numchk == strlen(str) - sign){
		num = strtol(str, NULL, 10);
		if ((num == LONG_MAX) | (num == LONG_MIN)) printf("Too looooooong!\n");
		else return 1;
	}else printf("Not a number! %c%c%c\n", 170, 95, 170);
	return 0;
}

int find(int results[MAX_BUFF], serie *store, char *key, int searchBy){
	int n, i;
	long int auxAno;
	
	switch(searchBy){
		case 0:
			for (i=0, n=0; i<leidos; i++){
				if (strcmp(buff[i].titulo, key) == 0){
					results[n] = i;
					n++;
				}
			}
			break;
		case 1:
			for (i=0, n=0; i<leidos; i++){
				if (strcmp(buff[i].director, key) == 0){
					results[n] = i;
					n++;
				}
			}
			break;
		case 2:
			if (validLong(key)){
				sscanf(key, "%ld", &auxAno);
				for (i=0, n=0; i<leidos; i++){
					if (buff[i].ano == auxAno){
						results[n] = i;
						n++;
					}
				}
			}else n = 0;
			break;
		case 3:
			for (i=0, n=0; i<leidos; i++){
				if (strcmp(buff[i].genero, key) == 0){
					results[n] = i;
					n++;
				}
			}
			break;
		case 4:
			for (i=0, n=0; i<leidos; i++){
				if (strcmp(buff[i].prota, key) == 0){
					results[n] = i;
					n++;
				}
			}
			break;
		default:
			n = 0;
	}
	return n;
}

void setCols(HWND hwnd){
	LVCOLUMN LvCol;
	LvCol.mask=LVCF_TEXT|LVCF_WIDTH;
	LvCol.cx=150;
	LvCol.pszText=TEXT("Título");
	ListView_InsertColumn(hwnd, 0, &LvCol);
	LvCol.pszText=TEXT("Director");
	ListView_InsertColumn(hwnd, 1, &LvCol);
	LvCol.cx=50;
	LvCol.pszText=TEXT("Año");
	ListView_InsertColumn(hwnd, 2, &LvCol);
	LvCol.cx=100;
	LvCol.pszText=TEXT("Género");
	ListView_InsertColumn(hwnd, 3, &LvCol);
	LvCol.cx=150;
	LvCol.pszText=TEXT("Protagonista");
	ListView_InsertColumn(hwnd, 4, &LvCol);
}

int loadLv(HWND hList){
	int i;
	SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);
	leidos = readFile(buff);
	for(i=0; i<leidos; i++) loadItem(hList, i, &buff[i]);
	return(i);
}

void loadItem(HWND hwnd, int idx, serie *s){
	int i;
	static WCHAR field[5][STR_LEN];
	serietowtx(field, s);
	LVITEM LvItem;
	LvItem.mask=LVIF_TEXT;
	LvItem.iItem=idx;
	LvItem.iSubItem=0;
	LvItem.pszText=field[0];
	ListView_InsertItem(hwnd, &LvItem);
	for(i=1; i<5; i++){
		LvItem.iSubItem=i;
		ListView_SetItemText(hwnd, idx, LvItem.iSubItem, field[i]);
	}
}