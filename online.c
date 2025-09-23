#include "gato.h"

void connectToDatabase(JUEGO *juego)
{
    juego->online.g_id = 0;
    juego->online.u_id[0] = -1;
    juego->online.u_id[1] = -1;
    juego->online.connected = TRUE;

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

    sprintf(buffer, "DELETE FROM ttt_Usuarios WHERE id_usuario = %ld", juego->online.u_id[0]);
    query(&juego->online.mysql, buffer, NULL);

    // cierra la conexion con al servidor
    mysql_close(&juego->online.mysql);
}

void seekMatch(JUEGO *juego)
{
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