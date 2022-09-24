#include <gtk/gtk.h>

void show_about(GtkWidget *widget, gpointer data) {
	//creazione di un buffer di pixel, contenente l'immagine 
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("logo_sapienza_neg.png", NULL);
	//creazione di una "about" dialog window 
	GtkWidget *dialog = gtk_about_dialog_new();
	//si setta il nome della "about" dialog
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog), "AVR Multi Motor Control");
	//si dichiara un buffer di stringhe contenente gli autori (deve terminare con null, altrimenti va in segmentation fault)
	const char* authors[]={ "Georgi Dimitrov", "Sara Attiani", NULL}; 
	//si settano gli autori nella "about" dialog (creerà il campo "credits")
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
	//si dichiara la versione del codice
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version 0.0"); 
	//si inserisce un commento esplicativo nella abut dialog (*****cambiare il commento a piacimento*****)
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	 "Arduino Multi Motor Control è un progetto per il controllo simultaneo di motori dc. Il controllo viene effettuato attraverso un processo master che si interfaccia con vari processi slave controllori, comunicandovi attraverso protocollo TWI.");
	//si setta il logo, prendendolo dal pixbuf creato in precedenza
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	//si mostra la finestra
	gtk_dialog_run(GTK_DIALOG (dialog));
	//si distrugge la finestra quando si preme il pulsante
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
	//dichiarazione di una serie di elementi che andranno a costituire l'interfaccia 
	GtkWidget* window, *about_button, *box, *output_box,*right_box, *input_box, *about_box, *textArea, *label_input, *label_output,*logo, *input,
	                   *set_button, *label_about, *choose_box, *label_choose, *choose_motor, *label_constraint, *label_warning, *label_info;
	
	//creazione di un buffer contenente i pixel del logo
	GdkPixbuf *pixbuf_logo = gdk_pixbuf_new_from_file("logo_AVR.png", NULL);
	
	//si inizializza gtk (è possibile passare al programma dei paramentri)
	gtk_init(&argc, &argv);
	
	//si dichiara che il programma preferisce il dark mode (visti i loghi in negativo)
	g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

	//creazione la finestra principale (si setta la posizione centrale rispetto allo schermo, la dimensione. il titolo)
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_title(GTK_WINDOW(window), "AVR Multi Motor Control");

	//si settano i bordi esterni della finestra 
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	
    //////////////////////////////// MENU BAR //////////////////////////////////////
	
	// Creazione della barra del MENU PRINCIPALE
	GtkWidget* mbar = gtk_menu_bar_new();
	//Creazione di una box verticale per mettere il menu bar in alto e il resto della
	//finestra sotto di essa
	GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	//Aggiungo il mbar al box vbox
	gtk_box_pack_start (GTK_BOX(main_box), mbar, TRUE, TRUE, 0);
	//Inserisco i menu item nella barra
	GtkWidget * help_mi = gtk_menu_item_new_with_label ("Help");
	gtk_menu_shell_append (GTK_MENU_SHELL (mbar), help_mi);
	
	//Crea un SOTTO MENU
	GtkWidget *help_menu = gtk_menu_new ();
	//Inserisco il sottomenu in un elemento del menu principale
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_mi), help_menu);
	//Creo due opzioni:
	// - help: per le istruzioni al programma
	// - about: con le informazioni sui creatori
	GtkWidget * help_op = gtk_menu_item_new_with_label ("Help");
	GtkWidget * about_op = gtk_menu_item_new_with_label ("About");
	gtk_menu_shell_append (GTK_MENU_SHELL (help_menu), help_op);
	gtk_menu_shell_append (GTK_MENU_SHELL (help_menu), about_op);
	
	
	
	 
	
	
	/////////////////////////////////////////////////////////////////////////////////
	
	// si creano e si aggiungono una serie di box che andranno a contenere gli elementi della finestra
	//gtk_box_new(<disposizione degli elementi>, <spazio tra gli elementi>
	
	/*ALBERO DEI CONTENITORI E DEGLI ELEMENTI: 
	 * window:
	 * 	 box:
	 * 		-->output_box
	 * 			 - logo
	 * 			 - label_output
	 * 			 - textArea
	 *  -	-->right_box
	 * 			-->choose_box
	 * 				 - label_choose 
	 * 				 - combo box (choose_motor)
	 *          -->input_box
	 * 				 - label_input
	 * 				 - input (di tipo entry)
	 * 				 - set_button
	 * 				 - label_constraint
	 * 				 - label_warning
	 * 			-->about_box
	 * 				 - label_about
	 * 				 - about_button
	*/
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
	gtk_box_set_baseline_position (GTK_BOX(box), GTK_BASELINE_POSITION_CENTER);
	output_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
	gtk_widget_set_margin_top (right_box, 200); 
	gtk_box_set_baseline_position (GTK_BOX(right_box), GTK_BASELINE_POSITION_CENTER);
	input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	choose_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	about_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top (about_box, 100); 
	
	//si aggiungono i container ad altri container seguendo la logica ad albero
	gtk_container_add (GTK_CONTAINER (right_box), choose_box);
	gtk_container_add (GTK_CONTAINER (right_box), input_box);
	gtk_container_add (GTK_CONTAINER (box), output_box);
	gtk_container_add (GTK_CONTAINER (box), right_box);
	gtk_container_add (GTK_CONTAINER (main_box), box );
	gtk_container_add (GTK_CONTAINER (window), main_box);
	
	// segue la construzione dei vari elementi
	
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
	choose_motor= gtk_combo_box_text_new();
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
	label_constraint= gtk_label_new ("Speed range: [-300, +300]");
	gtk_container_add(GTK_CONTAINER(right_box), label_constraint);
	label_warning= gtk_label_new ("Warning: if you go outside this range, the speed will not be applied!");
	gtk_container_add(GTK_CONTAINER(right_box), label_warning);
	
	/////////////////////////////////    ABOUT THE PROJECT        ////////////////////////////////////////
	//label_about= gtk_label_new ("Here you can find information:");
	//gtk_container_add(GTK_CONTAINER(about_box), label_about);
	//about_button= gtk_button_new_with_label("about the project");
	g_signal_connect (about_op, "activate", G_CALLBACK(show_about), (gpointer) window);
	//gtk_container_add (GTK_CONTAINER (about_box), about_button);
	//gtk_container_add (GTK_CONTAINER (right_box), about_box);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//si imposta la dimensione statica statica 
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	//si imposta la chiusura del programma 
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	//si mostra tutta la finestra appena creata 
	gtk_widget_show_all(window);

	//si chiama un ciclo principale che serve a controllare effettivamente il programma
	//per gestire gli eventi 
	gtk_main();

	return 0;
}

