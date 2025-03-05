#include "gato.h"

int main(int argc, char *argv[])
{
    JUEGO juego;
    
    system("clear");
    gtk_init(&argc, &argv);
    gameStartup(&juego);
    gtk_main();

    return 0;
}