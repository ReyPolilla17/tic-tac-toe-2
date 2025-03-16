gato.out: gato.o e_MainWindow.o f_MainWindow.o g_MainWindow.o myDialogs.o helpDialog.o aboutDialog.o confirmationDialog.o
	gcc -g -Wall -o gato.out gato.o e_MainWindow.o f_MainWindow.o g_MainWindow.o myDialogs.o helpDialog.o aboutDialog.o confirmationDialog.o `pkg-config --cflags --libs gtk+-2.0`

gato.o: gato.c
	gcc -g -Wall -c gato.c `pkg-config --cflags --libs gtk+-2.0`

e_MainWindow.o: e_MainWindow.c
	gcc -g -Wall -c e_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

f_MainWindow.o: f_MainWindow.c
	gcc -g -Wall -c f_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

g_MainWindow.o: g_MainWindow.c
	gcc -g -Wall -c g_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

myDialogs.o: myDialogs.c
	gcc -g -Wall -c myDialogs.c `pkg-config --cflags --libs gtk+-2.0`

helpDialog.o: helpDialog.c
	gcc -g -Wall -c helpDialog.c `pkg-config --cflags --libs gtk+-2.0`

aboutDialog.o: aboutDialog.c
	gcc -g -Wall -c aboutDialog.c `pkg-config --cflags --libs gtk+-2.0`

confirmationDialog.o: confirmationDialog.c
	gcc -g -Wall -c confirmationDialog.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm *.o
	rm *.out
