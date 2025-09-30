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
  GtkWidget *dialog_pointer; // Puntero al widget
  GtkDialog *dialog; // Puntero de la ventana
  GtkWidget *vBox; // Contenedor principal
  GtkWidget *mBox; // Contenedor de la información
  
  GtkWidget *about_logo; // Logo del programa
  gchar *about_title; // Título del programa
  gchar *about_version; // Version del programa
  gchar *about_comments; // Comentario del programa
  gchar *about_copyright; // Copyright del programa

  gint credit_pages_count; // Cuenta de páginas del programa
  AboutDialogPage *pages[3]; // Páginas del programa
} AboutDialog;

typedef struct def_OnlineConnectionDialog // Espera de adversario en línea
{
  GtkWidget *dialog; // Puntero al widget
  GtkWidget *progress; // Barra de progreso
  gint res; // Resultado de la ventana (Si se cerró o si se encontró adversario)
} OnlineConnectionDialog;

typedef struct def_NewGameModal // Ventana de juego nuevo
{
  GtkWidget *warning; // Advertencia
  GtkWidget *dialog; // Puntero al widget
  GtkWidget *radio; // Input adversario seleccionado
  GtkWidget *check; // Input modo difícil
  GtkWidget *entry_names[2]; // Inputs nombres de los jugadores

  gchar names[2][21]; // Nombres de los jugadores
  gint vs_ia; // Contra la computadora
  gint hard_mode; // Modo difícil
} NewGameModal;

typedef struct def_GenericModal // Ventana genérica que deba mostrar una advertencia
{
  GtkWidget *warning; // Label de la advertencia
} Generic_Modal;

typedef struct def_OnlineRegisterModal // Ventana de registro para el modo multijugador
{
  GtkWidget *warning; // Advertencia
  GtkWidget *dialog; // Puntero al widget
  GtkWidget *entry_name; // Input nombre del jugador

  gchar name[21]; // Nombre del jugador
} OnlineRegisterModal;

/**
 * Funciones genéricas
 */

void my_dialogs_on_button_clicked(GtkWidget *widget, gpointer data);
void on_modal_updated(GtkWidget *widget, gpointer data);


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


/**
 * Funciones de la ventana de créditos
 */

// Creación
// Página de créditos
AboutDialogPage *about_dialog_page_new(gchar *name);
void about_dialog_page_add_line(AboutDialogPage *page, gchar *line);

// Ventana de créditos
AboutDialog *about_dialog_new(gchar *title);
void about_dialog_set_title(AboutDialog *dialog, gchar *title);
void about_dialog_set_version(AboutDialog *dialog, gchar *version);
void about_dialog_set_comments(AboutDialog *dialog, gchar *comments);
void about_dialog_set_copyright(AboutDialog *dialog, gchar *copyright);
void about_dialog_set_logo(AboutDialog *dialog, GdkPixbuf *logo);
void about_dialog_add_page(AboutDialog *dialog, AboutDialogPage *page);
void about_dialog_run(AboutDialog *dialog);
void about_dialog_destroy(AboutDialog *dialog);

// Funcionamiento interno
void about_dialog_on_credits(GtkWidget *widget, gpointer data);


/**
 * Funciones de la ventana de juego nuevo
 */

// Creación y recuperación de datos
void new_game_modal_new(NewGameModal *info);
gint new_game_modal_get_info(NewGameModal *info);

// Funciones internas
void new_game_modal_radio_changed(GtkWidget *widget, gpointer data);


/**
 * Funciones de la ventana de registro para multijugador
 */

// Creación y recuperación de datos
void online_register_modal_new(OnlineRegisterModal *info);
gint online_register_modal_get_info(OnlineRegisterModal *info);


/**
 * Funciones de la ventana de espera de oponente en multijugador
 */
void online_connection_dialog_new(OnlineConnectionDialog **info);
void online_connection_dialog_destroy(OnlineConnectionDialog **dialog);


/**
 * Ventanas comunes
 */

// Creación
gint confirmation_dialog(gchar *title, gchar *message);
void warning_dialog(gchar *title, gchar *message);