/**
 * @file f_MainWindow.c
 * 
 * @brief Contiene todas las funciones que requiere la ventana principal para funcionar correctamente
 * 
 * @author Luis Julián Zamora Treviño
 * @date 16/03/2025
 */
#include "gato.h"

/**
 * Obtiene las coordenadas del botón del tablero que fue presionado
 * 
 * @param *juego Información de la partida
 * @param *button El botón que fue presionado
 * @param coords[2] Las coordenadas del botón seleccionado
 * 
 * @returns int (0 si no encuentra el botón, 1 si encuentra el botón)
 */
int getButton(JUEGO *juego, GtkWidget *button, int coords[2])
{
    int i = 0;
    int j = 0;

    // busca en todos los botones el botón seleccionado
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            if(button == juego->graficos.buttons[i][j]) // al encontrar el botón
            {
                // guarda las coordenadas
                coords[0] = i;
                coords[1] = j;

                // sale de la función
                return 1;
            }
        }
    }

    return 0;
}

/**
 * Inicializa la partida internamente
 * 
 * @param *juego información de la partida
 */
void gameStartup(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    // reinicia el historial
    juego->partida.turno = 0;
    juego->partida.turno_max = 0;

    juego->partida.historial[0].game_status = GAME_NOT_STARTED;
    juego->partida.historial[0].hist_val = 0;

    // reinicia la infroamcaión de los jugadores
    for(i = 0; i < 2; i++)
    {
        juego->partida.jugadores[i].hard_mode = 0;
        juego->partida.jugadores[i].ia = 0;
        juego->partida.jugadores[i].nombre[0] = 0;
    }

    // reinicia el tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            juego->partida.historial[0].tablero[i][j] = ' ';
        }
    }

    return;
}

/**
 * Inicializa la partida de forma general (grafica e internamente)
 * 
 * @param *juego Información de la partida
 */
void newGame(JUEGO *juego)
{
    NewGameModal info;

    gint res = GTK_RESPONSE_NO;

    int rn = 0;
    int v = 0;
    int np = 0;

	// Si hay una partida en curso
	if(juego->partida.historial[juego->partida.turno].game_status != GAME_NOT_STARTED)
	{
        // consulta y guarda en caso de ser necesario
        do
        {
            v = 1;
            res = confirmation_dialog("Partida en curso", "¿Deseas guardar la partida actual?");

            if((res == GTK_RESPONSE_YES && saveGame(juego) == GTK_RESPONSE_ACCEPT) || res == GTK_RESPONSE_NO)
            {
                v = 0;
            }
        } while(v);
	}

    // Si no se ha cancelado la acción
    if(res != GTK_RESPONSE_DELETE_EVENT)
    {
        new_game_modal_new(&info); // ventana de nueva partida

        // extrae los datos de la nueva partida
        if(new_game_modal_get_info(&info))
        {
            rn = rand() % 2; // orden al azar de los jugadores
            
            // limpia la ventana e inicializa internamente
            cleanScreen(juego);
            gameStartup(juego);

            // muestra el modo dificil si se desea
            if(info.hard_mode)
            {
                displayHardMode(juego);
            }

            // habilita opciones exclusivas de partida en curso
            gtk_widget_set_sensitive(juego->graficos.menuEnd, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuSave, TRUE);

            // Si el adversario va a ser la computadora
            juego->partida.jugadores[rn].hard_mode = info.hard_mode;
            juego->partida.jugadores[rn].ia = info.vs_ia;

            // si se enfrenta a la computadora, copia el nombre de la ia
            if(info.vs_ia)
            {
                strcpy(juego->partida.jugadores[rn].nombre, IA_NAME);
            }
            else // de lo contrario, copia el nombre del segundo jugador
            {
                strcpy(juego->partida.jugadores[rn].nombre, info.names[1]);
            }
            
            // El otro jugador siempre será el primer usuario introducido
            juego->partida.jugadores[(rn + 1) % 2].hard_mode = 0;
            juego->partida.jugadores[(rn + 1) % 2].ia = 0;
            
            strcpy(juego->partida.jugadores[(rn + 1) % 2].nombre, info.names[0]);

            // Muestra los nombres de los jugadores y sus fichas
            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[0]), juego->partida.jugadores[0].nombre);
            gtk_widget_show(juego->graficos.playerImg[0]);

            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[1]), juego->partida.jugadores[1].nombre);
            gtk_widget_show(juego->graficos.playerImg[1]);
            
            // Establece el status como listo para jugar
            juego->partida.historial[0].game_status = GAME_STARTED;

            // si el primer jugador es la computadora, juega su turno
            if(juego->partida.jugadores[0].ia)
            {
                np = 1;
                aiTurn(juego);
            }

            // muesta el jugador del turno actual
            gtk_widget_destroy(juego->graficos.playingImg);
            juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[np]);
                gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                gtk_widget_show(juego->graficos.playingImg);
        }
    }
    
    return;
}

/**
 * Guarda la partida en curso
 * 
 * @param *juego Información del juego
 * 
 * @return gint (El resultado de la ventana de guardado)
 */
gint saveGame(JUEGO *juego)
{
    FILE *fp;

    GtkWidget *dialog;

    gint res = 0;
    gchar *file;

    // muestra la ventana de guardado
    dialog = gtk_file_chooser_dialog_new("Guardar Partida", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    // si se da click en guardar
    if(res == GTK_RESPONSE_ACCEPT)
    {
        // obtiene la ruta
        file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        // guarda la partida en el archivo
        fp = fopen(file, "wb");
        fwrite(&juego->partida, sizeof(FUNCIONAL), 1, fp);
        fclose(fp);

        // libera memoria
        g_free(file);

        warning_dialog("Partida Guardada", "¡La partida ha sido guardada con éxito!");
    }

    gtk_widget_destroy(dialog);

    return res;
}

/**
 * Carga una partida
 * 
 * @param *juego Información de la partida
 */
void loadGame(JUEGO *juego)
{
    FILE *fp;

    FUNCIONAL new;
    
    GtkWidget *dialog;

    int success = 0;
    int v = 0;

    gint res = 0;
    gchar *file;

    // muestra la ventana de guardado
    dialog = gtk_file_chooser_dialog_new("Cargar Partida", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

    // mientras no se haya cargado exitosamente un archivo o no se cancele la acción de cargar
    do
    {
        v = 1;

        res = gtk_dialog_run(GTK_DIALOG(dialog)); // muestra la ventana

        // si se da click en guardar
        if(res == GTK_RESPONSE_ACCEPT)
        {
            // obtiene la ruta
            file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            
            // carga la partida del archivo
            fp = fopen(file, "rb");

            // si pudo abrir el archivo
            if(fp != NULL)
            {
                // intenta leer toda la información de la partida en una variable de paso
                success = fread(&new, sizeof(FUNCIONAL), 1, fp);

                fclose(fp);
                
                // si pudo leer la información y es correcta
                if(success && correctFileFormat(new))
                {
                    juego->partida = new; // asigna la información a la partida
                    coppyIntoGraphic(juego); // carga gráficamente la información recuperada
                }
                // de lo contrario
                else
                {
                    warning_dialog("Cargar Partida", "Archivo corrupro o incompatible...");
                    v = 0;
                }
            }
            // de lo contrario
            else
            {
                warning_dialog("Cargar Partida", "No se puede acceder al archivo...");
                v = 0;
            }

            // libera memoria
            g_free(file);
        }
    } while(!v);
        
    gtk_widget_destroy(dialog);

    return;
}

/**
 * Limpia la ventana principal
 * 
 * @param *juego Información de la partida
 */
void cleanScreen(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    GdkColor color;

    gdk_color_parse("#DCDAD5", &color);

    // cambia el fondo de la ventana principal
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    // esconde secretos
    gtk_widget_hide(juego->graficos.menuTruth);

    // oculta el jugador actuañ
    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);
     
    // reinicia los botones del tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            gtk_widget_destroy(juego->graficos.buttonImg[i][j]);
            juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
                gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[i][j]), juego->graficos.buttonImg[i][j]);
                gtk_widget_show(juego->graficos.buttonImg[i][j]);
            
            gtk_widget_modify_bg(juego->graficos.buttons[i][j], GTK_STATE_NORMAL, &color);
        }
    }
    
    // oculta las llamas y la información de los jugadores y desactiva los botones del historial
    for(i = 0; i < 2; i++)
    {
        gtk_widget_hide(juego->graficos.flames[i]);
        gtk_widget_hide(juego->graficos.playerImg[i]);

        gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[i]), NULL);

        gtk_widget_set_sensitive(juego->graficos.moveButtons[i], FALSE);
    }

    // detiene la música
    if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
    {
        system("killall aplay");
    }

    return;
}

/**
 * Muestra la ventana principal con el modo difícil
 * 
 * @param *juego Información de la partida
 */
void displayHardMode(JUEGO *juego)
{
    int i = 0;

    GdkColor color;

    gdk_color_parse("#790000", &color);

    // cambia el color de fondo de la ventana
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    // muestra secretos
    gtk_widget_show(juego->graficos.menuTruth);

    // muestra las llamas
    for(i = 0; i < 2; i++)
    {
        gtk_widget_show(juego->graficos.flames[i]);
    }

    // reproduce la música
    system("aplay ./MEDIA/hercules.wav &");

    return;
}

/**
 * Crea una instancia de historial
 * 
 * @param *juego La partuda en curso
 * @param replayable Si el turno perdenece a un usuario o a una ia
 * 
 * @returns void
 */
void logMove(JUEGO *juego, int replayable)
{
    // actualiza el valor máximo de turno
    juego->partida.turno ++;
    juego->partida.turno_max = juego->partida.turno;

    // establece si se puede volver a jugar ese turno y el estado del juego
    juego->partida.historial[juego->partida.turno].hist_val = replayable;
    juego->partida.historial[juego->partida.turno].game_status = juego->partida.historial[juego->partida.turno - 1].game_status;

    // copia el tablero de el turno anterior
    coppyBoard(juego->partida.historial[juego->partida.turno].tablero, juego->partida.historial[juego->partida.turno - 1].tablero);

    return;
}

/**
 * Copia un arreglo de caracteres 3 x 3 en otro arreglo de caracteres 3 x 3
 * 
 * @param dest[3][3] Arreglo destino
 * @param src[3][3] Arreglo origen
 */
void coppyBoard(char dest[3][3], char src[3][3])
{
    int i = 0;
    int j = 0;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            dest[i][j] = src[i][j];
        }
    }

    return;
}

/**
 * Al jugar un turno de usuario
 * 
 * @param *juego Información de la partida
 * @param x Coordenada x del botón presionado
 * @param y Coordenada y del botón presionado
 */
void turnPlayed(JUEGO *juego, int x, int y)
{
    int gameStatus = 0;

    // genera una nueva instancia para el historial a la que se puede regresar
    logMove(juego, 1);

    // coloca la ficha correspondiente
    juego->partida.historial[juego->partida.turno].tablero[x][y] = ICONS[(juego->partida.turno + 1) % 2];

    // Cambia la imagén del botón presionado
    gtk_widget_destroy(juego->graficos.buttonImg[x][y]);
    juego->graficos.buttonImg[x][y] = gtk_image_new_from_pixbuf(juego->graficos.m60[(juego->partida.turno + 1) % 2]);
        gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[x][y]), juego->graficos.buttonImg[x][y]);
        gtk_widget_show(juego->graficos.buttonImg[x][y]);
    
    // modifica el estado de los botones del historial
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
    
    // si el siguiente turno es de la computadora, lo juega
    if(juego->partida.jugadores[juego->partida.turno % 2].ia && !checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2]))
    {
        aiTurn(juego);
    }
    
    // muestra al jugador actual
    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);

    // revisa el estado del tablero
    gameStatus = checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2]);

    // en caso de que la partida haya terminado
    if(gameStatus)
    {
        // cambia el estado de la partida
        juego->partida.historial[juego->partida.turno].game_status = GAME_ENDED;

        // oculta el jugador actual
        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);

        // muestra la ventana de victoria o empate dependiendo del resultado de la partida
        if(gameStatus < 0)
        {
            tie_dialog(juego);
        }
        else
        {
            victory_dialog(juego);
        }
    }

    return;
}

/**
 * Juego de un turno de la ia
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void aiTurn(JUEGO *juego)
{
    int x = 0;
    int y = 0;

    int i = 0;

    // si es el primer turno de la partida
    if(!juego->partida.turno)
    {
        // no se podrá regresar al tablero vacío
        juego->partida.historial[0].hist_val = 0;
    }

    // genera una nueva instancia para el historial a la que no se puede regresar
    logMove(juego, 0);

    // si la ia esta en modo dificil tirará 2 veces, de lo contrario, solo 1 vez
    for(i = 0; i < 1 + juego->partida.jugadores[(juego->partida.turno + 1) % 2].hard_mode; i++)
    {
        // elije de manera inteligente un espacio en el que tirar
        chooseSpace(juego, &x, &y);

        juego->partida.historial[juego->partida.turno].tablero[x][y] = ICONS[(juego->partida.turno + 1) % 2];
    }

    // plasma la elección de forma gráfica
    coppyBoardIntoGraphic(juego);

    return;
}

/**
 * Elección de espacio para tirar de la ia
 * 
 * @param *juego La partida en curso
 * @param *x X en la que colocará la ficha
 * @param *y Y en la que colocará la ficha
 * 
 * @returns void
 */
void chooseSpace(JUEGO *juego, int *x, int *y)
{
    char copy[3][3]; // copia del tablero

    int f_wins[9][2]; // victorias a largo plazo
    int wins = 0; // cuenta de victorias a largo plazo

    int d_spaces[9][2]; // espacios disponibles
    int spaces = 0; // cuenta de espacios disponibles

    int r_place = 0; // lugar aleatorio

    // contadores
    int i = 0;
    int j = 0;

    int i2 = 0;
    int j2 = 0;

    // para cada espacio del tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            // si el lugar está libre
            if(juego->partida.historial[juego->partida.turno].tablero[i][j] == ' ')
            {
                // guarda el espacio disponible
                d_spaces[spaces][0] = i;
                d_spaces[spaces][1] = j;
                spaces ++;

                // crea una copia del tablero actual
                coppyBoard(copy, juego->partida.historial[juego->partida.turno].tablero);

                // coloca una ficha propia en el lugar disponible
                copy[i][j] = ICONS[(juego->partida.turno + 1) % 2];

                // si la ia gana al colocar esa ficha
                if(checkGame(copy, ICONS[(juego->partida.turno + 1) % 2]) == 1)
                {
                    // juega en esa posición
                    *x = i;
                    *y = j;

                    return;
                }
                // de lo contrario (revisión de victoria a largo plazo)
                else
                {
                    // para cada espacio en el tablero
                    for(i2 = 0; i2 < 3; i2++)
                    {
                        for(j2 = 0; j2 < 3; j2++)
                        {
                            // si el espacio está disponible
                            if(copy[i2][j2] == ' ')
                            {
                                // coloca una ficha en el espacio
                                copy[i2][j2] = ICONS[(juego->partida.turno + 1) % 2];

                                // si la ia gana al colocar esa ficha
                                if(checkGame(copy, ICONS[(juego->partida.turno + 1) % 2]) == 1)
                                {
                                    // guarda la posición que le dará una victoria a futuro
                                    f_wins[wins][0] = i2;
                                    f_wins[wins][1] = j2;

                                    wins++;
                                }

                                // quita la ficha que colocó
                                copy[i2][j2] = ' ';
                            }
                        }
                    }
                }
            }
        }
    }

    // para cada espacio en el tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            // si el espacio está disponible
            if(juego->partida.historial[juego->partida.turno].tablero[i][j] == ' ')
            {
                // copia el tablero
                coppyBoard(copy, juego->partida.historial[juego->partida.turno].tablero);
                
                // coloca una ficha del contrincante
                copy[i][j] = ICONS[juego->partida.turno % 2];

                // si el contrincante gana
                if(checkGame(copy, ICONS[juego->partida.turno % 2]) == 1)
                {
                    // bloquea esa posición
                    *x = i;
                    *y = j;

                    return;
                }
            }
        }
    }

    // si hay victorias a futuro
    if(wins)
    {
        // elige una victoria futura al azar y juega en ese lugar
        r_place = rand() % wins;

        *x = f_wins[r_place][0];
        *y = f_wins[r_place][1];
    }
    // de lo contrario
    else
    {
        // juega en un espacio disponible al azar
        r_place = rand() % spaces;

        *x = d_spaces[r_place][0];
        *y = d_spaces[r_place][1];
    }

    return;
}

/**
 * Revisa el estaso de un tablero
 * 
 * @param tablero[3][3] El tablero a revisar
 * @param played El último jugador que tiró
 * 
 * @returns int (-1 si el tablero está lleno y sin victorias, 0 si no hay victorias, 1 si hay victorias)
 */
int checkGame(char tablero[3][3], char played)
{
    int row = 0;
    int column = 0;

    int diagonal = 1;
    int iDiagonal = 1;

    int full = 1;

    int i = 0;
    int j = 0;

    // revisa si hay alguna victoria en fila o columna
    for(i = 0; i < 3 && !row && !column; i++)
    {
        // viable de victoria en fila o columna
        row = 1;
        column = 1;

        // para cada elemento de la fila y de la columna
        for(j = 0; j < 3; j++)
        {
            // si la ficha no es igual a la del jugador en fila
            if(tablero[i][j] != played)
            {
                row = 0;
            }

            // si la ficha no es igual a la del jugador en columna
            if(tablero[j][i] != played)
            {
                column = 0;
            }
        }
    }

    // revisa si el tablero está lleno o si hay una victoria en diagonal
    for(i = 0; i < 3; i++)
    {
        // revisa si está lleno
        for(j = 0; j < 3; j++)
        {
            // si hay un espacio disponible
            if(tablero[i][j] == ' ')
            {
                full = 0;
            }
        }

        // diagonal empezando por 0, 0
        if(tablero[i][i] != played)
        {
            diagonal = 0;
        }

        // diagonal empezando por 0, 2
        if(tablero[i][2 - i] != played)
        {
            iDiagonal = 0;
        }
    }

    // si hay alguna victoria
    if(row + column + diagonal + iDiagonal)
    {
        return 1;
    }
    // si el tablero está lleno
    else if(full)
    {
        return -1;
    }

    return 0;
}

/**
 * Copia el último tablero jugado en la ventana de juego
 * 
 * @param *juego Información de la partida
 */
void coppyBoardIntoGraphic(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    GdkColor color;

    // para todos los botones del tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            gdk_color_parse("#A3A3A3", &color);

            // elimina la imagen actual
            gtk_widget_destroy(juego->graficos.buttonImg[i][j]);
    
            // segun la ficha colocada internamente carga la imagen y el color necesarios
            switch(juego->partida.historial[juego->partida.turno].tablero[i][j])
            {
                case 'X':
                    juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[0]);
                    break;
                case 'O':
                    juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[1]);
                    break;
                default:
                    gdk_color_parse("#DCDAD5", &color);
    
                    juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
                    break;
            }
    
            // plasma los cambios en la celda
            gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[i][j]), juego->graficos.buttonImg[i][j]);
            gtk_widget_modify_bg(juego->graficos.buttons[i][j], GTK_STATE_NORMAL, &color);
            gtk_widget_show(juego->graficos.buttonImg[i][j]);
        }
    }

    return;
}

/**
 * Copia toda la información interna de la partida en la ventana de juego
 * 
 * @param *juego Información de la partida
 */
void coppyIntoGraphic(JUEGO *juego)
{
    int i = 0;

    cleanScreen(juego); // limpia la pantalla

    coppyBoardIntoGraphic(juego); // copia el estado del tablero

    // habilita las opciones exclusivas de partida en curso
    gtk_widget_set_sensitive(juego->graficos.menuEnd, TRUE);
    gtk_widget_set_sensitive(juego->graficos.menuSave, TRUE);

    // muestra los jugadores y sus fichas
    for(i = 0; i < 2; i++)
    {
        gtk_widget_show(juego->graficos.playerImg[i]);
        gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[i]), juego->partida.jugadores[i].nombre);
    }

    // si el juego ya terminó, muestra vaío el cuadro de jugador actual
    if(juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED)
    {
        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);
    }
    else // de lo contrario, muestra el jugador actual
    {
        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);
    }

    // si hay movimientos hacia atrás en el historial, habilita el botón correspondiente
    if(moreTurnsBackwards(juego))
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);
    }

    // si hay movimientos hacia adelante en el historial, habilita el botón correspondiente
    if(moreTurnsForwards(juego))
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[1], TRUE);
    }

    // si se juega en modo difícil
    if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
    {
        displayHardMode(juego);
    }

    return;
}

/**
 * Regresa al turno jugable anterior
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void lastTurn(JUEGO *juego)
{
    int i = juego->partida.turno;

    // si es el primer turno
    if(!juego->partida.turno)
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
    }
    // si el turno anterior es un turno jugable
    else if(juego->partida.historial[i].hist_val)
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[1], TRUE);

        // regresa a ese turno
        juego->partida.turno --;

        // muestra en la ventana el resultado del movimiento
        coppyBoardIntoGraphic(juego);

        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);

        // si ya no hay más movimientos hacia atrás, desactiva el botón correspondiente
        if(!moreTurnsBackwards(juego))
        {
            gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
        }
    }
    // de lo contrario
    else
    {
        // revisa 2 turnos antes del actual
        i -= 2;

        // si no hay turnos disponibles
        if(i < 0)
        {
            gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
        }
        // de lo contrario
        else
        {
            gtk_widget_set_sensitive(juego->graficos.moveButtons[1], TRUE);

            // regresa a ese turno
            juego->partida.turno = i;

            // muestra en la ventana el resultado del movimiento
            coppyBoardIntoGraphic(juego);

            gtk_widget_destroy(juego->graficos.playingImg);
            juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
                gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                gtk_widget_show(juego->graficos.playingImg);

            // si ya no hay más movimientos hacia atrás, desactiva el botón correspondiente
            if(!moreTurnsBackwards(juego))
            {
                gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
            }
        }
    }

    return;
}

/**
 * Determina si hay mas turnos jugables hacia atrás en el historial
 * 
 * @param *juego Información de la partida
 * 
 * @returns int (0 en caso de no haber más turnos, 1 de lo contrario)
 */
int moreTurnsBackwards(JUEGO *juego)
{
    int i = 0;

    // revisa todos los moviientos anteriores
    for(i = 0; i <= juego->partida.turno; i++)
    {
        if(juego->partida.historial[i].hist_val) // si algun turno anterior puede ser jugado, regresa 1
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Avanza al siguiente turno jugable
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void nextTurn(JUEGO *juego)
{
    int i = juego->partida.turno;

    // si es el último turno disponible
    if(juego->partida.turno >= juego->partida.turno_max)
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
    }
    // de lo contrario
    else
    {
        // si alguno de los jugadores es una ia, avanza un turno
        if((juego->partida.jugadores[0].ia || juego->partida.jugadores[1].ia) && juego->partida.historial[i + 1].game_status != GAME_ENDED)
        {
            i++;
        }
        
        // avanza un turno
        i++;

        // si el turno al que se desea ir está fuera de límites
        if(i > juego->partida.turno_max)
        {
            gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
        }
        // de lo contrario, se recorre a ese turno
        else
        {
            gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);

            juego->partida.turno = i;

            // plasma el resultado del movimiento en la ventana
            coppyBoardIntoGraphic(juego);

            // si la partida no ha terminado, muestra al jugador actual
            if(juego->partida.historial[i].game_status != GAME_ENDED)
            {
                gtk_widget_destroy(juego->graficos.playingImg);
                juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
                    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                    gtk_widget_show(juego->graficos.playingImg);
            }
            else // si la partida ya terminó, no muestra al jugador actual
            {                    
                gtk_widget_destroy(juego->graficos.playingImg);
                juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
                    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                    gtk_widget_show(juego->graficos.playingImg);
            }

            // si ya no hay más movimientos hacia adelante, desactiva el botón correspondiente
            if(!moreTurnsForwards(juego))
            {
                gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
            }
        }
    }

    return;
}

/**
 * Determina si hay más movimientos hacia adelante en el historial de la partida
 * 
 * @param *juego Información de la partida
 * 
 * @returns int (0 si no hay más movimientos hacia adelante, 1 de lo contrario)
 */
int moreTurnsForwards(JUEGO *juego)
{
    // si se está en el último turno posible, regresa 0
    if(juego->partida.turno == juego->partida.turno_max)
    {
        return 0;
    }

    return 1;
}

/**
 * Valida que el archivo cargado tenga información lógica de partida
 * 
 * @param gameData La información de partida extraida de un archivo
 * 
 * @returns int (1 si la información es correcta, 0 de lo contrario)
 */
int correctFileFormat(FUNCIONAL gameData)
{
    long int name_len = 0;

    int i = 0;
    int j = 0;
    int k = 0;

    // valida la cantidad de turnos jugados (entre 0 y 9)
    if(gameData.turno_max > 9 || gameData.turno_max < 0 || gameData.turno > 9 || gameData.turno < 0)
    {
        return 0;
    }

    // valida todos los turnos del historial
    for(k = 0; k < gameData.turno_max; k++)
    {
        // verifica valores de historial y estado de partida;
        if((gameData.historial[k].hist_val != 0 && gameData.historial[k].hist_val != 1) || (gameData.historial[k].game_status != GAME_ENDED && gameData.historial[k].game_status != GAME_NOT_STARTED && gameData.historial[k].game_status != GAME_STARTED))
        {
            return 0;
        }
        
        // verifica que el tablero tenga valores correctos
        for(i = 0; i < 3; i++)
        {
            for(j = 0; j < 3; j++)
            {
                if((gameData.historial[k].tablero[i][j] != ' ' && gameData.historial[k].tablero[i][j] != ICONS[0] && gameData.historial[k].tablero[i][j] != ICONS[1]))
                {
                    return 0;
                }
            }
        }
    }

    // verifica que los jugadores tengan valores lógicos
    for(i = 0; i < 2; i++)
    {
        name_len = strlen(gameData.jugadores[i].nombre);

        if((gameData.jugadores[i].hard_mode != 0 && gameData.jugadores[i].hard_mode != 1) || (gameData.jugadores[i].ia != 0 && gameData.jugadores[i].ia != 1) || (gameData.jugadores[i].hard_mode == 1 && !gameData.jugadores[i].ia) || name_len > 20 || !name_len)
        {
            return 0;
        }
    }

    return 1;
}