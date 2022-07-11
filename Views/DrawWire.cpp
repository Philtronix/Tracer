#include "DrawWire.h"

#include "../tracer.h"
#include "../model.h"

extern Model model[];
extern int   numModel;
extern int   zoom;
extern int   ScreenWidth;
extern int   ScreenHeight;

void DrawWire(cairo_t *cr)
{
    int i;
	int	h = (ScreenHeight / 2);
	int	w = (ScreenWidth / 2);
	int	p1;
	int	p2;
	int	p3;

    for (int m = 0; m < numModel; m++)
    {
        for (i = 0; i < model[m].numSurf; i++)
        {
            p1 = model[m].surfaces[i].p1 - 1;	// Model point numbers start at 1
            p2 = model[m].surfaces[i].p2 - 1;
            p3 = model[m].surfaces[i].p3 - 1;

            cairo_move_to(cr, (int)(model[m].tmp[p1].x * zoom) + w, (int)(model[m].tmp[p1].y * zoom) + h);
            cairo_line_to(cr, (int)(model[m].tmp[p2].x * zoom) + w, (int)(model[m].tmp[p2].y * zoom) + h);
            cairo_line_to(cr, (int)(model[m].tmp[p3].x * zoom) + w, (int)(model[m].tmp[p3].y * zoom) + h);
            cairo_line_to(cr, (int)(model[m].tmp[p1].x * zoom) + w, (int)(model[m].tmp[p1].y * zoom) + h);
        }
    }
}

// EOF
