#include "DrawPoints.h"

#include "../tracer.h"
#include "../model.h"

extern Model model[];
extern int   numModel;
extern int   zoom;
extern int        ScreenWidth;
extern int        ScreenHeight;

void DrawPoints(cairo_t *cr)
{
    int i;
	int	x1;
	int	y1;
	int	h = (ScreenHeight / 2);
	int	w = (ScreenWidth / 2);

    //g_print("view points\r\n");
    for (int m = 0; m < numModel; m++)
    {
        for (i = 0; i < model[m].numP; i++)
        {
            x1 = (int)(model[m].tmp[i].x * zoom) + w;
            y1 = (int)(model[m].tmp[i].y * zoom) + h;

            cairo_rectangle(cr, x1, y1, 3, 3);
        }
    }
}

// EOF
