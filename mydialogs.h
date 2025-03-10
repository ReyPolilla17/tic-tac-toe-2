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
 * Estructuras de las ventana créditos
 */
typedef struct def_AboutDialogPage // Página de créditos
{
  gchar *page_name; // Nombre de la sección
  gchar *page_content; // Contenido de la sección
} AboutDialogPage;

typedef struct def_AboutDialog // Ventana de créditos
{
  GtkWidget *dialog_pointer;
  GtkWidget *vBox;
  GtkWidget *mBox;
  GtkWidget *about_logo;
  gchar *about_title;
  gchar *about_version;
  gchar *about_comments;
  gchar *about_copyright;

  gint credit_pages_count;
  AboutDialogPage *pages[3];

  GtkDialog *dialog;
  GtkWidget *iButton;
  GtkWidget *crButton;
  GtkWidget *iclButton;
} AboutDialog;

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


/**
 * Funciones de la ventana de créditos
 */

// Creación
// Página de créditos
AboutDialogPage *about_dialog_page_new(const gchar *name);
void about_dialog_page_add_line(AboutDialogPage *page, const gchar *line);

// Ventana de créditos
AboutDialog *about_dialog_new(const gchar *title);
void about_dialog_set_title(AboutDialog *dialog, gchar *title);
void about_dialog_set_version(AboutDialog *dialog, gchar *version);
void about_dialog_set_comments(AboutDialog *dialog, gchar *comments);
void about_dialog_set_copyright(AboutDialog *dialog, gchar *copyright);
void about_dialog_set_logo(AboutDialog *dialog, GdkPixbuf *logo);
void about_dialog_add_page(AboutDialog *dialog, AboutDialogPage *page);
void about_dialog_run(AboutDialog *dialog);
void about_dialog_destroy(AboutDialog *dialog);

// Funcionamiento interno
void about_dialog_on_credits(GtkWidget *widget, gpointer *data);
void about_dialog_on_close(GtkWidget *widget, gpointer data);
void about_dialog_credits_on_close(GtkWidget *widget, gpointer data);