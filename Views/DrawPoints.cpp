#include "DrawPoints.h"

#include "../tracer.h"
#include "../model.h"

extern Model model;
extern int   zoom;

void DrawPoints(cairo_t *cr)
{
    int i;
	int	x1;
	int	y1;
	int	h = (VIEWSCRHEIGHT / 2);
	int	w = (VIEWSCRWIDTH / 2);

    //g_print("view points\r\n");
    for (i = 0; i < model.numP; i++)
    {
        x1 = (int)(model.tmp[i].x * zoom) + w;
        y1 = (int)(model.tmp[i].y * zoom) + h;

        cairo_rectangle(cr, x1, y1, 3, 3);
    }
}