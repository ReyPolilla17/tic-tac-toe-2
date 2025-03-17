#include "gato.h"

void stopTheApp(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	return;
}

// Menú Archivo
void guardarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data; 

	saveGame(juego);
}

void cargarPartida(GtkWidget *widget, gpointer data)
{
	g_print("Missing function...\n");
}

// Menú Juego
void nuevaPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data; 

	newGame(juego);

	return;
}

void terminarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	gint res = 0;

	int v = 0;

	do
	{
		v = 1;
		res = confirmation_dialog("Terminar Partida", "¿Deseas guardar la partida antes de terminarla?");

		if((res == GTK_RESPONSE_YES && saveGame(juego) == GTK_RESPONSE_ACCEPT) || res == GTK_RESPONSE_NO || res == GTK_RESPONSE_DELETE_EVENT)
		{
			v = 0;
		}
	} while(v);

	if(res != GTK_RESPONSE_DELETE_EVENT)
	{
		gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
		gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
	
		gameStartup(juego);
		cleanScreen(juego);
	}
	
	return;
}


// Menú Más

/**
 * Muestra una ventana con las instrucciones para usar el programa
 * 
 * @param *widget La opción del menú seleccionada
 * @param data Puntero a NULL
 * 
 * @returns void
 */
void comoJugar(GtkWidget *widget, gpointer data)
{
    HelpDialog *dialog;

	// Carga el archivo del manual y lo muestra en una ventana especializada
	dialog = help_dialog_new("Cómo Jugar?");
		help_dialog_populate_from_file(dialog, "./MEDIA/manual.txt", "END\n");
		help_dialog_run(dialog);

    return;
}

/**
 * Muestra los créditos del programa
 * 
 * @param *widget La opción del menú seleccionada
 * @param data Puntero a NULL
 */
void acercaDe(GtkWidget *widget, gpointer data)
{
	AboutDialog *about;
	AboutDialogPage *page;
	GError *error = NULL;

	// carga la vista principal de los créditos
	about = about_dialog_new("Acerca De");
		about_dialog_set_logo(about, gdk_pixbuf_new_from_file_at_size("./MEDIA/logo.png", 150, 150, &error));
		about_dialog_set_title(about, "Tic-Tac-Toe");
		about_dialog_set_version(about, "2.0");
		about_dialog_set_comments(about, "Programación Aplicada y Laboratorio");
		about_dialog_set_copyright(about, "Profesor Jorge Rodríguez García");

	// Agrega los créditos de programación
	page = about_dialog_page_new("Programación");
		about_dialog_page_add_line(page, "Sánchez Zepeda Ricardo (ict23rsz)");
		about_dialog_add_page(about, page);

	// Agrega los créditos de diseño
	page = about_dialog_page_new("Diseño");
		about_dialog_page_add_line(page, "Luis Julián Zamora Treviño (ict23jzt)");
		about_dialog_add_page(about, page);

	// Agrega los creditos de documentación
	page = about_dialog_page_new("Documentación");
		about_dialog_page_add_line(page, "Malouly Orozco Mariano (msc23mmo)");
		about_dialog_add_page(about, page);

	// muestra la ventana
	about_dialog_run(about);
}

/**
 * Muestra la verdad
 * 
 * @param *widget La opción del menu seleccionada
 * @param data Puntero a NULL
 * 
 * @returns void
 */
void laVerdad(GtkWidget *widget, gpointer data)
{
	HelpDialog *dialog;

	// Le muestra la verdad al usuario
	dialog = help_dialog_new("1fg289h73d.777");
		help_dialog_populate_from_file(dialog, "./MEDIA/1fg289h73d.777", "--");
		help_dialog_run(dialog);

	return;
}

// Botones del tablero

void board_button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	GdkColor color;

	if(juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED)
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

	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED)
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

	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && juego->partida.historial[juego->partida.turno].game_status == GAME_STARTED)
	{
		turnPlayed(juego, coords[0], coords[1]);
	}
	else if(juego->partida.historial[juego->partida.turno].game_status == GAME_NOT_STARTED)
	{
		newGame(juego);
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