/**
 * @file myDialogs.c
 * 
 * @brief Contiene funciones genéricas para todas las ventanas customizadas que utiliza el programa
 * 
 * @author Luis Julián Zamora Treviño
 * @date 15/03/2025
 */
#include "mydialogs.h"

/**
 * Al presionar algun botón interno de las ventanas customizadas
 * 
 * @param *widget El botón presionado
 * @param data El botón interno a presionar guardado como gpointer
 * 
 * @returns void
 */
void my_dialogs_on_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *button = (GtkWidget *) data;

    gtk_button_clicked(GTK_BUTTON(button));
    
    return;
}