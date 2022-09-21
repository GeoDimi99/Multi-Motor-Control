#include <gtk/gtk.h>

void show_about(GtkWidget *widget, gpointer data) {

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("logo_sapienza_neg.png", NULL);

	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog), "Arduino Multi Motor Control");
	const char* authors[]={ "Georgi Dimitrov", "Sara Attiani", NULL}; 
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors); 
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version 0.0"); 
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	 "Arduino Multi Motor Control è un progetto per il controllo simultaneo di motori dc. Il controllo viene effettuato attraverso un processo master che si interfaccia con vari processi slave controllori, comunicandovi attraverso protocollo TWI.");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {

	GtkWidget* window, *about_button, *about_button_box, *textArea, *scrollbar, *textEntry, *console;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_title(GTK_WINDOW(window), "Arduino Multi Motor Control");

	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
	
	/////////////////////////////////    ABOUT THE PROJECT        ////////////////////////////////////////
	
	about_button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add (GTK_CONTAINER (window), about_button_box);
	gtk_widget_set_halign (about_button_box,GTK_ALIGN_END);
	about_button= gtk_button_new_with_label("About the project");
	g_signal_connect (about_button, "clicked", G_CALLBACK(show_about), (gpointer) window);
	gtk_container_add (GTK_CONTAINER (about_button_box), about_button);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////        TEXT VIEW       //////////////////////////////////////////////
	textArea = gtk_text_view_new();
    //scrollbar= gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,gtk_scrollable_get_vadjustment(textArea));
    //textEntry = gtk_entry_new();

    //console = gtk_grid_new(); 

    //gtk_grid_attach(GTK_GRID(console), textArea, 0, 1, 0, 1);
    //gtk_grid_attach(GTK_GRID(console), scrollbar, 1, 2, 0, 1);
    //gtk_grid_attach(GTK_GRID(console), textEntry, 0, 2, 1, 2);
    //This code sets the preferred size for the widget, so it does not ask for extra space
    gtk_widget_set_size_request(textArea, 320, 240);
	gtk_container_add(GTK_CONTAINER(window), textArea);

	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

