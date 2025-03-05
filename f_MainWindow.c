#include "gato.h"

void gameStartup(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    srand(time(NULL));
    loadMainWindow(juego);

    juego->partida.turno = 0;
    juego->partida.turno_max = 0;

    juego->partida.historial[0].game_status = 0;
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