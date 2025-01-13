gato.out: gato.o g_MainWindow.o e_MainWindow.o helpDialog.o
	gcc -g -Wall -o gato.out gato.o g_MainWindow.o e_MainWindow.o helpDialog.o `pkg-config --cflags --libs gtk+-2.0`

gato.o: gato.c
	gcc -g -Wall -c gato.c `pkg-config --cflags --libs gtk+-2.0`

g_MainWindow.o: g_MainWindow.c
	gcc -g -Wall -c g_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

e_MainWindow.o: e_MainWindow.c
	gcc -g -Wall -c e_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

helpDialog.o: helpDialog.c
	gcc -g -Wall -c helpDialog.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm *.o
	rm *.out
