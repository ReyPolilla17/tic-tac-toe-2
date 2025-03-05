/**
 * @file helpDialog.h
 * 
 * @brief Contiene todas las funciones necesarias para crear un widget de ventana de ayuda
 * 
 * @author Luis Julián Zamora Treviño
 * @date 15/10/2024
 */
#include "gato.h"

/**
 * Creación del widget
 * 
 * @param *title El título de la ventana
 * 
 * @returns HelpDialog * - Puntero a la alocación de memoria del widget
 */
HelpDialog *help_dialog_new(gchar *title)
{
	HelpDialog *new; // el puntero a regresar
	GtkWidget *mhBox, *mBox, *hBox, *vBox, *eBox; // widgets auxiliares
	GdkColor color;

	// obtiene el color del fondo (en versiones futuras podría hacer esto customizable)
	gdk_color_parse("#4B6983", &color);

	// aloca el nuevo objeto
	new = (HelpDialog *) malloc(sizeof(HelpDialog));

	new->current = NULL; // lista dinámica

	// tamaños minimos por defecto
	new->maxTitH = 56;
	new->maxConH = 50;
	new->maxDiaW = 270;
	
	// dialog window sobre la que se trabaja
	new->dialog_pointer = gtk_dialog_new();

	// por practicidad, guarda el widget como dialog y le establece el titulo deseado
	new->dialog = GTK_DIALOG(new->dialog_pointer);
		gtk_window_set_title(GTK_WINDOW(new->dialog_pointer), title);
	
	mhBox = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(new->dialog->vbox), mhBox, TRUE, TRUE, 10);

	mBox = gtk_vbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(mhBox), mBox, TRUE, TRUE, 10);

	
	// Espacio en el que irá el titulo y el contenido de la ventana de ayuda
	eBox = gtk_event_box_new();
		gtk_widget_modify_bg(eBox, GTK_STATE_NORMAL, &color);
		gtk_widget_modify_bg(eBox, GTK_STATE_INSENSITIVE, &color);
		gtk_box_pack_start(GTK_BOX(mBox), eBox, TRUE, TRUE, 0);

	vBox = gtk_vbox_new(FALSE, 0);
		gtk_container_add(GTK_CONTAINER(eBox), vBox);
	
	hBox = gtk_hbox_new(FALSE, 10);
		gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, 6);

	// titulo de la ventana de ayuda
	new->tLabel = gtk_label_new(NULL);
		gtk_label_set_line_wrap(GTK_LABEL(new->tLabel), TRUE);
		gtk_box_pack_start(GTK_BOX(hBox), new->tLabel, FALSE, TRUE, 5);

	hBox = gtk_hbox_new(TRUE, 0);
		gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 1);

	// espacio en el que irá el contenido de la ventana de ayuda
	eBox = gtk_event_box_new();
		gtk_box_pack_start(GTK_BOX(hBox), eBox, TRUE, TRUE, 1);
	
	new->cBox = gtk_vbox_new(FALSE, 0);
		gtk_container_add(GTK_CONTAINER(eBox), new->cBox);
	
	hBox = gtk_hbox_new(FALSE, 10);
		gtk_box_pack_start(GTK_BOX(new->cBox), hBox, FALSE, FALSE, 20);
	
	// contenido de la ventana de ayuda
	new->cLabel = gtk_label_new(NULL);
		gtk_label_set_line_wrap(GTK_LABEL(new->cLabel), TRUE);
		gtk_box_pack_start(GTK_BOX(hBox), new->cLabel, FALSE, FALSE, 20);	

	// botones de la ventana de ayuda
	hBox = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(new->dialog->vbox), hBox, FALSE, TRUE, 0);

	// boton cerrar
	new->cButton = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(new->cButton), "clicked", GTK_SIGNAL_FUNC(help_dialog_on_close), new);
		gtk_button_set_image(GTK_BUTTON(new->cButton), gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON));
		gtk_button_set_label(GTK_BUTTON(new->cButton), "Cerrar");
		gtk_widget_set_size_request(new->cButton, 80, 32);
		gtk_box_pack_start(GTK_BOX(hBox), new->cButton, FALSE, FALSE, 10);

	// boton siguiente
	new->nButton = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(new->nButton), "clicked", GTK_SIGNAL_FUNC(help_dialog_on_next), new);
		gtk_button_set_image(GTK_BUTTON(new->nButton), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON));
		gtk_widget_set_size_request(new->nButton, 80, 32);
		gtk_widget_set_sensitive(new->nButton, FALSE);
		gtk_box_pack_end(GTK_BOX(hBox), new->nButton, FALSE, FALSE, 10);
	
	// boton anterior
	new->pButton = gtk_button_new();
		gtk_signal_connect(GTK_OBJECT(new->pButton), "clicked", GTK_SIGNAL_FUNC(help_dialog_on_past), new);
		gtk_button_set_image(GTK_BUTTON(new->pButton), gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON));
		gtk_widget_set_size_request(new->pButton, 80, 32);
		gtk_widget_set_sensitive(new->pButton, FALSE);
		gtk_box_pack_end(GTK_BOX(hBox), new->pButton, FALSE, FALSE, 0);
	
	// botón interno
	new->iButton = gtk_dialog_add_button(new->dialog, "\0", GTK_RESPONSE_CLOSE);

	gtk_widget_show_all(new->dialog_pointer);
	gtk_widget_hide(new->dialog->action_area); // esconde el botón interno
	
	return new;
}

/**
 * Agrega una página al HelpDialog
 * 
 * @param *dialog El widget al que agregarle la página
 * @param *title El titulo de la página
 * @param *content El contenido de la página
 * 
 * @returns void
 */
void help_dialog_add_page(HelpDialog *dialog, gchar *title, gchar *content)
{
	HelpPage *temp, *new;

	GtkRequisition titleSize, contentSize;

	// obtiene la longitud del titulo y del contenido
	int titleL = strlen(title);
	int contentL = strlen(content);
	
	// crea una página nueva
	new = (HelpPage *) malloc(sizeof(HelpPage));
	new->next = NULL;

	// como se les aplica un formato a las cadenas, su espacio debe alocarse un poco más grande que el tamaño de las cadenas originales
	new->title = (gchar *) malloc(sizeof(gchar) * (titleL + 73));
	new->content = (gchar *) malloc(sizeof(gchar) * (contentL + 14));

	// guarda las cadenas con el formato deseado
	sprintf(new->title, "<span foreground='white' weight='bold' size='x-large'><big>%s</big></span>", title);
	sprintf(new->content, "<span>%s</span>", content);

	// coloca la página en el widget para comparar tamaños
	gtk_label_set_markup(GTK_LABEL(dialog->tLabel), new->title);
	gtk_label_set_markup(GTK_LABEL(dialog->cLabel), new->content);
	
	gtk_widget_size_request(dialog->tLabel, &titleSize);
	gtk_widget_size_request(dialog->cBox, &contentSize);

	// si el titulo termina siendo más grande que el tamaño más grande anterior, lo reemplaza
	if(titleSize.height > dialog->maxTitH)
	{
		dialog->maxTitH	= titleSize.height;
	}

	// si el contenido termina siendo más grande que el tamaño más grande anterior, lo reemplaza
	if(contentSize.height > dialog->maxConH)
	{
		dialog->maxConH	= contentSize.height;
	}

	// si el ancho en general es más granque el el ancho anterior, lo reemplaza
	if(titleSize.width > dialog->maxDiaW)
	{
		dialog->maxDiaW	= titleSize.width;
	}
	else if(contentSize.width > dialog->maxDiaW)
	{
		dialog->maxDiaW	= contentSize.width;
	}

	// coloca la página al final de la lista dinámica
	temp = dialog->current;

	if(temp != NULL)
	{
		while(temp->next != NULL)
		{
			temp = temp->next;
		}

		temp->next = new;
		new->ant = temp;
	}
	else
	{
		new->ant = NULL;
		dialog->current = new;
	}

	return;
}

/**
 * Agrega páginas a un HelpDialog usando el contenido de un archivo de texto
 * 
 * El indicador de fin de sección debe ir en su propia linea, indicando el final tanto de titulos como de contenidos.
 * El último contenido puede o no tener un indicador de fin de sección.
 * 
 * @param *dialog El widget a modificar
 * @param *file La ruta del archivo a leer
 * @param *separator El indicador de fin de sección
 * 
 * @returns void
 */
void help_dialog_populate_from_file(HelpDialog *dialog, gchar *file, gchar *separator)
{
	FILE *fp;

	gchar *texts[2];
	
	gchar a = 0;

	gint start = 0;
	gint curr = 0;
	gint textUsage = 0;
	gint sepCount = 0;
	gint sepSize = strlen(separator);

	// intenta abrir el archivo, si falla, muestra una advertencia
	fp = fopen(file, "rb");
	
	if(fp == NULL)
	{
		g_warning("'%s' file not found, no pages will be aded to the Help Dialog...", file);
		return;
	}

	// leyendo todo el archivo caracter por caracter
	while(!feof(fp))
	{
		fread(&a, sizeof(gchar), 1, fp);

		// si el caracter coincide con el caracter del fin de sección
		if(a == separator[sepCount])
		{
			// si es el primer caracter del fin de sección
			if(!sepCount)
			{
				// guarda la posición antes de empezar a leer el fin de sección
                fseek(fp, -2 * sizeof(gchar), SEEK_CUR);
				curr = ftell(fp);
                fseek(fp, 2 * sizeof(gchar), SEEK_CUR);
			}

			// aumenta la cuenta de coincidencias
			sepCount++;

			// si coincide todo el fin de sección
			if(sepCount == sepSize)
			{
				// reinicia la cuenta
				sepCount = 0;

				// aloca la cadena correspondiente del tamaño de la sección
				texts[textUsage] = (gchar *) malloc(sizeof(gchar) * (curr - start + 1));
				
				// guarda la sección en la cadena
				fseek(fp, sizeof(gchar) * (start - curr - sepSize - 1), SEEK_CUR);
				fread(texts[textUsage], sizeof(gchar), (curr - start + 1), fp);
				texts[textUsage][curr - start] = 0;
				
				fseek(fp, sizeof(gchar) * (sepSize + 1), SEEK_CUR);

				// si la sección corresponde al contenido
				if(textUsage)
				{
					// agrega una página al widget y libera la memoria
					help_dialog_add_page(dialog, texts[0], texts[1]);

					free(texts[0]);
					free(texts[1]);
				}

				// alterna el tipo de sección
				textUsage += 1;
				textUsage %= 2;

				// guarda esa posición como el inicio de la siguiente seccion
				start = ftell(fp);
			}
		}
		// de lo contrario, reinicia el contador
		else if(sepCount)
		{
			sepCount = 0;
		}
	}

	// si queda un titulo sin contenido (se asume que no se colocó el último fin de sección)
	if(textUsage)
    {
		// guarda la posición
        curr = ftell(fp); 
        
		// aloca una cadena del tamaño correcto
        texts[textUsage] = (gchar *) malloc(sizeof(gchar) * (curr - start + 1));
        
		// guarda el contenido de la sección
        fseek(fp, sizeof(gchar) * (start - curr), SEEK_CUR);
        fread(texts[textUsage], sizeof(gchar), (curr - start + 1), fp);
        texts[textUsage][curr - start] = 0;

		// agrega la página y libera la memoria
		help_dialog_add_page(dialog, texts[0], texts[1]);

		free(texts[0]);
		free(texts[1]);
    }

	fclose(fp);

	return;
}

/**
 * Muestra el HelpDialog
 * 
 * @param *dialog El widget a mostrar
 * 
 * @returns void
 */
void help_dialog_run(HelpDialog *dialog)
{
	// si el widget tiene páginas
	if(dialog->current != NULL)
	{
		// regresa a la primera página
		while(dialog->current->ant != NULL)
		{
			dialog->current = dialog->current->ant;
		}

		// Si el widget tiene más de una página
		if(dialog->current->next != NULL)
		{
			// habilita el botón siguiente
			gtk_widget_set_sensitive(dialog->nButton, TRUE);
		}
		// de lo contrario
		else
		{
			// elimina los botones de anterior y siguiente
			gtk_widget_destroy(dialog->nButton);
			gtk_widget_destroy(dialog->pButton);
		}
		
		// fija los tamaños al máximo tamaño de cada elemento
		gtk_widget_set_size_request(dialog->tLabel, dialog->maxDiaW, dialog->maxTitH);
		gtk_widget_set_size_request(dialog->cBox, dialog->maxDiaW, dialog->maxConH);

		// coloca el titulo y el contenido de la primera página
		gtk_label_set_markup(GTK_LABEL(dialog->tLabel), dialog->current->title);
		gtk_label_set_markup(GTK_LABEL(dialog->cLabel), dialog->current->content);

		// muestra la ventana
		gtk_dialog_run(dialog->dialog);
	}
	// de lo contrario, muestra error
	else
	{
		g_warning("Trying to display an empty Help Dialog, the widget won't be displayed...");
	}

	// destruye el widget al momento de cerrarlo
	help_dialog_destroy(dialog);

	return;
}

/**
 * Destruye el HelpDialog
 * 
 * @param *dialog El widget a destruir
 * 
 * @returns void
 */
void help_dialog_destroy(HelpDialog *dialog)
{
	HelpPage *temp;

	// elimina la ventana
	gtk_widget_destroy(dialog->dialog_pointer);

	// regresa a la primera página
	while(dialog->current != NULL && dialog->current->ant != NULL)
	{
		dialog->current = dialog->current->ant;
	}

	// libera cada página del widget
	while(dialog->current != NULL)
	{
		temp = dialog->current;

		dialog->current = temp->next;

		free(temp->title);
		free(temp->content);
		free(temp);
	}

	// libera el widget
	free(dialog);
	
	return;
}

/**
 * Al presionar el botón anterior de un HelpDialog
 * 
 * @param *widget el widget que genera el evento
 * @param data el puntaro del HelpDialog
 * 
 * @returns void
 */
void help_dialog_on_past(GtkWidget *widget, gpointer data)
{
	// hace un cast para poder trabajar con el HelpDialog
	HelpDialog *dialog = (HelpDialog *) data;

	// si hay una página anterior
	if(dialog->current->ant != NULL)
	{
		// se recorre a la página anterior
		dialog->current = dialog->current->ant;
		
		// si no hay una página antes de la pagina actual
		if(dialog->current->ant == NULL)
		{
			// desactiva el botón
			gtk_widget_set_sensitive(dialog->pButton, FALSE);
		}

		// activa el botón de siguiente
		gtk_widget_set_sensitive(dialog->nButton, TRUE);

		// coloca el titulo y el contenido de la pagina actual
		gtk_label_set_markup(GTK_LABEL(dialog->tLabel), dialog->current->title);
		gtk_label_set_markup(GTK_LABEL(dialog->cLabel), dialog->current->content);
	}
	// de lo contrario
	else
	{
		// desactiva el botón anterior (just in case)
		gtk_widget_set_sensitive(dialog->pButton, FALSE);
	}
	
	return;
}

/**
 * Al presionar el botón siguiente en un HelpDialog
 * 
 * @param *widget El widget que genera el evento
 * @param data El punteor del HelpDialog
 * 
 * @returns void
 */
void help_dialog_on_next(GtkWidget *widget, gpointer data)
{
	// hace un cast para poder trabajar con el HelpDialog
	HelpDialog *dialog = (HelpDialog *) data;

	// si hay una página siguiente
	if(dialog->current->next != NULL)
	{
		// se recorre a la siguiente página
		dialog->current = dialog->current->next;
		
		// si la pagina actual no tiene siguiente página
		if(dialog->current->next == NULL)
		{
			// desactiva el botón siguiente
			gtk_widget_set_sensitive(dialog->nButton, FALSE);
		}

		// activa el botón anterior
		gtk_widget_set_sensitive(dialog->pButton, TRUE);

		// coloca el titulo y el contenido de la pagina actual
		gtk_label_set_markup(GTK_LABEL(dialog->tLabel), dialog->current->title);
		gtk_label_set_markup(GTK_LABEL(dialog->cLabel), dialog->current->content);
	}
	// de lo contrario
	else
	{
		// desactiva el botón siguiente (just in case)
		gtk_widget_set_sensitive(dialog->nButton, FALSE);
	}

	return;
}

/**
 * Al presionar el boton cerrar de un HelpDialog
 * 
 * @param *widget El widget que genera el evento
 * @param data El puntero del HelpDialog
 * 
 * @returns void
 */
void help_dialog_on_close(GtkWidget *widget, gpointer data)
{
	// hace un cast para poder trabajar con el HelpDialog
	HelpDialog *dialog = (HelpDialog *) data;

	// cierra la ventana
	gtk_button_clicked(GTK_BUTTON(dialog->iButton));

	return;
}