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

	GtkWidget* window, *about_button, *box, *output_box,*right_box, *input_box, *about_box, *textArea, *label_input, *label_output,*logo, *input,
	                   *set_button, *label_about, *choose_box, *label_choose, *choose_motor;
	
	GdkPixbuf *pixbuf_logo = gdk_pixbuf_new_from_file("logo_AVR.png", NULL);

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_title(GTK_WINDOW(window), "AVR Multi Motor Control");

	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
	
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
	gtk_box_set_baseline_position (GTK_BOX(box), GTK_BASELINE_POSITION_CENTER);
	output_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
	gtk_box_set_baseline_position (GTK_BOX(right_box), GTK_BASELINE_POSITION_CENTER);
	input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	choose_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	about_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	
	gtk_container_add (GTK_CONTAINER (right_box), choose_box);
	gtk_container_add (GTK_CONTAINER (right_box), input_box);
	gtk_container_add (GTK_CONTAINER (right_box), about_box);
	gtk_container_add (GTK_CONTAINER (box), output_box);
	gtk_container_add (GTK_CONTAINER (box), right_box);
	gtk_container_add (GTK_CONTAINER (window), box);
	
	
	//////////////////////////////////   TEXT VIEW  & LOGO  //////////////////////////////////////////////
	logo= gtk_image_new_from_pixbuf(pixbuf_logo);
	gtk_container_add(GTK_CONTAINER(output_box), logo);
	label_output= gtk_label_new ("Motor velocity,  Motor position:");
	gtk_container_add(GTK_CONTAINER(output_box), label_output);
	textArea = gtk_text_view_new();
    gtk_widget_set_size_request(textArea, 600, 500);
	gtk_container_add(GTK_CONTAINER(output_box), textArea);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textArea), 0);
	
	/////////////////////////////////      CHOOSE A MOTOR       //////////////////////////////////////////
	
	label_choose= gtk_label_new ("Choose a motor:");
	gtk_container_add(GTK_CONTAINER(choose_box), label_choose);
	choose_motor= gtk_combo_box_text_new ();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(choose_motor), "1", "Motor 1"); 
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(choose_motor), "2", "Motor 2"); 
	gtk_container_add(GTK_CONTAINER(choose_box), choose_motor);

	/////////////////////////////////        TEXT INPUT          //////////////////////////////////////////
	
	label_input= gtk_label_new ("Set the velocity:");
	gtk_container_add(GTK_CONTAINER(input_box), label_input);
	input= gtk_entry_new(); 
	gtk_container_add (GTK_CONTAINER (input_box), input);
	set_button=gtk_button_new_with_label("Set");
	//g_signal_connect (set_button, "clicked", G_CALLBACK(<funzione da chiamare>), NULL);
	gtk_container_add (GTK_CONTAINER(input_box), set_button);
	
	/////////////////////////////////    ABOUT THE PROJECT        ////////////////////////////////////////
	label_about= gtk_label_new ("Here you can find information:");
	gtk_container_add(GTK_CONTAINER(about_box), label_about);
	about_button= gtk_button_new_with_label("about the project");
	g_signal_connect (about_button, "clicked", G_CALLBACK(show_about), (gpointer) window);
	gtk_container_add (GTK_CONTAINER (about_box), about_button);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

