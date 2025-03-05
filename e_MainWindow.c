#include "gato.h"

void comoJugar(GtkWidget *widget, gpointer data)
{
    HelpDialog *dialog;

	dialog = help_dialog_new("Cómo Jugar?");

	help_dialog_populate_from_file(dialog, "./MEDIA/manual.txt", "END\n");

	help_dialog_run(dialog);

    return;
}

void stopTheApp(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();

	return;
}