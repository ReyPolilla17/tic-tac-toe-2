#include "mydialogs.h"

AboutDialog *about_dialog_new(const gchar *title)
{
  AboutDialog *new;
  GtkWidget *hBox;

  new = (AboutDialog *)malloc(sizeof(AboutDialog));

  new->dialog_pointer = gtk_dialog_new();
  new->about_comments = NULL;
  new->about_copyright = NULL;
  new->about_logo = NULL;
  new->about_title = NULL;
  new->about_version = NULL;
  new->credit_pages_count = 0;
  
  new->dialog = GTK_DIALOG(new->dialog_pointer);
    gtk_window_set_title(GTK_WINDOW(new->dialog_pointer), title);

  hBox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(new->dialog->vbox), hBox, TRUE, TRUE, 10);
  
  new->vBox = gtk_vbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(hBox), new->vBox, TRUE, TRUE, 10);

  new->mBox = gtk_vbox_new(FALSE, 10);
		gtk_box_pack_start(GTK_BOX(new->vBox), new->mBox, TRUE, TRUE, 0);

  new->iButton = gtk_dialog_add_button(new->dialog, "\0", GTK_RESPONSE_CLOSE);

  return new;
}

void about_dialog_set_logo(AboutDialog *dialog, GdkPixbuf *logo)
{
  if(dialog->about_logo != NULL)
  {
    g_free(dialog->about_logo);
  }
  
  dialog->about_logo = gtk_image_new_from_pixbuf(logo);

  return;
}

void about_dialog_set_title(AboutDialog *dialog, gchar *title)
{
  if(dialog->about_title != NULL)
  {
    free(dialog->about_title);
  }

  dialog->about_title = malloc(sizeof(gchar) * (strlen(title) + 1));
  strcpy(dialog->about_title, title);

  return;
}

void about_dialog_set_version(AboutDialog *dialog, gchar *version)
{
  if(dialog->about_version != NULL)
  {
    free(dialog->about_version);
  }

  dialog->about_version = malloc(sizeof(gchar) * (strlen(version) + 1));
  strcpy(dialog->about_version, version);

  return;
}

void about_dialog_set_comments(AboutDialog *dialog, gchar *comments)
{
  if(dialog->about_comments != NULL)
  {
    free(dialog->about_comments);
  }

  dialog->about_comments = malloc(sizeof(gchar) * (strlen(comments) + 1));
  strcpy(dialog->about_comments, comments);

  return;
}

void about_dialog_set_copyright(AboutDialog *dialog, gchar *copyright)
{
  if(dialog->about_copyright != NULL)
  {
    free(dialog->about_copyright);
  }

  dialog->about_copyright = malloc(sizeof(gchar) * (strlen(copyright) + 1));
  strcpy(dialog->about_copyright, copyright);

  return;
}

void about_dialog_add_page(AboutDialog *dialog, AboutDialogPage *page)
{
  if(dialog->credit_pages_count == 3)
  {
    g_warning("About Dialog credits already contains 3 pages, the page will not be added...");
  }
  else
  {
    dialog->pages[dialog->credit_pages_count] = page;
    dialog->credit_pages_count++;
  }

  return;
}

AboutDialogPage *about_dialog_page_new(const gchar *name)
{
  AboutDialogPage *new = malloc(sizeof(AboutDialogPage));

  new->page_name = malloc(sizeof(gchar) * (strlen(name) + 1));
  strcpy(new->page_name, name);

  new->page_content = NULL;

  return new;
}

void about_dialog_page_add_line(AboutDialogPage *page, const gchar *line)
{
  gchar *temp;

  if(page->page_content == NULL)
  {
    page->page_content = malloc(sizeof(gchar) * (strlen(line) + 1));
    strcpy(page->page_content, line);
  }
  else
  {
    temp = page->page_content;

    page->page_content = malloc(sizeof(gchar) * (strlen(temp) + strlen(line) + 2));
    sprintf(page->page_content, "%s\n%s", temp, line);
  }

  return;
}

void about_dialog_run(AboutDialog *dialog)
{
  GtkWidget *hBox, *label, *button;

  gchar *line;
  gint size = 1;
  gint f = 0;

  if(dialog->about_logo != NULL)
  {
    gtk_box_pack_start(GTK_BOX(dialog->mBox), dialog->about_logo, FALSE, FALSE, 0);
  }

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

  if(size > 1)
  {
    line = malloc(sizeof(gchar) * (size + strlen("<span weight='bold' size='xx-large'></span>")));
    sprintf(line, "<span weight='bold' size='xx-large'>");

    switch(f)
    {
      case 1:
        strcat(line, dialog->about_title);
        break;
      case -2:
        strcat(line, dialog->about_title);
        strcat(line, " ");
        strcat(line, dialog->about_version);
        break;
      case -1:
        strcat(line, dialog->about_version);
        break;
    }

    strcat(line, "</span>");

    label = gtk_label_new(NULL);
      gtk_label_set_markup(GTK_LABEL(label), line);
      gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);
    
    free(line);
  }

  if(dialog->about_comments != NULL)
  {
    label = gtk_label_new(dialog->about_comments);
      gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);
  }
    
  if(dialog->about_copyright != NULL)
  {
    line = malloc(sizeof(gchar) * (strlen(dialog->about_copyright) + strlen("<span size='small'></span>")));
    sprintf(line, "<span size='small'>%s</span>", dialog->about_copyright);
      
    label = gtk_label_new(NULL);
      gtk_label_set_markup(GTK_LABEL(label), line);
      gtk_box_pack_start(GTK_BOX(dialog->mBox), label, FALSE, FALSE, 0);
  }

  hBox = gtk_hbox_new(TRUE, 40);
    gtk_box_pack_end(GTK_BOX(dialog->vBox), hBox, FALSE, TRUE, 0);
  
  button = gtk_button_new();
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_on_close), dialog);
    gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON));
    gtk_button_set_label(GTK_BUTTON(button), "Cerrar");
    gtk_box_pack_end(GTK_BOX(hBox), button, FALSE, TRUE, 0);
    
  if(dialog->credit_pages_count)
  {
    button = gtk_button_new();
      gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_on_credits), dialog);
      gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_ABOUT, GTK_ICON_SIZE_BUTTON));
      gtk_button_set_label(GTK_BUTTON(button), "Créditos");
      gtk_box_pack_start(GTK_BOX(hBox), button, FALSE, TRUE, 0);
  }

  gtk_widget_show_all(dialog->dialog->vbox);
	gtk_widget_hide(dialog->dialog->action_area);
  gtk_dialog_run(dialog->dialog);

  about_dialog_destroy(dialog);

  return;
}

void about_dialog_destroy(AboutDialog *dialog)
{
  gint i = 0;

  gtk_widget_destroy(dialog->dialog_pointer);
  
  if(dialog->about_title != NULL)
  {
    free(dialog->about_title);
  }

  if(dialog->about_version != NULL)
  {
    free(dialog->about_version);
  }
  
  if(dialog->about_comments != NULL)
  {
    free(dialog->about_comments);
  }

  if(dialog->about_copyright != NULL)
  {
    free(dialog->about_copyright);
  }

  for(i = 0; i < dialog->credit_pages_count; i++)
  {
    free(dialog->pages[i]->page_name);
    free(dialog->pages[i]->page_content);
    free(dialog->pages[i]);
  }

  free(dialog);

  return;
}

void about_dialog_on_credits(GtkWidget *widget, gpointer *data)
{
  AboutDialog *about = (AboutDialog *) data;

  GtkWidget *dialog, *vBox, *hBox, *button, *iButton;
  GtkWidget *notebook, *label, *child;
  GtkTextBuffer *buffer;

  gint i = 0;

  dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Créditos");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 360, 260);
  
  iButton = gtk_dialog_add_button(GTK_DIALOG(dialog), "\0", GTK_RESPONSE_CLOSE);
  
  vBox = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 10);

  hBox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 0);
    
  notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(hBox), notebook, TRUE, TRUE, 10);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    
  for(i = 0; i < about->credit_pages_count; i++)
  {
    child = gtk_text_view_new();
      gtk_text_view_set_editable(GTK_TEXT_VIEW(child), FALSE);
      gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(child), FALSE);
    
    label = gtk_label_new(about->pages[i]->page_name);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(child));
      gtk_text_buffer_set_text(buffer, about->pages[i]->page_content, -1);
      gtk_notebook_append_page(GTK_NOTEBOOK(notebook), child, label);
  }

  hBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, 0);
  
  button = gtk_button_new();
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(about_dialog_credits_on_close), iButton);
    gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON));
    gtk_button_set_label(GTK_BUTTON(button), "Cerrar");
    gtk_box_pack_end(GTK_BOX(hBox), button, FALSE, FALSE, 10);
    gtk_widget_set_size_request(button, 85, -1);
    
  gtk_widget_show_all(dialog);
  gtk_widget_hide(GTK_DIALOG(dialog)->action_area);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
    
  return;
}

void about_dialog_credits_on_close(GtkWidget *widget, gpointer data)
{
  GtkWidget *button = (GtkWidget *) data;

  gtk_button_clicked(GTK_BUTTON(button));

  return;
}

void about_dialog_on_close(GtkWidget *widget, gpointer data)
{
	AboutDialog *dialog = (AboutDialog *) data;

	gtk_button_clicked(GTK_BUTTON(dialog->iButton));

	return;
}