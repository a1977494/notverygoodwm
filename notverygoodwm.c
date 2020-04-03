#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void scroll(Display * dpy, XEvent ev)
{
  switch (ev.xbutton.button) {
  case 4:
    XRaiseWindow(dpy, ev.xbutton.subwindow);
    XSetInputFocus(dpy, ev.xbutton.subwindow, RevertToPointerRoot, CurrentTime);
    break;
  case 5:
    XLowerWindow(dpy, ev.xbutton.subwindow);
    break;
  }  
}

int main()
{
  Display * dpy;
  Window root;
  XWindowAttributes attr;
  XEvent ev;
  XWindowChanges xwc;

  int memButtonPressKey = 0;
  int memButtonPressX = 0;
  int memButtonPressY = 0;
  

  
  if(!(dpy = XOpenDisplay(0x0))) return 1;

  root = DefaultRootWindow(dpy);

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
      if(ev.type == KeyPress) {

	switch (ev.xkey.keycode) {
	case 33: //p
	  system("dmenu_run &");
	  break;
	case 26: //e
	  system("emacs &");
	  break;
	case 67: //F1 test func
	  if(ev.xkey.subwindow != None) {
	    XRaiseWindow(dpy, ev.xkey.subwindow);
	    XSetInputFocus(dpy, ev.xkey.subwindow, RevertToPointerRoot, CurrentTime);
	    xwc.border_width = 5;
	    XConfigureWindow(dpy, ev.xkey.subwindow, CWBorderWidth, &xwc);
	    XSetWindowBorder(dpy, ev.xkey.subwindow, 30000);
	  }
	  break;
	default:
	  break;
	}
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
