/**
 * @file newGameDialog.c
 * 
 * @brief Contiene todas las funciones para mostrar una ventana de juego nuevo y recuperar los datos ingresados
 * 
 * @author Luis Julián Zamora Treviño
 * @date 16/03/2025
 */
#include "gato.h"

/**
 * Creación de la ventana de juego nuevo
 * 
 * @param *info La información de la ventana
 * 
 * @returns void
 */
void new_game_modal_new(NewGameModal *info)
{
    GtkWidget *mBox, *hBox, *vBox, *label, *button, *separator; // widgets auxiliares

    // Creación de la ventana
    info->dialog = gtk_dialog_new();
        gtk_window_set_title(GTK_WINDOW(info->dialog), "Nueva Partida");
        gtk_window_set_default_size(GTK_WINDOW(info->dialog), 250, 400);
    
    button = gtk_dialog_add_button(GTK_DIALOG(info->dialog), "Iniciar", GTK_RESPONSE_APPLY);
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));

    button = gtk_dialog_add_button(GTK_DIALOG(info->dialog), "Cancelar", GTK_RESPONSE_CANCEL);
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON));
    
    mBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(info->dialog)->vbox), mBox, TRUE, TRUE, 0);

    /**
     * Sección oponente
     */
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 20);

    // Título de la sección
    label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' size='large'>Adeversario:</span>");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 5);    

    hBox = gtk_hbox_new(TRUE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 0);
    
    // Opciones
    vBox = gtk_vbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hBox), vBox, TRUE, TRUE, 20);

    button = gtk_radio_button_new_with_label(NULL, "Jugador");
        gtk_box_pack_start(GTK_BOX(vBox), button, FALSE, FALSE, 0);
        
    vBox = gtk_vbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(hBox), vBox, TRUE, TRUE, 20);

    info->radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Computadora");
        gtk_signal_connect(GTK_OBJECT(info->radio), "toggled", GTK_SIGNAL_FUNC(new_game_modal_radio_changed), info);
        gtk_box_pack_start(GTK_BOX(vBox), info->radio, FALSE, FALSE, 0);

    // Modo difícil
    info->check = gtk_check_button_new_with_label("Modo difícil");
        gtk_widget_set_sensitive(info->check, FALSE);
        gtk_box_pack_start(GTK_BOX(vBox), info->check, FALSE, FALSE, 0);

    separator = gtk_hseparator_new();
        gtk_box_pack_start(GTK_BOX(mBox), separator, FALSE, FALSE, 0);

    /**
     * Sección Nombres de los jugadores
     */
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 20);
    
    // Título de la sección
    label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' size='large'>Jugador(es):</span>");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 5);
        
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 0);
        
    vBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(hBox), vBox, TRUE, TRUE, 5);
        
    // Primer jugador
    hBox = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, FALSE, 0);

    label = gtk_label_new("Jugador 1:");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 0);

    info->entry_names[0] = gtk_entry_new_with_max_length(20);
        gtk_signal_connect(GTK_OBJECT(info->entry_names[0]), "changed", GTK_SIGNAL_FUNC(new_game_modal_updated), info);
        gtk_box_pack_start(GTK_BOX(hBox), info->entry_names[0], TRUE, TRUE, 0);
    
    // Segundo jugdor
    hBox = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, FALSE, 0);

    label = gtk_label_new("Jugador 2:");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 0);
        
    info->entry_names[1] = gtk_entry_new_with_max_length(20);
        gtk_signal_connect(GTK_OBJECT(info->entry_names[0]), "changed", GTK_SIGNAL_FUNC(new_game_modal_updated), info);
        gtk_box_pack_start(GTK_BOX(hBox), info->entry_names[1], TRUE, TRUE, 0);
    
    // Mensaje de error
    info->warning = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(info->warning), "\0");
        gtk_box_pack_start(GTK_BOX(vBox), info->warning, FALSE, FALSE, 10);
    
    gtk_widget_show_all(info->dialog);

    return;
}

/**
 * Obtiene la información de los campos de la ventana de juego nuevo, validando todas las entradas.
 * Regresa 1 en caso de que se desee iniciar una nueva partida con datos válidos, 0 en caso de cancelar la acción
 * 
 * @param *info Información de la ventana
 * 
 * @returns gint 1 en caso de iniciar nueva partida, 0 en caso de cancelar
 */
gint new_game_modal_get_info(NewGameModal *info)
{
    gint i = 0; // contador
    gint v = 0; // validación de entradas
    gint res = 0; // resultado de la ventana

    // mientras las entradas no sean válidas
    while(!v)
    {
        // inicializa valores
        v = 1;
        res = 1;

        info->hard_mode = 0;
        info->vs_ia = 1;

        // Si se selecciona aceptar
        if(gtk_dialog_run(GTK_DIALOG(info->dialog)) == GTK_RESPONSE_APPLY)
        {
            // Si no se ha seleccionado contra computadora
            if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(info->radio)))
            {
                info->vs_ia = 0;
            }
            // si se ha seleccionado el modo difícil
            else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(info->check)))
            {
                info->hard_mode = 1;
            }

            // para todos los nombres que sean necesarios
            for(i = 0; (i < 2 - info->vs_ia) && v; i++)
            {
                // obtiene el valor ingresado
                strcpy(info->names[i], gtk_entry_get_text(GTK_ENTRY(info->entry_names[i])));

                // si no hay info, muestra un mnesaje de error y la entrada no es válida
                if(!strlen(info->names[i]))
                {
                    gtk_label_set_markup(GTK_LABEL(info->warning), "<span color='red'>Debes llenar todos los campos activos...</span>");

                    v = 0;
                }
            }

            // si se elije contra computadora y el nombre es el mismo, muestra un error y la entrada no es válida
            if(v && info->vs_ia && !strcmp(IA_NAME, info->names[0]))
            {
                gtk_label_set_markup(GTK_LABEL(info->warning), "<span color='red'>Solo puede haber un Hercules.raw...</span>");

                v = 0;
            }
            // si se elije contra jugador y el nombre es el mismo, uestra error y la entrada no es válida
            else if(v && !info->vs_ia && !strcmp(info->names[0], info->names[1]))
            {
                gtk_label_set_markup(GTK_LABEL(info->warning), "<span color='red'>Los nombres deben ser diferentes...</span>");

                v = 0;
            }
        }
        else // si se cancela la acción
        {
            res = 0;
        }
    }

    gtk_widget_destroy(info->dialog);

    return res;
}

/**
 * Al seleccionar una opción de contrincante
 * 
 * @param *widget La opción seleccionada
 * @param data La información de la ventana como gpointer
 * 
 * @returns void
 */
void new_game_modal_radio_changed(GtkWidget *widget, gpointer data)
{
    NewGameModal *info = (NewGameModal *)data;

    // Si se seleccionó contra computadora
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    {
        gtk_widget_set_sensitive(info->check, TRUE); // habilita la opción de seleccionar el modo dificil

        // vacía y deshabilita el campo del segundo nombre
        gtk_entry_set_text(GTK_ENTRY(info->entry_names[1]), "\0");
        gtk_widget_set_sensitive(info->entry_names[1], FALSE);
    }
    else // de lo contrario
    {
        // deshabilita y des selecciona la opción del modo difícil
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->check), FALSE);
        gtk_widget_set_sensitive(info->check, FALSE);

        // habilita la sección del segundo nombre
        gtk_widget_set_sensitive(info->entry_names[1], TRUE);
    }

    // Oculta el mensaje de advertencia
    new_game_modal_updated(widget, data);

    return;
}

/**
 * Al actualizar algun campo de la ventana
 * 
 * @param *widget El campo modificado
 * @param data La información de la ventana
 */
void new_game_modal_updated(GtkWidget *widget, gpointer data)
{
    NewGameModal *info = (NewGameModal *)data;

    // Oculta el mensaje de advertencia
    gtk_label_set_markup(GTK_LABEL(info->warning), "\0");

    return;
}