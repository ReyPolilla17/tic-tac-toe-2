#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mydialogs.h"

#define IA_NAME "hercules.raw"
#define ICONS "XO"

// Estados del juego (Para facilitar la lectura del código)
#define GAME_ENDED -1 // El juego ha temrinado (hay un ganador o un empate)
#define GAME_STARTED 1 // El juego está en curso
#define GAME_NOT_STARTED 0 // No hay una partida en curso

typedef struct def_movimiento // Turno jugado (historial)
{
    char tablero[3][3]; // Tablero en ese movimiento
    int game_status; // Estado de la partida en ese movimiento (constantes GAME_...)
    int hist_val; // Si es un turno jugable 
} MOVIMIENTO;

typedef struct def_jugador // Información del jugador
{
    char nombre[21]; // Nombre del jugador
    int hard_mode; // Modo dificil activo
    int ia; // si el jugador es una ia
} JUGADOR;

typedef struct def_funcional // Parte funcional del juego
{
    JUGADOR jugadores[2]; // Información de los jugadores
    MOVIMIENTO historial[10]; // Movimientos a lo largo de la partida
    int turno; // Número de turno actual
    int turno_max; // Máximo turno del que se tiene historial
} FUNCIONAL;

// estructura con todos los widgets que se utilizan en la ventana principal
typedef struct def_grafico
{
  GtkWidget *window;
  GtkWidget *menuTruth;
  GtkWidget *menuSave;
  GtkWidget *menuEnd;
  GtkWidget *playingImg;
  GtkWidget *playingBox;
  GtkWidget *moveButtons[2];
  GtkWidget *playerImg[2];
  GtkWidget *playerName[2];
  GtkWidget *flames[2];
  GtkWidget *buttons[3][3];
  GtkWidget *buttonImg[3][3];
  GdkPixbuf *m20[3];
  GdkPixbuf *m60[3];
  GdkPixbuf *hercules;
} GRAFICO;

typedef struct def_juego
{
    FUNCIONAL partida;
    GRAFICO graficos;
} JUEGO;

// inicialización
void gameStartup(JUEGO *juego);
void loadMainWindow(JUEGO *juego);

void stopTheApp(GtkWidget *widget, gpointer data);

// funciones del menú de la ventana principal
void guardarPartida(GtkWidget *widget, gpointer data);
void cargarPartida(GtkWidget *widget, gpointer data);
void nuevaPartida(GtkWidget *widget, gpointer data);
void terminarPartida(GtkWidget *widget, gpointer data);
void comoJugar(GtkWidget *widget, gpointer data);
void acercaDe(GtkWidget *widget, gpointer data);
void laVerdad(GtkWidget *widget, gpointer data);

// funciones de los botones del tablero
void board_button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data);
void board_button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data);
void board_button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data);

// funciones de los botones del historial
void history_past(GtkWidget *widget, gpointer data);
void history_next(GtkWidget *widget, gpointer data);

// Utilidades
int getButton(JUEGO *juego, GtkWidget *button, int coords[2]);

void coppyBoard(char dest[3][3], char src[3][3]);
void logMove(JUEGO *juego, int replayable);
void turnPlayed(JUEGO *juego, int x, int y);
int checkGame(char tablero[3][3], char played);

void aiTurn(JUEGO *juego);
void chooseSpace(JUEGO *juego, int *x, int *y);

void cleanScreen(JUEGO *juego);
void displayHardMode(JUEGO *juego);
void coppyBoardIntoGraphic(JUEGO *juego);
void coppyIntoGraphic(JUEGO *juego);

int moreTurnsBackwards(JUEGO *juego);
void lastTurn(JUEGO *juego);

int moreTurnsForwards(JUEGO *juego);
void nextTurn(JUEGO *juego);

void newGame(JUEGO *juego);
gint saveGame(JUEGO *juego);
void loadGame(JUEGO *juego);
int correctFileFormat(FUNCIONAL gameData);

void tie_dialog(JUEGO *juego);
void victory_dialog(JUEGO *juego);