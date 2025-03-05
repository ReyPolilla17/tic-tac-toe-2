#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Estructuras de la ventana de ayuda
 */
typedef struct def_HelpPage // Página de ayuda
{
	gchar *title; // Título de la página
	gchar *content; // Contenido de la página

	struct def_HelpPage *ant; // Página anterior
	struct def_HelpPage *next; // Página siguiente
} HelpPage;

typedef struct def_HelpDialog // Ventana de ayuda
{
	GtkWidget *dialog_pointer; // Puntero al widget
	GtkWidget *iButton; // Botón interno (cerrar)
	GtkWidget *cButton; // Botón cerrar
	GtkWidget *pButton; // Botón anterior
	GtkWidget *nButton; // Botón siguiente
	GtkWidget *tLabel; // Etiqueta título
	GtkWidget *cLabel; // Etiqueta contenido
	GtkWidget *cBox; // Espacio para el contenido

	GtkDialog *dialog; // Puntero de la ventana
	HelpPage *current; // Páginas del manual

	gint maxTitH; // Máxima altura del título
	gint maxConH; // Máxima altura del contenido
	gint maxDiaW; // Máximo ancho de la ventana
} HelpDialog;

/**
 * Funciones para el menú de ayuda
 */

// Creación
HelpDialog *help_dialog_new(gchar *title);
void help_dialog_populate_from_file(HelpDialog *dialog, gchar *file, gchar *separator);
void help_dialog_add_page(HelpDialog *dialog, gchar *title, gchar *content);
void help_dialog_run(HelpDialog *dialog);
void help_dialog_destroy(HelpDialog *dialog);

// Funcionamiento interno
void help_dialog_on_past(GtkWidget *widget, gpointer data);
void help_dialog_on_next(GtkWidget *widget, gpointer data);
void help_dialog_on_close(GtkWidget *widget, gpointer data);