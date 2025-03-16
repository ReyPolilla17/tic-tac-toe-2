#include "gato.h"

void gameStartup(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    juego->partida.turno = 0;
    juego->partida.turno_max = 0;

    juego->partida.historial[0].game_status = GAME_NOT_STARTED;
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

void newGame(JUEGO *juego)
{
    NewGameModal info;

    gint res = GTK_RESPONSE_NO;

    int rn = 0;
    int v = 0;

	// Si hay una partida en curso
	if(juego->partida.historial[juego->partida.turno].game_status != GAME_NOT_STARTED)
	{
        do
        {
            v = 1;
            res = confirmation_dialog("Partida en curso", "¿Deseas guardar la partida actual?");

            if((res == GTK_RESPONSE_YES && saveGame(juego) == GTK_RESPONSE_ACCEPT) || res == GTK_RESPONSE_NO)
            {
                v = 0;
            }
        } while(v);
	}

    if(res != GTK_RESPONSE_DELETE_EVENT)
    {
        new_game_modal_new(&info);

        if(new_game_modal_get_info(&info))
        {
            rn = rand() % 2;
            
            cleanScreen(juego);

            if(info.hard_mode)
            {
                displayHardMode(juego);
            }

            gtk_widget_set_sensitive(juego->graficos.menuEnd, TRUE);
            gtk_widget_set_sensitive(juego->graficos.menuSave, TRUE);

            gtk_widget_show(juego->graficos.playerImg[0]);
            gtk_widget_show(juego->graficos.playerImg[1]);

            juego->partida.jugadores[rn].hard_mode = info.hard_mode;
            juego->partida.jugadores[rn].ia = info.vs_ia;

            if(info.vs_ia)
            {
                strcpy(juego->partida.jugadores[rn].nombre, IA_NAME);
            }
            else
            {
                strcpy(juego->partida.jugadores[rn].nombre, info.names[1]);
            }
            
            juego->partida.jugadores[(rn + 1) % 2].hard_mode = info.hard_mode;
            juego->partida.jugadores[(rn + 1) % 2].ia = info.vs_ia;
            
            strcpy(juego->partida.jugadores[(rn + 1) % 2].nombre, info.names[0]);

            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[0]), juego->partida.jugadores[0].nombre);
            gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[1]), juego->partida.jugadores[1].nombre);

            gtk_widget_destroy(juego->graficos.playingImg);
            juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[0]);
                gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
                gtk_widget_show(juego->graficos.playingImg);
            
            juego->partida.historial[0].game_status = GAME_STARTED;
        }
    }
    
    return;
}

gint saveGame(JUEGO *juego)
{
    FILE *fp;

    GtkWidget *dialog;

    gint res = 0;
    gchar *file;

    // muestra la ventana de guardado
    dialog = gtk_file_chooser_dialog_new("Guardar Partida", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    // si se da click en guardar
    if(res == GTK_RESPONSE_ACCEPT)
    {
        // obtiene la ruta
        file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        // guarda la partida en el archivo
        fp = fopen(file, "wb");
        fwrite(&juego->partida, sizeof(FUNCIONAL), 1, fp);
        fclose(fp);

        // libera memoria
        g_free(file);

        warning_dialog("Partida Guardada", "¡La partida ha sido guardada con éxito!");
    }

    gtk_widget_destroy(dialog);

    return res;
}

void loadGame(JUEGO *juego)
{
    g_print("Función pendiente...\n");
    
    return;
}

void cleanScreen(JUEGO *juego)
{
    int i = 0;
    int j = 0;

    GdkColor color;

    gdk_color_parse("#DCDAD5", &color);

    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    gtk_widget_hide(juego->graficos.menuTruth);

    gtk_widget_destroy(juego->graficos.playingImg);
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
        gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);
        gtk_widget_show(juego->graficos.playingImg);
        
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            gtk_widget_destroy(juego->graficos.buttonImg[i][j]);
            juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
            gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[i][j]), juego->graficos.buttonImg[i][j]);
            gtk_widget_show(juego->graficos.buttonImg[i][j]);
        }
    }
    
    // oculta las llamas y la información de los jugadores
    for(i = 0; i < 2; i++)
    {
        gtk_widget_hide(juego->graficos.flames[i]);
        gtk_widget_hide(juego->graficos.playerImg[i]);

        gtk_label_set_label(GTK_LABEL(juego->graficos.playerName[i]), NULL);
    }

    return;
}

void displayHardMode(JUEGO *juego)
{
    int i = 0;

    GdkColor color;

    gdk_color_parse("#790000", &color);

    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

    gtk_widget_show(juego->graficos.menuTruth);

    // muestra las llamas
    for(i = 0; i < 2; i++)
    {
        gtk_widget_show(juego->graficos.flames[i]);
    }

    return;
}

int getButton(JUEGO *juego, GtkWidget *button, int coords[2])
{
    int i = 0;
    int j = 0;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            if(button == juego->graficos.buttons[i][j])
            {
                coords[0] = i;
                coords[1] = j;

                return 1;
            }
        }
    }

    return 0;
}