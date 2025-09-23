#include "gato.h"

// gcc -o ttt.out ttt.c -lnsl -lm -lz -lmysqlclient

#define server "localhost"
#define user "ict23jzt"
#define passwd "234126"
#define db "ict23jzt"

#define GAME_WON -2 // El juego ha temrinado ganador
#define GAME_TIE -1 // El juego ha temrinado hay un empate
#define GAME_STARTED 1 // El juego está en curso

#define ICONS "XO"

typedef struct def_Game
{
    char board[3][3];
    char players[2][21];
    int last_player;
    int p_status;
} Game;

typedef struct def_Data
{
    MYSQL mysql;
    char name[21];
    long int u_id[2];
    long int g_id;
} Data;

void menu(Data *data);
void buscarPartida(Data *data);
void query(MYSQL *mysql, char query[], MYSQL_RES **res);
void juegaGato(Data *data);
void printBoard(char board[3][4], char p1[21], char p2[21]);
int checkGame(char tablero[3][4], char played);
void playTurn(Data *data, char adv[21], char board[3][4], int lp);

int main(int argc, char *argv[])
{
    Data data;

    MYSQL_RES *res;
    MYSQL_ROW row;

    char buffer[1000];

    data.g_id = 0;
    data.u_id[0] = -1;
    data.u_id[1] = -1;

    srand(time(NULL));
    mysql_init(&data.mysql); // Prepara la conexion al servidor de bases de datos

    // Se conecta al servidor de base de datos y verifica que no haya error
    if(!mysql_real_connect(&data.mysql, server, user, passwd, db, 0, NULL, 0))
    {
        printf("Failed to connect to database: Error: %s\n", mysql_error(&data.mysql));
        return 0;
    }

    // Se conecta a la base de datos deseada
    if(mysql_select_db(&data.mysql, db))
    {
        printf("Error selecting database: Error: %s\n", mysql_error(&data.mysql));
        exit(1);
    }
    
    printf("Nombre: ");
    scanf("%[^\n]", data.name);
    getchar();

    // sprintf(buffer, "INSERT INTO ttt_Usuarios (nombre) VALUES (\"%s\")", data.name);
    // query(&data.mysql, buffer, NULL);

    // sprintf(buffer, "SELECT id_usuario FROM ttt_Usuarios WHERE nombre = \"%s\" ORDER BY id_usuario DESC LIMIT 1", data.name);
    // query(&data.mysql, buffer, &res);

    sprintf(buffer, "CALL ttt_AddUsuario(\"%s\")", data.name);
    query(&data.mysql, buffer, &res);
    printf("todo en orden hasta aquí\n");

    // Despliega el resultado del query
    row = mysql_fetch_row(res);
    if(row[0] != NULL)
    {
        sscanf(row[0], "%ld", &data.u_id[0]);
    }

    mysql_free_result(res);

    printf("%s %ld\n", data.name, data.u_id[0]);

    menu(&data);

    sprintf(buffer, "DELETE FROM ttt_Usuarios WHERE id_usuario = %ld", data.u_id[0]);
    query(&data.mysql, buffer, NULL);

    // cierra la conexion con al servidor
    mysql_close(&data.mysql);

    return 0;
}

void menu(Data *data)
{
    int op = 0;

    system("clear");

    do
    {
        printf("Opciones:\n");
        printf("\t1. Buscar Partida.\n");
        printf("\t2. Salir.\n");

        scanf("%d", &op);
        getchar();
        system("clear");

        switch(op)
        {
            case 1:
                buscarPartida(data);
                break;
            case 2:
                break;
            default:
                printf("Acción desconocida...\n");
                break;
        }
    } while(op != 2);

    return;
}

void query(MYSQL *mysql, char query[], MYSQL_RES **res)
{
    if(mysql_query(mysql, query))
    {
        printf("Error processing query \"%s\" Error: %s\n", query, mysql_error(mysql));
        exit(1);
    }

    if(res != NULL && !(*res = mysql_store_result(mysql)))
    {
        printf("Error storing results Error: %s\n", mysql_error(mysql));
        exit(1);
    }

    return;
}

void buscarPartida(Data *data)
{
    char buffer[1000];
    char buffer2[1000];
    char dots[4];

    long int u_ids[2];

    int r = rand() % 2;
    int i = 0;
    int m = 0;
    int c = 0;

    MYSQL_RES *res;
    MYSQL_RES *res2;
    MYSQL_ROW row;
    MYSQL_ROW row2;
    
    dots[0] = 0;

    sprintf(buffer, "INSERT INTO ttt_Buscando VALUES (%ld)", data->u_id[0]);
    query(&data->mysql, buffer, NULL);

    sprintf(buffer, "SELECT * FROM ttt_Buscando WHERE id_usuario != %ld LIMIT 1", data->u_id[0]);
    sprintf(buffer2, "SELECT * FROM ttt_Buscando WHERE id_usuario = %ld LIMIT 1", data->u_id[0]);

    // Mientras no encuente otro usuario para jugar
    do
    {
        // impresion dinámica de espera
        printf("Buscando Partida%s\n", dots);

        if(strlen(dots) < 3)
        {
            strcat(dots, ".");
        }
        else
        {
            dots[0] = 0;
        }

        // Busca un usuario en la lista de espera que no sea el usuario actual
        query(&data->mysql, buffer, &res);

        // Si no encuentra ningun usuario
        if(!(row = mysql_fetch_row(res)))
        {
            // libera el resultado
            mysql_free_result(res);

            // revisa que el usuario siga en la lista de espera
            query(&data->mysql, buffer2, &res2);

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

        system("clear");
    } while(!row && row2);

    if(!m)
    {
        sscanf(row[0], "%ld", &data->u_id[1]);
    
        u_ids[r] = data->u_id[0];
        u_ids[(r + 1) % 2] = data->u_id[1];
    
        for(i = 0; i < 2; i++)
        {
            sprintf(buffer, "DELETE FROM ttt_Buscando WHERE id_usuario = %ld", u_ids[i]);
            query(&data->mysql, buffer, NULL);
        }
    
        sprintf(buffer, "INSERT INTO ttt_Partida (p_status, last_player, id_usuario_1, id_usuario_2) VALUES (%d, %d, %ld, %ld)", GAME_STARTED, 1, u_ids[0], u_ids[1]);
        query(&data->mysql, buffer, NULL);
        
        mysql_free_result(res);
    }

    /**
     * Elimina registros repetidos
     */
    sprintf(buffer, "SELECT COUNT(*) FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld", data->u_id[0], data->u_id[0]);
    query(&data->mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%d", &c);
    mysql_free_result(res);

    sprintf(buffer, "DELETE FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld LIMIT %d", data->u_id[0], data->u_id[0], c - 1);
    query(&data->mysql, buffer, NULL);

    if(m)
    {
        sprintf(buffer, "SELECT id_usuario_1, id_usuario_2 FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld", data->u_id[0], data->u_id[0]);
        query(&data->mysql, buffer, &res);

        row = mysql_fetch_row(res);
        sscanf(row[0], "%ld", &data->u_id[1]);
        
        if(data->u_id[0] == data->u_id[1])
        {
            sscanf(row[1], "%ld", &data->u_id[1]);
        }

        mysql_free_result(res);
    }
    
    // printf("Found it!\n");
    juegaGato(data);

    return;
}

void juegaGato(Data *data)
{
    Game juego;
    char buffer[1000];
    char adv[21];
    char board[3][4];

    MYSQL_RES *res;
    MYSQL_ROW row;

    int p = 0;
    int lp = 0;
    int p1 = 0;
    int i = 0;
    int j = 0;

    char dots[4];
    dots[0] = 0;

    sprintf(buffer, "SELECT id_partida FROM ttt_Partida WHERE id_usuario_1 = %ld OR id_usuario_2 = %ld LIMIT 1", data->u_id[0], data->u_id[0]);
    query(&data->mysql, buffer, &res);

    row = mysql_fetch_row(res);
    sscanf(row[0], "%ld", &data->g_id);
    mysql_free_result(res);

    sprintf(buffer, "SELECT nombre FROM ttt_Usuarios WHERE id_usuario = %ld LIMIT 1", data->u_id[1]);
    query(&data->mysql, buffer, &res);
    
    row = mysql_fetch_row(res);

    if(row == NULL)
    {
        printf("%s\n", buffer);
        return;
    }

    sscanf(row[0], "%s", adv);
    mysql_free_result(res);

    sprintf(buffer, "SELECT fila_1, fila_2, fila_3, p_status, last_player, id_usuario_1 FROM ttt_Partida WHERE id_partida = %ld", data->g_id);

    while(p != GAME_TIE && p != GAME_WON)
    {
        system("clear");

        query(&data->mysql, buffer, &res);

        row = mysql_fetch_row(res);

        for(i = 0; i < 3; i++)
        {
            sscanf(row[i], "%s", board[i]);
        }

        sscanf(row[3], "%d", &p);
        sscanf(row[4], "%d", &lp);
        sscanf(row[5], "%d", &p1);

        mysql_free_result(res);

        if(p > 0)
        {
            if((!lp && p1 == data->u_id[0]) || lp && p1 != data->u_id[0])
            {
                playTurn(data, adv, board, lp);
            }
            else
            {
                printBoard(board, data->name, adv);
                printf("Esperando a adversario%s\n", dots);
    
                if(strlen(dots) < 3)
                {
                    strcat(dots, ".");
                }
                else
                {
                    dots[0] = 0;
                }
    
                sleep(1);
            }
        }
        else
        {
            printBoard(board, data->name, adv);
            printf("El juego ha temrinado!\n");
        }
    }

    return;    
}

void printBoard(char board[3][4], char p1[21], char p2[21])
{
    int i = 0;
    int j = 0;

    printf("Jugador: %s\n", p1);
    printf("Adversario: %s\n\n", p2);

    for(i = 0; i < 3; i++)
    {
        if(i)
        {
            printf("---+---+---\n");
        }
        for(j = 0; j < 3; j++)
        {
            if(j)
            {
                printf("|");
            }

            if(board[i][j] == 'n')
            {
                printf("   ");
            }
            else
            {
                printf(" %c ", board[i][j]);
            }
        }
        printf("\n");
    }

    return;
}

void playTurn(Data *data, char adv[21], char board[3][4], int lp)
{
    char buffer[1000];
    int r = 0;
    int c = 0;
    int e = 0;
    
    do
    {
        r = 0;
        
        printBoard(board, data->name, adv);
        printf("Tu turno!\n");
        printf("Ingresa una casilla del 1 al 9\n");
        
        scanf("%d", &c);
        getchar();
        system("clear");

        if(!(c >= 1 && c <= 9))
        {
            printf("Selecciona una casilla entre 1 y 9...\n\n");
            r = 1;
        }
        else
        {
            c--;
        }

        if(!r && board[c / 3][c % 3] != 'n')
        {
            printf("Ese espacio ya está ocupado...\n\n");
            r = 1;
        }

        if(!r)
        {
            board[c / 3][c % 3] = ICONS[lp];
            sprintf(buffer, "UPDATE ttt_Partida SET fila_%d = '%s', last_player = %d, p_status = %d WHERE id_partida = %ld", (c / 3) + 1, board[c/3], (lp + 1) % 2, checkGame(board, ICONS[lp]), data->g_id);
            query(&data->mysql, buffer, NULL);
        }
    } while(r);
}

/**
 * Revisa el estaso de un tablero
 * 
 * @param tablero[3][3] El tablero a revisar
 * @param played El último jugador que tiró
 * 
 * @returns int (-1 si el tablero está lleno y sin victorias, 1 si no hay victorias, -2 si hay victorias)
 */
int checkGame(char tablero[3][4], char played)
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
            if(tablero[i][j] == 'n')
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
        return -2;
    }
    // si el tablero está lleno
    else if(full)
    {
        return -1;
    }

    return 1;
}