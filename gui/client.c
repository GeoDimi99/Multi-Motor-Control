#include <string.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <semaphore.h>
#include "serial_linux.h"

#define bsize 20

pthread_t thread;
sem_t sem; 

GtkWidget* window;

GtkWidget* open_button; 

//variabili globali necessarie per realizzare l'output
GtkWidget* textArea;
GtkTextBuffer* text_buf; 
GtkTextIter iter; 
char* buf; 
int fd; 

//variabili globali necessarie per realizzare l'input
GtkEntryBuffer* buf_input1;
GtkEntryBuffer* buf_input2;



gboolean append_text(gpointer user_data){                    //inserisce del testo in coda alla text area
	gtk_text_buffer_get_end_iter(text_buf, &iter); 
	gtk_text_buffer_insert(text_buf, &iter, buf, -1);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textArea),&iter,0,FALSE,1.0,0);
	sem_post(&sem);
	return G_SOURCE_REMOVE; 
}

void* output(void * arg){                                  //funzione chiamata in un thread separato
	while (1) {
		int n_read=read(fd, buf, bsize-1); 
		*(buf + n_read)= '\0';  
		g_idle_add(append_text, NULL);                    //chiama la funzione append text nel thread main
		sem_wait(&sem); 
	}
}

void input(GtkWidget *widget, gpointer data){                     //gestore del bottone Set, invia l'input
	int len_1= gtk_entry_buffer_get_length(buf_input1); 
	int len_2= gtk_entry_buffer_get_length(buf_input2); 
	const char* vel_1= gtk_entry_buffer_get_text(buf_input1);
	const char* vel_2= gtk_entry_buffer_get_text(buf_input2);
	int v1 = atoi(vel_1);
	int v2 = atoi(vel_2);
	if(v1 < -380 || v1 > 380 || v2 < -380 || v2 > 380 ){
		GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE, "Error: input must be between [-380,380]!\n");
				gtk_dialog_run(GTK_DIALOG(dialog));
				gtk_widget_destroy(dialog);
				return;
	}
	write(fd, vel_1, len_1+1);                                     //non sono sicura di questo
	write(fd, vel_2, len_2+1);                                     //non sono sicura di questo 
}

void open_serial(GtkWidget *widget, gpointer data){        //gestore del bottone per aprire e chiudere la seriale 
	GtkWidget *dialog;
	if (!strcmp(gtk_button_get_label(GTK_BUTTON(open_button)), "Open")){
		const char**  args= (const char**)data; 
		const char* filename= args[0];
		int baudrate=atoi(args[1]);
		fd= serial_open(filename);
		if (fd<=0) {
			dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE, "Error opening serial device [%s] ... \n", filename);
			gtk_window_set_title(GTK_WINDOW(dialog), "Error input device");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return;
		} 
		int attribs= serial_set_interface_attribs(fd, baudrate, 0);
		if (attribs) {
			dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE, "Error setting baudrate [%d] ... \n", baudrate);
			gtk_window_set_title(GTK_WINDOW(dialog), "Error input baudrate");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return;
		}
		serial_set_blocking(fd, 1);
		
		///// OUTPUT ///// --->occorre lanciarlo in un thread separato
		if (pthread_create(&thread, NULL, output,NULL)){
			dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE, "An error occurred opening thread...\n");
			gtk_window_set_title(GTK_WINDOW(dialog), "Error output");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_button_set_label (GTK_BUTTON(open_button), "Close"); 
	}
	else{
		if (pthread_cancel(thread)){
			dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE, "An error occurred deleting thread ...\n");
			gtk_window_set_title(GTK_WINDOW(dialog), "Error output");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_button_set_label (GTK_BUTTON(open_button), "Open"); 
		close(fd); 
	}
}


void show_about(GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("logo_sapienza_neg.png", NULL);
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog), "AVR Multi Motor Control");
	const char* authors[]={ "Georgi Dimitrov", "Sara Attiani", NULL}; 
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Version 0.0"); 
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Arduino Multi Motor Control is a project made for controlling multiple dc motors. That is realized by having a master process which comuncate with some slave processes, using TWI protocol (implementation of I2C).");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

void show_help(GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("logo_AVR.png", NULL);
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(dialog), "AVR Multi Motor Control");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
	"USE INSTRUCTIONS: \n \n  -Open a connection with your AVR device by setting it's path and it's baudrate \n \n- Set the desired speed for your motors and click on the 'Set' button \n \n Warning!: the speed range must fall into [-380, +380]. Attempts to set the speed out of range will be ignored.");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
	
	sem_init(&sem, 0, 0); 
	buf= (char*) malloc(sizeof(char)*bsize); 
	
	//window variabile globale
	
	// IN Window :
	 GtkWidget *main_box;
	
	// IN Main box :
	GtkWidget* mbar;
	GtkWidget* box;
	
	// IN Box:
	GtkWidget* left_box;
	GtkWidget* right_box;
	
	// IN Left box:
	GtkWidget* logo;
	GtkWidget* label_output;
	GtkWidget* scroll_box; 
	// Globale GtkWidget* textArea;
	text_buf = gtk_text_buffer_new(NULL);
	GtkWidget *serial_box;
	
	// IN Right box:
	GtkWidget* input_box;
	GtkWidget* choose_box;
	
	// IN Serial box:
	GtkWidget* dev_lab;
	GtkEntryBuffer* dev_buf = gtk_entry_buffer_new("/dev/ttyACM0", strlen("/dev/ttyACM0"));
	GtkWidget* dev_input;
	GtkWidget* baud_lab;
	GtkEntryBuffer* baud_buf = gtk_entry_buffer_new("19200", strlen("19200"));
	GtkWidget* baud_input;
	//open_button variabile globale
	
	// IN Input box:
	GtkWidget* input1_box; 
	GtkWidget* input2_box; 
	GtkWidget* label_input_1;
	GtkWidget* label_input_2;
	GtkWidget* input_1;
	GtkWidget* input_2;
	//buf_input1 variabile globale 
	//buf_input2 variabile globale
	GtkWidget* set_box;
	GtkWidget* set_button;
	GtkWidget* label_constraint;
	
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
	
	//Creazione della barra del MENU PRINCIPALE
	mbar = gtk_menu_bar_new();
	//Creazione di una box verticale per mettere il menu bar in alto e il resto della
	//finestra sotto di essa
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	//Aggiungo il mbar al box main_box
	gtk_box_pack_start (GTK_BOX(main_box), mbar, TRUE, TRUE, 0);
	////Inserisco un menu item (Settings) nella barra
	//GtkWidget * settings_mi = gtk_menu_item_new_with_label ("Settings");
	//gtk_menu_shell_append (GTK_MENU_SHELL (mbar), settings_mi);
	//Inserisco un menu item (help) nella barra 
	GtkWidget * help_mi = gtk_menu_item_new_with_label ("Menu");
	gtk_menu_shell_append (GTK_MENU_SHELL (mbar), help_mi);
	
	//Crea un SOTTO MENU (Help)
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
	
	
	//Attiviamo l'azione "about" ed "help"
	g_signal_connect (about_op, "activate", G_CALLBACK(show_about), NULL);
	g_signal_connect (help_op, "activate", G_CALLBACK(show_help), NULL);
	
	/////////////////////////////////////////////////////////////////////////////////
	
	// si creano e si aggiungono una serie di box che andranno a contenere gli elementi della finestra
	//gtk_box_new(<disposizione degli elementi>, <spazio tra gli elementi>)
	
	
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
	gtk_box_set_baseline_position (GTK_BOX(box), GTK_BASELINE_POSITION_CENTER);
	left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	serial_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
	gtk_widget_set_margin_top (right_box, 300); 
	gtk_box_set_baseline_position (GTK_BOX(right_box), GTK_BASELINE_POSITION_CENTER);
	input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	input1_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	input2_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	set_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_margin_top(set_box, 22); 
	
	
	//si aggiungono i container ad altri container seguendo la logica ad albero
	gtk_container_add (GTK_CONTAINER (right_box), input_box);
	gtk_container_add (GTK_CONTAINER (input_box), input1_box);
	gtk_container_add (GTK_CONTAINER (input_box), input2_box);	
	gtk_container_add (GTK_CONTAINER (input_box), set_box);
	gtk_container_add (GTK_CONTAINER (box), left_box);
	gtk_container_add (GTK_CONTAINER (box), right_box);
	gtk_container_add (GTK_CONTAINER (main_box), box );
	gtk_container_add (GTK_CONTAINER (window), main_box);
	
	// segue la construzione dei vari elementi
	
	//////////////////////////////////   LEFT BOX  //////////////////////////////////////////////
	logo= gtk_image_new_from_pixbuf(pixbuf_logo);
	gtk_container_add(GTK_CONTAINER(left_box), logo);
	label_output= gtk_label_new ("Motor velocity,  Motor position:");
	gtk_container_add(GTK_CONTAINER(left_box), label_output);
	scroll_box= gtk_scrolled_window_new(NULL, NULL); 
	gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll_box), 600); 
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_box), 500); 
	textArea = gtk_text_view_new_with_buffer(text_buf);
    gtk_widget_set_size_request(textArea, 600, 500);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textArea), 0);
	gtk_container_add(GTK_CONTAINER(scroll_box), textArea);
	gtk_container_add(GTK_CONTAINER(left_box), scroll_box);
	
	const char* data[2]; 
	
	serial_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	dev_lab = gtk_label_new ("Dev:");
	gtk_container_add (GTK_CONTAINER(serial_box), dev_lab);
	dev_input = gtk_entry_new_with_buffer(dev_buf);
	gtk_container_add (GTK_CONTAINER(serial_box), dev_input);
	baud_lab = gtk_label_new ("Baudrate:");
	gtk_container_add (GTK_CONTAINER(serial_box), baud_lab);
	baud_input = gtk_entry_new_with_buffer(baud_buf);
	gtk_container_add (GTK_CONTAINER(serial_box), baud_input);
	data[0]= (const char*) gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(dev_input))); 
	data[1]= (const char*) gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(baud_input))); 
	open_button = gtk_button_new_with_label("Open");
	g_signal_connect(open_button, "clicked", G_CALLBACK(open_serial), data);
	gtk_container_add (GTK_CONTAINER(serial_box), open_button);
	gtk_container_add (GTK_CONTAINER(left_box), serial_box);
	

	/////////////////////////////////        TEXT INPUT          //////////////////////////////////////////
	
	label_input_1= gtk_label_new ("Motor 1:");
	gtk_container_add(GTK_CONTAINER(input1_box), label_input_1);
	buf_input1= gtk_entry_buffer_new("100", strlen("100"));
	gtk_entry_buffer_set_max_length(buf_input1, 5);
	buf_input2= gtk_entry_buffer_new("100", strlen("100"));
	gtk_entry_buffer_set_max_length(buf_input2, 5);
	input_1= gtk_entry_new_with_buffer(buf_input1); 
	gtk_container_add (GTK_CONTAINER (input1_box), input_1);
	label_input_2= gtk_label_new ("Motor 2:");
	gtk_container_add(GTK_CONTAINER(input2_box), label_input_2);
	input_2= gtk_entry_new_with_buffer(buf_input2); 
	gtk_container_add (GTK_CONTAINER (input2_box), input_2);
	set_button=gtk_button_new_with_label("Set");
	gtk_container_add (GTK_CONTAINER(set_box), set_button);
	g_signal_connect (set_button, "clicked", G_CALLBACK(input), NULL);
	label_constraint= gtk_label_new ("Speed range: [-380, +380]");
	gtk_container_add(GTK_CONTAINER(right_box), label_constraint);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//si imposta la dimensione statica statica 
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	//si imposta la chiusura del programma 
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	//si mostra tutta la finestra appena creata 
	gtk_widget_show_all(window);

	//si chiama un ciclo principale che serve a controllare effettivamente il programma
	//per gestire gli eventi 
	gtk_main();
	
	sem_destroy(&sem); 
	free(buf); 

	return 0;
}

