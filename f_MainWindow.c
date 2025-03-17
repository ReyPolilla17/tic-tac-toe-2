#include "gato.h"

void gameStartup(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    juego->partida.turno = 0;
    juego->partida.turno_max = 0;

    juego->partida.historial[0].game_status = GAME_NOT_STARTED;
    juego->partida.historial[0].hist_val = 0;

    for(i = 0; i < 2; i++)
    {
        juego->partida.jugadores[i].hard_mode = 0;
        juego->partida.jugadores[i].ia = 0;
        juego->partida.jugadores[i].nombre[0] = 0;
    }

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            juego->partida.historial[0].tablero[i][j] = ' ';
        }
    }

    return;
}

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

    if(res != GTK_RESPONSE_DELETE_EVENT)
    {
        new_game_modal_new(&info);

        if(new_game_modal_get_info(&info))
        {
            rn = rand() % 2;
            
            cleanScreen(juego);
            gameStartup(juego);

            if(info.hard_mode)
            {
                displayHardMode(juego);
            }

            gtk_widget_set_sensitive(juego->graficos.menuEnd, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuSave, TRUE);

            gtk_widget_show(juego->graficos.playerImg[0]);
            gtk_widget_show(juego->graficos.playerImg[1]);

            juego->partida.jugadores[rn].hard_mode = info.hard_mode;
            juego->partida.jugadores[rn].ia = info.vs_ia;

            if(info.vs_ia)
            {
                strcpy(juego->partida.jugadores[rn].nombre, IA_NAME);
            }
            else
            {
                strcpy(juego->partida.jugadores[rn].nombre, info.names[1]);
            }
            
            juego->partida.jugadores[(rn + 1) % 2].hard_mode = 0;
            juego->partida.jugadores[(rn + 1) % 2].ia = 0;
            
            strcpy(juego->partida.jugadores[(rn + 1) % 2].nombre, info.names[0]);

            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[0]), juego->partida.jugadores[0].nombre);
            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[1]), juego->partida.jugadores[1].nombre);
            
            juego->partida.historial[0].game_status = GAME_STARTED;

            if(juego->partida.jugadores[0].ia)
            {
                np = 1;
                aiTurn(juego);
            }

            gtk_widget_destroy(juego->graficos.playingImg);
            juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[np]);
                gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                gtk_widget_show(juego->graficos.playingImg);
        }
    }
    
    return;
}

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

void loadGame(JUEGO *juego)
{
    FILE *fp;

    GtkWidget *dialog;

    int success = 0;
    int v = 0;

    gint res = 0;
    gchar *file;

    // muestra la ventana de guardado
    dialog = gtk_file_chooser_dialog_new("Cargar Partida", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

    do
    {
        v = 1;

        res = gtk_dialog_run(GTK_DIALOG(dialog));

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
                // intenta leer toda la información de la partida de golpe
                success = fread(&juego->partida, sizeof(FUNCIONAL), 1, fp);

                fclose(fp);
                
                // si pudo leer la información
                if(success)
                {
                    coppyIntoGraphic(juego);
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
    } while(!success && !v);

    gtk_widget_destroy(dialog);

    return;
}

void cleanScreen(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    GdkColor color;

    gdk_color_parse("#DCDAD5", &color);

    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    gtk_widget_hide(juego->graficos.menuTruth);

    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);
        
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

    if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
    {
        system("killall aplay");
    }

    return;
}

void displayHardMode(JUEGO *juego)
{
    int i = 0;

    GdkColor color;

    gdk_color_parse("#790000", &color);

    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    gtk_widget_show(juego->graficos.menuTruth);

    // muestra las llamas
    for(i = 0; i < 2; i++)
    {
        gtk_widget_show(juego->graficos.flames[i]);
    }

    system("aplay ./MEDIA/hercules.wav &");

    return;
}

int getButton(JUEGO *juego, GtkWidget *button, int coords[2])
{
    int i = 0;
    int j = 0;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            if(button == juego->graficos.buttons[i][j])
            {
                coords[0] = i;
                coords[1] = j;

                return 1;
            }
        }
    }

    return 0;
}

void turnPlayed(JUEGO *juego, int x, int y)
{
    int gameStatus = 0;

    // genera una nueva instancia para el historial a la que se puede regresar
    logMove(juego, 1);

    // coloca la ficha correspondiente
    juego->partida.historial[juego->partida.turno].tablero[x][y] = ICONS[(juego->partida.turno + 1) % 2];

    gtk_widget_destroy(juego->graficos.buttonImg[x][y]);
    juego->graficos.buttonImg[x][y] = gtk_image_new_from_pixbuf(juego->graficos.m60[(juego->partida.turno + 1) % 2]);
        gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[x][y]), juego->graficos.buttonImg[x][y]);
        gtk_widget_show(juego->graficos.buttonImg[x][y]);
    
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
    
    if(juego->partida.jugadores[juego->partida.turno % 2].ia && !checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2]))
    {
        aiTurn(juego);
    }
    
    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);

    gameStatus = checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2]);

    if(gameStatus)
    {
        juego->partida.historial[juego->partida.turno].game_status = GAME_ENDED;

        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);

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

void coppyBoardIntoGraphic(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    GdkColor color;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            gdk_color_parse("#A3A3A3", &color);
            gtk_widget_destroy(juego->graficos.buttonImg[i][j]);
    
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
    
            gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[i][j]), juego->graficos.buttonImg[i][j]);
            gtk_widget_modify_bg(juego->graficos.buttons[i][j], GTK_STATE_NORMAL, &color);
            gtk_widget_show(juego->graficos.buttonImg[i][j]);
        }
    }

    return;
}

void coppyIntoGraphic(JUEGO *juego)
{
    int i = 0;

    cleanScreen(juego);

    coppyBoardIntoGraphic(juego);

    gtk_widget_set_sensitive(juego->graficos.menuEnd, TRUE);
    gtk_widget_set_sensitive(juego->graficos.menuSave, TRUE);

    for(i = 0; i < 2; i++)
    {
        gtk_widget_show(juego->graficos.playerImg[i]);
        gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[i]), juego->partida.jugadores[i].nombre);
    }

    if(juego->partida.historial[juego->partida.turno].game_status != GAME_ENDED)
    {
        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);
    }
    else
    {
        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);
    }

    if(moreTurnsBackwards(juego))
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);
    }

    if(moreTurnsForwards(juego))
    {
        gtk_widget_set_sensitive(juego->graficos.moveButtons[1], TRUE);
    }

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

        coppyBoardIntoGraphic(juego);

        gtk_widget_destroy(juego->graficos.playingImg);
        juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
            gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
            gtk_widget_show(juego->graficos.playingImg);

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

            coppyBoardIntoGraphic(juego);

            gtk_widget_destroy(juego->graficos.playingImg);
            juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
                gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                gtk_widget_show(juego->graficos.playingImg);

            if(!moreTurnsBackwards(juego))
            {
                gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
            }
        }
    }

    return;
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

            coppyBoardIntoGraphic(juego);

            if(juego->partida.historial[i].game_status != GAME_ENDED)
            {
                gtk_widget_destroy(juego->graficos.playingImg);
                juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
                    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                    gtk_widget_show(juego->graficos.playingImg);
            }
            else
            {                    
                gtk_widget_destroy(juego->graficos.playingImg);
                juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
                    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                    gtk_widget_show(juego->graficos.playingImg);
            }


            if(!moreTurnsForwards(juego))
            {
                gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);
            }
        }
    }

    return;
}

int moreTurnsBackwards(JUEGO *juego)
{
    int i = 0;

    for(i = 0; i <= juego->partida.turno; i++)
    {
        if(juego->partida.historial[i].hist_val)
        {
            return 1;
        }
    }

    return 0;
}

int moreTurnsForwards(JUEGO *juego)
{
    if(juego->partida.turno == juego->partida.turno_max)
    {
        return 0;
    }

    return 1;
}