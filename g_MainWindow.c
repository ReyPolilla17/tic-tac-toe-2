/**
 * @file g_MainWindow.c.c
 *
 * @brief Contiene la carga de la ventana principal
 *
 * @author Luis Julián Zamora Treviño
 * @date 05/Mar/2025
*/

#include "gato.h"

/**
 * crea la ventana principal con todos los elementos necesarios,
 * guardando punteros a widgets que serán usados en diversos momentos durante la ejecución
 *
 * @author Luis Julián Zamora Treviño
 *
 * @param *juego estructura principal del juego
 *
 * @return void
*/
void loadMainWindow(JUEGO *juego)
{
  // caja principal
  GtkWidget *mainBox;

  // contenedores genericos
  GtkWidget *hBox, *hBox1, *vBox, *vBox1, *eventbox;
  GtkWidget *label, *image;

  // para el menu
  GtkWidget *menuBar, *menuItem, *submenu, *subItem;
  GtkAccelGroup *group;

  // Para el modo dificil
  GtkWidget *flameContainter[2];

  // elemento de color
  GdkColor dark_grey;
  GdkColor light_grey;

  // contadores
  int i = 0;
  int j = 0;

  // iniciaización de variables
  gdk_color_parse("#DCDAD5", &light_grey);
  gdk_color_parse("#7a7a7a", &dark_grey);

  // creación de imagenes que se utilizan en diversos puntos de la partida
  // fichas de juego en diferentes resoluciones (60 * 60 y 20 * 20) blank sirve como un placeholder
  juego->graficos.m20[0] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/x.png", 20, 20, TRUE, NULL);
  juego->graficos.m20[1] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/o.png", 20, 20, TRUE, NULL);
  juego->graficos.m20[2] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/blank.png", 20, 20, TRUE, NULL);

  juego->graficos.m60[0] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/x.png", 60, 60, TRUE, NULL);
  juego->graficos.m60[1] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/o.png", 60, 60, TRUE, NULL);
  juego->graficos.m60[2] = gdk_pixbuf_new_from_file_at_scale("./MEDIA/blank.png", 60, 60, TRUE, NULL);

  // otras imagenes necesarias
  juego->graficos.hercules = gdk_pixbuf_new_from_file_at_scale("./MEDIA/hercules.raw", 200, 200, TRUE, NULL);


  // crea una ventana con el titulo y le asocia la función destruir
  juego->graficos.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(juego->graficos.window), 910, 540);
    gtk_window_set_title(GTK_WINDOW(juego->graficos.window), "Tic-Tac-Toe 2");
    gtk_signal_connect(GTK_OBJECT(juego->graficos.window), "destroy", GTK_SIGNAL_FUNC(stopTheApp), juego);

  // crea la caja que va a contener todos los elementos
  mainBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(juego->graficos.window), mainBox);

  /**
   * Sección del menú
   */

  // crea un grupo para combinaciones de teclas y lo mete a la ventana
  group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(juego->graficos.window), group);

  // crea una barra de menu y la mete a la caja principal
  menuBar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(mainBox), menuBar, FALSE, TRUE, 0);

  // crea una opcion del menu y la asocia al menu
  menuItem = gtk_image_menu_item_new();
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), gtk_image_new_from_stock(GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_LARGE_TOOLBAR));
    gtk_menu_item_set_label(GTK_MENU_ITEM(menuItem), "Archivo");
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

  // crea un submenu para la opcion y le asocia el grupo de combinaciones de teclas
  submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), submenu);
    gtk_menu_set_accel_group(GTK_MENU(submenu), group);

  // crea una subopción y le agrega una combinación de teclas
  juego->graficos.menuSave = gtk_menu_item_new_with_mnemonic("Guardar");
    gtk_widget_add_accelerator(juego->graficos.menuSave, "activate", group, 'S', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(juego->graficos.menuSave), "activate", G_CALLBACK(guardarPartida), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), juego->graficos.menuSave);
    gtk_widget_set_sensitive(juego->graficos.menuSave, FALSE);

  // crea una subopción y le agrega una combinación de teclas
  subItem = gtk_menu_item_new_with_mnemonic("Abrir");
    gtk_widget_add_accelerator(subItem, "activate", group, 'O', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(cargarPartida), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);


  // lo mimso para las otras 3 opciones del menu
  menuItem = gtk_image_menu_item_new();
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_LARGE_TOOLBAR));
    gtk_menu_item_set_label(GTK_MENU_ITEM(menuItem), "Juego");
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

  submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), submenu);
    gtk_menu_set_accel_group(GTK_MENU(submenu), group);

  subItem = gtk_menu_item_new_with_mnemonic("Nuevo");
    gtk_widget_add_accelerator(subItem, "activate", group, 'N', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(nuevaPartida), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);

  juego->graficos.menuEnd = gtk_menu_item_new_with_mnemonic("Terminar");
    gtk_widget_add_accelerator(juego->graficos.menuEnd, "activate", group, 'F', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(juego->graficos.menuEnd), "activate", G_CALLBACK(terminarPartida), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), juego->graficos.menuEnd);
    gtk_widget_set_sensitive(juego->graficos.menuEnd, FALSE);


  menuItem = gtk_image_menu_item_new();
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), gtk_image_new_from_stock(GTK_STOCK_INFO, GTK_ICON_SIZE_LARGE_TOOLBAR));
    gtk_menu_item_set_label(GTK_MENU_ITEM(menuItem), "Más");
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

  submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), submenu);
    gtk_menu_set_accel_group(GTK_MENU(submenu), group);

  subItem = gtk_menu_item_new_with_mnemonic("Cómo jugar");
    gtk_widget_add_accelerator(subItem, "activate", group, 'H', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(comoJugar), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);

  subItem = gtk_menu_item_new_with_mnemonic("Acerca de");
    gtk_widget_add_accelerator(subItem, "activate", group, 'A', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(acercaDe), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);

  juego->graficos.menuTruth = gtk_menu_item_new_with_mnemonic("Truth");
    gtk_widget_add_accelerator(juego->graficos.menuTruth, "activate", group, 'T', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(juego->graficos.menuTruth), "activate", G_CALLBACK(laVerdad), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), juego->graficos.menuTruth);


  menuItem = gtk_image_menu_item_new();
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menuItem), gtk_image_new_from_stock(GTK_STOCK_INFO, GTK_ICON_SIZE_LARGE_TOOLBAR));
    gtk_menu_item_set_label(GTK_MENU_ITEM(menuItem), "Online");
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuItem);

  submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), submenu);
    gtk_menu_set_accel_group(GTK_MENU(submenu), group);

  juego->graficos.menuName = gtk_menu_item_new_with_mnemonic("Establecer Nombre");
    gtk_widget_add_accelerator(subItem, "activate", group, 'U', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(registrarUsuario), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);

  juego->graficos.menuSeek = gtk_menu_item_new_with_mnemonic("Buscar Partida");
    gtk_widget_add_accelerator(subItem, "activate", group, 'P', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(subItem), "activate", G_CALLBACK(buscarPartida), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), subItem);
    gtk_widget_set_sensitive(juego->graficos.menuSeek, FALSE);

  juego->graficos.menuForfeit = gtk_menu_item_new_with_mnemonic("Rendirse");
    gtk_widget_add_accelerator(juego->graficos.menuTruth, "activate", group, 'F', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_signal_connect(GTK_OBJECT(juego->graficos.menuTruth), "activate", G_CALLBACK(rendirse), juego);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), juego->graficos.menuTruth);
    gtk_widget_set_sensitive(juego->graficos.menuForfeit, FALSE);

  /**
   * Fin de la seccion del menu
   */

  /**
   * Sección del título
   */
  // crea una caja para contener una imagen
  vBox = gtk_vbox_new(TRUE, 0);

  image = gtk_image_new_from_file("./MEDIA/title.gif");
    gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 20);

  // crea una caja para contener el titulo y la mete a la ventana
  eventbox = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(mainBox), eventbox, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(eventbox), vBox);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &dark_grey);
    gtk_widget_modify_bg(eventbox, GTK_STATE_INSENSITIVE, &dark_grey);
  /**
   * Fin de la seción del título
   */

  /**
   * Sección del juego
   */

  // caja horizontal que contriene las 3 secciones de la partida (turno actual, tablero, jugadores)
  hBox1 = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), hBox1, TRUE, TRUE, 0);

  // contiene las flamas del modo dificil
  flameContainter[0] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hBox1), flameContainter[0], TRUE, TRUE, 0);

  hBox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(flameContainter[0]), hBox, FALSE, FALSE, 20);

  // caja de turno actual
  juego->graficos.playingBox = gtk_vbox_new(FALSE, 10);

  eventbox = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(hBox), eventbox, TRUE, TRUE, 60);
    gtk_container_add(GTK_CONTAINER(eventbox), juego->graficos.playingBox);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &light_grey);
    gtk_widget_modify_bg(eventbox, GTK_STATE_INSENSITIVE, &light_grey);

  // etiqueta de la sección
  label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold'><big>Jugando:</big></span>");
    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), label, FALSE, TRUE, 10);

  // crea una imagen para indicar al jugador actual
  juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 20);

  // tablero

  // toda la seccion del tablero está en una caja de eventos para que no le afecte el cambio de fondo de la ventana principal
  eventbox = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(hBox1), eventbox, TRUE, TRUE, 0);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &light_grey);
    gtk_widget_modify_bg(eventbox, GTK_STATE_INSENSITIVE, &light_grey);

  hBox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(eventbox), hBox);

  vBox1 = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(hBox), vBox1, TRUE, TRUE, 10);

  // crea una caja para contener el tablero y la mete a la ventana, le mete a la caja del tablero una caja vertical
  eventbox = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(vBox1), eventbox, TRUE, TRUE, 10);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &dark_grey);
    gtk_widget_modify_bg(eventbox, GTK_STATE_INSENSITIVE, &dark_grey);

  vBox = gtk_vbox_new(TRUE, 5);
    gtk_container_add(GTK_CONTAINER(eventbox), vBox);

  // crea 3 filas de botones
  for(i = 0; i < 3; i++)
  {
    // crea una caja horizontal con espacio de los elementos de 5px y la mete a la caja vertical
    hBox = gtk_hbox_new(TRUE, 5);
      gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 0);

    // crea 3 botones
    for(j = 0; j < 3; j++)
    {
      // crea una caja de eventos y le asocia diversos eventos para estilos
      juego->graficos.buttons[i][j] = gtk_event_box_new();
        gtk_widget_modify_bg(juego->graficos.buttons[i][j], GTK_STATE_NORMAL, &light_grey);
        gtk_widget_modify_bg(juego->graficos.buttons[i][j], GTK_STATE_INSENSITIVE, &light_grey);
        g_signal_connect(G_OBJECT(juego->graficos.buttons[i][j]), "button_press_event", G_CALLBACK(board_button_pressed), juego);
        g_signal_connect(G_OBJECT(juego->graficos.buttons[i][j]), "enter-notify-event", G_CALLBACK(board_button_hover), juego);
        g_signal_connect(G_OBJECT(juego->graficos.buttons[i][j]), "leave-notify-event", G_CALLBACK(board_button_leave), juego);
        gtk_event_box_set_above_child(GTK_EVENT_BOX(juego->graficos.buttons[i][j]), TRUE);
        gtk_box_pack_start(GTK_BOX(hBox), juego->graficos.buttons[i][j], TRUE, TRUE, 0);

      // establece una imagen transparente
      juego->graficos.buttonImg[i][j] = gtk_image_new_from_pixbuf(juego->graficos.m60[2]);
        gtk_container_add(GTK_CONTAINER(juego->graficos.buttons[i][j]), juego->graficos.buttonImg[i][j]);
    }
  }

  // Sección de botones de antes y después
  hBox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(vBox1), hBox, FALSE, TRUE, 10);

  // crea los dos botones
  juego->graficos.moveButtons[0] = gtk_button_new();
    gtk_box_pack_start(GTK_BOX(hBox), juego->graficos.moveButtons[0], FALSE, TRUE, 0);
    gtk_button_set_image(GTK_BUTTON(juego->graficos.moveButtons[0]), gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON));
    gtk_signal_connect(GTK_OBJECT(juego->graficos.moveButtons[0]), "clicked", G_CALLBACK(history_past), juego);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);

  juego->graficos.moveButtons[1] = gtk_button_new();
    gtk_box_pack_start(GTK_BOX(hBox), juego->graficos.moveButtons[1], FALSE, TRUE, 0);
    gtk_button_set_image(GTK_BUTTON(juego->graficos.moveButtons[1]), gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON));
    gtk_signal_connect(GTK_OBJECT(juego->graficos.moveButtons[1]), "clicked", G_CALLBACK(history_next), juego);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[1], FALSE);


  // sección de jugadores (Contiene las flamas del modo dificil)
  flameContainter[1] = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hBox1), flameContainter[1], TRUE, TRUE, 0);
  
  hBox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(flameContainter[1]), hBox, FALSE, FALSE, 20);

  vBox = gtk_vbox_new(FALSE, 10);

  eventbox = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(hBox), eventbox, TRUE, TRUE, 20);
    gtk_container_add(GTK_CONTAINER(eventbox), vBox);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &light_grey);
    gtk_widget_modify_bg(eventbox, GTK_STATE_INSENSITIVE, &light_grey);


  // titulo
  label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<span weight='bold'><big>Jugadores:</big></span>");
    gtk_box_pack_start(GTK_BOX(vBox), label, FALSE, TRUE, 10);

  
  // imagen y nombre de los jugadores
  for(i = 0; i < 2; i++)
  {
    hBox = gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, TRUE, 10);

    juego->graficos.playerImg[i] = gtk_image_new_from_pixbuf(juego->graficos.m20[i]);
      gtk_box_pack_start(GTK_BOX(hBox), juego->graficos.playerImg[i], FALSE, TRUE, 10);

    juego->graficos.playerName[i] = gtk_label_new(NULL);
      gtk_box_pack_start(GTK_BOX(hBox), juego->graficos.playerName[i], FALSE, TRUE, 0);
  }


  // muestra los widgets
  gtk_widget_show_all(juego->graficos.window);


  // Coloca las flamas del modo dificil en la parte inferior de los laterales, despues de mostrar los otros widgets para tenerlas ocultas
  // Oculta las imagenes de los jugadores
  for(i = 0; i < 2; i++)
  {
    gtk_widget_hide(juego->graficos.playerImg[i]);

    juego->graficos.flames[i] = gtk_image_new_from_file("./MEDIA/fire.gif");
      gtk_widget_set_size_request(juego->graficos.flames[i], 10, 200);
      gtk_box_pack_end(GTK_BOX(flameContainter[i]), juego->graficos.flames[i], FALSE, TRUE, 0);
  }

  // oculta la verdad
  gtk_widget_hide(juego->graficos.menuTruth);
  
  return;
}