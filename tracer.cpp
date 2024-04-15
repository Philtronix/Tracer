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
#include "Primitives/icosphere.h"
#include "Primitives/Rectangle.h"
#include "Primitives/cube.h"
#include "Primitives/Torus.h"
#include "Primitives/Cylinder.h"
#include "Primitives/Cone.h"

#include "Views/DrawPoints.h"
#include "Views/DrawWire.h"
#include "Views/DrawFlat.h"
#include "Views/DrawGouraud.h"
#include "Views/DrawPhil.h"

#define NORMAL_LEN 20

//#define DEBUG(x) g_print(x)
#define DEBUG(x)

static void   DrawView(cairo_t *cr, int style);
static Vec3D  CrossProduct(Vec3D a, Vec3D b);
static void   AddRow(const char *text);
static void   OpenFile(char *filename);

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
int   seletedModelRow = -1;


// 3D Variables
double rotateY = 0.0;
double rotateX = 0.0;
double rotateZ = 0.0;
double zoom = 1.0;
double pitch = 0.0;
double yaw = 0.0;
Vec3D  eye(100.0, 100.0, 100.0);
Vec3D  target(0.0, 0.0, 0.0);
item   sortList[9000];
Vec3D  lightPos(10.0, 10.0, 10.0);
Vec3D  lightPosition(10.0, 10.0, 10.0);

///////////////////////////////////////////////
GtkTreeStore *treeStore;
GtkTreeView  *tv1;
GtkTreeViewColumn *cx1;
GtkTreeViewColumn *cx2;
GtkTreeSelection  *selection;
GtkCellRenderer   *cr1;
GtkCellRenderer   *cr2;

GtkTreeIter iter;		// iterators
//GtkTreeIter iterChild1;	// iterators
//GtkTreeIter iterChild2;	// iterators

GtkWidget *entry_name;
GtkWidget *entry_xPos;
GtkWidget *entry_yPos;
GtkWidget *entry_zPos;

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

	///////////////////////////////////////////////////////////////////////////
	// Edit boxes
	entry_name = GTK_WIDGET(gtk_builder_get_object(builder, "Name"));
	entry_xPos = GTK_WIDGET(gtk_builder_get_object(builder, "XPos"));
	entry_yPos = GTK_WIDGET(gtk_builder_get_object(builder, "YPos"));
	entry_zPos = GTK_WIDGET(gtk_builder_get_object(builder, "ZPos"));

	///////////////////////////////////////////////////////////////////////////
	// Treeview
	treeStore = GTK_TREE_STORE(gtk_builder_get_object(builder, "treeStore"));
	tv1	      = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tv1"));
	cx1       = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cx1"));		// Column 1
	cx2       = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cx2"));		// Column 2
	cr1       = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "cr1"));			// Column 1 renderer
	cr2       = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "cr2"));			// Column 2 renderer
	selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "selection"));	// tree view selection

	gtk_tree_view_column_add_attribute(cx1, cr1, "text", 0);	// Attach renderer to column
	gtk_tree_view_column_add_attribute(cx2, cr2, "active", 1);	// Attach renderer to column & make box alterable

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tv1));

	///////////////////////////////////////////////////////////////////////////
	// Read initial values from glade
	eye.x = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "eyeX")));
	eye.y = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "eyeY")));
	eye.z = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "eyeZ")));
	g_print("eye = %.2f %.2f %.2f\r\n", eye.x, eye.y, eye.z);

	target.x = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "targetX")));
	target.y = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "targetY")));
	target.z = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "targetZ")));
	g_print("target = %.2f %.2f %.2f\r\n", target.x, target.y, target.z);

	lightPosition.x = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "lightX")));
	lightPosition.y = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "lightY")));
	lightPosition.z = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "lightZ")));
	g_print("lightPosition = %.2f %.2f %.2f\r\n", lightPosition.x, lightPosition.y, lightPosition.z);

	pitch = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "pitch")));
	g_print("pitch = %.2f\r\n", pitch);

	yaw = gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "yaw")));
	g_print("yaw = %.2f\r\n", yaw);

	///////////////////////////////////////////////////////////////////////////
	// Activate main window	
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

	///////////////////////////////////////////////////////////////////////////
	// Load model
	if (getcwd(szPath, sizeof(szPath)) != NULL)
	{
		numModel = 0;

		// Create an Icosphere
		model[numModel] = Icosphere(10, 2, Vec3D(200.0, 0.0, 50.0));
		model[numModel].SetColour(ColourRef{255, 0, 0});
		strcpy(model[0].name, "Light");
		AddRow(model[0].name);
		model[numModel].position.x = lightPosition.x;
		model[numModel].position.y = lightPosition.y;
		model[numModel].position.z = lightPosition.z;
		numModel++;

//		model[1].LoadObjFile(szPath, (char *)"/Models/Monkey.obj", 160);		// Texture, normals
//		model[1].LoadObjFile("/home/phil/Work/GTK/Tracer/Models/Teapot_2.obj");	// Normals
//		model[1].LoadObjFile(szPath, (char *)"/Models/icosahedron.obj", 150);	// Normals
//		model[1].LoadObjFile(szPath, (char *)"/Models/BigTeapot.obj", 100);		// -
//		model[1].LoadObjFile(szPath, (char *)"/Models/Teapot.obj", 6);			// Texture. normals
//		model[1].LoadObjFile(szPath, (char *)"/Models/cube.obj", 100);			// Texture, normals
//		model[1].LoadObjFile(szPath, (char *)"/Models/axis.obj", 20);			// 0


/*
		strcpy(model[numModel].name, "Teapot");
		AddRow(model[numModel].name);
		model[numModel].position = Vec3D(0.0, 0.0, 0.0);
*/

		// Create an Icosphere
		model[numModel] = Icosphere(100, 3, Vec3D(0.0, 0.0, 0.0));
		model[numModel].SetColour(ColourRef{40, 40, 255});
		strcpy(model[numModel].name, "Blue Sphere");
		AddRow(model[numModel].name);
		numModel++;

		// Create an Icosphere
		model[numModel] = Icosphere(50, 2, Vec3D(200.0, 0.0, 50.0));
		model[numModel].SetColour(ColourRef{255, 0, 0});
		strcpy(model[numModel].name, "Red Sphere");
		AddRow(model[numModel].name);
		numModel++;

		// Create an Icosphere
		model[numModel] = Icosphere(75, 2, Vec3D(0.0, 200.0, 0.0));
		model[numModel].SetColour(ColourRef{0, 255, 0});
		strcpy(model[numModel].name, "Green Sphere");
		AddRow(model[numModel].name);
		numModel++;

		// Create a rectangle
		model[numModel] = Rectangle(600, 600, 10, 10, Vec3D(0.0, 0.0, 0.0));
		strcpy(model[numModel].name, "Rectangle");
		AddRow(model[numModel].name);
		numModel++;

		// Create a Cube
		model[numModel] = Cube(300, 20, 300, Vec3D(-200.0, 0.0, 0.0));
		model[numModel].SetColour(ColourRef{40, 40, 255});
		strcpy(model[numModel].name, "Cube");
		AddRow(model[numModel].name);
		numModel++;

		// Create a Torus
		model[numModel] = Torus(10, 20, 50.0, 20.0, Vec3D(0.0, -200.0, 50.0));
		model[numModel].SetColour(ColourRef{240, 40, 240});
		strcpy(model[numModel].name, "Torus");
		AddRow(model[numModel].name);
		numModel++;

		// Create a Cylinder
		model[numModel] = Cylinder(100, 50, 20, Vec3D(200.0, -200.0, 0.0));
		model[numModel].SetColour(ColourRef{40, 240, 240});
		strcpy(model[numModel].name, "Cylinder");
		AddRow(model[numModel].name);
		numModel++;

		// Create a Cone
		model[numModel] = Cone(100, 50, 10, Vec3D(200.0, 200.0, 0.0));
		model[numModel].SetColour(ColourRef{40, 40, 255});
		strcpy(model[numModel].name, "Cone");
		AddRow(model[numModel].name);
		numModel++;

	}
	else
	{
		g_print("Failed to get path.\r\n");
	}

	///////////////////////////////////////////////////////////////////////////
	// Start GTK
	gtk_main();

	return EXIT_SUCCESS;
}

static void AddRow(const char *text)
{
	gtk_tree_store_append (treeStore, &iter, NULL);
	gtk_tree_store_set(treeStore, &iter, 0, text, -1);
	gtk_tree_store_set(treeStore, &iter, 1, TRUE, -1);
}

///////////////////////////////////////////////////////////////////////////////
// GTK Callback functions

extern "C" G_MODULE_EXPORT void on_rotateY_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	model[seletedModelRow].rotateY = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_rotateX_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	model[seletedModelRow].rotateX = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_rotateZ_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	model[seletedModelRow].rotateZ = gtk_adjustment_get_value(adjustment);
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_zoom_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	model[seletedModelRow].zoom = gtk_adjustment_get_value(adjustment);
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
	lightPosition.x = gtk_adjustment_get_value(adjustment);
	model[0].position.x = lightPosition.x;
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_lightY_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	lightPosition.y = gtk_adjustment_get_value(adjustment);
	model[0].position.y = lightPosition.y;
	gtk_widget_queue_draw(screen);
}

extern "C" G_MODULE_EXPORT void on_lightZ_value_changed(GtkAdjustment* adjustment, gpointer data) 
{
	lightPosition.z = gtk_adjustment_get_value(adjustment);
	model[0].position.z = lightPosition.z;
	gtk_widget_queue_draw(screen);
}

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

extern "C" G_MODULE_EXPORT void on_select_changed(GtkTreeSelection *c) 
{ 
	gchar        *value;
	gboolean     box;
	GtkTreeModel *treeModel;
	int len;
	int pos = 0;
	char szBuffer[100] = {0};
	GtkTreePath *path;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(c), &treeModel, &iter) == FALSE)
	{
		return;
	}
 
	gtk_tree_model_get(treeModel, &iter, 0, &value,  -1); // get column 0
	gtk_tree_model_get(treeModel, &iter, 1, &box,  -1);   // get column 1

	// Clear the old text
	gtk_editable_delete_text(GTK_EDITABLE(entry_name), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(entry_xPos), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(entry_yPos), 0, -1);
	gtk_editable_delete_text(GTK_EDITABLE(entry_zPos), 0, -1);

	// Which row is selected ?
	path = gtk_tree_model_get_path(treeModel, &iter);
	seletedModelRow = gtk_tree_path_get_indices(path)[0];

	// Set the text boxes to the correct values
	// Copy in the model Nme from selected row
	pos = 0;
	len = strlen(value);
	gtk_editable_insert_text(GTK_EDITABLE(entry_name), value, len, &pos);

	pos = 0;
	sprintf(szBuffer, "%f", model[seletedModelRow].position.x);
	len = strlen(szBuffer);
	gtk_editable_insert_text(GTK_EDITABLE(entry_xPos), szBuffer, len, &pos);

	pos = 0;
	sprintf(szBuffer, "%f", model[seletedModelRow].position.y);
	len = strlen(szBuffer);
	gtk_editable_insert_text(GTK_EDITABLE(entry_yPos), szBuffer, len, &pos);

	pos = 0;
	sprintf(szBuffer, "%f", model[seletedModelRow].position.z);
	len = strlen(szBuffer);
	gtk_editable_insert_text(GTK_EDITABLE(entry_zPos), szBuffer, len, &pos);

	gtk_adjustment_set_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "rotateX")), model[seletedModelRow].rotateX);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "rotateY")), model[seletedModelRow].rotateY);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "rotateZ")), model[seletedModelRow].rotateZ);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, "zoom")), model[seletedModelRow].zoom);
}

extern "C" G_MODULE_EXPORT void	on_cr2_toggled(GtkCellRendererToggle *cell, gchar *path_string)
{ 
	GtkTreeIter		iter;
	GtkTreeModel	*treeModel;
	gboolean		t = FALSE;
	gchar			*text;
	int				m;

	treeModel = gtk_tree_view_get_model(tv1);							// get the tree model
	gtk_tree_model_get_iter_from_string(treeModel, &iter, path_string);	// get iter from path
	gtk_tree_model_get(treeModel, &iter, 0, &text, -1);					// get the text pointer of col 0
	gtk_tree_model_get(treeModel, &iter, 1, &t, -1);					// get the boolean value of col 1

	m = atoi(path_string);
	if (t == FALSE)
	{
		t = TRUE;
		model[m].show = true;
	}
	else 
	{
		t = FALSE; // toggle
		model[m].show = false;
	}

	gtk_widget_queue_draw(screen);
	gtk_tree_store_set(treeStore, &iter, 1, t, -1); // alter col 1 check box
	return;
}

extern "C" G_MODULE_EXPORT void on_Name_changed(GtkEntry *e)
{
	const char *text = gtk_entry_get_text(e);

	if (seletedModelRow >= 0)
	{
		if (strlen(text) > 0)
		{
			if (0 != strcmp(text, model[seletedModelRow].name))
			{
				// Update model
				strcpy(model[seletedModelRow].name, text);

				// Update tree view
				gtk_tree_store_set(treeStore, &iter, 0, text, -1);
			}
		}
	}
}

extern "C" G_MODULE_EXPORT void on_XPos_changed(GtkEntry *e)
{
	const char *text = gtk_entry_get_text(e);

	if (strlen(text) > 0)
	{
		double val = strtod(text, NULL);
		if (val != model[seletedModelRow].position.x)
		{
			model[seletedModelRow].position.x = val;
			gtk_widget_queue_draw(screen);
		}
	}
}

extern "C" G_MODULE_EXPORT void on_YPos_changed(GtkEntry *e)
{
	const char *text = gtk_entry_get_text(e);

	if (strlen(text) > 0)
	{
		double val = strtod(text, NULL);
		if (val != model[seletedModelRow].position.y)
		{
			model[seletedModelRow].position.y = val;
			gtk_widget_queue_draw(screen);
		}
	}
}

extern "C" G_MODULE_EXPORT void on_ZPos_changed(GtkEntry *e)
{
	const char *text = gtk_entry_get_text(e);

	if (strlen(text) > 0)
	{
		double val = strtod(text, NULL);
		if (val != model[seletedModelRow].position.z)
		{
			model[seletedModelRow].position.z = val;
			gtk_widget_queue_draw(screen);
		}
	}
}

extern "C" G_MODULE_EXPORT void on_FileNew_activate(GtkMenuItem *m)
{
	g_print("File New\r\n");
}

extern "C" G_MODULE_EXPORT void on_FileOpen_activate(GtkMenuItem *m)
{
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open File",
										(GtkWindow *)window,
										action,
										("_Cancel"),
										GTK_RESPONSE_CANCEL,
										("_Open"),
										GTK_RESPONSE_ACCEPT,
										NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		g_print("Open File : <%s>\r\n", filename);
		OpenFile(filename);

		g_free (filename);
	}

	gtk_widget_destroy (dialog);	
}

extern "C" G_MODULE_EXPORT void on_FileSave_activate(GtkMenuItem *m)
{
	g_print("File Save\r\n");
}

extern "C" G_MODULE_EXPORT void on_FileSaveAs_activate(GtkMenuItem *m)
{
	g_print("File Save As\r\n");
}

extern "C" G_MODULE_EXPORT void on_FileQuit_activate(GtkMenuItem *m)
{
	g_print("File Quit\r\n");
	gtk_main_quit();
}

extern "C" G_MODULE_EXPORT void on_HelpAbout_activate(GtkMenuItem *m)
{
	g_print("Help About\r\n");
}

static void OpenFile(char *filename)
{
	// Load the file
	if (true == model[numModel].LoadObjFile(filename))
	{
		// Set the objects name
		char *last = strrchr(filename, '/');
		if (last != NULL)
		{
			last++;
			strcpy(model[numModel].name, last);

			model[numModel].SetColour(ColourRef{240, 40, 240});

			// Add to tree view
			AddRow(model[numModel].name);
			numModel++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Drawing functions

// Drawing callback
extern "C" G_MODULE_EXPORT gboolean on_screen_draw(GtkWidget *widget, cairo_t *cr, gpointer data) 
{
	DEBUG("	///////////////////////////////////////////////////////////////////////////\r\non_screen_draw()\r\n");
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

//     x y z w
// r1  a b c d     x   ax + by + cz + d
// r2  e f g h  *  y = ex + fy + gz + h
// r3  i j k l     z   ix + jy + kz + l
// r4  0 0 0 1     1         1
/*
void ViewMatrix(Vec3D *result, Matrix4 *matrix, Vec3D *data)
{
//	result->x = (matrix->r1.x * data->x) + (matrix->r1.y * data->y) + (matrix->r1.z * data->z) + matrix->r1.x;
//	result->y = (matrix->r2.x * data->x) + (matrix->r2.y * data->y) + (matrix->r2.z * data->z) + matrix->r2.y;
//	result->z = (matrix->r3.x * data->x) + (matrix->r3.y * data->y) + (matrix->r3.z * data->z) + matrix->r3.z;
	result->x = (matrix->r1.x * data->x) + (matrix->r1.y * data->y) + (matrix->r1.z * data->z) + matrix->r1.w;
	result->y = (matrix->r2.x * data->x) + (matrix->r2.y * data->y) + (matrix->r2.z * data->z) + matrix->r2.w;
	result->z = (matrix->r3.x * data->x) + (matrix->r3.y * data->y) + (matrix->r3.z * data->z) + matrix->r3.w;
	result->w = 1;
}
*/

void multPointMatrix(const Vec3D &in, Vec3D &out, const Matrix4 &M);

void glOrtho(
    const double &b, const double &t, const double &l, const double &r,
    const double &n, const double &f,
    Matrix4 &M);

#include <limits>

void DrawView(cairo_t *cr, int style)
{
	int		i;
	int		h = (ScreenHeight / 2);
	int		w = (ScreenWidth / 2);
	Matrix4 world;
	Matrix4 scale;
	Matrix4 trans;
	Matrix4 test;
	Matrix4 camera;
	Matrix4 persp;
	Matrix4 ortho;
	Matrix4 proj;
	Matrix4 normals;
	Matrix4 fps;
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
	Vec3D	vertTmp;
	Vec3D	vertTmp2;
	double  radPitch;
	double  radYaw;

	DEBUG("DrawView()\r\n");

	for (int m = 0; m < numModel; m++)
	{
		// Set up the view matrix
		world.World(model[m].rotateX, model[m].rotateY, model[m].rotateZ);
		scale.Scale(model[m].zoom, model[m].zoom, model[m].zoom);
		trans.Translate(model[m].position.x, model[m].position.y, model[m].position.z);

		if (true)
		{
			// --- lookAt ---
			Vec3D bob = Vec3D(0.0, 1.0, 0.0);
			camera.lookAt(eye, target, bob);

			test = world * scale * trans * camera;
			normals = world * scale * trans * camera;

			// ---- Move the light ----
			multPointMatrix(lightPosition, lightPos, camera);
		}
		else
		{
			// Pitch must be in the range of [-90 ... 90] degrees and 
			// yaw must be in the range of [0 ... 360] degrees.
			// Pitch and yaw variables must be expressed in radians.
			radPitch = (pitch * PI) / 180.0;
			radYaw = (yaw * PI) / 180.0;
			fps.FPSViewRH(target, radPitch, radYaw);

			test = world * scale * trans * fps;
			normals = world * scale * trans * fps;

			// ---- Move the light ----
			multPointMatrix(lightPosition, lightPos, fps);
//			lightPos = lightPosition;
		}

		// ---- Move the points ----
		DEBUG("points ");
		for (i = 0; i < model[m].numP; i++)
		{
			// Transform to camera space
			//                        in          out         matrix
			multPointMatrix(model[m].data[i], model[m].tmp[i], test);
		}
		DEBUG("- [done]\r\n");

		// ---- Move the normals ----
		if (showNormals)
		{
			DEBUG("show normals");
			for (i = 0; i < model[m].numNorm; i++)
			{
				tmpVec = model[m].data[i];
				tmpVec.x += model[m].normal[i].x * NORMAL_LEN;
				tmpVec.y += model[m].normal[i].y * NORMAL_LEN;
				tmpVec.z += model[m].normal[i].z * NORMAL_LEN;

				multPointMatrix(tmpVec, vertTmp, normals);
				model[m].showNorm[i] = vertTmp;
			}
			DEBUG(" - [done]\r\n");
		}

		DEBUG("normals");
		for (i = 0; i < model[m].numNorm; i++)
		{
			multPointMatrix(model[m].normal[i], vertTmp, normals);
			model[m].tmpNorm[i] = vertTmp;
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

		for (int m = 0; m < numModel; m++)
		{
			for (i = 0; i < model[m].numP; i++)
			{
				if (true == model[m].show)
				{
					// Only show normal if it is visible
//					if (model[0].showNorm[i].z > 0)
//					if (model[0].showNorm[i].z < model[0].tmp[i].z)
					{
						//g_print("%0.2f %0.2f %0.2f \r\n", model[0].normal[i].x, model[0].normal[i].y, model[0].normal[i].z);
						x1 = (int)model[m].tmp[i].x + w;
						y1 = (int)model[m].tmp[i].y + h;

						x2 = (int)model[m].showNorm[i].x + w;
						y2 = (int)model[m].showNorm[i].y + h;

						cairo_move_to(cr, x1, y1);
						cairo_line_to(cr, x2, y2);
					}
//					else
//					{
//						g_print("%0.2f %0.2f %0.2f \r\n", model[0].normal[i].x, model[0].normal[i].y, model[0].normal[i].z);
//					}
				}
			}
		}
		DEBUG(" - [done]\r\n");
	}

	DEBUG("DrawView() [done]\r\n");
}

///////////////////////////////////////////////////////////////////////////////

static Vec3D CrossProduct(Vec3D a, Vec3D b)
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
		if (true == model[m].show)
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

///////////////////////////////////////////////////////////////////////////////////////

// Point-Matrix multiplication. The input point is assumed to have Cartesian
// coordinates but because we will multiply this point by a 4x4 matrix we actually
// assume it is a point with homogeneous coordinatess (x, y, z, w = 1).
// The point-matrix results in another point with homogeneous coordinates (x', y', z', w').
// To get back to Cartesian coordinates we need to noramlized these coordinates: (x'/w', y'/w', z'/w').
void multPointMatrix(const Vec3D &in, Vec3D &out, const Matrix4 &M)
{
	//out = in * Mproj;
	out.x   = in.x * M[0][0] + in.y * M[1][0] + in.z * M[2][0] + /* in.z = 1 */ M[3][0];
	out.y   = in.x * M[0][1] + in.y * M[1][1] + in.z * M[2][1] + /* in.z = 1 */ M[3][1];
	out.z   = in.x * M[0][2] + in.y * M[1][2] + in.z * M[2][2] + /* in.z = 1 */ M[3][2];
	float w = in.x * M[0][3] + in.y * M[1][3] + in.z * M[2][3] + /* in.z = 1 */ M[3][3];

	// normalize if w is different than 1 (convert from homogeneous to Cartesian coordinates)
	if (w != 1)
	{
		out.x /= w;
		out.y /= w;
		out.z /= w; 
	}
}

// Set the OpenGL orthographic projection matrix
void glOrtho(const double &b, const double &t, const double &l, const double &r,
			 const double &n, const double &f,
			 Matrix4 &M)
{
	// set OpenGL perspective projection matrix
	M[0][0] = 2 / (r - l);
	M[0][1] = 0;
	M[0][2] = 0;
	M[0][3] = 0;

	M[1][0] = 0;
	M[1][1] = 2 / (t - b);
	M[1][2] = 0;
	M[1][3] = 0;

	M[2][0] = 0;
	M[2][1] = 0;
	M[2][2] = -2 / (f - n);
	M[2][3] = 0;

	M[3][0] = -(r + l) / (r - l);
	M[3][1] = -(t + b) / (t - b);
	M[3][2] = -(f + n) / (f - n);
	M[3][3] = 1;
}

// EOF