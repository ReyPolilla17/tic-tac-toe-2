#include "gato.h"

void tie_dialog(JUEGO *juego)
{
    AboutDialog *dialog;

    dialog = about_dialog_new("Fin del juego");
        about_dialog_set_title(dialog, "Empate");
    
    if(juego->partida.jugadores[0].ia || juego->partida.jugadores[1].ia)
    {
        about_dialog_set_logo(dialog, juego->graficos.hercules);

        if(juego->partida.jugadores[0].hard_mode || juego->partida.jugadores[1].hard_mode)
        {
            about_dialog_set_comments(dialog, "hercules.raw dormirá por otro siglo.");
            about_dialog_set_copyright(dialog, "Neutral Ending");
        }
        else
        {
            about_dialog_set_comments(dialog, "hercules.raw parece sorprendido.");
        }
    }
    else
    {
        about_dialog_set_comments(dialog, "¡El juego ha terminado en empate!");
    }

    about_dialog_run(dialog);

    return;
}

void victory_dialog(JUEGO *juego)
{
    AboutDialog *dialog;

    JUGADOR winner = juego->partida.jugadores[(juego->partida.turno + 1) % 2];
    JUGADOR loser = juego->partida.jugadores[(juego->partida.turno) % 2];

    gchar *winMessage;
    
    dialog = about_dialog_new("Fin del juego");

    if(winner.ia)
    {
        about_dialog_set_title(dialog, "Derrota");
        about_dialog_set_logo(dialog, juego->graficos.hercules);

        if(winner.hard_mode)
        {
            about_dialog_set_comments(dialog, "La humanidad está condenada.");
            about_dialog_set_copyright(dialog, "Bad Ending");
        }
        else
        {
            about_dialog_set_comments(dialog, "hercules.raw te mira con desprecio...");
        }
    }
    else if(loser.ia)
    {
        about_dialog_set_title(dialog, "Victoria");
        about_dialog_set_logo(dialog, juego->graficos.hercules);

        if(loser.hard_mode)
        {
            about_dialog_set_comments(dialog, "¡Has derrotado a un terror cósmico!");
            about_dialog_set_copyright(dialog, "Good Ending");
        }
        else
        {
            about_dialog_set_comments(dialog, "hercules.raw te respeta un poco más que antes.");
        }
    }
    else
    {
        winMessage = (gchar *) malloc(sizeof(gchar) * (strlen("¡La victoria es para !") + strlen(winner.nombre) + 1));
            sprintf(winMessage, "¡La victoria es para %s!", winner.nombre);
        
        about_dialog_set_title(dialog, "Victoria");
        about_dialog_set_logo(dialog, juego->graficos.m60[(juego->partida.turno + 1) % 2]);
        about_dialog_set_comments(dialog, winMessage);

        free(winMessage);
    }

    about_dialog_run(dialog);

    return;
}