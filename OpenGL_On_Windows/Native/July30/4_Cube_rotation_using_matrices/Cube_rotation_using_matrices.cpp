#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<gl\GL.h>
#include<gl\glu.h>
#include<math.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#define MY_NAME_INITIAL "JCG"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool gbIsFullScreen = false;
HWND ghwnd;
bool gbActiveWindow=false;
bool gbEscapeKeyPressed = false;
HDC ghdc;
HGLRC ghrc;
GLfloat angle_cube = 0.0f;

GLfloat identity_matrix[16]={1.0f ,0.0f, 0.0f, 0.0f,
								 0.0f ,1.0f, 0.0f, 0.0f,
								 0.0f ,0.0f, 1.0f, 0.0f,
								 0.0f ,0.0f, 0.0f, 1.0f};
	
	GLfloat tx=0.0f, ty=0.0f, tz=-6.0f;
	

	GLfloat translation_matrix[16]={1.0f, 0.0f, 0.0f, 0.0f,
								    0.0f, 1.0f, 0.0f, 0.0f,
								    0.0f, 0.0f, 1.0f, 0.0f,
								    tx, ty, tz, 1.0f};
	
	GLfloat Sx=0.75f, Sy=0.75f, Sz=0.75f;
	
	GLfloat scale_matrix[16]={Sx, 0.0f, 0.0f, 0.0f,
							  0.0f, Sy, 0.0f, 0.0f,
							  0.0f, 0.0f, Sz, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f};
							  
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void initialize();
	void uninitialize();
	void update();
	void display();
	int iScreenWidth, iScreenHeight;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Cube rotation");	
	bool bDone = false;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW|CS_OWNDC;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);


	if (!RegisterClassEx(&wndclass)) {
		MessageBox(NULL, TEXT("Failed to register wndclass. Exiting"), TEXT("Error"), MB_OK);
		exit(EXIT_FAILURE);
	}
	iScreenWidth = GetSystemMetrics(0);
	iScreenHeight = GetSystemMetrics(1);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
		((iScreenWidth / 2) - 400), ((iScreenHeight / 2) - 300),
		800,600,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;
	initialize();
	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
				update();
				display();
			if (gbActiveWindow == true) {
				if (gbEscapeKeyPressed == true) {
					bDone = true;
				}
			}
		}
	}
	uninitialize();
	return((int)msg.wParam);
}
void initialize() {
	void resize(int,int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits=32;
	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	resize(800, 600);
}
void resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	
}

void drawCube() {
	glLineWidth(1);
	glBegin(GL_QUADS);
	//front face
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(-1.0f, 1.0f, 1.0f); //left top
	glVertex3f(-1.0f, -1.0f, 1.0f); //left bottom
	glVertex3f(1.0f, -1.0f, 1.0f); //right bottom
	glVertex3f(1.0f, 1.0f, 1.0f); //right top
	
	//right face
	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(1.0f, 1.0f, 1.0f); //left top
	glVertex3f(1.0f, -1.0f, 1.0f); //left bottom
	glVertex3f(1.0f, -1.0f, -1.0f); //right bottom
	
	glVertex3f(1.0f, 1.0f, -1.0f); //right top
	
	//back face
	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(1.0f, 1.0f, -1.0f); // left top
	glVertex3f(1.0f, -1.0f, -1.0f); //left bottom
	glVertex3f(-1.0f, -1.0f, -1.0f); //right bottom
	glVertex3f(-1.0f, 1.0f, -1.0f); //right top
	
	//left face
	glColor3ub(100, 149, 237); //corn flower
	glVertex3f(-1.0f, 1.0f, -1.0f); //left top
	glVertex3f(-1.0f, -1.0f, -1.0f);; //left bottom
	glVertex3f(-1.0f, -1.0f, 1.0f); //left bottom
	glVertex3f(-1.0f, 1.0f, 1.0f); //left top

	//top face
	glColor3ub(255, 0, 255); //corn flower
	glVertex3f(-1.0f, 1.0f, 1.0f); //left top
	glVertex3f(1.0f, 1.0f, 1.0f); //left top
	glVertex3f(1.0f, 1.0f, -1.0f);; //left bottom
	glVertex3f(-1.0f, 1.0f, -1.0f); //left bottom
	
	//bottom face
	glColor3ub(255, 128, 0); //corn flower
	glVertex3f(-1.0f, -1.0f, 1.0f); //left top
	glVertex3f(1.0f, -1.0f, 1.0f); //left top
	glVertex3f(1.0f, -1.0f, -1.0f);; //left bottom
	glVertex3f(-1.0f, -1.0f, -1.0f); //left bottom
	
	
	glEnd();
}
void display() {
	GLfloat angle_in_radian=(angle_cube*(3.14159/180));
	GLfloat x_rotation_matrix[16]={1.0f ,0.0f, 0.0f, 0.0f,
								   0.0f ,(float)cos(angle_in_radian), -(float)sin(angle_in_radian), 0.0f,
								   0.0f ,(float)sin(angle_in_radian), (float)cos(angle_in_radian), 0.0f,
								   0.0f ,0.0f, 0.0f, 1.0f};
	GLfloat y_rotation_matrix[16]={(float)cos(angle_in_radian) ,0.0f, (float)sin(angle_in_radian), 0.0f,
								   0.0f ,1.0f, 0.0f, 0.0f,
								   -(float)sin(angle_in_radian) ,0.0f, (float)cos(angle_in_radian), 0.0f,
								   0.0f ,0.0f, 0.0f, 1.0f};
	GLfloat z_rotation_matrix[16]={(float)cos(angle_in_radian), -(float)sin(angle_in_radian), 0.0f,0.0f,
								   (float)sin(angle_in_radian), (float)cos(angle_in_radian),0.0f, 0.0f, 
								   0.0f ,0.0f, 1.0f,0.0f,
								   0.0f ,0.0f, 0.0f, 1.0f};							   
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(identity_matrix);
	glMultMatrixf(translation_matrix);
	glMultMatrixf(scale_matrix);
	glMultMatrixf(x_rotation_matrix);
	glMultMatrixf(y_rotation_matrix);
	glMultMatrixf(z_rotation_matrix);
	drawCube();
	
	SwapBuffers(ghdc);
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE
	DWORD dwStyle;
	WINDOWPLACEMENT wpPrev;
	if (gbIsFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	DWORD dwStyle;
	static WINDOWPLACEMENT wpPrev;
	BOOL isWp;
	HMONITOR hMonitor;
	MONITORINFO monitorInfo;
	BOOL isMonitorInfo;
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (LOWORD(wParam)) {
		case VK_ESCAPE:
			gbEscapeKeyPressed = true;
			break;
		case 0x46:	//f
			dwStyle = GetWindowLong(hwnd, GWL_STYLE);
			if (gbIsFullScreen == false) {
				if (dwStyle & WS_OVERLAPPEDWINDOW) {
					wpPrev.length = sizeof(WINDOWPLACEMENT);
					isWp = GetWindowPlacement(hwnd, &wpPrev);
					hMonitor = MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY);
					monitorInfo.cbSize = sizeof(MONITORINFO);
					isMonitorInfo = GetMonitorInfo(hMonitor, &monitorInfo);
					if (isWp == TRUE && isMonitorInfo) {
						SetWindowLong(hwnd,GWL_STYLE,dwStyle&~WS_OVERLAPPEDWINDOW);
						SetWindowPos(hwnd, HWND_TOP,
							monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.top,
							monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
							monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
						ShowCursor(FALSE);
						gbIsFullScreen = true;
					}
				}
			} else { //restore
					SetWindowLong(hwnd,GWL_STYLE ,dwStyle | WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
					SetWindowPlacement(hwnd, &wpPrev);
					SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
					ShowCursor(TRUE);
					gbIsFullScreen = false;
			}
			break;

		default:
			break;
		}
			break;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
void update() {
	if (angle_cube < 360.0f) {
		angle_cube = angle_cube + 0.01f;
	}
	else
		angle_cube = 0.0f;
}