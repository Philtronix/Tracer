// Philtronix ltd

#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "Matrix4.h"

#include "tracer.h"
#include "model.h"
#include "icosphere.h"
#include "Rectangle.h"
#include "cube.h"

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
Model model[10];
int   numModel = 0;
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
item   sortList[9000];
Vec3D  lightPos(10.0, 10.0, 10.0);

///////////////////////////////////////////////
/*
static GtkTargetEntry entries[] = {
  { "GTK_LIST_BOX_ROW", GTK_TARGET_SAME_APP, 0 }
};


static void
drag_begin (GtkWidget      *widget,
            GdkDragContext *context,
            gpointer        data)
{
  GtkWidget *row;
  GtkAllocation alloc;
  cairo_surface_t *surface;
  cairo_t *cr;
  int x, y;
  double sx, sy;

  row = gtk_widget_get_ancestor (widget, GTK_TYPE_LIST_BOX_ROW);
  gtk_widget_get_allocation (row, &alloc);
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, alloc.width, alloc.height);
  cr = cairo_create (surface);

  gtk_style_context_add_class (gtk_widget_get_style_context (row), "drag-icon");
  gtk_widget_draw (row, cr);
  gtk_style_context_remove_class (gtk_widget_get_style_context (row), "drag-icon");

  gtk_widget_translate_coordinates (widget, row, 0, 0, &x, &y);
  cairo_surface_get_device_scale (surface, &sx, &sy);
  cairo_surface_set_device_offset (surface, -x * sx, -y * sy);
  gtk_drag_set_icon_surface (context, surface);

  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void
drag_data_get (GtkWidget        *widget,
               GdkDragContext   *context,
               GtkSelectionData *selection_data,
               guint             info,
               guint             time,
               gpointer          data)
{
  gtk_selection_data_set (selection_data,
                          gdk_atom_intern_static_string ("GTK_LIST_BOX_ROW"),
                          32,
                          (const guchar *)&widget,
                          sizeof (gpointer));
}

static void
drag_data_received (GtkWidget        *widget,
                    GdkDragContext   *context,
                    gint              x,
                    gint              y,
                    GtkSelectionData *selection_data,
                    guint             info,
                    guint32           time,
                    gpointer          data)
{
  GtkWidget *target;
  GtkWidget *row;
  GtkWidget *source;
  int pos;

  target = widget;

  pos = gtk_list_box_row_get_index (GTK_LIST_BOX_ROW (target));
  row = (GtkWidget*)gtk_selection_data_get_data (selection_data);
  source = gtk_widget_get_ancestor (row, GTK_TYPE_LIST_BOX_ROW);

  if (source == target)
    return;

  g_object_ref (source);
  gtk_container_remove (GTK_CONTAINER (gtk_widget_get_parent (source)), source);
  gtk_list_box_insert (GTK_LIST_BOX (gtk_widget_get_parent (target)), source, pos);
  g_object_unref (source);
}



static GtkWidget * create_row (const gchar *text)
{
  GtkWidget *row, *handle, *box, *label, *image;

  row = gtk_list_box_row_new ();

  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
  g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);
  gtk_container_add (GTK_CONTAINER (row), box);

  handle = gtk_event_box_new ();
  image = gtk_image_new_from_icon_name ("open-menu-symbolic", (GtkIconSize)1);
  gtk_container_add (GTK_CONTAINER (handle), image);
  gtk_container_add (GTK_CONTAINER (box), handle);

  label = gtk_label_new (text);
  gtk_container_add_with_properties (GTK_CONTAINER (box), label, "expand", TRUE, NULL);

  gtk_drag_source_set (handle, GDK_BUTTON1_MASK, entries, 1, GDK_ACTION_MOVE);
  g_signal_connect (handle, "drag-begin", G_CALLBACK (drag_begin), NULL);
  g_signal_connect (handle, "drag-data-get", G_CALLBACK (drag_data_get), NULL);

  gtk_drag_dest_set (row, GTK_DEST_DEFAULT_ALL, entries, 1, GDK_ACTION_MOVE);
  g_signal_connect (row, "drag-data-received", G_CALLBACK (drag_data_received), NULL);

  return row;
}
*/


///////////////////////////////////////////////

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


///////////////////////////////////////////////
#if 0
/*
	GtkWidget *list;
	GtkWidget *row;
	gchar *text;

	list = gtk_list_box_new ();
	gtk_list_box_set_selection_mode (GTK_LIST_BOX (list), GTK_SELECTION_NONE);
	gtk_container_add (GTK_CONTAINER (window), list);

	for (int i = 0; i < 20; i++)
	{
		text = g_strdup_printf ("Row %d", i);
		row = create_row (text);
		gtk_list_box_insert (GTK_LIST_BOX (list), row, -1);
	}
*/
//    listbox = gtk_list_new ();
	GtkListBox* listbox;
	listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "bob"));

	listbox

//    gtk_signal_connect (GTK_OBJECT (listbox), "selection_changed",
//            GTK_SIGNAL_FUNC (listbox_changed), "selection_changed");

    /* --- Set listbox style. --- */
//    gtk_list_set_selection_mode (GTK_LIST (listbox), GTK_SELECTION_BROWSE);

    /* --- Make it visible --- */
//    gtk_widget_show (listbox);

    /* --- Add items into the listbox --- */
    AddListItem (listbox, "This is a listbox");
    AddListItem (listbox, "Quite useful ... ");
    AddListItem (listbox, "When it needs to be.");
    AddListItem (listbox, "This list can be ");
    AddListItem (listbox, "quite long, you know.");
#endif

//////////////////////////////////////////////

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
		Vec3D pos;
		pos.x = 0.0;
		pos.y = 0.0;
		pos.z = 0.0;

//		model[0].LoadObjFile(szPath, (char *)"/Models/Monkey.obj", 160);		// Texture, normals
//		model[0].LoadObjFile(szPath, (char *)"/Models/Teapot_2.obj", 3);		// Normals
//		model[0].LoadObjFile(szPath, (char *)"/Models/icosahedron.obj", 150);	// Normals
//		model[0].LoadObjFile(szPath, (char *)"/Models/BigTeapot.obj", 100);		// -
//		model[0].LoadObjFile(szPath, (char *)"/Models/Teapot.obj", 6);			// Texture. normals
//		model[0].LoadObjFile(szPath, (char *)"/Models/cube.obj", 100);			// Texture, normals
//		model[0].LoadObjFile(szPath, (char *)"/Models/axis.obj", 20);			// -

		model[0].position = pos;

		// Create an Icosphere
		model[0] = Icosphere(100, 3, pos);

		pos.x = 200.0;
		pos.y =   0.0;
		pos.z =   0.0;
		model[1] = Icosphere(50, 2, pos);
		model[1].SetColour(ColourRef{255, 0, 0});

		pos.x =   0.0;
		pos.y = 200.0;
		pos.z =   0.0;
		model[2] = Icosphere(75, 2, pos);
		model[2].SetColour(ColourRef{0, 255, 0});

		// Create a rectangle
		pos.x = 0.0;
		pos.y = 0.0;
		pos.z = 0.0;
		model[3] = Rectangle(600, 600, 10, 10, pos);
		model[3].SetColour(ColourRef{40, 40, 255});

		// Create a Cube
		pos.x = -200.0;
		pos.y =    0.0;
		pos.z =    0.0;
		model[4] = Cube(300, 20, 300, pos);
		model[4].SetColour(ColourRef{40, 40, 255});

		numModel = 5;
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

	// Radians = (degrees * pi) / 180;
	double radPitch = (pitch * PI) / 180.0;
	double radYaw = (yaw * PI) / 180.0;

	// OLD
	view.FPSViewRH(eye, radPitch, radYaw);

	// NEW
//	Vec3D vUp = { 0.0, 1.0, 0.0 };
//	view.LookAtRH(eye, target, vUp);

	for (int m = 0; m < numModel; m++)
	{
		// Move the points
		DEBUG("points ");
		for (i = 0; i < model[m].numP; i++)
		{
			// Move the model to the correct position
			tmpVec = model[m].data[i] + model[m].position;

			// ---- TEST ----
			if (0 == m)
			{
				tmpVec = tmpVec + target;
			}

			ViewMatrix(&model[m].tmp[i], &view, &tmpVec);
		}
		DEBUG("- [done]\r\n");

		// Move the normals
		if (showNormals)
		{
			DEBUG("show normals");
			for (i = 0; i < model[m].numNorm; i++)
			{
				tmpVec = model[m].data[i];
				tmpVec.x += model[m].normal[i].x * NORMAL_LEN;
				tmpVec.y += model[m].normal[i].y * NORMAL_LEN;
				tmpVec.z += model[m].normal[i].z * NORMAL_LEN;

				ViewMatrix(&model[m].showNorm[i], &view, &tmpVec);
			}
			DEBUG(" - [done]\r\n");
		}

		DEBUG("normals");
		for (i = 0; i < model[m].numNorm; i++)
		{
			ViewMatrix(&model[m].tmpNorm[i], &view, &model[m].normal[i]);
		}
		DEBUG(" - [done]\r\n");
	}

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
//		SortSurfaces();
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

		for (i = 0; i < model[0].numP; i++)
		{
			// Only show normal if it is visible
//			if (model[0].showNorm[i].z > 0)
			{
				//g_print("%0.2f %0.2f %0.2f \r\n", model[0].normal[i].x, model[0].normal[i].y, model[0].normal[i].z);
				x1 = (int)(model[0].tmp[i].x * zoom) + w;
				y1 = (int)(model[0].tmp[i].y * zoom) + h;

				x2 = (int)(model[0].showNorm[i].x * zoom) + w;
				y2 = (int)(model[0].showNorm[i].y * zoom) + h;

				cairo_move_to(cr, x1, y1);
				cairo_line_to(cr, x2, y2);
			}
//			else
//			{
//				g_print("%0.2f %0.2f %0.2f \r\n", model[0].normal[i].x, model[0].normal[i].y, model[0].normal[i].z);
//			}
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

	int s = 0;
	for (int m = 0; m < numModel; m++)
	{
		// Calculate average Z values
		for (int i = 0; i < model[m].numSurf; i++)
		{
			p1 = model[m].surfaces[i].p1 - 1;	// Model point numbers start at 1
			p2 = model[m].surfaces[i].p2 - 1;
			p3 = model[m].surfaces[i].p3 - 1;

			sortList[s].avz = (model[m].tmp[p1].z + model[m].tmp[p2].z + model[m].tmp[p3].z) / 3;
			sortList[s].surface = i;
			sortList[s].model = m;
			s++;
		}
	}

	int numsurf = s;
	DEBUG("SortSurfaces()\r\n");

	// Now  perform a bubble sort
	// loop to access each array element
	for (int step = 0; step < numsurf - 1; ++step)
	{
		// loop to compare array elements
		for (int i = 0; i < numsurf - step - 1; ++i)
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