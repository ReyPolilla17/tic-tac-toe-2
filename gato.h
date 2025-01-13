#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IA_NAME "hercules.raw"
#define ICONS "XO"

typedef struct def_movimiento
{
    char tablero[3][3];
    int game_status;
    int hist_val;
} MOVIMIENTO;

typedef struct def_jugador
{
    char nombre[1000];
    int hard_mode;
    int ia;
} JUGADOR;

typedef struct def_funcional
{
    JUGADOR jugadores[2];
    MOVIMIENTO historial[10];
    int turno;
    int turnoMax;
} FUNCIONAL;

// estructura con todos los widgets que se utilizan en la ventana principal
typedef struct def_grafico
{
  GtkWidget *window;
  GtkWidget *playingImg;
  GtkWidget *playingBox;
  GtkWidget *moveButtons[2];
  GtkWidget *playerImg[2];
  GtkWidget *playerName[2];
  GtkWidget *flames[2];
  GtkWidget *buttons[3][3];
  GtkWidget *buttonImg[3][3];
  GdkPixbuf *m20[3];
  GdkPixbuf *m40[3];
  GdkPixbuf *hercules;
  GdkPixbuf *logo;
} GRAFICO;

typedef struct def_juego
{
    FUNCIONAL partida;
    GRAFICO graficos;
} JUEGO;

// estructuras para el menú de ayuda
typedef struct def_HelpPage
{
	gchar *title;
	gchar *content;

	struct def_HelpPage *ant;
	struct def_HelpPage *next;
} HelpPage;

typedef struct def_HelpDialog
{
	GtkWidget *dialog_pointer;
	GtkWidget *iButton;
	GtkWidget *cButton;
	GtkWidget *pButton;
	GtkWidget *nButton;
	GtkWidget *tLabel;
	GtkWidget *cLabel;
	GtkWidget *cBox;

	GtkDialog *dialog;
	HelpPage *current;

	gint maxTitH;
	gint maxConH;
	gint maxDiaW;
} HelpDialog;

int loadGame(JUEGO *juego);
void playGame(JUEGO *juego);

void printBoard(char tablero[3][3]);
int validateInput(char frase[]);
void displayHelp(void);

void playTurn(JUEGO *juego);

void aiTurn(JUEGO *juego);
void chooseSpace(JUEGO *juego, int *x, int *y);

void setupNewGame(JUEGO *juego);
void coppyBoard(char dest[3][3], char src[3][3]);
void logMove(JUEGO *juego, int replayable);
void lastTurn(JUEGO *juego);
void nextTurn(JUEGO *juego);
int checkGame(char tablero[3][3], char played);

void loadMainWindow(JUEGO *juego);


// funciones del menú de la ventana principal
void comoJugar(GtkWidget *widget, gpointer data);
void StopTheApp(GtkWidget *widget, gpointer data);


// funciones para el menú de ayuda

HelpDialog *help_dialog_new(gchar *title);
void help_dialog_populate_from_file(HelpDialog *dialog, gchar *file, gchar *separator);
void help_dialog_add_page(HelpDialog *dialog, gchar *title, gchar *content);
void help_dialog_run(HelpDialog *dialog);
void help_dialog_destroy(HelpDialog *dialog);

void help_dialog_on_past(GtkWidget *widget, gpointer data);
void help_dialog_on_next(GtkWidget *widget, gpointer data);
void help_dialog_on_close(GtkWidget *widget, gpointer data);