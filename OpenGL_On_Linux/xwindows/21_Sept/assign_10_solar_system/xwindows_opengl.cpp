#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

using namespace std;

bool gbFullScreen=false;


Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
FILE *fp;
GLXContext gGLXContext;
GLUquadric *quadric;
void resize(int,int);
GLfloat angleTri,angleRect=360.0;
static int year = 0;
static int day = 0;
int main(void){
	void CreateWindow(void);
	void ToggleFullscreen();
	void uninitialize();
	void initialize();
	void display();
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	char ascii_val[32];
	CreateWindow();
	bool bDone=false;

	XEvent event;
	KeySym keysym;
	initialize();
	fp=fopen("log.txt","w");
	fprintf(fp,"Writting logs\n");
	while(bDone==false){
	
	  while(XPending(gpDisplay)){
		XNextEvent(gpDisplay,&event);

		switch(event.type){
			case MapNotify:
				break;
			case KeyPress:
				//keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
				XLookupString(&event.xkey,ascii_val,sizeof(ascii_val),&keysym,NULL);
				switch(keysym){
					case XK_Escape:
							bDone=true;
							break;
					case XK_F:
					case XK_f:
						if(gbFullScreen==false){
								ToggleFullscreen();
								gbFullScreen=true;							
						}else{
								ToggleFullscreen();
								gbFullScreen=false;
						}
						break;
					case 'D':
						day = (day + 3) %360;
						break;
					case 'd':
						day=(day - 3) %360;
						break;
					case 'Y':
						year = (year + 3) % 360;
						break;
					case 'y':
						year = (year - 3) % 360;
						break;
				default:
						break;

				}
				break;
			case ButtonPress:
				switch(event.xbutton.button){
					case 1:// like WM_LBUTTONDOWN
						break;
					case 2:// like WM_MBUTTONDOWN
						break;
					default:
						break;

				}
			case MotionNotify:
					break;
			case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
			case Expose:
					break;
			case DestroyNotify:
					break;
			case 33: //close button event
					bDone=true;
			default:
				break;
		}
      }
		display();	
	}
	fclose(fp);
	return(0);
}

void CreateWindow(){
	void uninitialize();
	//variable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	static int frameBufferAttributes[]={
		GLX_DEPTH_SIZE, 24,
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER, 1,
		GLX_ALPHA_SIZE, 1,
		None

	};

	gpDisplay=XOpenDisplay(NULL);

	if(gpDisplay==NULL){
		
		printf("Error: Unable to Open X Display.\n Exiting now....\n");
		uninitialize();
		exit(1);

	}

	defaultScreen=XDefaultScreen(gpDisplay);
	
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
									RootWindow(gpDisplay,gpXVisualInfo->screen),
									gpXVisualInfo->visual,
									AllocNone);
	gColormap=winAttribs.colormap;

	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask|VisibilityChangeMask|ButtonPressMask|KeyPressMask|PointerMotionMask|StructureNotifyMask;

	styleMask=CWBorderPixel|CWBackPixel|CWEventMask|CWColormap;

	gWindow=XCreateWindow(gpDisplay,
					RootWindow(gpDisplay, gpXVisualInfo->screen),
					0,
					0,
					giWindowWidth,
					giWindowHeight,
					0,
					gpXVisualInfo->depth,
					InputOutput,
					gpXVisualInfo->visual,
					styleMask,
					&winAttribs);

					if(!gWindow)
					{
						printf("Error: Failed to Create Main Window.\n Exiting now..\n");
						uninitialize();
						exit(1);
					}
	XStoreName(gpDisplay,gWindow,"FirstWindow");
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

	XMapWindow(gpDisplay,gWindow);
}
void initialize(){
	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	resize(giWindowWidth,giWindowHeight);
}
void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);


	glPushMatrix();

	glRotatef(90.0f,0.0f,0.0f,1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric=gluNewQuadric();
	glColor3f(1.0f,1.0f,0.0f);
	gluSphere(quadric,0.75f,30,30);
	glPopMatrix();

	glPushMatrix();

	glRotatef((GLfloat)year,0.0f,1.0f,0.0f);
	glTranslatef(1.5f,0.0f,0.0f);
	glRotatef((GLfloat)day,0.0f,1.0f,0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric,0.2f,20,20);

	glPopMatrix();

	glXSwapBuffers(gpDisplay,gWindow);
	gluDeleteQuadric(quadric);
}

void resize(int width, int height)
{
	if(height==0)
		height=1;
	glViewport(0.0f,0.0f,(GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

}
void ToggleFullscreen()
{
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};
	
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));

	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=gbFullScreen? 0:1;

	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;

	XSendEvent(gpDisplay,
		RootWindow(gpDisplay,gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev);

}

void uninitialize(){
	
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();
	
	if(currentGLXContext !=NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}

	if(gGLXContext){
		glXDestroyContext(gpDisplay,gGLXContext);
	}

	if(gWindow){
		XDestroyWindow(gpDisplay,gWindow);

	}
	if(gColormap){
		XFreeColormap(gpDisplay,gColormap);
	}
	
	if(gpXVisualInfo){
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay){
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}














