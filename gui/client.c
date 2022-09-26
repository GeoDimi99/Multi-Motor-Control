#include <string.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "serial_linux.h"

/*ALBERO DEI CONTENITORI E DEGLI ELEMENTI: 
	 * window:
	 *   main_box:
	 *   	--> mbar 
	 * 	 	--> box:
	 * 				-->left_box
	 * 			 		- logo
	 * 			 		- label_output
	 * 			 		- textArea
	 * 					--> Serial box
	 * 						- 
	 *  -			-->right_box
	 *          		-->input_box
	 * 						-->input1_box
	 * 				 			- label_input_1
	 * 				 			- input_1 (di tipo entry)
	 * 						-->input2_box
	 * 				 			- label_input_2
	 * 				 			- input_2 (di tipo entry)
	 *						-->set_box
	 * 				 			- set_button
	 * 				 		- label_constraint
	 * 			
	 * 				 
	 * 				 
	*/
	
// Window (Finestra principale):

#define bsize 20

pthread_t thread;

GtkWidget* window;

GtkWidget* open_button; 

//variabili condivise necessarie per realizzare l'output
GtkTextBuffer* text_buf; 
GtkTextIter iter; 
char* buf; 
int fd; 
////////////////////////////////////////////////////////

gboolean append_char(gpointer user_data){
	const char* buffer= (const char*)user_data; 
	gtk_text_buffer_get_end_iter(text_buf, &iter); 
	gtk_text_buffer_insert(text_buf, &iter, buffer, -1); 
	return G_SOURCE_REMOVE; 
}

void* output(void * arg){ 
	while (1) {
		buf= (char*) malloc(sizeof(char)*bsize); 
		int n_read=read(fd, buf, bsize-1);
		*(buf + n_read)= '\0';  
		printf("%s", buf); 
		g_idle_add(append_char, buf); 
	}
}
	
void open_serial(GtkWidget *widget, gpointer data){
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
		dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_OTHER,
				GTK_BUTTONS_CLOSE,"Serial opened! ... \n");
			gtk_window_set_title(GTK_WINDOW(dialog), "Success");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
		
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
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Arduino Multi Motor Control è un progetto per il controllo simultaneo di motori dc. Il controllo viene effettuato attraverso un processo master che si interfaccia con vari processi slave controllori, comunicandovi attraverso protocollo TWI.");
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
	"ISTRUZIONI D'USO: \n \n  -Aprire la connessione con il dispositivo AVR master impostando il path del dispositivo e la banda \n \n- Impostare la velocità desiderata per i motori ed applicarla premendo il tasto 'Set' \n \n Warning!: il range della velocità deve essere nell'intervallo [-300, +300]. Tentativi di applicare velocità fuori dal range consentito verranno ignorati.");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
	
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
	GtkWidget* textArea;
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
	textArea = gtk_text_view_new_with_buffer(text_buf);
    gtk_widget_set_size_request(textArea, 600, 500);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textArea), 0);
	gtk_container_add(GTK_CONTAINER(left_box), textArea);
	
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
	input_1= gtk_entry_new(); 
	gtk_container_add (GTK_CONTAINER (input1_box), input_1);
	label_input_2= gtk_label_new ("Motor 2:");
	gtk_container_add(GTK_CONTAINER(input2_box), label_input_2);
	input_2= gtk_entry_new(); 
	gtk_container_add (GTK_CONTAINER (input2_box), input_2);
	set_button=gtk_button_new_with_label("Set");
	gtk_container_add (GTK_CONTAINER(set_box), set_button);
	//g_signal_connect (set_button, "clicked", G_CALLBACK(<funzione da chiamare>), NULL);
	label_constraint= gtk_label_new ("Speed range: [-300, +300]");
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

	return 0;
}

