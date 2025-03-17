/**
 * @file endDialogs.c
 * 
 * @brief Ventanas que se muestran al terminar la partida
 * 
 * @author Luis Julián Zamora Treviño
 * @date 16/03/2025
 */
#include "gato.h"

/**
 * Ventana de empate
 * 
 * @param *juego Información de la partida
 */
void tie_dialog(JUEGO *juego)
{
    AboutDialog *dialog;

    // Crea la ventana base
    dialog = about_dialog_new("Fin del juego");
        about_dialog_set_title(dialog, "Empate");
    
    // Si se enfrentó a la computadora
    if(juego->partida.jugadores[0].ia || juego->partida.jugadores[1].ia)
    {
        about_dialog_set_logo(dialog, juego->graficos.hercules); // Establece el logo

        // En caso de ser en modo difícil
        if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
        {
            about_dialog_set_comments(dialog, "hercules.raw dormirá por otro siglo.");
            about_dialog_set_copyright(dialog, "Neutral Ending");
        }
        else // modo normal
        {
            about_dialog_set_comments(dialog, "hercules.raw parece sorprendido.");
        }
    }
    else // contra otro jugador
    {
        about_dialog_set_comments(dialog, "¡El juego ha terminado en empate!");
    }

    about_dialog_run(dialog); // muestra la ventana

    return;
}

/**
 * Ventana de ganador
 * 
 * @param *juego Información de la partida
 */
void victory_dialog(JUEGO *juego)
{
    AboutDialog *dialog;

    // extrae al ganador y al perdedor, para facilitar la escritura
    JUGADOR winner = juego->partida.jugadores[(juego->partida.turno + 1) % 2];
    JUGADOR loser = juego->partida.jugadores[(juego->partida.turno) % 2];

    gchar *winMessage;
    
    dialog = about_dialog_new("Fin del juego"); // inicia la ventana

    // si el ganador fue la computadora
    if(winner.ia)
    {
        about_dialog_set_title(dialog, "Derrota");
        about_dialog_set_logo(dialog, juego->graficos.hercules);

        if(winner.hard_mode) // modo difícil
        {
            about_dialog_set_comments(dialog, "La humanidad está condenada.");
            about_dialog_set_copyright(dialog, "Bad Ending");
        }
        else // modo normal
        {
            about_dialog_set_comments(dialog, "hercules.raw te mira con desprecio...");
        }
    }
    else if(loser.ia) // si el perdedor fue la computadora
    {
        about_dialog_set_title(dialog, "Victoria");
        about_dialog_set_logo(dialog, juego->graficos.hercules);

        if(loser.hard_mode) // modo difícil
        {
            about_dialog_set_comments(dialog, "¡Has derrotado a un terror cósmico!");
            about_dialog_set_copyright(dialog, "Good Ending");
        }
        else // modo normal
        {
            about_dialog_set_comments(dialog, "hercules.raw te respeta un poco más que antes.");
        }
    }
    else // jugador contra jugador
    {
        // Crea el mensaje de victoria
        winMessage = (gchar *) malloc(sizeof(gchar) * (strlen("¡La victoria es para !") + strlen(winner.nombre) + 1));
            sprintf(winMessage, "¡La victoria es para %s!", winner.nombre);
        
        // Establece la ficha del ganador
        about_dialog_set_title(dialog, "Victoria");
        about_dialog_set_logo(dialog, juego->graficos.m60[(juego->partida.turno + 1) % 2]);
        about_dialog_set_comments(dialog, winMessage);

        free(winMessage);
    }

    about_dialog_run(dialog); // muestra la ventana

    return;
}