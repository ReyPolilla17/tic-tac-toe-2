#include "gato.h"

void online_connection_dialog_new(OnlineConnectionDialog **info)
{
    GtkWidget *mBox, *hBox, *label, *button; // widgets auxiliares

    *info = (OnlineConnectionDialog *) malloc(sizeof(OnlineConnectionDialog));

    // Creación de la ventana
    (*info)->dialog = gtk_dialog_new();
        gtk_window_set_title(GTK_WINDOW((*info)->dialog), "Buscando Partida");
        gtk_window_set_default_size(GTK_WINDOW((*info)->dialog), 250, 100);
    
    mBox = gtk_vbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG((*info)->dialog)->vbox), mBox, TRUE, TRUE, 0);


    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 20);
    
    // Título de la sección
    label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold' size='large'>Esperando Oponente</span>");
        gtk_box_pack_start(GTK_BOX(hBox), label, TRUE, TRUE, 5);

    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 0);

    (*info)->progress = gtk_progress_bar_new();
        gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR((*info)->progress), 0.1);
        gtk_box_pack_start(GTK_BOX(mBox), (*info)->progress, FALSE, FALSE, 0);
    
    // Agrega el botón con la respuesta
    button = gtk_button_new();
        gtk_button_set_label(GTK_BUTTON(button), "Cancelar");
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON));
        gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(my_dialogs_on_button_clicked), gtk_dialog_add_button(GTK_DIALOG((*info)->dialog), "\0", GTK_RESPONSE_CANCEL));
        gtk_box_pack_end(GTK_BOX(mBox), button, FALSE, FALSE, 5);

    return;
}

void online_connection_dialog_run(OnlineConnectionDialog **dialog)
{
        
    // muesta las partes necesarias
    gtk_widget_show_all((*dialog)->dialog);
    gtk_widget_hide(GTK_DIALOG((*dialog)->dialog)->action_area);
    
    // ejecuta el dialog
    (*dialog)->res = gtk_dialog_run(GTK_DIALOG((*dialog)->dialog));

    online_connection_dialog_destroy(dialog);

    return;
}

void online_connection_dialog_destroy(OnlineConnectionDialog **dialog)
{
    g_print("Fin1");
    gtk_widget_destroy((*dialog)->dialog);
    g_print("Fin2");
    free(*dialog);
    g_print("Fin3");

    *dialog = NULL;

    return;
}

void online_connection_dialog_pulse(OnlineConnectionDialog *dialog)
{
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(dialog->progress));

    return;
}