// GraphicalErrorDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GraphicalErrorDemo.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <strstream>
#include <Windows.h>
#include <commdlg.h>

using namespace std;

#define MAX_LOADSTRING 100

int totalrows = 0;
vector<HWND*> losTextBoxes;
vector<HWND*> losButtons;

struct word {
	string text;
	bool active = false;
	int y = 30;
	int x = 10;
	int error = 0;
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT				SetColors(HDC, HWND);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TextBoxMessage(HWND, UINT, WPARAM, LPARAM);

vector<word> textBoxes;

vector<string>slice(string src, char sym, char* skip, bool trim, bool singlesep)
{
	vector<string> accum;
	string tmps;

	for (int i = 0; ; i++)
	{
		if (i == src.length() || src[i] == sym )
		{
			if (singlesep || !tmps.empty()) accum.push_back(tmps);
			tmps.clear();
		}
		else
		{
			bool found = false;
			if (skip) for (int j = 0; skip[j]; j++) if (skip[j] == src[i]) found = true;
			if (!found) tmps += src[i];
		}
		if (i == src.length()) break;
	}
	if (trim)
		for (unsigned int i = 0; i < accum.size(); i++)
		{
			while (!accum[i].empty() && accum[i][0] == ' ') accum[i].erase(0, 1);
			while (!accum[i].empty() && accum[i][accum[i].size() - 1] == ' ') accum[i].erase(accum[i].size() - 1);
		}
	return accum;
}
string trim(string s)
{
	while (!s.empty() && (s[0] == ' ' || s[0] == '\t')) s.erase(0, 1);
	while (!s.empty() && (s[s.length() - 1] == ' ' || s[0] == '\t')) s.erase(s.length() - 1, 1);
	return s;
}

string blankstr = "";
void ProcessFile(const char* fname)
{
	ifstream ifile(fname);
	int currenty = 0;
	bool advance = false;

	while (ifile.good())
	{
		string s;
		getline(ifile, s);
		vector<string>values = slice(s, '\t', NULL, true, true);

		for (unsigned int i = 0; i < values.size(); ++i) {
			if (values.at(i).compare("") == 0) {
				values.erase(values.begin() + i);
			}
		}
		if (values.size() < 1) continue;
		/*for (unsigned int i = 0; i < values.size(); ++i) {
			cout << values.at(i) << "|";
		}*/
		//cout << "\n";
		if (values.at(0).compare("") != 0) {
			if (advance) {
				word newword2, newword3;
				newword2.text = blankstr;
				newword3.text = blankstr;
				newword2.x = 310;
				newword3.x = 615;
				newword2.y = currenty;
				newword3.y = currenty;
				textBoxes.push_back(newword2);
				textBoxes.push_back(newword3);
			}
			word newword;
			newword.text = values.at(0);
			newword.x = 5;
			currenty += 25;
			newword.y = currenty;
			advance = true;
			if (values.at(values.size() - 1).compare("!not recognized") == 0) {
				newword.error = 1;
			}
			textBoxes.push_back(newword);
			if (values.at(values.size() - 1).compare("!incomplete previous line") == 0) {
				textBoxes.at(textBoxes.size() - 2).error = 2;
				word newword2, newword3;
				newword2.x = 310;
				newword3.x = 615;
				newword2.y = currenty - 25;
				newword3.y = currenty - 25;
				newword2.text = blankstr;
				newword3.text = blankstr;
				textBoxes.push_back(newword2);
				textBoxes.push_back(newword3);
				getline(ifile, s);
			}
		}
		else {
			if (advance) {
				advance = false;
			}
			else {
				currenty += 25;
			}
			word newword, newword2;
			newword.text = values.at(1);
			newword.x = 310;
			newword.y = currenty;
			newword2.x = 615;
			newword2.y = currenty;
			if (values.size() > 2 && values.at(values.size() - 1).compare("!no table number") != 0) {
				if (values.at(values.size() - 1).compare("!errors") == 0) {
					if (values.at(1).substr(0, 2).compare("/*") == 0) {
						newword.error = 3;
					}
					if (values.at(2).substr(0, 2).compare("/*") == 0) {
						values.at(2) = values.at(2).substr(2, values.at(2).size() - 4);
						newword2.error = 3;
					}
				}
				newword2.text = values.at(2);
				textBoxes.push_back(newword);
				textBoxes.push_back(newword2);
			}
			else {
				newword.error = 2;
				textBoxes.push_back(newword);
				newword2.text = blankstr;
				textBoxes.push_back(newword2);
			}
		}
	}
	ifile.close();
	totalrows = currenty / 25;
	/*currenty = 0;
	for (unsigned int i = 0; i < textBoxes.size(); ++i) {
		if (textBoxes.at(i).y != currenty) {
			lines.push_back(new structline);
			lines.at(lines.size() - 1)->y = textBoxes.at(i).y;
			if (textBoxes.at(i).x == 5) {
				lines.at(lines.size() - 1)->comp = &textBoxes.at(i);
			}
			else {
				lines.at(lines.size() - 1)->scomp = &textBoxes.at(i);
			}
			currenty = textBoxes.at(i).y;
		}
		else {
			if (textBoxes.at(i).x == 310) lines.at(lines.size() - 1)->scomp = &textBoxes.at(i);
			else if (textBoxes.at(i).x == 615) lines.at(lines.size() - 1)->index = &textBoxes.at(i);
			else cout << "Error";
		}
	}
	for (unsigned int i = 0; i < lines.size(); ++i) {
		cout << lines.at(i)->y << " ";
		if (lines.at(i)->comp != NULL) wcout << lines.at(i)->comp->text << "|\t";
		if (lines.at(i)->scomp != NULL) wcout << lines.at(i)->scomp->text << "|\t";
		if (lines.at(i)->index != NULL) wcout << lines.at(i)->index->text << "|\t";
		cout << "\n";
	}
	cout << totalrows << " " << lines.size() << "\n";*/
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	AllocConsole();
	FILE* pCout;
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	//cout << "I know Kung Fu\n";
	//ProcessFile("Q:\\Public\\Diky\\SHIP2019\\Tyler Renken\\GUI_Demos\\CompleteOutputMarked.txt");
	//ProcessFile("Q:\\Public\\Diky\\SHIP2019\\Tyler Renken\\NON-OCR_EXTRACTION_DATA\\Non-Digital-Origin_Target_Output_Samples\\CompleteOutput.txt");
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPHICALERRORDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICALERRORDEMO));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICALERRORDEMO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHICALERRORDEMO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LPCSTR font = "Consolas";
HFONT hfont = CreateFontA(16, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, font);

void update(HWND hWnd, int yPos, int height) {
	for (unsigned int foo = 0; foo < textBoxes.size() ; ++foo) {
		int id = -1;
		for (unsigned int i = 0; i < losTextBoxes.size(); ++i) {
			if (foo + 200 == GetDlgCtrlID(*losTextBoxes.at(i))) {
				id = i;
				break;
			}
		}
		if (textBoxes.at(foo).y > yPos - 40 && textBoxes.at(foo).y < yPos + height) {
			if (id < 0) {
				textBoxes.at(foo).active = true;
				HWND *newTextBox = new HWND(CreateWindow(TEXT("Edit"), wstring(textBoxes.at(foo).text.begin(), textBoxes.at(foo).text.end()).c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, textBoxes.at(foo).x, textBoxes.at(foo).y - yPos, 300, 20, hWnd, (HMENU)(200 + foo), NULL, NULL));

				SendMessage(*newTextBox, WM_SETFONT,
					(WPARAM) hfont,
					TRUE);
				losTextBoxes.push_back(newTextBox);
			}
		}
		else {
			if (id > -1) {
				DestroyWindow(*losTextBoxes.at(id));
				delete losTextBoxes.at(id);
				textBoxes.at(foo).active = false;
				losTextBoxes.erase(losTextBoxes.begin() + id);
			}
		}
		if (foo + 1 < textBoxes.size()) {
			if (textBoxes.at(foo + 1).y > yPos + height && textBoxes.at(foo + 1).active == false) break;
		}
		if (foo + 20 < textBoxes.size()) {
			if (textBoxes.at(foo + 20).y < yPos - 40 && textBoxes.at(foo + 20).active == false) foo += 20;
		}
	}
	for (unsigned int i = 1; i <= totalrows; ++i) {
		int id = -1;
		for (unsigned int i2 = 0; i2 < losButtons.size(); ++i2) {
			if (i + textBoxes.size() == GetDlgCtrlID(*losButtons.at(i2))) {
				id = i2;
				break;
			}
		}
		if ((signed) (i * 25) > (yPos - 40) && (i * 25) < (yPos + height)) {
			if (id < 0) {
				HWND* newButton = new HWND(CreateWindow(L"BUTTON", L"Add Row", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 920, (i * 25) - yPos, 100, 20, hWnd, (HMENU)(textBoxes.size() + i), NULL, NULL));
				losButtons.push_back(newButton);
			}
		}
		else {
			if (id > -1) {
				DestroyWindow(*losButtons.at(id));
				delete losButtons.at(id);
				losButtons.erase(losButtons.begin() + id);
			}
		}
	}
}

void clearBoxes() {
	while (0 < losTextBoxes.size()) {
		DestroyWindow(*losTextBoxes.at(0));
		delete losTextBoxes.at(0);
		losTextBoxes.erase(losTextBoxes.begin());
	}
	textBoxes.clear();
	while (0 < losButtons.size()) {
		DestroyWindow(*losButtons.at(0));
		delete losButtons.at(0);
		losButtons.erase(losTextBoxes.begin());
	}
}

int yypos = 0;
using convert_type = std::codecvt_utf8<wchar_t>;
wstring_convert<convert_type, wchar_t> converter;

int verifyint = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	
	double yChar = 10;
	int height = 1;
	SCROLLINFO si;

	RECT rect;
	if (GetWindowRect(hWnd, &rect)) {
		height = rect.bottom - rect.top;
	}
	if (height == 0) ++height;
	yChar = (double)(totalrows * 25.0 + 20.0) / (double)(height) - 60;
	if (totalrows * 25 + 20 < height - 60) yChar = 0;

	int yPos = 0;

	switch (message)
	{
	case WM_CREATE:
		update(hWnd, yypos, height);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		cout << "Message";
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			exit(0);
			break;
		case ID_FILE_OPEN:
		{
			char filename[MAX_PATH];

			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = L"Text Files\0*.txt\0Any File\0*.*\0";
			wchar_t wtext[MAX_PATH];
			size_t outSize;
			size_t size = strlen(filename) + 1;
			mbstowcs_s(&outSize, wtext, size, filename, size - 1);
			ofn.lpstrFile = wtext;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = L"Select a File";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
				
			if (GetOpenFileName(&ofn))
			{
				clearBoxes();
				size_t charsConverted = 0;
				wcstombs_s(&charsConverted, filename, sizeof(filename), wtext, MAX_PATH);
				for (unsigned int i = 0; i < strlen(filename); ++i) {
					if (filename[i] == '\\') {
						memmove(filename + i + 1 + 1, filename + i + 1, MAX_PATH - (i + 1) - 1);
						filename[i + 1] = '\\';
						++i;
					}
				}
				ProcessFile(filename);
				update(hWnd, yypos, height);
			}

		break;
		}
		case ID_FILE_SAVE:
		{
			ofstream ofile("CleanedOutput.txt");
			int prev_y = textBoxes.at(0).y;
			for (unsigned int i = 0; i < textBoxes.size(); ++i) {
				if (textBoxes.at(i).text.compare("") == 0) continue;
				if (textBoxes.at(i).y != prev_y) {
					ofile << "\n";
					prev_y = textBoxes.at(i).y;
				}
				if (textBoxes.at(i).x == 5) {
					ofile << textBoxes.at(i).text << "\n";
				}
				else {
					ofile << "\t\t" << textBoxes.at(i).text;
				}
			}
			ofile.close();
			break;
		}
		default:
			break;
		}
		if (GetDlgCtrlID((HWND)lParam) > 199 && (unsigned) GetDlgCtrlID((HWND)lParam) < 200 + textBoxes.size())
		{
			int bufSize = 10000;
			LPTSTR newstr = new TCHAR[bufSize];
			GetDlgItemText(hWnd, GetDlgCtrlID((HWND)lParam), newstr, bufSize);
			textBoxes.at(GetDlgCtrlID((HWND)lParam) - 200).text = converter.to_bytes(newstr);
			delete[] newstr;
			break;
		}
		if (GetDlgCtrlID((HWND)lParam) > textBoxes.size() && GetDlgCtrlID((HWND)lParam) < textBoxes.size() + totalrows) {
			cout << "Click";
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_VSCROLL:
		// Get all the vertial scroll bar information.
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);
		// Save the position for comparison later on.
		yPos = si.nPos;
		switch (LOWORD(wParam))
		{
			// User clicked the HOME keyboard key.
		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// User clicked the END keyboard key.
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			si.nPos -= 1;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			si.nPos += 1;
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

			// User dragged the scroll box.
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			ScrollWindow(hWnd, 0, (int) (yChar* (yPos - (double) si.nPos)), NULL, NULL);
			UpdateWindow(hWnd);
			yypos = (int)yChar * si.nPos;
			
			update(hWnd, yypos, height );
		}
		return 0;
	break;
	case WM_SIZE:
		update(hWnd, yypos, height);
		break;
	case WM_CTLCOLOREDIT:
	{

		for (unsigned int i = 0; i < textBoxes.size(); ++i) {
			if (i + 200 == GetDlgCtrlID((HWND)lParam)) {
				switch (textBoxes.at(i).error) {
				case 0:
					break;
				case 1:
					SetTextColor((HDC)wParam, RGB(169, 0, 0));
					break;
				case 2:
					SetTextColor((HDC)wParam, RGB(0, 0, 255));
					break;
				case 3:
					SetTextColor((HDC)wParam, RGB(255, 0, 0));
					break;
				default:
					SetTextColor((HDC)wParam, RGB(255, 0, 255));
				}
			}
		}
		return (LRESULT)GetStockObject(WHITE_BRUSH);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hWnd, SB_VERT, &si);
		yPos = si.nPos;
		
		LPCSTR string1 = "First Compound";
		TextOutA(hdc, 5, 5 - (int) yChar * yPos , string1, 15);

		LPCSTR string2 = "Next Compounds";
		TextOutA(hdc, 310, 5 - (int) yChar * yPos, string2, 15);

		LPCSTR string3 = "Location";
		TextOutA(hdc, 615, 5 - (int) yChar * yPos, string3, 9);
		
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		_CrtDumpMemoryLeaks();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// colour control
LRESULT SetColors(HDC dc, HWND win)
{
	SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
	SetBkMode(dc, OPAQUE);
	SetBkColor(dc, GetSysColor(COLOR_WINDOW));
	return (LRESULT)CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}
