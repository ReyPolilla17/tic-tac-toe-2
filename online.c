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
}

void seekMatch(JUEGO *juego)
{
    printf("A");
    // char buffer[1000];

    // sprintf(buffer, "INSERT INTO ttt_Buscando VALUES (%ld)", juego->online.u_id[0]);
    // query(&juego->online.mysql, buffer, NULL);

    // g_timeout_add_full(G_PRIORITY_DEFAULT, 400, (GSourceFunc)seekMatchLoop, (gpointer)juego, (GDestroyNotify)playOnline);
    // online_connection_dialog_new(&juego->online.dialog);
    
    // juegaGato(data);

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

        // obtiene una fila del segundo resultado
        if(!(row2 = mysql_fetch_row(res2)))
        {
            m = 1;
        }
        
        mysql_free_result(res2);
        
        if(!m)
        {
            sleep(1);
        }
    }

    if(!row && row2)
    {
        online_connection_dialog_pulse(juego->online.dialog);

        return TRUE;
    }

    online_connection_dialog_destroy(&juego->online.dialog);

    if(!m)
    {
        sscanf(row[0], "%ld", &juego->online.u_id[1]);
    
        u_ids[r] = juego->online.u_id[0];
        u_ids[(r + 1) % 2] = juego->online.u_id[1];
    
        for(i = 0; i < 2; i++)
        {
            sprintf(buffer, "DELETE FROM ttt_Buscando WHERE id_usuario = %ld", u_ids[i]);
            query(&juego->online.mysql, buffer, NULL);
        }
    
        sprintf(buffer, "INSERT INTO ttt_Partida (p_status, last_player, id_usuario_1, id_usuario_2) VALUES (%d, %d, %ld, %ld)", GAME_STARTED, 1, u_ids[0], u_ids[1]);
        query(&juego->online.mysql, buffer, NULL);
        
        mysql_free_result(res);
    }

    /**
     * Elimina registros repetidos
     */
    sprintf(buffer, "SELECT COUNT(*)-1 FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld", juego->online.u_id[0], juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%d", &c);
    mysql_free_result(res);

    if(c > 0)
    {
        sprintf(buffer, "DELETE FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld LIMIT %d", juego->online.u_id[0], juego->online.u_id[0], c - 1);
        query(&juego->online.mysql, buffer, NULL);
    }

    if(m)
    {
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

    return FALSE;
}

void playOnline(JUEGO *juego)
{
    printf("AAAA\n");
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