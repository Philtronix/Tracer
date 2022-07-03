// Philtronix ltd

#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "Matrix4.h"

#include "tracer.h"
#include "model.h"
#include "icosphere.h"

#include "Views/DrawPoints.h"
#include "Views/DrawWire.h"
#include "Views/DrawFlat.h"
#include "Views/DrawGouraud.h"
#include "Views/DrawPhil.h"

#define NORMAL_LEN 20

//#define DEBUG(x) g_print(x)
#define DEBUG(x)

static void DrawView(cairo_t *cr, int style);

Vec3D  CrossProduct(Vec3D a, Vec3D b);

// GTK Objects
GtkBuilder *builder; 
GtkWidget  *window;
GtkWidget  *screen;
GdkPixbuf  *pixbuf;

int   n_channels;
int   rowstride;
int   pixWidth;
int   pixHeight;
Model model;
char  szPath[400];
bool  showNormals = false;
int   view = VIEW_WIRE;
int   ScreenWidth = 0;
int   ScreenHeight = 0;

// 3D Variables
double pitch = 0.0;
double yaw = 0.0;
int    zoom = 1;
Vec3D  eye(0.0, 0.0, 0.0);
Vec3D  target(0.0, 0.0, 0.0);
double zbuffer[VIEWSCRWIDTH][VIEWSCRHEIGHT];
item   sortList[9000];
Vec3D  lightPos(10.0, 10.0, 10.0);

int main(int argc, char *argv[]) 
{
	gtk_init(&argc, &argv);

	// Load glade file
	builder = gtk_builder_new_from_file ("tracer.glade");
 
	// Get access to objects
	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
	screen = GTK_WIDGET(gtk_builder_get_object(builder, "screen"));

	// Default view = wire frame
	GtkToggleButton* button;
	button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "wireframe"));
	gtk_toggle_button_set_active(button, true);
	view = VIEW_WIRE;

	// Force minnimum window sizes
	gtk_widget_set_size_request(window, VIEWSCRWIDTH + 500, VIEWSCRHEIGHT + 100);
	gtk_widget_set_size_request(screen, VIEWSCRWIDTH, VIEWSCRHEIGHT);

	// Connect signals
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show(window);

	// Get window size
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(screen, alloc);
	int w = alloc->width;
	int h = alloc->height;
	g_print("w = %d, h = %d\r\n", w, h);
	g_free(alloc);
    ScreenWidth = w;
    ScreenHeight = h;

	// Create pixbuf
	pixbuf     = gdk_pixbuf_new( GDK_COLORSPACE_RGB, FALSE, 8, w, h);
	n_channels = gdk_pixbuf_get_n_channels(pixbuf);
	pixWidth   = gdk_pixbuf_get_width(pixbuf);
	pixHeight  = gdk_pixbuf_get_height(pixbuf);

	// Ensure that the pixbuf is valid
	g_assert(gdk_pixbuf_get_colorspace(pixbuf) == GDK_COLORSPACE_RGB);
	g_assert(gdk_pixbuf_get_bits_per_sample(pixbuf) == 8);
	rowstride = gdk_pixbuf_get_rowstride(pixbuf);

	// Fill it
	gdk_pixbuf_fill(pixbuf, 0xffff00ff);

	// Load model
	if (getcwd(szPath, sizeof(szPath)) != NULL)
	{
//		model.LoadObjFile(szPath, (char *)"/Models/Monkey.obj", 160);		// Texture, normals
//		model.LoadObjFile(szPath, (char *)"/Models/Teapot_2.obj", 3);		// Normals
//		model.LoadObjFile(szPath, (char *)"/Models/icosahedron.obj", 150);	// Normals
		model.LoadObjFile(szPath, (char *)"/Models/BigTeapot.obj", 100);	// -
//		model.LoadObjFile(szPath, (char *)"/Models/Teapot.obj", 6);			// Texture. normals
//		model.LoadObjFile(szPath, (char *)"/Models/cube.obj", 100);			// Texture, normals

		// Create an Icosphere
//		Create(3);

		// Add Normals
//		model.Mesh_normalise();
	}
	else
	{
		g_print("Failed to get path.\r\n");
	}

	// Start GTK
	gtk_main();

	return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// GTK Callback functions

extern "C" G_MODULE_EXPORT void on_pitch_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	pitch = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_yaw_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	yaw = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_zoom_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	zoom = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_eyeX_value_changed(GtkAdjustment* adjustment, gpointer data)
{
	eye.x = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_eyeY_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	eye.y = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_eyeZ_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	eye.z = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_targetX_value_changed(GtkAdjustment* adjustment, gpointer data)
{
	target.x = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_targetY_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	target.y = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_targetZ_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	target.z = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_lightX_value_changed(GtkAdjustment* adjustment, gpointer data)
{
	lightPos.x = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_lightY_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	lightPos.y = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_lightZ_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	lightPos.z = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_showNormals_toggled(GtkToggleButton* toggle_button, gpointer data) 
{
	if (true == gtk_toggle_button_get_active(toggle_button))
	{
		g_print("Show Normals - active\r\n");
		showNormals = true;
	}
	else
	{
		g_print("Show Normals - inactive\r\n");
		showNormals = false;
	}

	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_points_toggled(GtkToggleButton* button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Points\r\n");
		view = VIEW_POINTS;
		gtk_widget_queue_draw(screen);
	}
}

extern "C" G_MODULE_EXPORT void on_wireframe_toggled(GtkToggleButton* button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Wire frame\r\n");
		view = VIEW_WIRE;
		gtk_widget_queue_draw(screen);
	}
}

extern "C" G_MODULE_EXPORT void on_flat_toggled(GtkToggleButton* button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Flat\r\n");
		view = VIEW_FLAT;
		gtk_widget_queue_draw(screen);
	}
}

extern "C" G_MODULE_EXPORT void on_gouraud_toggled(GtkToggleButton* button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Gouraud\r\n");
		view = VIEW_GOURAUD;
		gtk_widget_queue_draw(screen);
	}
}

extern "C" G_MODULE_EXPORT void on_phong_toggled(GtkToggleButton* button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Phong\r\n");
		view = VIEW_PHONG;
		gtk_widget_queue_draw(screen);
	}
}

extern "C" G_MODULE_EXPORT void on_raytrace_toggled(GtkToggleButton* button, gpointer data) 
{
	if (gtk_toggle_button_get_active(button))
    {
		g_print("Ray Trace\r\n");
		view = VIEW_RAYTRACE;
		gtk_widget_queue_draw(screen);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Drawing functions

// Drawing callback
extern "C" G_MODULE_EXPORT gboolean on_screen_draw(GtkWidget *widget, cairo_t *cr, gpointer data) 
{
	DEBUG("on_screen_draw()\r\n");
	cairo_set_line_width(cr, 1.0);
			
	// Draw the image at 110, 90, except for the outermost 10 pixels.
	gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
	cairo_rectangle(cr, 0, 0, pixWidth, pixHeight);
	cairo_fill(cr);

	// Erase background
	gdk_pixbuf_fill( pixbuf, 0xffff00ff );

	// TODO : Only do this at init!
	GtkAllocation* alloc = g_new(GtkAllocation, 1);
	gtk_widget_get_allocation(widget, alloc);
	int w = alloc->width;
	int h = alloc->height;
	g_free(alloc);

	// Draw border
	cairo_move_to(cr, 1, 1);
	cairo_line_to(cr, w, 1);
	cairo_line_to(cr, w, h);
	cairo_line_to(cr, 1, h);
	cairo_line_to(cr, 1, 1);

	// Set line colour
	cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);

	// --- load model ---
	DrawView(cr, view);

	// Update
	cairo_stroke(cr);

	DEBUG("on_screen_draw() - [done]\r\n");
	return FALSE;
}

void PutPixel(GdkPixbuf *pixbuf, int x, int y, guchar red, guchar green, guchar blue)
{
	// Ensure that the coordinates are in a valid range
	if ((x >= 0 && x < pixWidth) &&
	    (y >= 0 && y < pixHeight))
	{
		// The pixel buffer in the GdkPixbuf instance
		// TODO: Do once only ?
		guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

		// The pixel we wish to modify
		guchar *p = pixels + y * rowstride + x * n_channels;
		p[0] = red;
		p[1] = green;
		p[2] = blue;
	}
}

///////////////////////////////////////////////////////////////////////////////

// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

// a b c d     x   ax + by + cz + d
// e f g h  *  y = ex + fy + gz + h
// i j k l     z   ix + jy + kz + l
// 0 0 0 1     1         1
void ViewMatrix(Vec3D *result, Matrix4 *matrix, Vec3D *data)
{
	result->x = (matrix->r1.x * data->x) + (matrix->r1.y * data->y) + (matrix->r1.z * data->z) + matrix->r1.x;
	result->y = (matrix->r2.x * data->x) + (matrix->r2.y * data->y) + (matrix->r2.z * data->z) + matrix->r2.y;
	result->z = (matrix->r3.x * data->x) + (matrix->r3.y * data->y) + (matrix->r3.z * data->z) + matrix->r3.z;
	result->w = 1;
}

void DrawView(cairo_t *cr, int style)
{
	int		i;
	int		h = (VIEWSCRHEIGHT / 2);
	int		w = (VIEWSCRWIDTH / 2);
	Matrix4 view;
	Vec3D	v1;
	Vec3D	v2;
	Vec3D	v3;
	Vec3D	n1;
	Vec3D	n2;
	Vec3D	n3;
	int		x1;
	int		y1;
	int		x2;
	int		y2;
	Vec3D	sNorm;
	Vec3D	centre;
	Vec3D	direction;
	Vec3D	tmpVec;

	DEBUG("DrawView()\r\n");

/*
	// Clear zBuffer
	for (int y = 0; y < VIEWSCRHEIGHT; y++)
	{
		for (int x = 0; x < VIEWSCRWIDTH; x++)
		{
			zbuffer[x][y] = -50000;
		}
	}
*/

	// Radians = (degrees * pi) / 180;
	double radPitch = (pitch * PI) / 180.0;
	double radYaw = (yaw * PI) / 180.0;

	// OLD
	view.FPSViewRH(eye, radPitch, radYaw);

	// NEW
//	Vec3D vUp = { 0.0, 1.0, 0.0 };
//	view.LookAtRH(eye, target, vUp);

	// Move the points
	DEBUG("points");
	for (i = 0; i < model.numP; i++)
	{
		ViewMatrix(&model.tmp[i], &view, &(model.data[i]));
	}
	DEBUG(" - [done]\r\n");

	// Move the normals
	if (showNormals)
	{
		DEBUG("show normals");
		for (i = 0; i < model.numNorm; i++)
		{
			tmpVec = model.data[i];
			tmpVec.x += model.normal[i].x * NORMAL_LEN;
			tmpVec.y += model.normal[i].y * NORMAL_LEN;
			tmpVec.z += model.normal[i].z * NORMAL_LEN;

			ViewMatrix(&model.showNorm[i], &view, &tmpVec);
		}
		DEBUG(" - [done]\r\n");
	}

	DEBUG("normals");
	for (i = 0; i < model.numNorm; i++)
	{
		ViewMatrix(&model.tmpNorm[i], &view, &model.normal[i]);
	}
	DEBUG(" - [done]\r\n");

	switch (style)
	{
	case VIEW_POINTS:
		DrawPoints(cr);
		break;

	case VIEW_WIRE:
		DrawWire(cr);
		break;

	case VIEW_FLAT:
		SortSurfaces();
		DrawFlat(cr);
		break;

	case VIEW_GOURAUD:
		SortSurfaces();
		DrawGouraud(cr);
		break;

	case VIEW_PHONG:
		SortSurfaces();
		DrawPhil(cr);
		break;

	case VIEW_RAYTRACE:
		g_print("view Ray trace\r\n");
		break;

	default:
		g_print("Bad view type\r\n");
	}

	if (showNormals)
	{
		DEBUG("Show normals");

		// Set line colour to full red
		cairo_stroke(cr);
		cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);

		for (i = 0; i < model.numP; i++)
		{
			// Only show normal if it is visible
			if (model.showNorm[i].z > 0)
			{
				x1 = (int)(model.tmp[i].x * zoom) + w;
				y1 = (int)(model.tmp[i].y * zoom) + h;

				x2 = (int)(model.showNorm[i].x * zoom) + w;
				y2 = (int)(model.showNorm[i].y * zoom) + h;

				cairo_move_to(cr, x1, y1);
				cairo_line_to(cr, x2, y2);
			}
		}
		DEBUG(" - [done]\r\n");
	}

	DEBUG("DrawView() [done]\r\n");
}

///////////////////////////////////////////////////////////////////////////////

Vec3D CrossProduct(Vec3D a, Vec3D b)
{
	Vec3D OutVector = { 0, 0, 0 };

	OutVector.x = (a.y * b.z - a.z * b.y);
	OutVector.y = (a.z * b.x - a.x * b.z);
	OutVector.z = (a.x * b.y - a.y * b.x);

	return OutVector;
}

double DotProduct(Vec3D a, Vec3D b)
{
	return acos(a.x * b.x + a.y * b.y + a.z * b.z);
}

Vec3D GetSurfaceNormal(Vec3D a, Vec3D b, Vec3D c)
{
	// Find the first vector and second vector
	Vec3D VectorAB = { b.x - a.x, b.y - a.y, b.z - a.z };
	Vec3D VectorAC = { c.x - a.x, c.y - a.y, c.z - a.z };

	// Normalize vectors
	double LengthAB = sqrt((VectorAB.x * VectorAB.x) + (VectorAB.y * VectorAB.y) + (VectorAB.z * VectorAB.z));
	VectorAB.x = VectorAB.x / LengthAB;
	VectorAB.y = VectorAB.y / LengthAB;
	VectorAB.z = VectorAB.z / LengthAB;

	double LengthAC = sqrt((VectorAC.x * VectorAC.x) + (VectorAC.y * VectorAC.y) + (VectorAC.z * VectorAC.z));
	VectorAC.x = VectorAC.x / LengthAC;
	VectorAC.y = VectorAC.y / LengthAC;
	VectorAC.z = VectorAC.z / LengthAC;

	// Return the computed cross product
	return CrossProduct(VectorAB, VectorAC);
}

void SortSurfaces()
{
	int    p1;
	int    p2;
	int    p3;

	DEBUG("SortSurfaces()\r\n");

	// Calculate average Z values
	for (int i = 0; i < model.numSurf; i++)
	{
		p1 = model.surfaces[i].p1 - 1;	// Model point numbers start at 1
		p2 = model.surfaces[i].p2 - 1;
		p3 = model.surfaces[i].p3 - 1;

		sortList[i].avz = (model.tmp[p1].z + model.tmp[p2].z + model.tmp[p3].z) / 3;
		sortList[i].surface = i;
	}

	DEBUG("SortSurfaces() bubble\r\n");

	// Now  perform a bubble sort
	// loop to access each array element
	for (int step = 0; step < model.numSurf - 1; ++step)
	{
		// loop to compare array elements
		for (int i = 0; i < model.numSurf - step - 1; ++i)
		{
			// compare two adjacent elements
			// change > to < to sort in descending order
			if (sortList[i].avz > sortList[i + 1].avz)
			{
				// swapping occurs if elements are not in the intended order
				item temp = sortList[i];
				sortList[i] = sortList[i + 1];
				sortList[i + 1] = temp;
			}
		}
	}

	DEBUG("SortSurfaces() done\r\n");
}

// EOF