/**
 * @file gato.c
 * 
 * @brief contiene la función principal del programa
 * 
 * @author Luis Julián Zamora Treviño
 * @date 23/09/2024
 */
#include "gato.h"

/**
 * Función principal del programa
 * 
 * @param argc Cuenta de argumentos de entrada
 * @param *argv[] Arreglo de argumentos de entrada
 */
int main(int argc, char *argv[])
{
    JUEGO juego;

    srand(time(NULL)); // inicializa random

    // inicializa gtk
    system("clear");
    gtk_init(&argc, &argv);

    // inicializa la ventana principal
    gameStartup(&juego);
    loadMainWindow(&juego);
    connectToDatabase(&juego);

    // inicia el loop de gtk
    gtk_main();

    return 0;
}