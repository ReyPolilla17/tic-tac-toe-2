#include "gato.h"

int main(int argc, char *argv[])
{
    JUEGO juego;
    
    int op = 0;

    srand(time(NULL));

    gtk_init(&argc, &argv);
    
    system("clear");

    loadMainWindow(&juego);

    gtk_main();

    return 0;


    do
    {
        printf("Selecicone una opcion:\n");
        printf("1. Nuevo juego.\n");
        printf("2. Cargar partida.\n");
        printf("3. Como jugar.\n");
        printf("4. Salir.\n");

        scanf("%d", &op);
        getchar();
        system("clear");

        switch(op)
        {
            case 1:
            {
                setupNewGame(&juego);
                playGame(&juego);

                break;
            }
            case 2:
            {
                if(loadGame(&juego))
                {
                    playGame(&juego);
                }

                break;
            }
            case 3:
            {
                displayHelp();

                break;
            }
            case 4:
            {
                break;
            }
            default:
            {
                printf("Accion desconocida...\n\n");
                break;
            }
        }
    } while (op != 4);

    return 0;
}

/**
 * Reinicia los valores de la partida anterior y establece una nueva partida segun los que elija el usuario
 * 
 * @param *juego El juego que será reiniciado
 * 
 * @returns void
 */
void setupNewGame(JUEGO *juego)
{
    int vs_ai = validateInput("Deseas jugar contra una computadora?");
    int ran = rand() % 2;
    int jug = 1;

    int i = 0;
    int j = 0;

    // reincia nombre, si es ia, si está en modo dificil de ambos jugadores
    for(i = 0; i < 2; i++)
    {
        juego->partida.jugadores[i].ia = 0;
        juego->partida.jugadores[i].hard_mode = 0;
        juego->partida.jugadores[i].nombre[0] = 0;
    }
    
    // reinicia el tablero de la primera posición del historial
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            juego->partida.historial[0].tablero[i][j] = ' ';
        }
    }

    // establece el primer turno como partida en curso y valido como historial
    juego->partida.historial[0].game_status = 1;
    juego->partida.historial[0].hist_val = 1;

    // reincia el valor maximo del historial y establece el turno como el primero
    juego->partida.turno = 0;
    juego->partida.turnoMax = 0;

    // si se desea jugar contra ia
    if(vs_ai)
    {
        // establece al primer jugador al azar como ia y si el ususario lo decide, activa su modo dificil
        juego->partida.jugadores[ran].ia = 1;
        juego->partida.jugadores[ran].hard_mode = validateInput("Deseas jugar en modo dificil?");

        // coloca el nombre de la ia en ese jugador
        strcpy(juego->partida.jugadores[ran].nombre, IA_NAME);
    }
    // de lo contrario
    else
    {
        // solicita el nombre del primer jugador y lo guarda
        printf("Jugador %d:\n", jug);
        printf("Introduce tu nombre: ");

        scanf("%[^\n]", juego->partida.jugadores[ran].nombre);
        getchar();
        system("clear");

        jug++;
    }

    // mientras ambos usuarios tengan el mismo nombre
    do
    {
        // solicita un nombre para el segundo jugador y lo guarda
        printf("Jugador %d:\n", jug);
        printf("Introduce tu nombre: ");

        scanf("%[^\n]", juego->partida.jugadores[(ran + 1) % 2].nombre);
        getchar();
        system("clear");

        // si ambos jugadores tienen el midmo nombre
        if(!strcmp(juego->partida.jugadores[0].nombre, juego->partida.jugadores[1].nombre))
        {
            // si se está enfrentando a una ia
            if(vs_ai)
            {
                printf("Solo puede haber un %s.\n\n", IA_NAME); // "easter egg"
            }
            // de lo contrario
            else
            {
                printf("No pueden tener el mismo nombre.\n\n");
            }
        }
    } while(!strcmp(juego->partida.jugadores[0].nombre, juego->partida.jugadores[1].nombre));

    return;
}

/**
 * Intenta cargar un archivo binario con la información de la partida
 * 
 * @param *juego El objeto en el que se guardará la información del archivo
 * 
 * @returns int (1 en caso de lectura correcta, 0 en caso de fallo)
 */
int loadGame(JUEGO *juego)
{
    FILE *fp;
    char fileName[1000];
    int success = 0;

    // solicita una ruta de archivo e intenta abrirla
    printf("Introduce el nombre del archivo:\n");
    scanf("%[^\n]", fileName);
    getchar();

    fp = fopen(fileName, "rb");

    // si pudo abrir el archivo
    if(fp != NULL)
    {
        // intenta leer toda la información de la partida de golpe
        success = fread(&juego->partida, sizeof(FUNCIONAL), 1, fp);

        fclose(fp);
        
        // si pudo leer la información
        if(success)
        {
            return 1;
        }
        // de lo contrario
        else
        {
            printf("Archivo corrupto o incompatible...\n\n");
        }
    }
    // de lo contrario
    else
    {
        printf("No se puede acceder al archivo...\n\n");
    }

    return 0;
}

/**
 * Guarda una partida en el estado en el que esté con el nombre que el usuario desee
 * 
 * @param *juego La partida de la que se guarda la información
 */
void saveGame(JUEGO *juego)
{
    FILE *fp;
    char fileName[1000];

    printf("Introduce el nombre del archivo:\n");
    scanf("%[^\n]", fileName);
    getchar();

    fp = fopen(fileName, "wb");

    fwrite(&juego->partida, sizeof(FUNCIONAL), 1, fp);
    fclose(fp);

    return;
}

/**
 * El desarrollo de una partida
 * 
 * @param *juego El objeto sobre el que se desarrolla la partida
 * 
 * @returns void
 */
void playGame(JUEGO *juego)
{
    int winner = 0;
    int op = 0;

    // mientras no se elija regresar al menu
    do
    {
        // revisa el estado de la partida
        winner = checkGame(juego->partida.historial[juego->partida.turno].tablero, ICONS[juego->partida.turno % 2]);

        // si ya no se puede continuar el juego
        if(winner)
        {
            // cambia el estado de la partida a terminado
            juego->partida.historial[juego->partida.turno].game_status = -1;

            // si el juego terminó en empate
            if(winner < 0)
            {
                printf("El juego ha terminado en empate.\n\n");
            }
            // si el jugador anterior ganó
            else
            {
                printf("Ha ganado %s!\n\n", juego->partida.jugadores[(juego->partida.turno + 1) % 2].nombre);
            }

            // imprime el tablero y las opciones disponibles
            printBoard(juego->partida.historial[juego->partida.turno].tablero);

            printf("1. Regresar turno.\n");
            printf("2. Guardar partida.\n");
            printf("3. Regresar al menu.\n");

            scanf("%d", &op);
            getchar();
            system("clear");

            // opciones disponibles
            switch(op)
            {
                // regresdar 1 turno
                case 1:
                {
                    lastTurn(juego);
                    break;
                }
                // guardar partida
                case 2:
                {
                    saveGame(juego);

                    // si el usuario desea regresar al menu tras guardar la partida
                    if(validateInput("Partida guardada!\n\nDeseas regresar al menu?"))
                    {
                        op = 5;
                    }

                    break;
                }
                // regresar al menu
                case 3:
                {
                    // si el usuario desea guardar antes de salir
                    if(validateInput("Deseas guardar la partida antes de salir?"))
                    {
                        saveGame(juego);
                        printf("Partida guardada!\n\n");
                    }

                    op = 5;

                    break;
                }
                default:
                {
                    printf("Accion desconocida...\n\n");
                    break;
                }
            }
        }
        // si el jugador actual no es una ia
        else if(!juego->partida.jugadores[juego->partida.turno % 2].ia)
        {
            // imprime de quien es el turno, el tablero y las opciones disponibles
            printf("Turno de %s\n\n", juego->partida.jugadores[juego->partida.turno % 2].nombre);

            printBoard(juego->partida.historial[juego->partida.turno].tablero);

            printf("1. Poner ficha.\n");
            printf("2. Regresar turno.\n");
            printf("3. Avanzar turno.\n");
            printf("4. Guardar partida.\n");
            printf("5. Regresar al menu.\n");

            scanf("%d", &op);
            getchar();
            system("clear");

            // opciones disponibles
            switch(op)
            {
                // poner ficha
                case 1:
                {
                    playTurn(juego);
                    break;
                }
                // regresar un turno
                case 2:
                {
                    lastTurn(juego);
                    break;
                }
                // avanzar un turno
                case 3:
                {
                    nextTurn(juego);
                    break;
                }
                // guardar partida
                case 4:
                {
                    saveGame(juego);

                    // si se desea regresar al menú
                    if(validateInput("Partida guardada!\n\nDeseas regresar al menu?"))
                    {
                        op = 5;
                    }

                    break;
                }
                // regresar al menú
                case 5:
                {
                    if(validateInput("Deseas guardar la partida antes de salir?"))
                    {
                        saveGame(juego);
                        printf("Partida guardada!\n\n");
                    }

                    break;
                }
                default:
                {
                    printf("Accion desconocida...\n\n");
                    break;
                }
            }
        }
        // si el juego no ha terminado y el jugador actual es una ia
        else if(juego->partida.historial[juego->partida.turno].game_status != -1)
        {
            aiTurn(juego);
        }
    } while(op != 5);
    
    return;
}

/**
 * Juego de un turno de un usuario
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void playTurn(JUEGO *juego)
{
    int place = 0;
    int valid = 0;

    // si el juego no ha terminado
    if(juego->partida.historial[juego->partida.turno].game_status == 1)
    {
        // mientras el usuario no elija una cordenada válida
        do
        {
            // imprime el tablero actual y solicita elegir un lugar
            printBoard(juego->partida.historial[juego->partida.turno].tablero);
            
            printf("Elige un lugar del 0 al 8: ");

            scanf("%d", &place);
            getchar();
            system("clear");

            // si el lugar está fuera de rango
            if(!(place >= 0 && place <= 8))
            {
                printf("Debe ser un lugar entre el 0 y el 8...\n\n");
            }
            // si el lugar está ocupado
            else if(juego->partida.historial[juego->partida.turno].tablero[place / 3][place % 3] != ' ')
            {
                printf("El espacio ya fue ocupado...\n\n");
            }
            // si el lugar es válido
            else
            {
                // genera una nueva instancia para el historial a la que se puede regresar
                logMove(juego, 1);

                // coloca la ficha correspondiente
                juego->partida.historial[juego->partida.turno].tablero[place / 3][place % 3] = ICONS[juego->partida.turno % 2];

                // cierra el bucle
                valid = 1;
            }
        } while(!valid);
    }
    // de lo contrario
    else if(juego->partida.historial[juego->partida.turno].game_status == -1)
    {
        printf("El juego ha temrinado...\n\n");
    }

    return;
}

/**
 * Juego de un turno de la ia
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void aiTurn(JUEGO *juego)
{
    int x = 0;
    int y = 0;

    int i = 0;

    // si es el primer turno de la partida
    if(!juego->partida.turno)
    {
        // no se podrá regresar al tablero vacío
        juego->partida.historial[0].hist_val = 0;
    }

    // genera una nueva instancia para el historial a la que no se puede regresar
    logMove(juego, 0);

    // si la ia esta en modo dificil tirará 2 veces, de lo contrario, solo 1 vez
    for(i = 0; i < 1 + juego->partida.jugadores[(juego->partida.turno + 1) % 2].hard_mode; i++)
    {
        // elije de manera inteligente un espacio en el que tirar
        chooseSpace(juego, &x, &y);

        juego->partida.historial[juego->partida.turno].tablero[x][y] = ICONS[juego->partida.turno % 2];
    }

    return;
}

/**
 * Elección de espacio para tirar de la ia
 * 
 * @param *juego La partida en curso
 * @param *x X en la que colocará la ficha
 * @param *y Y en la que colocará la ficha
 * 
 * @returns void
 */
void chooseSpace(JUEGO *juego, int *x, int *y)
{
    char copy[3][3]; // copia del tablero

    int f_wins[9][2]; // victorias a largo plazo
    int wins = 0; // cuenta de victorias a largo plazo

    int d_spaces[9][2]; // espacios disponibles
    int spaces = 0; // cuenta de espacios disponibles

    int r_place = 0; // lugar aleatorio

    // contadores
    int i = 0;
    int j = 0;

    int i2 = 0;
    int j2 = 0;

    // para cada espacio del tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            // si el lugar está libre
            if(juego->partida.historial[juego->partida.turno].tablero[i][j] == ' ')
            {
                // guarda el espacio disponible
                d_spaces[spaces][0] = i;
                d_spaces[spaces][1] = j;
                spaces ++;

                // crea una copia del tablero actual
                coppyBoard(copy, juego->partida.historial[juego->partida.turno].tablero);

                // coloca una ficha propia en el lugar disponible
                copy[i][j] = ICONS[juego->partida.turno % 2];

                // si la ia gana al colocar esa ficha
                if(checkGame(copy, ICONS[juego->partida.turno % 2]) == 1)
                {
                    // juega en esa posición
                    *x = i;
                    *y = j;

                    return;
                }
                // de lo contrario (revisión de victoria a largo plazo)
                else
                {
                    // para cada espacio en el tablero
                    for(i2 = 0; i2 < 3; i2++)
                    {
                        for(j2 = 0; j2 < 3; j2++)
                        {
                            // si el espacio está disponible
                            if(copy[i2][j2] == ' ')
                            {
                                // coloca una ficha en el espacio
                                copy[i2][j2] = ICONS[juego->partida.turno % 2];

                                // si la ia gana al colocar esa ficha
                                if(checkGame(copy, ICONS[juego->partida.turno % 2]) == 1)
                                {
                                    // guarda la posición que le dará una victoria a futuro
                                    f_wins[wins][0] = i2;
                                    f_wins[wins][1] = j2;

                                    wins++;
                                }

                                // quita la ficha que colocó
                                copy[i2][j2] = ' ';
                            }
                        }
                    }
                }
            }
        }
    }

    // para cada espacio en el tablero
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            // si el espacio está disponible
            if(juego->partida.historial[juego->partida.turno].tablero[i][j] == ' ')
            {
                // copia el tablero
                coppyBoard(copy, juego->partida.historial[juego->partida.turno].tablero);
                
                // coloca una ficha del contrincante
                copy[i][j] = ICONS[(juego->partida.turno + 1) % 2];

                // si el contrincante gana
                if(checkGame(copy, ICONS[(juego->partida.turno + 1) % 2]) == 1)
                {
                    // bloquea esa posición
                    *x = i;
                    *y = j;

                    return;
                }
            }
        }
    }

    // si hay victorias a futuro
    if(wins)
    {
        // elige una victoria futura al azar y juega en ese lugar
        r_place = rand() % wins;

        *x = f_wins[r_place][0];
        *y = f_wins[r_place][1];
    }
    // de lo contrario
    else
    {
        // juega en un espacio disponible al azar
        r_place = rand() % spaces;

        *x = d_spaces[r_place][0];
        *y = d_spaces[r_place][1];
    }

    return;
}

/**
 * Crea una instancia de historial
 * 
 * @param *juego La partuda en curso
 * @param replayable Si el turno perdenece a un usuario o a una ia
 * 
 * @returns void
 */
void logMove(JUEGO *juego, int replayable)
{
    // actualiza el valor máximo de turno
    juego->partida.turno ++;
    juego->partida.turnoMax = juego->partida.turno;

    // establece si se puede volver a jugar ese turno y el estado del juego
    juego->partida.historial[juego->partida.turno].hist_val = replayable;
    juego->partida.historial[juego->partida.turno].game_status = juego->partida.historial[juego->partida.turno - 1].game_status;

    // copia el tablero de el turno anterior
    coppyBoard(juego->partida.historial[juego->partida.turno].tablero, juego->partida.historial[juego->partida.turno - 1].tablero);

    return;
}

/**
 * Copia un arreglo de caracteres 3 x 3 en otro arreglo de caracteres 3 x 3
 * 
 * @param dest[3][3] Arreglo destino
 * @param src[3][3] Arreglo origen
 */
void coppyBoard(char dest[3][3], char src[3][3])
{
    int i = 0;
    int j = 0;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            dest[i][j] = src[i][j];
        }
    }

    return;
}

/**
 * Regresa al turno jugable anterior
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void lastTurn(JUEGO *juego)
{
    int i = juego->partida.turno;

    // si es el primer turno
    if(!juego->partida.turno)
    {
        printf("No hay turnos jugables antes de este...\n\n");
    }
    // si el turno anterior es un turno jugable
    else if(juego->partida.historial[i].hist_val)
    {
        // regresa a ese turno
        juego->partida.turno --;
    }
    // de lo contrario
    else
    {
        // revisa 2 turnos antes del actual
        i -= 2;

        // si no hay turnos disponibles
        if(i < 0)
        {
            printf("No hay turnos jugables antes de este...\n\n");
        }
        // de lo contrario
        else
        {
            // regresa a ese turno
            juego->partida.turno = i;
        }
    }

    return;
}

/**
 * Avanza al siguiente turno jugable
 * 
 * @param *juego La partida en curso
 * 
 * @returns void
 */
void nextTurn(JUEGO *juego)
{
    int i = juego->partida.turno;

    // si es el último turno disponible
    if(juego->partida.turno >= juego->partida.turnoMax)
    {
        printf("No hay turnos jugables despues de este...\n\n");
    }
    // de lo contrario
    else
    {
        // si alguno de los jugadores es una ia, avanza un turno
        if(!juego->partida.historial[i].hist_val || juego->partida.jugadores[(i + 1) % 2].ia)
        {
            i++;
        }
        
        // avanza un turno
        i++;

        // si el turno al que se desea ir está fuera de límites
        if(i > juego->partida.turnoMax)
        {
            printf("No hay turnos jugables despues de este...\n\n");
        }
        // de lo contrario, se recorre a ese turno
        else
        {
            juego->partida.turno = i;
        }
    }

    return;
}

/**
 * Revisa el estaso de un tablero
 * 
 * @param tablero[3][3] El tablero a revisar
 * @param played El último jugador que tiró
 * 
 * @returns int (-1 si el tablero está lleno y sin victorias, 0 si no hay victorias, 1 si hay victorias)
 */
int checkGame(char tablero[3][3], char played)
{
    int row = 0;
    int column = 0;

    int diagonal = 1;
    int iDiagonal = 1;

    int full = 1;

    int i = 0;
    int j = 0;

    // revisa si hay alguna victoria en fila o columna
    for(i = 0; i < 3 && !row && !column; i++)
    {
        // viable de victoria en fila o columna
        row = 1;
        column = 1;

        // para cada elemento de la fila y de la columna
        for(j = 0; j < 3; j++)
        {
            // si la ficha no es igual a la del jugador en fila
            if(tablero[i][j] != played)
            {
                row = 0;
            }

            // si la ficha no es igual a la del jugador en columna
            if(tablero[j][i] != played)
            {
                column = 0;
            }
        }
    }

    // revisa si el tablero está lleno o si hay una victoria en diagonal
    for(i = 0; i < 3; i++)
    {
        // revisa si está lleno
        for(j = 0; j < 3; j++)
        {
            // si hay un espacio disponible
            if(tablero[i][j] == ' ')
            {
                full = 0;
            }
        }

        // diagonal empezando por 0, 0
        if(tablero[i][i] != played)
        {
            diagonal = 0;
        }

        // diagonal empezando por 0, 2
        if(tablero[i][2 - i] != played)
        {
            iDiagonal = 0;
        }
    }

    // si hay alguna victoria
    if(row + column + diagonal + iDiagonal)
    {
        return 1;
    }
    // si el tablero está lleno
    else if(full)
    {
        return -1;
    }

    return 0;
}

/**
 * Muestra el como usar el programa, no desarrollado
 * 
 * @returns void
 */
void displayHelp(void)
{
    return;
}

/**
 * Imprime el tablero
 * 
 * @param tablero[3][3] El tablero a imprimir
 * 
 * @returns void
 */
void printBoard(char tablero[3][3])
{
    int i = 0;
    int j = 0;
    
    // para cada fila
    for(i = 0; i < 3; i++)
    {
        // si no es la primera fila, imprime un separador
        if(i)
        {
            printf("---+---+---\n");
        }

        // para cada columna
        for(j = 0; j < 3; j++)
        {
            // si no es la primera columna, imprime un separador
            if(j)
            {
                printf("|");
            }

            // imprime el caracter correspondiente
            printf(" %c ", tablero[i][j]);
        }

        // fin de la fila
        printf("\n");
    }

    // fin del tablero
    printf("\n");

    return;
}

/**
 * Hace una pregunta al usuario y valida la respuesta que le da
 * 
 * @param frase[] La pregunta al usuario
 * 
 * @returns int (0 si la respuesta es negativa, 1 si es positiva)
 */
int validateInput(char frase[])
{
    int s = 0;
    char r = 0;
    
    // mientras el usuario no proporcione una respuesta válida
    do
    {
        // imprime la pregunta y revisa el primer caracter de respuesta
        printf("%s (s/n)\n", frase);
        r = getchar();
        getchar();
        system("clear");

        // si la respuesta fue en mayusculas, las pasa a minusculas
        if(r >= 'A' && r <= 'Z')
        {
            r += 32;
        }

        // segun la respuesta
        switch(r)
        {
            // positiva
            case 's':
            {
                s = 1;

                break;
            }
            // negativa
            case 'n':
            {
                s = 0;

                break;
            }
            // desconocida
            default:
            {
                s = -1;

                printf("Accion desconocida...\n\n");

                break;
            }
        }
    } while(s < 0);

    return s;
}