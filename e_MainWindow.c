#include "gato.h"

void comoJugar(GtkWidget *widget, gpointer data)
{
    HelpDialog *dialog;

	dialog = help_dialog_new("Como jugar?");

	help_dialog_populate_from_file(dialog, "./MEDIA/manual.txt", "END\n");

	help_dialog_run(dialog);

    return;
}

void StopTheApp(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	return;
}