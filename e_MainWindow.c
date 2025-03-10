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
	AboutDialog *about;
	AboutDialogPage *page;
	GError *error = NULL;

	about = about_dialog_new("Acerca De");
		about_dialog_set_logo(about, gdk_pixbuf_new_from_file_at_size("./MEDIA/logo.png", 150, 150, &error));
		about_dialog_set_title(about, "Tic-Tac-Toe");
		about_dialog_set_version(about, "2.0");
		about_dialog_set_comments(about, "Programación Aplicada y Laboratorio");
		about_dialog_set_copyright(about, "Profesor Jorge Rodríguez García");

	page = about_dialog_page_new("Programación");
		about_dialog_page_add_line(page, "Sánchez Zepeda Ricardo (ict23rsz)");
		about_dialog_add_page(about, page);

	page = about_dialog_page_new("Diseño");
		about_dialog_page_add_line(page, "Luis Julián Zamora Treviño (ict23jzt)");
		about_dialog_add_page(about, page);

	page = about_dialog_page_new("Documentación");
		about_dialog_page_add_line(page, "Malouly Orozco Mariano (msc23mmo)");
		about_dialog_add_page(about, page);

	about_dialog_run(about);
}

void laVerdad(GtkWidget *widget, gpointer data)
{
	HelpDialog *dialog;

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