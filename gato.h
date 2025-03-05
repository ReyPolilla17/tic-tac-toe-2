#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mydialogs.h"

#define IA_NAME "hercules.raw"
#define ICONS "XO"

typedef struct def_movimiento // Turno jugado (historial)
{
    char tablero[3][3]; // Tablero en ese movimiento
    int game_status; // Estado de la partida en ese movimiento (-1 -> terminado, 1 -> en curso, 0 -> no iniciada)
    int hist_val; // Si es un turno jugable 
} MOVIMIENTO;

typedef struct def_jugador // Información del jugador
{
    char nombre[1000]; // Nombre del jugador
    int hard_mode; // Modo dificil activo
    int ia; // si el jugador es una ia
} JUGADOR;

typedef struct def_funcional // Parte funcional del juego
{
    JUGADOR jugadores[2]; // Información de los jugadores
    MOVIMIENTO historial[10]; // Movimientos a lo largo de la partida
    int turno; // Número de turno actual
    int turnoMax; // Máximo turno del que se tiene historial
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
  GdkPixbuf *m60[3];
  GdkPixbuf *hercules;
  GdkPixbuf *logo;
} GRAFICO;

typedef struct def_juego
{
    FUNCIONAL partida;
    GRAFICO graficos;
} JUEGO;

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
void acercaDe(GtkWidget *widget, gpointer data);
void StopTheApp(GtkWidget *widget, gpointer data);