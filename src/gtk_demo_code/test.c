/*
  https://docs.gtk.org/gtk3/getting_started.html
 */

#define DEMO 3

#include <gtk/gtk.h>

static void activate (GtkApplication* app,
                      gpointer        user_data)
{
  GtkWidget *window;

  /* Create a window widget */
  window = gtk_application_window_new (app);
  /* Set the title. GTK_WINDOW() casts the widget to type WINDOW */
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  /* Set its size */
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 200);
  /* Show all widgets */
  gtk_widget_show_all (window);
}

static void print_hello(GtkWidget *widget,
                        gpointer   data)
{
   g_print ("Hello World\n");
}

static void activate2(GtkApplication *app,
                      gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  /* Create a window widget */
  window = gtk_application_window_new (app);
  /* Set title */
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  /* Set size */
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  /* Create a box for a button */
  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  /* Add the button box to the window */
  gtk_container_add (GTK_CONTAINER (window), button_box);

  /* Create a button with a label */
  button = gtk_button_new_with_label ("Hello World");
  /* Add the button to the button box */
  gtk_container_add (GTK_CONTAINER (button_box), button);

  /* When the button is clicled call the print_hello() function which prints to the command line */
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  /* Also destroy the window when clicked. If we used g_signal_connect(), we would destroy the button
   not the window */
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

  /* Show all the widgets */
  gtk_widget_show_all (window);
}

static void activate3(GtkApplication *app,
                      gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);

  button = gtk_button_new_with_label ("Button 1");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  /* Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  button = gtk_button_new_with_label ("Button 2");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  /* Place the second button in the grid cell (1, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (window);

}


int main (int    argc,
          char **argv)
{
  GtkApplication *app;
  int status;

  /* Create and name the application */
  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

  /* Connect the activate signal to the activate function above */
#if (DEMO==1)
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
#elif (DEMO==2)
  g_signal_connect (app, "activate", G_CALLBACK (activate2), NULL);
#elif (DEMO==3)
  g_signal_connect (app, "activate", G_CALLBACK (activate3), NULL);
#endif

  /* Run the application which will send the activate signal */
  status = g_application_run (G_APPLICATION (app), argc, argv);
  /* Free up the application */
  g_object_unref (app);

  return status;
}
