gato.out: gato.o online.o e_MainWindow.o f_MainWindow.o g_MainWindow.o helpDialog.o aboutDialog.o newGameDialog.o onlineRegisterDialog.o endDialogs.o commonDialogs.o
	gcc -g -Wall -o gato.out gato.o online.o e_MainWindow.o f_MainWindow.o g_MainWindow.o helpDialog.o aboutDialog.o newGameDialog.o endDialogs.o commonDialogs.o `pkg-config --cflags --libs gtk+-2.0` -lnsl -lm -lz -lmysqlclient

gato.o: gato.c
	gcc -g -Wall -c gato.c `pkg-config --cflags --libs gtk+-2.0`

online.o: online.c
	gcc -g -Wall -c online.c `pkg-config --cflags --libs gtk+-2.0`

e_MainWindow.o: e_MainWindow.c
	gcc -g -Wall -c e_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

f_MainWindow.o: f_MainWindow.c
	gcc -g -Wall -c f_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

g_MainWindow.o: g_MainWindow.c
	gcc -g -Wall -c g_MainWindow.c `pkg-config --cflags --libs gtk+-2.0`

helpDialog.o: helpDialog.c
	gcc -g -Wall -c helpDialog.c `pkg-config --cflags --libs gtk+-2.0`

aboutDialog.o: aboutDialog.c
	gcc -g -Wall -c aboutDialog.c `pkg-config --cflags --libs gtk+-2.0`

newGameDialog.o: newGameDialog.c
	gcc -g -Wall -c newGameDialog.c `pkg-config --cflags --libs gtk+-2.0`

onlineRegisterDialog.o: onlineRegisterDialog.c
	gcc -g -Wall -c onlineRegisterDialog.c `pkg-config --cflags --libs gtk+-2.0`

endDialogs.o: endDialogs.c
	gcc -g -Wall -c endDialogs.c `pkg-config --cflags --libs gtk+-2.0`

commonDialogs.o: commonDialogs.c
	gcc -g -Wall -c commonDialogs.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm *.o
	rm *.out
