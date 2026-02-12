/************************************************************************/
/**

   Program:    TimeTracker
   \file       timetracker.c
   
   \version    V0.2
   \date       12.02.26   
   \brief      Very simple time tracker
   
   \copyright  (c) Prof. Andrew C. R. Martin 2026
   \author     Prof. Andrew C. R. Martin
   \par
               abYinformatics, Ltd
   \par
               andrew@abyinformatics.com
               andrew@bioinf.org.uk
               
**************************************************************************

   This program is licensed under the GNU Public Licence V3.0 or above

**************************************************************************

   Description:
   ============
   A very simple time tracker/stopwatch that logs times to a CSV file.
   The intention is to add multiple named tasks
   
**************************************************************************

   Usage:
   ======
   timetracker

**************************************************************************

   Revision History:
   =================
   V0.1    12.02.26  Initial version with just one button
   V0.2    12.02.26  Added Quit button

*************************************************************************/
/* Includes
*/
#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include "bioplib/macros.h"
#include "config.h"

/************************************************************************/
/* Defines and macros
*/
#define PROGNAME "TimeTracker"

/************************************************************************/
/* Globals
*/
time_t gTotalTime = (time_t)0;
time_t gStartTime;
time_t gStopTime;
FILE   *gFpRecord = NULL;
CONFIG *gConfig   = NULL;

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
static void logtime(int state);
static void output_state(GtkToggleButton *source, gpointer user_data);
static void activate(GtkApplication *app, gpointer user_data);
static void ReadCSS(char *cssFile);
static CONFIG *ReadOrCreateConfig(char *cfgFile);


/************************************************************************/
int main(int argc, char **argv)
{
   GtkApplication *app;
   int status = 1;
   char *fnm;
   char *configFile = NULL;

   if((configFile = getConfigFilePath(PROGNAME,
                                      "timetracker.conf"))==NULL)
   {
      fprintf(stderr,"Error: Unable to create/access config directory\n");
      return(1);
   }

   if((gConfig = ReadOrCreateConfig(configFile))==NULL)
   {
      fprintf(stderr,"Error: Unable to read or create config file (%s)\n",
              configFile);
      FREE(configFile);
      return(1);
   }

   FREE(configFile);
   
   /* Open the tracking file (append)                                   */
   if((fnm = getConfig(gConfig, "log"))==NULL)
   {
      fprintf(stderr,"Error: 'log' missing from config\n");
      return(1);
   }
   if((gFpRecord = fopen(fnm, "a"))==NULL)
   {
      fprintf(stderr, "Unable to open time tracking file: %s\n", fnm);
   }
   else
   {
      /* Read CSS info for colours                                      */
      gtk_init(&argc, &argv);
      ReadCSS(getConfig(gConfig, "css"));
      
      /* Create and name the application                                */
      app = gtk_application_new("org.acrm.timetracker",
                                G_APPLICATION_DEFAULT_FLAGS);
      
      /* Connect the activate signal to the activate function           */
      g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

      /* Run the application which will send the activate signal        */
      status = g_application_run(G_APPLICATION(app), argc, argv);
      
      /* Free up the application resources                              */
      g_object_unref(app);
      
      /* Log that we have closed the application                        */
      fprintf(gFpRecord,"\"CLOSED\",,,,\n");
      fclose(gFpRecord);
   }
   return status;
}


/************************************************************************/
static void ReadCSS(char *cssFile)
{
   GtkCssProvider *provider;
   GdkDisplay     *display;
   GdkScreen      *screen;
   GError         *error = 0;

   provider = gtk_css_provider_new();
   display  = gdk_display_get_default();
   screen   = gdk_display_get_default_screen(display);
   gtk_style_context_add_provider_for_screen(screen,
             GTK_STYLE_PROVIDER(provider),
             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
   gtk_css_provider_load_from_file(provider,
                                   g_file_new_for_path(cssFile),
                                   &error);
   g_object_unref(provider);

   if(error)
      printf("Error reading CSS file: %s\n", cssFile);
}


/************************************************************************/
static void logtime(int state)
{
   char *tString;

   if(state)   /* Start was pressed                                     */
   {
      tString = ctime(&gStartTime);
      TERMINATE(tString);
      printf("START: %s\n", tString);
      fprintf(gFpRecord,"\"%s\",", getConfig(gConfig, "project"));
      fprintf(gFpRecord,"\"%s\",", getConfig(gConfig, "task"));
      fprintf(gFpRecord,"\"%s\",", tString);
   }
   else        /* Stop was pressed                                      */
   {
      struct tm *pTmTotal,
                *pTmDiff;
      time_t    diffTime = gStopTime - gStartTime;

      tString = ctime(&gStopTime);
      TERMINATE(tString);

      printf("STOP: %s; ", tString);

      pTmDiff  = localtime(&diffTime);
      printf("TIME: %02d:%02d:%02d; ",
            pTmDiff->tm_hour - 1, pTmDiff->tm_min, pTmDiff->tm_sec);

      pTmTotal = localtime(&gTotalTime);
      printf("TOTAL: %02d:%02d:%02d\n",
            pTmTotal->tm_hour - 1, pTmTotal->tm_min, pTmTotal->tm_sec);
      
      fprintf(gFpRecord,"\"%s\",", tString);
      pTmDiff  = localtime(&diffTime);
      fprintf(gFpRecord,"%02d:%02d:%02d,",
            pTmDiff->tm_hour - 1,  pTmDiff->tm_min,  pTmDiff->tm_sec);
      pTmTotal = localtime(&gTotalTime);
      fprintf(gFpRecord,"%02d:%02d:%02d\n",
            pTmTotal->tm_hour - 1, pTmTotal->tm_min, pTmTotal->tm_sec);
   }
   fflush(gFpRecord);
}


/************************************************************************/
static void output_state(GtkToggleButton *source, gpointer user_data)
{
   if(gtk_toggle_button_get_active(source))
   {
      /* Start pressed                                                  */
      time(&gStartTime);
      logtime(1);

      /* Change the label                                               */
      gtk_button_set_label(GTK_BUTTON(source), (gchar *)"Stop");
      gtk_widget_set_name(GTK_WIDGET(source), "toggle_red");
   }
   else
   {
      /* Stop pressed                                                   */
      time(&gStopTime);
      gTotalTime += (gStopTime - gStartTime);
      logtime(0);

      gtk_button_set_label(GTK_BUTTON(source), (gchar *)"Start");
      gtk_widget_set_name(GTK_WIDGET(source), "toggle_green");
   }
}


/************************************************************************/
static void activate(GtkApplication *app, gpointer user_data)
{
   GtkWidget *window;
   GtkWidget *grid;
   GtkWidget *toggle_ss;
   GtkWidget *btn_quit;
   GtkWidget *label_project;
   GtkWidget *label_task;
   
   /* Editable text
      GtkWidget *view;
      GtkTextBuffer *buffer;

      view   = gtk_text_view_new();
      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
      gtk_text_buffer_set_text(buffer, getConfig(gConfig, "project"), -1);
   */
   
   /* Create a window widget with title and size                        */
   window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Time Tracker");
   gtk_window_set_default_size(GTK_WINDOW(window), 100, 80);

   /* Create a grid to contain the layout and add it to the window      */
   grid = gtk_grid_new();
   gtk_container_add(GTK_CONTAINER(window), grid);

   /* Create a text item containing the project name                    */
   label_project = gtk_label_new(getConfig(gConfig, "project"));
   gtk_widget_set_name(GTK_WIDGET(label_project), "label_project");
   
   /* Create a text item containing the task name                       */
   label_task = gtk_label_new(getConfig(gConfig, "task"));
   gtk_widget_set_name(GTK_WIDGET(label_task), "label_task");
   
   /* Create the toggle box and button and add the output_state()
      function when button is toggled
   */ 
   toggle_ss = gtk_toggle_button_new_with_label("Start");
   g_signal_connect(toggle_ss, "toggled", G_CALLBACK(output_state), NULL);
   gtk_widget_set_name(toggle_ss, "toggle_green");

   /* Create the quit button and exit when it is clicked                */
   btn_quit = gtk_button_new_with_label("Quit");
   g_signal_connect_swapped (btn_quit, "clicked",
                             G_CALLBACK(gtk_widget_destroy), window);
   gtk_widget_set_name(btn_quit, "btn_quit");

   /* Add the text to the grid                                          */
   gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(label_project), 0,0,3,1);
   gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(label_task),    0,1,3,1);
   
   /* Add the toggle to the grid                                        */
   gtk_grid_attach(GTK_GRID(grid), toggle_ss, 0,2,3,1);

   /* Add the quit button                                               */
   gtk_grid_attach(GTK_GRID(grid), btn_quit, 1,3,1,1);

   /* Show all the widgets                                              */
   gtk_widget_show_all(window);
}


/************************************************************************/
CONFIG *ReadOrCreateConfig(char *cfgFile)
{
   CONFIG *config = NULL,
          *c      = NULL;
   char   buffer[CONFIG_MAXBUFF],
          *ptr;
   FILE   *fp = NULL;
   int    missing = 0;

   if((config = readConfig(cfgFile)) == NULL)
      missing = 1;

   /* If the config linked list is still unpopuluated, populate with
      defaults
   */
   if(config==NULL)
   {
      char *cssFile = getConfigFilePath(PROGNAME, "timetracker.css");
      
      config = setConfig(config, "project", "project");
      setConfig(config, "task", "task");
      setConfig(config, "log", "TimeTracker.csv");
      if(cssFile == NULL)
      {
         setConfig(config, "css", "timetracker.css");
      }
      else
      {
         setConfig(config, "css", cssFile);
         FREE(cssFile);
      }
   }

   if(missing)
   {
      /* Config file doesn't exist, so create and write it              */
      if(writeConfig(cfgFile, config)!=0)
      {
         fprintf(stderr, "Error: Unable to write config file (%s)\n",
                 cfgFile);
      }
   }

   return(config);
}

