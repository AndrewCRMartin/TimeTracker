/************************************************************************/
/**

   Program:    TimeTracker
   \file       timetracker.c
   
   \version    V0.1
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

/************************************************************************/
/* Globals
*/
time_t gTotalTime = (time_t)0;
time_t gStartTime;
FILE *gFpRecord = NULL;
CONFIG *gConfig = NULL;

/************************************************************************/
/* Prototypes
*/
int main(int argc, char **argv);
static void logtime(int state, time_t theTime);
static void output_state(GtkToggleButton *source, gpointer user_data);
static void activate(GtkApplication *app, gpointer user_data);
static void ReadCSS(char *cssFile);
static CONFIG *ReadOrCreateConfig(char *cfgFile);


/************************************************************************/
int main(int argc, char **argv)
{
   GtkApplication *app;
   int status = 1;
   char *fnm = "TimeTrackerRecord.csv";
   static char *configFile = "timetracker.config";

   if((gConfig = ReadOrCreateConfig(configFile))==NULL)
   {
      fprintf(stderr,"Error: Unable to read or create config file (%s)\n",
              configFile);
   }
   
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
      ReadCSS("timetracker.css");
      
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
      fprintf(gFpRecord,"\"CLOSED\",,\n");
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
static void logtime(int state, time_t theTime)
{
   char *tString;
   tString = ctime(&theTime);
   TERMINATE(tString);

   if(state)
   {
      printf("START: %s\n", tString);
      fprintf(gFpRecord,"\"%s\",", tString);
   }
   else
   {
      struct tm *pTm;
      pTm = localtime(&gTotalTime);

      printf("STOP: %s; ", tString);
      printf("TOTAL: %02d:%02d:%02d\n",
            pTm->tm_hour - 1, pTm->tm_min, pTm->tm_sec);
      
      fprintf(gFpRecord,"\"%s\",", tString);
      fprintf(gFpRecord,"%02d:%02d:%02d\n",
            pTm->tm_hour - 1, pTm->tm_min, pTm->tm_sec);
   }
}


/************************************************************************/
static void output_state(GtkToggleButton *source, gpointer user_data)
{
   if(gtk_toggle_button_get_active(source))
   {
      gtk_button_set_label(GTK_BUTTON(source), (gchar *)"Stop");
      gtk_widget_set_name(GTK_WIDGET(source), "toggle_red");

      time(&gStartTime);
      logtime(1, gStartTime);
   }
   else
   {
      time_t stopTime;
      time(&stopTime);
      gTotalTime += (stopTime - gStartTime);
      logtime(0, stopTime);
      gtk_button_set_label(GTK_BUTTON(source), (gchar *)"Start");
      gtk_widget_set_name(GTK_WIDGET(source), "toggle_green");
   }
}


/************************************************************************/
static void activate(GtkApplication *app, gpointer user_data)
{
   GtkWidget *window;
   GtkWidget *box, *toggle;
   
   /* Create a window widget with title and size                        */
   window = gtk_application_window_new(app);
   gtk_window_set_title(GTK_WINDOW(window), "Time Tracker");
   gtk_window_set_default_size(GTK_WINDOW(window), 100, 50);
   
   box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
   
   toggle = gtk_toggle_button_new_with_label("Start");
   gtk_widget_set_name(toggle, "toggle_green");
   
   /* Call the output_state() function when button is toggled           */
   g_signal_connect(toggle, "toggled", G_CALLBACK(output_state), NULL);
   
   /* Add the toggle to the box and then the box to the window          */
   gtk_container_add(GTK_CONTAINER(box), toggle);
   gtk_container_add(GTK_CONTAINER(window), box);
   
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
      config = setConfig(config, "project", "project");
      setConfig(config, "task", "task");
      setConfig(config, "log", "TimeTracker.csv");
      setConfig(config, "css", "timetracker.css");
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

