#include "gato.h"

void stopTheApp(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	return;
}

// Menú Archivo
void guardarPartida(GtkWidget *widget, gpointer data)
{
	
}

void cargarPartida(GtkWidget *widget, gpointer data)
{

}

// Menú Juego
void nuevaPartida(GtkWidget *widget, gpointer data)
{
	
}

void terminarPartida(GtkWidget *widget, gpointer data)
{

}


// Menú Más
void comoJugar(GtkWidget *widget, gpointer data)
{
    HelpDialog *dialog;

	dialog = help_dialog_new("Cómo Jugar?");

	help_dialog_populate_from_file(dialog, "./MEDIA/manual.txt", "END\n");

	help_dialog_run(dialog);

    return;
}

void acercaDe(GtkWidget *widget, gpointer data)
{

}

// Botones del tablero

void board_button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	GdkColor color;

	if(juego->partida.historial[juego->partida.turno].game_status != -1)
	{
		gdk_color_parse("#A3A3A3", &color);
		gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
	}

	return;
}

void board_button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	GdkColor color;

	int coords[2];

	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && juego->partida.historial[juego->partida.turno].game_status != -1)
	{
		gdk_color_parse("#DCDAD5", &color);
		gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
	}

	return;
}

void board_button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	int coords[2];

	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && juego->partida.historial[juego->partida.turno].game_status != -1)
	{
		g_print("%d, %d\n", coords[0], coords[1]);
	}

	return;
}

// botones de historial
void history_past(GtkWidget *widget, gpointer data)
{

}

void history_next(GtkWidget *widget, gpointer data)
{

}