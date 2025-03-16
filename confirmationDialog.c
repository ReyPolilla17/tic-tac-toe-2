/**
 * @file confirmationDialog.c
 * 
 * @brief Contiene las funciones necesarias para la creación de una ventana de confirmación de fácil ejecución
 * con las repsuestas si y no
 * 
 * @author Luis Julián Zamora Treviño
 * @date 15/03/2025
 */
#include "mydialogs.h"

/**
 * Función de creación de la ventana de confiamción, regresa las constantes 
 *  - GTK_RESPONSE_YES           ->  En caso de seleciconar Si
 *  - GTK_RESPONSE_NO            ->  En caso de seleciconar No
 *  - GTK_RESPONSE_DELETE_EVENT  ->  En caso de cerrar la ventana 
 * 
 * @param *title El título de la ventana
 * @param *message La pregunta de la ventana
 * 
 * @returns gint
 */
gint confirmation_dialog(gchar *title, gchar *message)
{
    GtkWidget *dialog, *vBox, *hBox, *label, *button; // widgets de la ventana

    gint res = 0; // resultado

    // Aloca una cadena para la pregunta de la ventana con formato
    gchar *span = malloc(sizeof(gchar) * (strlen(message) + strlen("<span size='large'></span>") + 1));
        sprintf(span, "<span size='large'>%s</span>", message);

    // genera la ventana
    dialog = gtk_dialog_new();
        gtk_window_set_title(GTK_WINDOW(dialog), title);
        
    vBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 10);
        
    hBox = gtk_hbox_new(TRUE, 10);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, 10);

    // genera la parte de la pregunta
    label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), span);
        gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
        gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 10);

    hBox = gtk_hbox_new(TRUE, 10);
        gtk_box_pack_end(GTK_BOX(vBox), hBox, FALSE, TRUE, 0);
    
    // Agrega los botones con las respuestas
    button = gtk_button_new();
        gtk_button_set_label(GTK_BUTTON(button), "Si");
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON));
        gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(my_dialogs_on_button_clicked), gtk_dialog_add_button(GTK_DIALOG(dialog), "\0", GTK_RESPONSE_YES));
        gtk_widget_set_size_request(button, 100, -1);
        gtk_box_pack_start(GTK_BOX(hBox), button, FALSE, FALSE, 0);
        
    button = gtk_button_new();
        gtk_button_set_label(GTK_BUTTON(button), "No");
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON));
        gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(my_dialogs_on_button_clicked), gtk_dialog_add_button(GTK_DIALOG(dialog), "\0", GTK_RESPONSE_NO));
        gtk_widget_set_size_request(button, 100, -1);
        gtk_box_pack_end(GTK_BOX(hBox), button, FALSE, FALSE, 0);
    
    // muesta las partes necesarias
    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_hide(GTK_DIALOG(dialog)->action_area);
    
    // ejecuta el dialog
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    // libera la memoria necesaria
    gtk_widget_destroy(dialog);
    free(span);

    return res; // regresa el resultado
}