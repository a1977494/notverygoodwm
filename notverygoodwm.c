#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Window wprev = 0;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAPLEN 256

void wSetBorder (Display * d, Window w)
{
  XWindowChanges xwc;
  xwc.border_width = 5;
  XConfigureWindow(d, w, CWBorderWidth, &xwc);
  XSetWindowBorder(d, w, 30000);
}
void wUnsetBorder (Display * d, Window w)
{
  XWindowChanges xwc;
  xwc.border_width = 5;
  XConfigureWindow(d, w, CWBorderWidth, &xwc);
  XSetWindowBorder(d, w, 10000);
}
void wfocus (Display * d, Window w)
{
  XRaiseWindow(d, w);
  XSetInputFocus(d, w, RevertToPointerRoot, CurrentTime);

  // set border
  //  wSetBorder(d, w);

  // unset border
  //if (wprev != 0 && wprev != w)
  //  {
  // xwc.border_width = 0;
  // XConfigureWindow(d, wprev, CWBorderWidth, &xwc);
  // XSetWindowBorder(d, wprev, 0);
  //}
  //wprev = w;
}
void scroll(Display * d, XEvent e)
{
  switch (e.xbutton.button) {
  case 4:
    wfocus(d, e.xbutton.subwindow);
    break;
  case 5:
    XLowerWindow(d, e.xbutton.subwindow);
    break;
  }  
}
void whide (Display * d, Window w)
{
  //XIconifyWindow(d, w, DefaultScreen(d));
  XUnmapWindow(d, w);
  XFlush(d);
}
void wshow (Display * d, Window w)
{
  XMapWindow(d, w);
  XFlush(d);
}
int catcher( Display *disp, XErrorEvent *xe )
{
  printf( "Something had happened, bruh.\n" );
  return 0;
}
struct map {
  Display * d;
  Window w;
  unsigned int key;
  int show;
};

struct map map[MAPLEN] = {0};
void mapUnmap (Display * d, Window w, unsigned int k)
{
  for (int i = 0; i < MAPLEN; i++) {
    if (map[i].key == k) {
      if (map[i].show)
	{
	  whide(d, map[i].w);
	  map[i].show = 0;
	}
      else
	{
	  wshow(d, map[i].w);
	  wfocus(d, map[i].w);
	  map[i].show = 1;
	}
      return;
    }
  }
  for (int i = 0; i < MAPLEN; i++) {
    if(map[i].key == 0) {
      whide(d, w);
      map[i].key = k;
      map[i].d = d;
      map[i].w = w;
      map[i].show = 0;
      break;
    }
  }
}
/*
  struct stateWindow
  {
  Window w;
  int number;
  struct list_head node;
  };
*/
int main()
{
  Display * dpy;
  Window root;
  XWindowAttributes attr;
  XEvent ev;
  XWindowChanges xwc;

  // mouse
  int memButtonPressKey = 0;
  int memButtonPressX = 0;
  int memButtonPressY = 0;

  // border window
  Window borderNow = 0;
  
  // hide window
  //  Window wls[10];
  Window wtmp = 0;
  
  if(!(dpy = XOpenDisplay(0x0))) return 1;
  XSetErrorHandler( catcher );
  

  root = DefaultRootWindow(dpy);
  XSelectInput(dpy, root, SubstructureNotifyMask | EnterWindowMask | LeaveWindowMask);

  int modCount = 4;
  unsigned int mod[] = {0x40,0x42,0x50,0x52,0x2040,0x2042,0x2050,0x2052};
  //unsigned int mod[] = {0x8,0xa,0x18,0x1a,0x2040,0x2042,0x2050,0x2052};
  for(int i = 0; i < modCount; i++) {
    XGrabKey(dpy, AnyKey, mod[i], root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(dpy, AnyKey, mod[i], root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
  }

  for(;;)
    {
      XNextEvent(dpy, &ev);

      printf("e.type %i\n", ev.type);
      fflush(stdout);

      if(ev.type == CreateNotify && ev.xcreatewindow.parent == root && ev.xcreatewindow.window != None) {
	//if(ev.type == CreateNotify && ev.xcreatewindow.window != None) {
	//if(ev.type == CreateNotify && ev.xcreatewindow.override_redirect == False && ev.xcreatewindow.parent == root) {
	//if(ev.type == CreateNotify) {
	XSelectInput(dpy, ev.xcreatewindow.window, SubstructureNotifyMask | EnterWindowMask | LeaveWindowMask);
	printf("create\n");
	fflush(stdout);
      }

      if(ev.type == DestroyNotify) {
	borderNow = 0;
	printf("destroy\n");
	fflush(stdout);
      }
      
      //if(ev.type == EnterNotify && ev.xcrossing.root == root && ev.xcrossing.subwindow == None ) {
      if(ev.type == EnterNotify && ev.xcrossing.window != borderNow) {
	XClassHint *hint = XAllocClassHint();
	XGetClassHint(dpy, ev.xcrossing.window, hint);
	printf("focusin %li %li %li %s\n", ev.xcrossing.root, ev.xcrossing.window, ev.xcrossing.subwindow, hint->res_class);
	fflush(stdout);

	// no need border only focus
	//XSetInputFocus(dpy, ev.xcrossing.window, RevertToPointerRoot, CurrentTime);
	//XRaiseWindow(dpy, ev.xcrossing.window);


	
	//wSetBorder(dpy, ev.xcrossing.window);
	//if (borderNow != 0)
	//  wUnsetBorder(dpy, borderNow);
	//borderNow = ev.xcrossing.window;
      }
      //if(ev.type == LeaveNotify ) {
      //printf("focusout %li %li %li\n", ev.xcrossing.root, ev.xcrossing.window, ev.xcrossing.subwindow);
      //wUnsetBorder(dpy, ev.xcrossing.window);
      //borderNow = 0;
      //}
      
      /*if(ev.type == EnterNotify && ev.xcrossing.subwindow != None) {
	printf("focusin\n");
	fflush(stdout);
	wSetBorder(dpy, ev.xcrossing.window);
	}*/
      //if(ev.type == LeaveNotify && ev.xcrossing.root == root) {
      /*if(ev.type == LeaveNotify && ev.xcrossing.subwindow != None) {
	printf("focusout\n");
	fflush(stdout);
	wUnsetBorder(dpy, ev.xcrossing.window);
	}*/
      
      if(ev.type == KeyPress) {
	switch (ev.xkey.keycode) {
	case 40: // d = dmenu
	  system("dmenu_run &");
	  break;
	case 54: // c = console
	  system("sakura &");
	  break;
	default:
	  mapUnmap(dpy, ev.xkey.subwindow, ev.xkey.keycode);
	}
	/*
	  if (ev.xkey.state & ShiftMask) {
	  //ShiftMask
	  //show hide window
	  mapUnmap(dpy, ev.xkey.subwindow, ev.xkey.keycode);
	  } else {
	  switch (ev.xkey.keycode) {
	  case 33: // p = programm
	  system("dmenu_run &");
	  break;
	  case 26: // e = emacs
	  system("emacs &");
	  break;
	  case 54: // c = console
	  system("sakura &");
	  break;
	  case 56: //b = browser
	  system("chromium &");
	  break;
	  case 43: // h = hide
	  if(ev.xkey.subwindow != None && wtmp == 0) {
	  wtmp = ev.xkey.subwindow;
	  whide(dpy, wtmp);
	  }
	  break;
	  case 39: // s = show
	  if(wtmp != 0) {
	  wshow(dpy, wtmp);
	  wtmp = 0;
	  }
	  break;
	  
	  case 67: // F1 test func
	  if(ev.xkey.subwindow != None) {
	  XRaiseWindow(dpy, ev.xkey.subwindow);
	  XSetInputFocus(dpy, ev.xkey.subwindow, RevertToPointerRoot, CurrentTime);
	  xwc.border_width = 5;
	  XConfigureWindow(dpy, ev.xkey.subwindow, CWBorderWidth, &xwc);
	  XSetWindowBorder(dpy, ev.xkey.subwindow, 30000);
	  }
	  break;
	  }
	  }
	*/
      }
      else if(ev.type == ButtonPress && ev.xbutton.subwindow != None && ev.xbutton.button == 5)
	scroll(dpy,ev);
      else if(ev.type == ButtonPress && ev.xbutton.subwindow != None && ev.xbutton.button == 4)
	scroll(dpy,ev);
      else if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
	  XGrabPointer(dpy, ev.xbutton.subwindow, True,
		       PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
		       GrabModeAsync, None, None, CurrentTime);
	  XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);

	  // save button and pos
	  memButtonPressKey = ev.xbutton.button;
	  memButtonPressX = ev.xbutton.x_root;
	  memButtonPressY = ev.xbutton.y_root;

	  // no need border only focus
	  XSetInputFocus(dpy, ev.xbutton.subwindow, RevertToPointerRoot, CurrentTime);
	  XRaiseWindow(dpy, ev.xbutton.subwindow);

        }
      else if(ev.type == ButtonRelease)
	{
	  XUngrabPointer(dpy, CurrentTime);
	}
      else if(ev.type == MotionNotify)
        {

	  // close WM when motion scroll button
	  if (memButtonPressKey == 2)
	    return 0;
	  
	  int xdiff, ydiff;
	  while(XCheckTypedEvent(dpy, MotionNotify, &ev));
	  xdiff = ev.xbutton.x_root - memButtonPressX;
	  ydiff = ev.xbutton.y_root - memButtonPressY;
	  XMoveResizeWindow(dpy, ev.xmotion.window,
			    attr.x + (memButtonPressKey==1 ? xdiff : 0),
			    attr.y + (memButtonPressKey==1 ? ydiff : 0),
			    MAX(1, attr.width + (memButtonPressKey==3 ? xdiff : 0)),
			    MAX(1, attr.height + (memButtonPressKey==3 ? ydiff : 0)));
        }
    }
}
