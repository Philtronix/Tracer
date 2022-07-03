#include "DrawWire.h"

#include "../tracer.h"
#include "../model.h"

extern Model model;
extern int   zoom;

void DrawWire(cairo_t *cr)
{
    int i;
	int	h = (VIEWSCRHEIGHT / 2);
	int	w = (VIEWSCRWIDTH / 2);
	int	p1;
	int	p2;
	int	p3;

    //g_print("view wire\r\n");
    for (i = 0; i < model.numSurf; i++)
    {
        p1 = model.surfaces[i].p1 - 1;	// Model point numbers start at 1
        p2 = model.surfaces[i].p2 - 1;
        p3 = model.surfaces[i].p3 - 1;

        cairo_move_to(cr, (int)(model.tmp[p1].x * zoom) + w, (int)(model.tmp[p1].y * zoom) + h);
        cairo_line_to(cr, (int)(model.tmp[p2].x * zoom) + w, (int)(model.tmp[p2].y * zoom) + h);
        cairo_line_to(cr, (int)(model.tmp[p3].x * zoom) + w, (int)(model.tmp[p3].y * zoom) + h);
        cairo_line_to(cr, (int)(model.tmp[p1].x * zoom) + w, (int)(model.tmp[p1].y * zoom) + h);
    }
}

// EOF
