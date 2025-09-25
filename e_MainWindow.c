/**
 * @file e_MainWindow.c
 * 
 * @brief Contiene todas las funciones de eventos que suceden en la ventana principal
 * 
 * @author Luis Julián Zamora Treviño
 * @date 23/09/2025
 */
#include "gato.h"

/**
 * Al cerrar la ventana principal
 * 
 * @param *widget La ventana principal
 * @param data Información del juego como gpointer
 */
void stopTheApp(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	// Si se conectó a la base de datos
	if(juego->online.connected)
	{
		closeConnectionToDatabase(juego);
	}

	// en caso de que haya música sonando
	if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
	{
		system("killall aplay");
	}

	// detiene el loop
	gtk_main_quit();

	return;
}

// Menú Archivo

/**
 * Opción guardar partida
 * 
 * @param *widget La opción seleccionada
 * @param data Información del juego como gpointer
 */
void guardarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data; 

	// en caso de guardar sin partida en curso (no debería pasar)
	if(juego->partida.historial[juego->partida.turno].game_status == GAME_NOT_STARTED)
	{
		warning_dialog("Sin Partida en Curso", "No hay una partida en curso para guardar.");
		gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
	}
	else
	{
		saveGame(juego); // guarda partida
	}

	return;
}

/**
 * Opción cargar partida
 * 
 * @param *widget La opción seleccionada
 * @param data Información del juego como gpointer
 */
void cargarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	gint res = 0;
	gint v = 1;
	
	// si hay una partida en curso
	if(juego->partida.historial[juego->partida.turno].game_status != GAME_NOT_STARTED)
	{
		// consulta si se desea guardar partida y en caso de aceptar guarda
		do
		{
			res = confirmation_dialog("Cargar Partida", "¿Desea guardar la partida en curso?");

			if((res == GTK_RESPONSE_YES && saveGame(juego) == GTK_RESPONSE_ACCEPT) || res == GTK_RESPONSE_NO || res == GTK_RESPONSE_DELETE_EVENT)
			{
				v = 0;
			}
		} while(v);
	}

	// si no se ha cancelado la acción, carga la partida
	if(res != GTK_RESPONSE_DELETE_EVENT)
	{
		loadGame(juego);
	}

	return;
}

// Menú Juego

/**
 * Opcion nueva partida
 * 
 * @param *widget La opción seleccionada
 * @param data Información del juego como gpointer
 */
void nuevaPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data; 

	newGame(juego); // inicia una nueva partida

	return;
}

/**
 * Opción terminar partida
 *
 * @param *widget La opción seleccionada
 * @param data Información del juego como gpointer
 */
void terminarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	gint res = 0;

	int v = 0;

	// si no hay una partida en curso (no debería pasar)
	if(juego->partida.historial[juego->partida.turno].game_status == GAME_NOT_STARTED)
	{
		warning_dialog("Sin Partida en Curso", "No hay una partida en curso para terminar.");
		gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);

		return;
	}

	// Consulta si se desa guardar y guarda en caso de que si
	do
	{
		v = 1;
		res = confirmation_dialog("Terminar Partida", "¿Deseas guardar la partida antes de terminarla?");

		if((res == GTK_RESPONSE_YES && saveGame(juego) == GTK_RESPONSE_ACCEPT) || res == GTK_RESPONSE_NO || res == GTK_RESPONSE_DELETE_EVENT)
		{
			v = 0;
		}
	} while(v);

	// si no se ha cancelado la acción, limpia la ventana
	if(res != GTK_RESPONSE_DELETE_EVENT)
	{
		gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
		gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
	
		cleanScreen(juego);
		gameStartup(juego);
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

/**
 * Registra al usuario para el online
 * 
 * @param *widget La opción del menu seleccionada
 * @param data Puntero a NULL
 * 
 * @returns void
 */
void registrarUsuario(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	registerUser(juego);

	return;
}

/**
 * Busca una partida online
 * 
 * @param *widget La opción del menu seleccionada
 * @param data Puntero a NULL
 * 
 * @returns void
 */
void buscarPartida(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	seekMatch(juego);

	return;
}

/**
 * Se rinde en el online
 * 
 * @param *widget La opción del menu seleccionada
 * @param data Puntero a NULL
 * 
 * @returns void
 */
void rendirse(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	forfeit(juego);

	return;
}

// Botones del tablero

/**
 * Al pasar el cursor sobre un botón del tablero
 * 
 * @param *eventbox El botón seleccionado
 * @param *event Evento ocurrido
 * @param data Información del juego como gpointer
 */
void board_button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	GdkColor color;

	// Si la partida no se ha terminado, cambia el color del fondo
	if(juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED && ((juego->online.playing && juego->partida.jugadores[juego->partida.turno % 2].online_id == juego->online.u_id[0]) || !juego->online.playing))
	{
		gdk_color_parse("#A3A3A3", &color);
		gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
	}

	return;
}

/**
 * Al sacar el cursor de un botón del tablero
 * 
 * @param *eventbox El botón seleccionado
 * @param *event Evento ocurrido
 * @param data Información del juego como gpointer
 */
void board_button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	GdkColor color;

	int coords[2];

	// Si el botón no ha sido clickeado y no se ha terminado la partida, reinicia el color de fondo
	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && (juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED && ((juego->online.playing && juego->partida.jugadores[juego->partida.turno % 2].online_id == juego->online.u_id[0]) || !juego->online.playing)))
	{
		gdk_color_parse("#DCDAD5", &color);
		gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
	}

	return;
}

/**
 * Al presionar un botón del tablero
 * 
 * @param *eventbox El botón seleccionado
 * @param *event Evento ocurrido
 * @param data Información del juego como gpointer
 */
void board_button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	int coords[2];

	// si es capaz de recuperar el botón, no ha sido presionado y el juego no ha terminado
	if(getButton(juego, eventbox, coords) && juego->partida.historial[juego->partida.turno].tablero[coords[0]][coords[1]] == ' ' && juego->partida.historial[juego->partida.turno].game_status == GAME_STARTED)
	{
		if(juego->online.playing && juego->partida.jugadores[juego->partida.turno % 2].online_id == juego->online.u_id[0])
		{
			g_print("A\n");
		}
		else
		{
			turnPlayed(juego, coords[0], coords[1]); // juega un turno
		}
	}
	else if(juego->partida.historial[juego->partida.turno].game_status == GAME_NOT_STARTED) // si no se ha iniciado la partida
	{
		newGame(juego); // establece una nueva partida
	}

	return;
}

// botones de historial

/**
 * Turno pasado
 * 
 * @param *widget El botón seleccionado
 * @param data Información del juego como gpointer
 */
void history_past(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;

	lastTurn(juego); // intenta regresar al turno pasado

	return;
}

/**
 * Siguiente turno
 * 
 * @param *widget El botón seleccionado
 * @param data Infomración del juego ocmo gpointer
 */
void history_next(GtkWidget *widget, gpointer data)
{
	JUEGO *juego = (JUEGO *)data;
	
	nextTurn(juego); // intenta ir al siguiente turno

	return;
}