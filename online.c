#include "gato.h"

void connectToDatabase(JUEGO *juego)
{
    juego->online.g_id = 0;
    juego->online.u_id[0] = -1;
    juego->online.u_id[1] = -1;
    juego->online.connected = TRUE;
    juego->online.dialog = NULL;

    mysql_init(&juego->online.mysql); // Prepara la conexion al servidor de bases de datos

    // Se conecta al servidor de base de datos y verifica que no haya error
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

void closeConnectionToDatabase(JUEGO *juego)
{
    char buffer[1000];

    // rendirse

    // si se registró un jugador
    if(juego->online.u_id[0] != -1)
    {
        sprintf(buffer, "DELETE FROM ttt_Usuarios WHERE id_usuario = %ld", juego->online.u_id[0]);
        query(&juego->online.mysql, buffer, NULL);
    }

    // cierra la conexion con al servidor
    mysql_close(&juego->online.mysql);

    return;
}

void seekMatch(JUEGO *juego)
{
    char buffer[1000];

    sprintf(buffer, "INSERT INTO ttt_Buscando VALUES (%ld)", juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, NULL);

    online_connection_dialog_new(&juego->online.dialog);
    g_timeout_add(400, (GSourceFunc)seekMatchLoop, (gpointer)juego);

    online_connection_dialog_run(&juego->online.dialog);

    if(juego->online.dialog != NULL)
    {
        sprintf(buffer, "DELETE FROM ttt_Buscando WHERE id_usuario = %ld", juego->online.u_id[0]);
        query(&juego->online.mysql, buffer, NULL);

        online_connection_dialog_destroy(&juego->online.dialog);
    }
    else
    {
        playOnline(juego);
    }

    return;
}

gboolean seekMatchLoop(gpointer data)
{
    JUEGO *juego = (JUEGO *)data;

    MYSQL_RES *res;
    MYSQL_RES *res2;
    MYSQL_ROW row;
    MYSQL_ROW row2;

    char buffer[1000];
    char buffer2[1000];

    long int u_ids[2];

    int r = rand() % 2;
    int i = 0;
    int m = 0;
    int c = 0;

    // Si se canceló la búsqueda de partida, detiene el loop
    if(juego->online.dialog == NULL)
    {
        return FALSE;
    }

    // Búsqueda de usuarios con y sin la id del jugador
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
        online_connection_dialog_pulse(juego->online.dialog);

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

void playOnline(JUEGO *juego)
{
    g_print("%s\n", juego->online.name[0]);
    g_print("%s\n", juego->online.name[1]);
    // char buffer[1000];
    // char adv[21];
    // char board[3][4];

    // MYSQL_RES *res;
    // MYSQL_ROW row;

    // int p = 0;
    // int lp = 0;
    // int p1 = 0;
    // int i = 0;
    // int j = 0;

    // char dots[4];
    // dots[0] = 0;

    // sprintf(buffer, "SELECT fila_1, fila_2, fila_3, p_status, last_player, id_usuario_1 FROM ttt_Partida WHERE id_partida = %ld", juego->online.g_id);

    // while(p != GAME_TIE && p != GAME_WON)
    // {
    //     system("clear");

    //     query(&juego->online.mysql, buffer, &res);

    //     row = mysql_fetch_row(res);

    //     for(i = 0; i < 3; i++)
    //     {
    //         sscanf(row[i], "%s", board[i]);
    //     }

    //     sscanf(row[3], "%d", &p);
    //     sscanf(row[4], "%d", &lp);
    //     sscanf(row[5], "%d", &p1);

    //     mysql_free_result(res);

    //     if(p > 0)
    //     {
    //         if((!lp && p1 == juego->online.u_id[0]) || lp && p1 != juego->online.u_id[0])
    //         {
    //             playTurn(data, adv, board, lp);
    //         }
    //         else
    //         {
    //             printBoard(board, juego->online.name, adv);
    //             printf("Esperando a adversario%s\n", dots);
    
    //             if(strlen(dots) < 3)
    //             {
    //                 strcat(dots, ".");
    //             }
    //             else
    //             {
    //                 dots[0] = 0;
    //             }
    
    //             sleep(1);
    //         }
    //     }
    //     else
    //     {
    //         printBoard(board, juego->online.name, adv);
    //         printf("El juego ha temrinado!\n");
    //     }
    // }

    return;
}

void forfeit(JUEGO *juego)
{
    return;
}

void registerUser(JUEGO *juego)
{
    OnlineRegisterModal info;

    MYSQL_RES *res;
    MYSQL_ROW row;

    char buffer[1000];

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

void query(MYSQL *mysql, char query[], MYSQL_RES **res)
{
    if(mysql_query(mysql, query))
    {
        printf("Error processing query \"%s\" Error: %s\n", query, mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    if(res != NULL && !(*res = mysql_store_result(mysql)))
    {
        printf("Error storing results Error: %s\n", mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    return;
}