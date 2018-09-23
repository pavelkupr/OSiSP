#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
void DrawTransparentBitmap(HDC, HANDLE, short,
	short, COLORREF, HWND);
void ChangeAngle(HDC, double);
void DrawSprite(HDC, int, int, HWND);
int i=0, j=0;
double angle = 0;
OPENFILENAME ofn; 
wchar_t szFile[1024];
HANDLE hbitmap;
bool isImg = false;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)
{
	MSG msg;
	RECT screen_rect;
	HWND hWnd;
	HMENU hMenu;
	LPCTSTR lpzClass = TEXT("MyClass");

	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;
	GetWindowRect(GetDesktopWindow(), &screen_rect); 
	int x = screen_rect.right / 2 - 500;
	int y = screen_rect.bottom / 2 - 300;

	hWnd = CreateWindow(lpzClass, TEXT("ОСиСП работа №1"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 1000, 600, NULL, NULL,
		hInstance, NULL);

	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 0, L"Загрузка картинки");
	SetMenu(hWnd, hMenu);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"files(*.btm;)|*.btm;";

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;  
}

ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS wcWindowClass = { 0 };
	wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	wcWindowClass.hInstance = hInst;
	wcWindowClass.lpszClassName = lpzClassName;
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindowClass.hbrBackground = CreateSolidBrush(0x00772463);
	return RegisterClass(&wcWindowClass); 
}
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetGraphicsMode(hDC, GM_ADVANCED);
		if (!isImg)
		{
			DrawSprite(hDC, i, j, hWnd);
		}
		else
		{
			DrawTransparentBitmap(hDC, hbitmap, i, j, 0x00FFFFFF, hWnd);
		}
		
		EndPaint(hWnd, &ps);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'w':
		case 'W':
			j -= 5;
			break;
		case 's':
		case 'S':
			j += 5;
			break;
		case 'a':
		case 'A':
			i -= 5;
			break;
		case 'd':
		case 'D':
			i += 5;
			break;
		case 'e':
		case 'E':
			angle += 0.01;
			break;
		case 'q':
		case 'Q':
			angle -= 0.01;
			break;
		}
		InvalidateRect(hWnd, 0, true);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			j -= 5;
			break;
		case VK_DOWN:
			j += 5;
			break;
		case VK_LEFT :
			i -= 5;
			break;
		case VK_RIGHT:
			i += 5;
			break;
		case VK_ESCAPE:
			MessageBox(hWnd, TEXT("Выход"), TEXT("событие"), MB_OKCANCEL);
			PostQuitMessage(0);
			break;
		}
		InvalidateRect(hWnd, 0, true);
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			if (GetKeyState(VK_SHIFT) & 0x8000)
				i += 5;
			else
				j += 5;
		else
			if (GetKeyState(VK_SHIFT) & 0x8000)
				i -= 5;
			else
				j -= 5;
		InvalidateRect(hWnd, 0, true);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 0:  
			if (GetOpenFileName(&ofn) != 0)
			{
				hbitmap = LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				isImg = true;
				InvalidateRect(hWnd, 0, true);
			}
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	//case WM_ERASEBKGND:
		//break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawTransparentBitmap(HDC hDC, HANDLE hBitmap, short xStart,
	short yStart, COLORREF cTransparentColor, HWND hWnd)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;
	RECT	   rect;

	hdcTemp = CreateCompatibleDC(hDC);
	SelectObject(hdcTemp, hBitmap);   

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            
	ptSize.y = bm.bmHeight;           
	DPtoLP(hdcTemp, &ptSize, 1);      

	hdcBack = CreateCompatibleDC(hDC);
	hdcObject = CreateCompatibleDC(hDC);
	hdcMem = CreateCompatibleDC(hDC);
	hdcSave = CreateCompatibleDC(hDC);

	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem = CreateCompatibleBitmap(hDC, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hDC, ptSize.x, ptSize.y);


	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	SetMapMode(hdcTemp, GetMapMode(hDC));

	GetClientRect(hWnd, (LPRECT)&rect);
	DPtoLP(hDC, (LPPOINT)&rect, 2);

	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);
	cColor = SetBkColor(hdcTemp, cTransparentColor);
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);
	SetBkColor(hdcTemp, cColor);
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hDC, rect.right / 2 - ptSize.x / 2 + xStart, rect.bottom / 2 - ptSize.y / 2+yStart, SRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hDC, 0,0, SRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	BitBlt(hdcMem, 0,0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT); 

	ChangeAngle(hDC,angle);
	GetClientRect(hWnd, (LPRECT)&rect);
	DPtoLP(hDC, (LPPOINT)&rect, 2);

	SetViewportOrgEx(hDC, xStart, yStart, NULL);
	BitBlt(hDC, rect.right / 2 - ptSize.x/2, rect.bottom / 2 - ptSize.y/2, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);

	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}

void ChangeAngle(HDC hdc, double angle)
{
	XFORM xForm;

	xForm.eM11 = (FLOAT)cos(angle);
	xForm.eM12 = (FLOAT)sin(angle);
	xForm.eM21 = (FLOAT)-sin(angle);
	xForm.eM22 = (FLOAT)cos(angle);
	xForm.eDx = (FLOAT) 0.0;
	xForm.eDy = (FLOAT) 0.0;

	SetWorldTransform(hdc, &xForm);
}

void DrawSprite(HDC hDC,int i,int j,HWND hWnd)
{
	RECT rect;
	const int x = 40, y = 80;
	ChangeAngle(hDC, angle);

	GetClientRect(hWnd, (LPRECT)&rect);
	DPtoLP(hDC, (LPPOINT)&rect, 2);
	SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));

	SetViewportOrgEx(hDC, i, j, NULL);
	Rectangle(hDC, rect.right / 2 - x / 2, rect.bottom / 2 + y / 2, rect.right / 2 + x / 2, rect.bottom / 2 - y / 2);
}