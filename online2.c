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

void juegaGato(Data *data);
void printBoard(char board[3][4], char p1[21], char p2[21]);
void playTurn(Data *data, char adv[21], char board[3][4], int lp);

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