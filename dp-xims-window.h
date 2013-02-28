#ifndef _DP_XIMS_WINDOW_H_
#define _DP_XIMS_WINDOW_H_

extern GdkWindow*	dp_xims_window_new ();
extern Window		dp_xims_window_get_xid (GdkWindow* xims_window);

extern void dp_event_loop (void);
#endif
