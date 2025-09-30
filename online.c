/**
 * @file online.c
 * 
 * @brief Contiene todas las funciones para ejecutar una partida en modo en línea
 * 
 * @author Luis Julián Zamora Treviño
 * @date 30/09/2025
 */
#include "gato.h"

/**
 * Ejecuta un query a la base de datos y guarda el resultado si así se desea
 * 
 * @param *mysql Información de la conexión a la base de datos
 * @param query[] El query a ejecutar
 * @param **res El resultado del query si se desea guardar, enviar NULL de lo contrario
 * 
 * @returns void
 */
void query(MYSQL *mysql, char query[], MYSQL_RES **res)
{
    // Ejecuta el query y de haber un error lo imprime y cierra el programa (no fue mi diseño pero me dio flojera cambiarlo)
    if(mysql_query(mysql, query))
    {
        printf("Error processing query \"%s\" Error: %s\n", query, mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    // Si se desea guardar el resultado y hubo un error al hacerlo, imprime el error y cierra el programa
    if(res != NULL && !(*res = mysql_store_result(mysql)))
    {
        printf("Error storing results Error: %s\n", mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    return;
}

/**
 * Conexión a la base de datos
 * 
 * @param *juego La información de juego en la que guardará la conexión a MySql
 * 
 * @returns void
 */
void connectToDatabase(JUEGO *juego)
{
    // Inicializa la parte en línea
    juego->online.g_id = 0;
    juego->online.u_id[0] = -1;
    juego->online.u_id[1] = -1;
    juego->online.connected = TRUE;
    juego->online.playing = FALSE;
    juego->online.dialog = NULL;

    mysql_init(&juego->online.mysql); // Prepara la conexion al servidor de bases de datos

    // Se conecta al servidor de base de datos y verifica que no haya error
    // Si hay un error, deshabilita la opción de jugar en línea
    if(!mysql_real_connect(&juego->online.mysql, "localhost", "ict23jzt", "234126", "ict23jzt", 0, NULL, 0))
    {
        gtk_widget_hide(juego->graficos.menuOnline);
        juego->online.connected = FALSE;
    }
    else if(mysql_select_db(&juego->online.mysql, "ict23jzt"))
    {
        gtk_widget_hide(juego->graficos.menuOnline);
        juego->online.connected = FALSE;
    }

    return;
}

/**
 * Cierra la conexión a la base de datos para el final del programa
 * 
 * @param *juego La información interna del programa
 * 
 * @returns void
 */
void closeConnectionToDatabase(JUEGO *juego)
{
    char buffer[1000]; // Cadena de caracteres en la que se escriben los queries

    // si se registró un jugador
    if(juego->online.u_id[0] != -1)
    {
        // Si se estaba jugando una partida, la termina
        if(juego->online.playing)
        {
            forfeit(juego);
        }
        
        // Elimina al usuario de la base de datos
        sprintf(buffer, "DELETE FROM ttt_Usuarios WHERE id_usuario = %ld", juego->online.u_id[0]);
        query(&juego->online.mysql, buffer, NULL);
    }

    // cierra la conexion con al servidor
    mysql_close(&juego->online.mysql);

    return;
}

/**
 * Registra al usuario en la base de datos o cambia su nombre
 * 
 * @param *juego La información del programa
 * 
 * @returns void
 */
void registerUser(JUEGO *juego)
{
    OnlineRegisterModal info;

    // Elementos con los que interactuar para los queries
    char buffer[1000];
    MYSQL_RES *res;
    MYSQL_ROW row;

	// Si hay una partida en curso
	if(juego->partida.historial[juego->partida.turno].game_status != GAME_NOT_STARTED)
	{
        // Desactiva la opción de cambiar nombre
        gtk_widget_set_sensitive(juego->graficos.menuName, FALSE);

        return;
	}
    
    online_register_modal_new(&info); // ventana de registro de usuario

    // extrae los datos del usuario
    if(online_register_modal_get_info(&info))
    {
        // habilita Buscar partida
        gtk_widget_set_sensitive(juego->graficos.menuSeek, TRUE);

        // Copia el nombre introducido y lo muestra en pantalla
        strcpy(juego->online.name[0], info.name);

        sprintf(buffer, "Online (%s)", info.name);
        gtk_menu_item_set_label(GTK_MENU_ITEM(juego->graficos.menuOnline), buffer);

        // Si el usuario no está registrado
        if(juego->online.u_id[0] == -1)
        {
            // Guarda el usuario y obtiene su ID
            sprintf(buffer, "INSERT INTO ttt_Usuarios (nombre) VALUES (\"%s\")", info.name);
            query(&juego->online.mysql, buffer, NULL);

            sprintf(buffer, "SELECT LAST_INSERT_ID() my_id");
            query(&juego->online.mysql, buffer, &res);

            // Guarda el ID del jugador registrado
            row = mysql_fetch_row(res);
            if(row[0] != NULL)
            {
                sscanf(row[0], "%ld", &juego->online.u_id[0]);
            }
            
            mysql_free_result(res);
        }
        else
        {
            // Si el usuario ya estaba registrado, actualiza el registro en la base de datos
            sprintf(buffer, "UPDATE ttt_Usuarios SET nombre = \"%s\" WHERE id_usuario = %ld", info.name, juego->online.u_id[0]);
            query(&juego->online.mysql, buffer, NULL);
        }
    }

    return;
}

/**
 * Busca a un oponente para jugar en línea
 * 
 * @param *juego la información del programa
 * 
 * @returns void
 */
void seekMatch(JUEGO *juego)
{
    char buffer[1000]; // Cadena de caracteres en la que se escriben los queries

    // Introduce al usuario en la búsqueda de oponentes
    sprintf(buffer, "INSERT INTO ttt_Buscando VALUES (%ld)", juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, NULL);

    // Abre una ventana de espera que buscará cada 400 ms a un oponente
    online_connection_dialog_new(&juego->online.dialog);
    g_timeout_add(400, (GSourceFunc)seekMatchLoop, (gpointer)juego);

    juego->online.dialog->res = gtk_dialog_run(GTK_DIALOG(juego->online.dialog->dialog));

    // Si se canceló la opción de buscar oponente
    if(juego->online.dialog != NULL)
    {
        // Elimina al usuario de la búsquda de oponentes
        sprintf(buffer, "DELETE FROM ttt_Buscando WHERE id_usuario = %ld", juego->online.u_id[0]);
        query(&juego->online.mysql, buffer, NULL);

        online_connection_dialog_destroy(&juego->online.dialog);
    }
    else // De lo contrario, juega en línea
    {
        playOnline(juego);
    }

    return;
}

/**
 * Loop de búsqueda de oponente
 * 
 * @param data Información del programa como gpointer
 * 
 * @returns gboolean (TRUE, si debe ejecutar de nuevo el loop, FALSE de lo contrario)
 */
gboolean seekMatchLoop(gpointer data)
{
    JUEGO *juego = (JUEGO *)data;

    // Elementos con los que interactuar para el primer query
    char buffer[1000];
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    // Elementos con los que interactuar para el primer query
    char buffer2[1000];
    MYSQL_RES *res2;
    MYSQL_ROW row2;

    long int u_ids[2]; // IDs de los jugadores

    int r = rand() % 2; // Orden aleatorio de turnos
    int m = 0; // Si el usuario fue encontrado por un oponente
    int c = 0; // Conteo de partidas repetidas
    
    int i = 0; // Contador

    // Si se canceló la búsqueda de partida, detiene el loop
    if(juego->online.dialog == NULL)
    {
        return FALSE;
    }

    // Revisa que el usuario siga en la lista de espera y si hay usuarios en la lista
    sprintf(buffer, "SELECT * FROM ttt_Buscando WHERE id_usuario != %ld LIMIT 1", juego->online.u_id[0]);
    sprintf(buffer2, "SELECT * FROM ttt_Buscando WHERE id_usuario = %ld LIMIT 1", juego->online.u_id[0]);

    // Busca un adversario en la lista de espera
    query(&juego->online.mysql, buffer, &res);

    // Si no encuentra ningun adversario
    if(!(row = mysql_fetch_row(res)))
    {
        // libera el resultado
        mysql_free_result(res);

        // revisa que el usuario siga en la lista de espera
        query(&juego->online.mysql, buffer2, &res2);

        // Si el usuario fue encontrado por un adversario, activa la bandera
        if(!(row2 = mysql_fetch_row(res2)))
        {
            m = 1;
        }
        
        // libera el resultado
        mysql_free_result(res2);
    }

    // Si no hay ususarios en busca de partida y el usuario sigue en la lista de espera
    if(!row && row2)
    {
        // Vuelve a llamar el loop y avanza la línea de espera
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(juego->online.dialog->progress));

        return TRUE;
    }

    // Si se encontró un adversario

    // Elimina la pantalla de espera
    online_connection_dialog_destroy(&juego->online.dialog);

    // Si el usuario encontró a un adversario
    if(!m)
    {
        // guarda su id
        sscanf(row[0], "%ld", &juego->online.u_id[1]);
    
        // acomoda los IDs de los jugadores de forma aleatoria
        u_ids[r] = juego->online.u_id[0];
        u_ids[(r + 1) % 2] = juego->online.u_id[1];
    
        // Elimina a ambos usuarios de la lista de búsqueda
        for(i = 0; i < 2; i++)
        {
            sprintf(buffer, "DELETE FROM ttt_Buscando WHERE id_usuario = %ld", u_ids[i]);
            query(&juego->online.mysql, buffer, NULL);
        }
    
        // Guarda a los ususarios en una partida nueva
        sprintf(buffer, "INSERT INTO ttt_Partida (p_status, id_usuario_1, id_usuario_2) VALUES (%d, %ld, %ld)", GAME_STARTED, u_ids[0], u_ids[1]);
        query(&juego->online.mysql, buffer, NULL);
        
        mysql_free_result(res);
    }

    /**
     * Elimina registros repetidos
     */
    
    // Busca otras partidas con las mismas IDs
    sprintf(buffer, "SELECT COUNT(*) - 1 FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld", juego->online.u_id[0], juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%d", &c);
    mysql_free_result(res);

    // Si hay más de un registro en la lista de partida con el id del usuario, los elimina
    if(c > 0)
    {
        sprintf(buffer, "DELETE FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld LIMIT %d", juego->online.u_id[0], juego->online.u_id[0], c - 1);
        query(&juego->online.mysql, buffer, NULL);
    }

    // Si el usuario fue encontrado por un adversario
    if(m)
    {
        // guarda el id del adversario
        sprintf(buffer, "SELECT id_usuario_1, id_usuario_2 FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld", juego->online.u_id[0], juego->online.u_id[0]);
        query(&juego->online.mysql, buffer, &res);

        row = mysql_fetch_row(res);
        sscanf(row[0], "%ld", &juego->online.u_id[1]);
        
        if(juego->online.u_id[0] == juego->online.u_id[1])
        {
            sscanf(row[1], "%ld", &juego->online.u_id[1]);
        }

        mysql_free_result(res);
    }

    // incialización de partida

    // guarda la id de la partida
    sprintf(buffer, "SELECT id_partida FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld LIMIT 1", juego->online.u_id[0], juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%ld", &juego->online.g_id);
    mysql_free_result(res);

    // guarda el nombre del adversario
    sprintf(buffer, "SELECT nombre FROM ttt_Usuarios WHERE id_usuario = %ld LIMIT 1", juego->online.u_id[1]);
    query(&juego->online.mysql, buffer, &res);
    
    row = mysql_fetch_row(res);
    sscanf(row[0], "%s", juego->online.name[1]);
    mysql_free_result(res);

    return FALSE;
}

/**
 * Ejecución de la partida en línea
 * 
 * @param *juego Información del programa
 * 
 * @returns void
 */
void playOnline(JUEGO *juego)
{
    // Elementos con los que interactuar para los queries
    char buffer[1000];
    MYSQL_RES *res;
    MYSQL_ROW row;

    long int u1_id = 0; // Id del primer usuario que va a jugar

    // indica que hay una partida en línea ocurriendo
    juego->online.playing = TRUE;

    // limpia la ventana e inicializa internamente
    cleanScreen(juego);
    gameStartup(juego);

    // Obtiene el id del primer jugador
    sprintf(buffer, "SELECT id_usuario_1 FROM ttt_Partida WHERE id_partida = %ld", juego->online.g_id);
    query(&juego->online.mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%ld", &u1_id);
    mysql_free_result(res);

    // Si el usuario es el primer jugador
    if(u1_id == juego->online.u_id[0])
    {
        // guarda los nombres de la manera que debe ser y espera a que el jugador tire
        sprintf(juego->partida.jugadores[0].nombre, "%s (tu)", juego->online.name[0]);
        juego->partida.jugadores[0].online_id = juego->online.u_id[0];
        
        strcpy(juego->partida.jugadores[1].nombre, juego->online.name[1]);
        juego->partida.jugadores[1].online_id = juego->online.u_id[1];
    }
    else
    {
        // Guarda los nombres de la manera que debe ser y espera a que el adversario tire
        strcpy(juego->partida.jugadores[0].nombre, juego->online.name[1]);
        juego->partida.jugadores[0].online_id = juego->online.u_id[1];
        
        sprintf(juego->partida.jugadores[1].nombre, "%s (tu)", juego->online.name[0]);
        juego->partida.jugadores[1].online_id = juego->online.u_id[0];

        g_timeout_add(400, (GSourceFunc)onlineGameLoop, juego); // Loop de espera de adversario
    }

    // Desactiva las opciones de juego normal, cambiar nombre y buscar partida; activa rendirse
    gtk_widget_set_sensitive(juego->graficos.menuName, FALSE);
    gtk_widget_set_sensitive(juego->graficos.menuSeek, FALSE);
    gtk_widget_set_sensitive(juego->graficos.menuForfeit, TRUE);

    gtk_widget_set_sensitive(juego->graficos.menuFile, FALSE);
    gtk_widget_set_sensitive(juego->graficos.menuGame, FALSE);

    // Muestra los nombres de los jugadores y sus fichas
    gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[0]), juego->partida.jugadores[0].nombre);
    gtk_widget_show(juego->graficos.playerImg[0]);

    gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[1]), juego->partida.jugadores[1].nombre);
    gtk_widget_show(juego->graficos.playerImg[1]);
    
    // Establece el status como listo para jugar
    juego->partida.historial[0].game_status = GAME_STARTED;

    // muesta el jugador del turno actual
    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[0]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);

    return;
}

/**
 * Espera para el turno del adversario
 * 
 * @param data Información del programa como gpointer
 * 
 * @returns gboolean (TRUE si debe seguir esperando, FALSE, si es turno del jugador)
 */
gboolean onlineGameLoop(gpointer data)
{
    JUEGO *juego = (JUEGO *)data;

    // Elementos con los que interactuar para los queries
    char buffer[1000];
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    // Información de la partida
    char board[4];
    int gameStatus = 0;
    int turn = 0;

    // Contadores
    int i = 0;
    int j = 0;

    // Si por alguna razón ya no se está jugando, termina el loop
    if(!juego->online.playing)
    {
        gtk_widget_hide(juego->graficos.waitingOpponentBar);
        return FALSE;
    }

    // Obtiene la información necesaria de la partida
    sprintf(buffer, "SELECT fila_1, fila_2, fila_3, p_status, turn FROM ttt_Partida WHERE id_partida = %ld", juego->online.g_id);
    query(&juego->online.mysql, buffer, &res);
    
    row = mysql_fetch_row(res);

    // Si no encuentra la partida
    if(!row)
    {
        // Reinicia el estado del programa, muestra la ventana de rendición y detiene el loop
        gameStartup(juego);

        juego->online.playing = FALSE;
        juego->online.g_id = 0;
        juego->online.u_id[1] = -1;
        juego->online.name[1][0] = 0;
        
        gtk_widget_set_sensitive(juego->graficos.menuName, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuSeek, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuForfeit, FALSE);
        
        gtk_widget_set_sensitive(juego->graficos.menuFile, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuGame, TRUE);
        
        gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
        gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
        gtk_widget_hide(juego->graficos.waitingOpponentBar);
        
        forfeit_dialog(juego);
        
        return FALSE;
    }

    // Obtiene el turno en el que va
    sscanf(row[4], "%d", &turn);

    // Si el turno es uno más adelante del turno que el programa tenía
    if(turn > juego->partida.turno)
    {
        // Cambia al número de turno actual
        juego->partida.turno = turn;

        // Guarda el estado del juego y lo considera como un momento válido del historial (para evitar comportamientos extraños)
        sscanf(row[3], "%d", &juego->partida.historial[juego->partida.turno].game_status);
        juego->partida.historial->hist_val = 1;

        // para cada línea del tablero
        for(i = 0; i < 3; i++)
        {
            // escanea la línea del tablero
            sscanf(row[i], "%s", board);

            // cambia ciertos caracteres por espacios vacíos (debido a limitantes de sql)
            for(j = 0; j < 3; j++)
            {
                if(board[j] == 'n')
                {
                    juego->partida.historial[juego->partida.turno].tablero[i][j] = ' ';
                }
                else
                {
                    juego->partida.historial[juego->partida.turno].tablero[i][j] = board[j];
                }
            }
        }

        mysql_free_result(res);

        // Muestra el nuevo tablero en lo gráfico
        coppyIntoGraphic(juego);

        // revisa el estado de la partida (victoria, empate, nada)
        gameStatus = checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2], juego->partida.winboard);

        // si la partida ha acabado de alguna manera
        if(gameStatus)
        {
            // Habilita/deshabilita los campos necesarios
            gtk_widget_set_sensitive(juego->graficos.menuName, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuSeek, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuForfeit, FALSE);
            
            gtk_widget_set_sensitive(juego->graficos.menuFile, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuGame, TRUE);
            
            gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
            gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
            
            // según el estado en el que terminó la partida
            switch(gameStatus)
            {
                case -1: // Empate
                    tie_dialog(juego);
                    break;
                case 1: // Victoria 
                    victory_dialog(juego);
                    break;
            }

            // Limpia la pantalla e inicializa los parametros de la partida
            cleanScreen(juego);
            gameStartup(juego);

            // Reestablece la infromación de partidas en línea
            juego->online.playing = FALSE;
            juego->online.g_id = 0;
            juego->online.u_id[1] = -1;
            juego->online.name[1][0] = 0;
        }
    }
    else // Si sigue en el mismo turno, sigue esperando al adversario
    {
        mysql_free_result(res);
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(juego->graficos.waitingOpponentBar));

        return TRUE;
    }

    // detiene el loop y oculta la barra de espera
    gtk_widget_hide(juego->graficos.waitingOpponentBar);
    return FALSE;
}

/**
 * Al jugar un turno en una partida en línea
 * 
 * @param *juego Información del programa
 * @param x Coordenada x del tiro
 * @param y Coordenada y del tiro
 * 
 * @returns void
 */
void onlineTurnPlayed(JUEGO *juego, int x, int y)
{
    char buffer[1000]; // Cadena de caracteres en la que se escriben los queries

    char row[4]; // Línea que se modifica

    int gameStatus = 0; // Estado de la partida
    
    int i = 0; // Contador

    // genera una nueva instancia para el historial a la que se puede regresar (No aplica en el online, pero evita comportamientos raros)
    logMove(juego, 1);

    // coloca la ficha correspondiente
    juego->partida.historial[juego->partida.turno].tablero[x][y] = ICONS[(juego->partida.turno + 1) % 2];

    // Cambia la imagén del botón presionado
    gtk_widget_destroy(juego->graficos.buttonImg[x][y]);
    juego->graficos.buttonImg[x][y] = gtk_image_new_from_pixbuf(juego->graficos.m60[(juego->partida.turno + 1) % 2]);
        gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[x][y]), juego->graficos.buttonImg[x][y]);
        gtk_widget_show(juego->graficos.buttonImg[x][y]);
    
    // muestra al jugador actual
    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[juego->partida.turno % 2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);
    
    // Muestra la barra de espera
    gtk_widget_show(juego->graficos.waitingOpponentBar);

    // revisa el estado del tablero
    gameStatus = checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[(juego->partida.turno + 1) % 2], juego->partida.winboard);

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
        
        // Habilita/deshabilita los campos correspondientes
        gtk_widget_set_sensitive(juego->graficos.menuName, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuSeek, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuForfeit, FALSE);
        
        gtk_widget_set_sensitive(juego->graficos.menuFile, TRUE);
        gtk_widget_set_sensitive(juego->graficos.menuGame, TRUE);
        
        gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
        gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);
    }

    // Guarda la fila de una manera que MySql pueda almacenar
    for(i = 0; i < 3; i++)
    {
        if(juego->partida.historial[juego->partida.turno].tablero[x][i] == ' ')
        {
            row[i] = 'n';
        }
        else
        {
            row[i] = juego->partida.historial[juego->partida.turno].tablero[x][i];
        }
    }

    row[3] = 0;

    // Actualiza la base de datos
    sprintf(buffer, "UPDATE ttt_Partida SET fila_%d = '%s', turn = %d, p_status = %d WHERE id_partida = %ld", x + 1, row, juego->partida.turno, juego->partida.historial[juego->partida.turno].game_status, juego->online.g_id);
    query(&juego->online.mysql, buffer, NULL);

    // Si la partida terminó
    if(gameStatus)
    {
        // según el estado en el que terminó la partida
        switch(gameStatus)
        {
            case -1: // Empate
                tie_dialog(juego);
                break;
            case 1: // Victoria 
                g_timeout_add(400, (GSourceFunc)winningPulse, juego);
                victory_dialog(juego);
                break;
        }

        // Limpia la pantalla e inicializa los parametros de la partida
        cleanScreen(juego);
        gameStartup(juego);

        // Reestablece la infromación de partidas en línea
        juego->online.playing = FALSE;
        juego->online.g_id = 0;
        juego->online.u_id[1] = -1;
        juego->online.name[1][0] = 0;
    }
    else
    {
        // Si no ha terminado, espera al turno siguiente
        g_timeout_add(400, (GSourceFunc)onlineGameLoop, juego);
    }

    return;
}

/**
 * Rendirse
 * 
 * @param *juego Información del programa
 * 
 * @returns void
 */
void forfeit(JUEGO *juego)
{
    char buffer[1000]; // Cadena de caracteres en la que se escriben los queries

    // Elimina la partida de la base de datos
    sprintf(buffer, "DELETE FROM ttt_Partida WHERE id_partida = %ld", juego->online.g_id);
    query(&juego->online.mysql, buffer, NULL);

    // reestablece valores de partida en línea
    juego->online.playing = FALSE;
    juego->online.g_id = 0;
    juego->online.u_id[1] = -1;
    juego->online.name[1][0] = 0;

    // limpia pantalla e inicializa internamente
    cleanScreen(juego);
    gameStartup(juego);

    // Habilita/deshabilita los campos correspondientes
    gtk_widget_set_sensitive(juego->graficos.menuName, TRUE);
    gtk_widget_set_sensitive(juego->graficos.menuSeek, TRUE);
    gtk_widget_set_sensitive(juego->graficos.menuForfeit, FALSE);

    gtk_widget_set_sensitive(juego->graficos.menuFile, TRUE);
    gtk_widget_set_sensitive(juego->graficos.menuGame, TRUE);

    gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);
	gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);

    return;
}