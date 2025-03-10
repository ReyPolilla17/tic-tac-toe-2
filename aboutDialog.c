/**
 * @file aboutDialog.c
 * 
 * @brief Contiene todas las funciones necesarias para mostrar la información de un programa junto con sus créditos.
 *  Para mantener un formato entendible, no se pueden tener más de 3 campos en la sección de créditos
 * 
 * @author Luis jUlián Zamora Treviño
 * @date 10/03/2025
 */

#include "mydialogs.h"

/**
 * Crea una página para agregar a un AboutDialog en la sección de créditos
 * 
 * @param *name El nombre de la sección de créditos
 * 
 * @returns AboutDialogPage * - Puntero a la alocación generada
 */
AboutDialogPage *about_dialog_page_new(gchar *name)
{
    AboutDialogPage *new = malloc(sizeof(AboutDialogPage)); // aloca el puntero

    // coloca el nombre
    new->page_name = malloc(sizeof(gchar) * (strlen(name) + 1));
    strcpy(new->page_name, name);

    // inicializa variables
    new->page_content = NULL;

    return new;
}

/**
 * Agrega una línea a la página de créditos indicada
 * 
 * @param *page La sección de créditos a la que agregar la línea
 * @param *line La línea a agregar
 * 
 * @returns void
 */
void about_dialog_page_add_line(AboutDialogPage *page, gchar *line)
{
    gchar *temp;

    // si la sección está vacía
    if(page->page_content == NULL)
    {
        // agrega la línea
        page->page_content = malloc(sizeof(gchar) * (strlen(line) + 1));
        strcpy(page->page_content, line);
    }
    else // de lo contrario
    {
        // agrega la línea al contenido ya existente
        temp = page->page_content;

        page->page_content = malloc(sizeof(gchar) * (strlen(temp) + strlen(line) + 2));
        sprintf(page->page_content, "%s\n%s", temp, line);
    }

    return;
}

/**
 * Creación del widget
 * 
 * @param *title El título de la ventana
 * 
 * @returns AboutDialog * Puntero a la alocación del widget
 */
AboutDialog *about_dialog_new(gchar *title)
{
    AboutDialog *new;
    GtkWidget *hBox;

    new = (AboutDialog *)malloc(sizeof(AboutDialog)); // aloca memoria

    // inicializa variables
    new->dialog_pointer = gtk_dialog_new();
    new->about_comments = NULL;
    new->about_copyright = NULL;
    new->about_logo = NULL;
    new->about_title = NULL;
    new->about_version = NULL;
    new->credit_pages_count = 0;

    // establece el título
    new->dialog = GTK_DIALOG(new->dialog_pointer);
        gtk_window_set_title(GTK_WINDOW(new->dialog_pointer), title);

    // confina el contenido de la ventana
    hBox = gtk_hbox_new(TRUE, 10);
        gtk_box_pack_start(GTK_BOX(new->dialog->vbox), hBox, TRUE, TRUE, 10);

    new->vBox = gtk_vbox_new(FALSE, 20);
        gtk_box_pack_start(GTK_BOX(hBox), new->vBox, TRUE, TRUE, 10);

    new->mBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(new->vBox), new->mBox, TRUE, TRUE, 0);

    // crea un botón interno
    new->iButton = gtk_dialog_add_button(new->dialog, "\0", GTK_RESPONSE_CLOSE);

    return new;
}

/**
 * Establece el logo a mostrar en un AboutDialog a partir de un GdkPixbuf
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *logo El logo a colocar
 * 
 * @returns void
 */
void about_dialog_set_logo(AboutDialog *dialog, GdkPixbuf *logo)
{
    // libera memoria de ser necesario
    if(dialog->about_logo != NULL)
    {
        g_free(dialog->about_logo);
    }

    // guarda la imagen
    dialog->about_logo = gtk_image_new_from_pixbuf(logo);

    return;
}

/**
 * Establece el títutlo del programa en un AboutDialog
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *title El título a establecer
 * 
 * @returns void
 */
void about_dialog_set_title(AboutDialog *dialog, gchar *title)
{
    // Libera memoria de ser necesario
    if(dialog->about_title != NULL)
    {
        free(dialog->about_title);
    }

    // Establece nuevo título
    dialog->about_title = malloc(sizeof(gchar) * (strlen(title) + 1));
    strcpy(dialog->about_title, title);

    return;
}

/**
 * Establece la version del programa en un AboutDialog
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *version La version del programa
 */
void about_dialog_set_version(AboutDialog *dialog, gchar *version)
{
    // Libera memoria de ser necesario
    if(dialog->about_version != NULL)
    {
        free(dialog->about_version);
    }

    // Establece la versión
    dialog->about_version = malloc(sizeof(gchar) * (strlen(version) + 1));
    strcpy(dialog->about_version, version);

    return;
}

/**
 * Establece un comentario en la ventana de un AboutDialog
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *comments El comentario a agregar
 */
void about_dialog_set_comments(AboutDialog *dialog, gchar *comments)
{
    // Libera memoria de ser necesario
    if(dialog->about_comments != NULL)
    {
        free(dialog->about_comments);
    }

    // Esctablece el comentario
    dialog->about_comments = malloc(sizeof(gchar) * (strlen(comments) + 1));
    strcpy(dialog->about_comments, comments);

  return;
}

/**
 * Establece el copyright del programa en un AboutDialog
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *copyright El mensaje de copyright a agregar
 */
void about_dialog_set_copyright(AboutDialog *dialog, gchar *copyright)
{
    // Libera memoria de ser necesario
    if(dialog->about_copyright != NULL)
    {
        free(dialog->about_copyright);
    }

    // Establece el mensaje de copyright
    dialog->about_copyright = malloc(sizeof(gchar) * (strlen(copyright) + 1));
    strcpy(dialog->about_copyright, copyright);

    return;
}

/**
 * Agrega una página a la sección de créditos de un AboutDialog, no pueden haber más de 3 páginas en un AboutDialog
 * 
 * @param *dialog El AboutDialog a modificar
 * @param *page La página a agregar
 */
void about_dialog_add_page(AboutDialog *dialog, AboutDialogPage *page)
{
    // si hay más de 3 páginas, regresa un error
    if(dialog->credit_pages_count == 3)
    {
        g_warning("About Dialog credits already contains 3 pages, the page will not be added...");
    }
    else // de lo contrario, guarda el puntero y suma la cuenta de páginas
    {
        dialog->pages[dialog->credit_pages_count] = page;
        dialog->credit_pages_count++;
    }

    return;
}

/**
 * Ejecución de un AboutDialog
 * 
 * @param *dialog El AboutDialog a ejecutar
 */
void about_dialog_run(AboutDialog *dialog)
{
    GtkWidget *hBox, *label, *button;

    gchar *line;
    gint size = 1;
    gint f = 0;

    // coloca el logo si se estableció uno
    if(dialog->about_logo != NULL)
    {
        gtk_box_pack_start(GTK_BOX(dialog->mBox), dialog->about_logo, FALSE, FALSE, 0);
    }

    // Primera línea (título + versión)

    // almacena el tamaño de la cadena que se va a necesitar
    if(dialog->about_title != NULL)
    {
        size += strlen(dialog->about_title);

        f++;
    }

    if(dialog->about_version != NULL)
    {
        size += strlen(dialog->about_version);

        f++;
        f*=-1;
    }

    // Si existe primera línea
    if(size > 1)
    {
        // crea una cadena del tamaño exacto considerando el formato a darle
        line = malloc(sizeof(gchar) * (size + strlen("<span weight='bold' size='xx-large'></span>")));
        sprintf(line, "<span weight='bold' size='xx-large'>");

        // según los campos presentes, coloca los valores correspondientes
        switch(f)
        {
            case 1: // solo título
                strcat(line, dialog->about_title);
            break;
            case -2: // título y version
                strcat(line, dialog->about_title);
                strcat(line, " ");
                strcat(line, dialog->about_version);
            break;
            case -1: // solo version
                strcat(line, dialog->about_version);
            break;
        }

        // cierra el formato
        strcat(line, "</span>");

        // coloca la cadena en la ventana y libera memoria
        label = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(label), line);
            gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);

        free(line);
    }

    // si hay comentarios, los coloca
    if(dialog->about_comments != NULL)
    {
        label = gtk_label_new(dialog->about_comments);
            gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);
    }

    // si hay copyright, lo coloca con formato
    if(dialog->about_copyright != NULL)
    {
        line = malloc(sizeof(gchar) * (strlen(dialog->about_copyright) + strlen("<span size='small'></span>")));
        sprintf(line, "<span size='small'>%s</span>", dialog->about_copyright);
            
        label = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(label), line);
            gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);
    }

    // botones
    hBox = gtk_hbox_new(TRUE, 40);
        gtk_box_pack_end(GTK_BOX(dialog->vBox), hBox, FALSE, TRUE, 0);

    // botón cerrar
    button = gtk_button_new();
        gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_on_close), dialog);
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON));
        gtk_button_set_label(GTK_BUTTON(button), "Cerrar");
        gtk_box_pack_end(GTK_BOX(hBox), button, FALSE, TRUE, 0);

    // botón créditos (solo si hay páginas de créditos)
    if(dialog->credit_pages_count)
    {
        button = gtk_button_new();
            gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_on_credits), dialog);
            gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_ABOUT, GTK_ICON_SIZE_BUTTON));
            gtk_button_set_label(GTK_BUTTON(button), "Créditos");
            gtk_box_pack_start(GTK_BOX(hBox), button, FALSE, TRUE, 0);
    }

    // muestra los widgets deseados y corre el GtkDialog
    gtk_widget_show_all(dialog->dialog->vbox);
    gtk_widget_hide(dialog->dialog->action_area);
    gtk_dialog_run(dialog->dialog);

    // libera memoria
    about_dialog_destroy(dialog);

    return;
}

/**
 * Libera memoria al finalizar el uso de un AboutDialog
 * 
 * @param *dialog El AboutDialog a liberar
 */
void about_dialog_destroy(AboutDialog *dialog)
{
    gint i = 0;

    // elimina la ventana
    gtk_widget_destroy(dialog->dialog_pointer);

    // libera memoria del título de ser necesario
    if(dialog->about_title != NULL)
    {
        free(dialog->about_title);
    }

    // libera memoria de la versión de ser necesario
    if(dialog->about_version != NULL)
    {
        free(dialog->about_version);
    }

    // libera memoria de los comentarios de ser necesario
    if(dialog->about_comments != NULL)
    {
        free(dialog->about_comments);
    }

    // libera memoria del copyright de ser necesario
    if(dialog->about_copyright != NULL)
    {
        free(dialog->about_copyright);
    }

    // libera las páginas de créditos que se hayan creado
    for(i = 0; i < dialog->credit_pages_count; i++)
    {
        free(dialog->pages[i]->page_name);
        free(dialog->pages[i]->page_content);
        free(dialog->pages[i]);
    }

    // libera la estructura
    free(dialog);

    return;
}

/**
 * Al presional el botón Créditos de un AboutDialog
 * 
 * @param *widget El botón presionado
 * @param data el AboutDialog como gpointer
 */
void about_dialog_on_credits(GtkWidget *widget, gpointer data)
{
    AboutDialog *about = (AboutDialog *) data;

    GtkWidget *dialog, *vBox, *hBox, *button, *iButton;
    GtkWidget *notebook, *label, *child;
    GtkTextBuffer *buffer;

    gint i = 0;

    // Crea un nuevo GtkDiaog para mostrar los créditos
    dialog = gtk_dialog_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "Créditos");
        gtk_window_set_default_size(GTK_WINDOW(dialog), 360, 260);

    // Botón interno
    iButton = gtk_dialog_add_button(GTK_DIALOG(dialog), "\0", GTK_RESPONSE_CLOSE);

    // contenedor de las páginas
    vBox = gtk_vbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 10);

    hBox = gtk_hbox_new(TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 0);

    // visualizador de las páginas
    notebook = gtk_notebook_new();
        gtk_box_pack_start(GTK_BOX(hBox), notebook, TRUE, TRUE, 10);
        gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);

    // inserta todas las páginas del AboutDialog
    for(i = 0; i < about->credit_pages_count; i++)
    {
        // la página
        child = gtk_text_view_new();
            gtk_text_view_set_editable(GTK_TEXT_VIEW(child), FALSE);
            gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(child), FALSE);

        // el título de la sección y su contenido
        label = gtk_label_new(about->pages[i]->page_name);
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(child));
            gtk_text_buffer_set_text(buffer, about->pages[i]->page_content, -1);
            gtk_notebook_append_page(GTK_NOTEBOOK(notebook), child, label);
    }

    // botón cerrar
    hBox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, 0);

    button = gtk_button_new();
        gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_credits_on_close), iButton);
        gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON));
        gtk_button_set_label(GTK_BUTTON(button), "Cerrar");
        gtk_box_pack_end(GTK_BOX(hBox), button, FALSE, FALSE, 10);
        gtk_widget_set_size_request(button, 85, -1);

    // muestra los widgets necesarios
    gtk_widget_show_all(dialog);
    gtk_widget_hide(GTK_DIALOG(dialog)->action_area);

    // muestra la ventana y la destruye al aterminar
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return;
}

/**
 * Al cerrar la ventana de créditos
 * 
 * @param *widget El botón presionado
 * @param data El botón interno para cerrar la ventana
 */
void about_dialog_credits_on_close(GtkWidget *widget, gpointer data)
{
  GtkWidget *button = (GtkWidget *) data;

  gtk_button_clicked(GTK_BUTTON(button));

  return;
}

/**
 * Al cerrar el AboutDialog
 * 
 * @param *widget El botón presionado
 * @param data El AboutDialog
 */
void about_dialog_on_close(GtkWidget *widget, gpointer data)
{
	AboutDialog *dialog = (AboutDialog *) data;

	gtk_button_clicked(GTK_BUTTON(dialog->iButton));

	return;
}