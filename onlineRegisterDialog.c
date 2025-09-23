/**
 * @file onlineRegisterDialog.c
 * 
 * @brief Contiene todas las funciones para mostrar una ventana de registro para el multijugador y recuperar los datos ingresados
 * 
 * @author Luis Julián Zamora Treviño
 * @date 23/09/2025
 */
#include "gato.h"

/**
 * Creación de la ventana de registro al multijugador
 * 
 * @param *info La información de la ventana
 * 
 * @returns void
 */
void online_register_modal_new(OnlineRegisterModal *info)
{
    GtkWidget *mBox, *hBox, *vBox, *label, *button; // widgets auxiliares

    // Creación de la ventana
    info->dialog = gtk_dialog_new();
        gtk_window_set_title(GTK_WINDOW(info->dialog), "Jugar en Línea");
        gtk_window_set_default_size(GTK_WINDOW(info->dialog), 250, 400);
    
    button = gtk_dialog_add_button(GTK_DIALOG(info->dialog), "Aceptar", GTK_RESPONSE_APPLY);
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));
    
    mBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(info->dialog)->vbox), mBox, TRUE, TRUE, 0);


    /**
     * Sección Nombre del jugador
     */
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 20);
    
    // Título de la sección
    label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' size='large'>Multijugador:</span>");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 5);
        
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 0);
        
    vBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(hBox), vBox, TRUE, TRUE, 5);
        
    // Input del nombre
    hBox = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, FALSE, 0);

    label = gtk_label_new("Nombre:");
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, FALSE, 0);

    info->entry_name = gtk_entry_new_with_max_length(20);
        gtk_signal_connect(GTK_OBJECT(info->entry_name), "changed", GTK_SIGNAL_FUNC(on_modal_updated), info);
        gtk_box_pack_start(GTK_BOX(hBox), info->entry_name, TRUE, TRUE, 0);
    
    // Mensaje de error
    info->warning = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(info->warning), "\0");
        gtk_box_pack_start(GTK_BOX(vBox), info->warning, FALSE, FALSE, 10);
    
    gtk_widget_show_all(info->dialog);

    return;
}

/**
 * Obtiene la información del campo de la ventana de registro al multijugador, validando todas las entradas.
 * Regresa 1 en caso de que el nombre sea válido, 0 en caso de cancelar la acción
 * 
 * @param *info Información de la ventana
 * 
 * @returns gint 1 en caso de un registro correcto, 0 en caso de cancelar
 */
gint online_register_modal_get_info(OnlineRegisterModal *info)
{
    gint v = 0; // validación de entradas
    gint res = 0; // resultado de la ventana

    // mientras las entradas no sean válidas
    while(!v)
    {
        // inicializa valores
        v = 1;
        res = 1;

        // Si se selecciona aceptar
        if(gtk_dialog_run(GTK_DIALOG(info->dialog)) == GTK_RESPONSE_APPLY)
        {
            strcpy(info->name, gtk_entry_get_text(GTK_ENTRY(info->entry_name)));

            // si no hay info, muestra un mnesaje de error y la entrada no es válida
            if(!strlen(info->name))
            {
                gtk_label_set_markup(GTK_LABEL(info->warning), "<span color='red'>Debes llenar todos los campos activos...</span>");

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