#include "gato.h"

void comoJugar(GtkWidget *widget, gpointer data)
{
    HelpDialog *dialog;

	dialog = help_dialog_new("Cómo Jugar?");

	help_dialog_populate_from_file(dialog, "./MEDIA/manual.txt", "END\n");

	help_dialog_run(dialog);

    return;
}

/**
 * Muestra Información sobre los creadores del programa
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void acercaDe(GtkWidget *widget, gpointer data)
{
	JUEGO *datos = (JUEGO *)data;
	GtkWidget *dialog;

	// principales encargados (todos contribuimos en todo, pero a cada uno se le asigno la coordinación de cada parte del proyecto)

	// El principal encargado de la parte funcional
	const gchar *authors[] = {
		"Sánchez Zepeda Ricardo (ict23rsz)",
	NULL
	};

	// el principal encargado de la documentación
	const gchar *documenters[] = {
		"Malouly Orozco Mariano (msc23mmo)",
	NULL
	};

	// el principal encargado de la parte gráfica
	const gchar *artists[] = {
		"Zamora Treviño Luis Julián (ict23jzt)",
	NULL
	};

	dialog = gtk_about_dialog_new();
		gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Tic Tac Toe"); // nombre del proyecto
		gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "2.0"); // version
		gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Programación Aplicada y Laboratorio\nMaestro Jorge Rodríguez García"); // se usa de subtitulo
		gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors); // programador principal
		gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters); // documentador principal
		gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), artists); // diseñador principal
		gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), datos->graficos.logo); // logo del proyecto

	// muestra la ventana
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return;
}

void StopTheApp(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	return;
}