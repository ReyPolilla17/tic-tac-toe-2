#include "gato.h"

int main(int argc, char *argv[])
{
    JUEGO juego;

    srand(time(NULL));

    system("clear");
    gtk_init(&argc, &argv);

    gameStartup(&juego);
    loadMainWindow(&juego);

    gtk_main();

    return 0;
}