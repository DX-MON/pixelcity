/*-----------------------------------------------------------------------------

  Win.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  Create the main window and make it go.

-----------------------------------------------------------------------------*/

#define MOUSE_MOVEMENT          0.5F

#ifdef _WINDOWS
	#include <windows.h>
#endif
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#ifdef _WINDOWS
	#include <scrnsave.h>
#endif
#include "Camera.h"
#include "Car.h"
#include "Entity.h"
#include "glTypes.h"
#include "Macro.h"
#include "Random.h"
#include "Render.h"
#include "Texture.h"
#include "OS.h"
#include "World.h"
#include "Visible.h"
#include <gdk/gdkkeysyms.h>

GTKGLWindow *hwnd;
int width;
int height;
int half_width;
int half_height;
bool lmb;
bool rmb;
bool mouse_forced;
GTKPoint select_pos;
GTKPoint mouse_pos;
bool quit;

class Events : public GTKEvents
{
private:
	static bool RButtonDown(GtkWidget *widget, GdkEventButton *event);
	static bool LButtonDown(GtkWidget *widget, GdkEventButton *event);
	static bool RButtonUp(GtkWidget *widget, GdkEventButton *event);
	static bool LButtonUp(GtkWidget *widget, GdkEventButton *event);
	GTKGLWindow *GLWindow;

public:
	static bool ButtonDown(GtkWidget *widget, GdkEventButton *event, void *data);
	static bool ButtonUp(GtkWidget *widget, GdkEventButton *event, void *data);
	static bool KeyDown(GtkWidget *widget, GdkEventKey *event, void *data);
	static bool Paint(GtkWidget *widget, GdkEventExpose *event, void *data);
	static bool MoveSize(GtkWidget *widget, GdkEventConfigure *event, void *data);
	static bool MouseMove(GtkWidget *widget, GdkEventMotion *event, void *data);

	void Init(GTKWindow *Window);
	void Deinit();

	uint32_t ButtonDownID;
	uint32_t ButtonUpID;
	uint32_t PaintID;
	uint32_t MoveSizeID;
	uint32_t MouseMoveID;
	uint32_t KeyDownID;
};

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
									n o t e
-----------------------------------------------------------------------------*/

void WinPopup(char* message, ...)
{
	va_list marker;
	char *buf;
	int nBuf;

	va_start(marker, message);
	nBuf = vsnprintf(NULL, 0, message, marker) + 1;
	va_end(marker);
	buf = (char *)malloc(nBuf);
	va_start(marker, message);
	vsprintf(buf, message, marker);
	va_end(marker);
	hwnd->MessageBox(GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, (const char *)buf, APP_TITLE);
	free(buf);
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int WinWidth(void)
{
	return width;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void WinMousePosition(int* x, int* y)
{
	*x = select_pos.x;
	*y = select_pos.y;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int WinHeight(void)
{
	return height;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void WinTerm(void)
{
#if !SCREENAVER
	delete hwnd;
#endif
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GTKGLWindow *WinHwnd(void)
{
	return hwnd;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void AppQuit()
{
	quit = true;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool Events::Paint(GtkWidget *widget, GdkEventExpose *event, void *)
{
	CameraUpdate();
	EntityUpdate();
	WorldUpdate();
	TextureUpdate();
	VisibleUpdate();
	CarUpdate();
	RenderUpdate();
	return true;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void AppInit(void)
{
	RandomInit(time(NULL));
	CameraInit();
	RenderInit();
	TextureInit();
	WorldInit();
}

/*-----------------------------------------------------------------------------
								W i n M a i n
-----------------------------------------------------------------------------*/

void AppTerm(void)
{
	TextureTerm();
	WorldTerm();
	RenderTerm();
	CameraTerm();
	WinTerm();
}

void Events::Init(GTKWindow *Wnd)
{
	Window = Wnd;
	GLWindow = (GTKGLWindow *)Window;
	EventHandlers.ButtonDown = Events::ButtonDown;
	EventHandlers.Paint = Events::Paint;
	EventHandlers.MoveSize = Events::MoveSize;
	EventHandlers.MouseMove = Events::MouseMove;
	EventHandlers.KeyDown = Events::KeyDown;
	ButtonDownID = Window->SetHandler("button_press_event", (void *)Events::ButtonDown);
	ButtonUpID = Window->SetHandler("button_release_event", (void *)Events::ButtonUp);
	PaintID = Window->SetHandler("expose_event", (void *)Events::Paint);
	MoveSizeID = Window->SetHandler("configure_event", (void *)Events::MoveSize);
	MouseMoveID = Window->SetHandler("motion_notify_event", (void *)Events::MouseMove);
	KeyDownID = Window->SetHandler("key_press_event", (void *)Events::KeyDown);
	Window->SetEventsHandled(GDK_BUTTON_PRESS_MASK | GDK_EXPOSURE_MASK | GDK_STRUCTURE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_KEY_PRESS_MASK);
	if (Window->GetQuitHandlerID() == 0)
		Window->RegisterQuitFunction();
}

void Events::Deinit()
{
	Window->RemoveHandler(ButtonDownID);
	Window->RemoveHandler(ButtonUpID);
	Window->RemoveHandler(PaintID);
	Window->RemoveHandler(MoveSizeID);
	Window->RemoveHandler(MouseMoveID);
	Window->RemoveHandler(KeyDownID);
	Window->UnregisterQuitFunction();
}

/*-----------------------------------------------------------------------------
								W i n M a i n
-----------------------------------------------------------------------------*/

#ifdef _WINDOWS
int PASCAL WinMain(HINSTANCE, HINSTANCE, char *, int)
#else
int main(int argc, char **argv)
#endif
{
	GTKGL::GTKInit(argc, argv);

	WinInit();
	AppInit();
	CameraReset();
	hwnd->DoMessageLoop();
	AppTerm();
	return 0;
}

bool Events::KeyDown(GtkWidget *widget, GdkEventKey *event, void *data)
{
	bool Control, Alt;
	uint32_t modifiers, Key;
	if (event->is_modifier == TRUE)
		return false;

	modifiers = event->state;
	Key = GTKKey::ToUpper(event->keyval);

	if ((modifiers & GDK_CONTROL_MASK) != 0)
		Control = true;
	else if ((modifiers & GDK_MOD1_MASK) != 0)
		Alt = true;

	if (Key == 'R')
		WorldReset();
	else if (Key == 'W')
		RenderWireframeToggle();
	else if (Key == 'E')
		RenderEffectCycle();
	else if (Key == 'L')
		RenderLetterboxToggle();
	else if (Key == 'F')
		RenderFPSToggle();
	else if (Key == 'G')
		RenderFogToggle();
	else if (Key == 'T')
		RenderFlatToggle();
	else if (Key == GDK_F1)
		RenderHelpToggle();
	else if (Key == GDK_Escape)
		hwnd->Close();
	//Dev mode keys
	else if (Key == 'C')
		CameraAutoToggle();
	else if (Key == 'B')
		CameraNextBehavior();
	else if (Key == GDK_F5)
		CameraReset();
	else if (Key == GDK_Up)
		CameraMedial(1.0f);
	else if (Key == GDK_Down)
		CameraMedial(-1.0f);
	else if (Key == GDK_Left)
		CameraLateral(1.0f);
	else if (Key == GDK_Right)
		CameraLateral(-1.0f);
	else if (Key == GDK_Prior)
		CameraVertical(1.0f);
	else if (Key == GDK_Next)
		CameraVertical(-1.0f);
	else
		return false;
	return true;
}

bool Events::ButtonDown(GtkWidget *widget, GdkEventButton *event, void *)
{
	if (event->button == 0)
		return RButtonDown(widget, event);
	else if (event->button == 1)
		return LButtonDown(widget, event);
	return false;
}

bool Events::LButtonDown(GtkWidget *widget, GdkEventButton *event)
{
	lmb = true;
	return true;
}

bool Events::RButtonDown(GtkWidget *widget, GdkEventButton *event)
{
	rmb = true;
	return true;
}

bool Events::ButtonUp(GtkWidget *widget, GdkEventButton *event, void *)
{
	if (event->button == 0)
		return RButtonUp(widget, event);
	else if (event->button == 1)
		return LButtonUp(widget, event);
	return false;
}

bool Events::LButtonUp(GtkWidget *widget, GdkEventButton *event)
{
	lmb = false;
	return true;
}

bool Events::RButtonUp(GtkWidget *widget, GdkEventButton *event)
{
	rmb = false;
	return true;
}

bool Events::MoveSize(GtkWidget *widget, GdkEventConfigure *event, void *data)
{
	width = event->width; // width of client area
	height = event->height; // height of client area
	half_width = width / 2;
	half_height = height / 2;
	RenderResize();
	return true;
}

bool Events::MouseMove(GtkWidget *widget, GdkEventMotion *event, void *data)
{
	GTKPoint p;
	int delta_x, delta_y;

	hwnd->GetCursorPos(&p);
	if (p.x < 0 || p.x > width)
		return true;
	else if (p.y < 0 || p.y > height)
		return true;
	else if (!mouse_forced && !lmb && !rmb)
		select_pos = p;
	if (!mouse_forced && (rmb || lmb))
	{
		delta_x = (float)(mouse_pos.x - p.x) * MOUSE_MOVEMENT;
		delta_y = (float)(mouse_pos.y - p.y) * MOUSE_MOVEMENT;
		if (rmb && lmb)
		{
			GLvector pos;
			CameraPan(delta_x);
			pos = CameraPosition();
			pos.y += delta_y;
			CameraPositionSet(pos);
		}
		else if (rmb)
		{
			CameraPan(delta_x);
			CameraForward(delta_y);
		}
		else if (lmb)
		{
			GLvector angle;
			angle = CameraAngle();
			angle.y -= delta_x;
			angle.x += delta_y;
			CameraAngleSet(angle);
		}
	}
	else
		mouse_forced = false;
	mouse_pos = p;
	return true;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool Shutdown(GtkWidget *widget, GdkEvent *event, void *data)
{
	hwnd->RemoveTimeout();
	hwnd->QuitMessageLoop();
	hwnd->DestroyGLFonts();
	hwnd->Destroy();
	return false;
}

bool WinInit(void)
{
	GTKRect Desktop;

	hwnd = new GTKGLWindow(GTK_WINDOW_TOPLEVEL, GTKGLWindow::MakeStandardConfig(), (void *)Shutdown, GDK_GL_RGBA_TYPE, true);
	hwnd->SetTitle(APP_TITLE);
	hwnd->SetBorderless();
	//hwnd->SetSize(width, height);
	hwnd->SetFullscreen();
	hwnd->SetAlwaysOnTop();
	hwnd->SetFocus(hwnd);
	hwnd->ShowWindow();
	Desktop = hwnd->GetWindowRect();
	width = Desktop.bottom - Desktop.top;
	height = Desktop.right - Desktop.left;
	hwnd->SetLocation(0, 0);
	hwnd->SetEvents(new Events());

	half_width = width / 2;
	half_height = height / 2;

	return true;
}
