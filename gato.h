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
    int turno_max; // Máximo turno del que se tiene historial
} FUNCIONAL;

// estructura con todos los widgets que se utilizan en la ventana principal
typedef struct def_grafico
{
  GtkWidget *window;
  GtkWidget *menuMore;
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
  GdkPixbuf *logo;
} GRAFICO;

typedef struct def_juego
{
    FUNCIONAL partida;
    GRAFICO graficos;
} JUEGO;

// inicialización
void gameStartup(JUEGO *juego);
void loadMainWindow(JUEGO *juego);

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

void stopTheApp(GtkWidget *widget, gpointer data);

// Utilidades
int getButton(JUEGO *juego, GtkWidget *button, int coords[2]);


// void gtk_entry_set_max_length (GtkEntry *entry, gint max_length);