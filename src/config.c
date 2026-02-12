/* Simple config file reading */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "bioplib/macros.h"


/************************************************************************/
CONFIG *readConfig(char *cfgFile)
{
   CONFIG *config = NULL,
          *c      = NULL;
   FILE   *fp     = NULL;
   char   buffer[CONFIG_MAXBUFF],
          *ptr;

   /* If the config file exists, then read it                           */
   if(!access(cfgFile, R_OK))
   {
      if((fp=fopen(cfgFile, "r")) == NULL)
      {
         return(NULL);
      }
      else
      {
         while(fgets(buffer, CONFIG_MAXBUFF, fp)!=NULL)
         {
            TERMINATE(buffer);
            if(config == NULL)
            {
               INIT(config, CONFIG);
               c = config;
            }
            else
            {
               ALLOCNEXT(c, CONFIG);
            }
            if(c==NULL)
            {
               FREELIST(config, CONFIG);
               return(NULL);
            }

            /* If the line contains an = sign                           */
            if((ptr = strchr(buffer, '='))!=NULL)
            {
               /* Copy from the next character into the value           */
               strncpy(c->value, ptr+1, CONFIG_MAXVAL-1);
               c->value[CONFIG_MAXVAL-1] = '\0';

               /* Terminate at the = sign and copy the first part into
                  the key
               */
               *ptr = '\0';
               strncpy(c->key, buffer, CONFIG_MAXKEY-1);
               c->key[CONFIG_MAXKEY-1] = '\0';
            }
         }
         FCLOSE(fp);
      }
   }
   return(config);
}

/************************************************************************/
CONFIG *setConfig(CONFIG *config, char *key, char *value)
{
   CONFIG *c = NULL;

   for(c=config; c!=NULL; NEXT(c))
   {
      if(!strcmp(c->key, key))
      {
         strcpy(c->value, value);
         return(c);
      }
   }

   /* If we get here, we didn't find the key so we need to add it       */
   if(config==NULL)
   {
      INIT(config, CONFIG);
      c = config;
   }
   else
   {
      c = config;
      LAST(c);
      ALLOCNEXT(c, CONFIG);
   }

   if(c==NULL)
   {
      FREELIST(config, CONFIG);
      return(NULL);
   }

   strcpy(c->key,   key);
   strcpy(c->value, value);
   
   return(c);
}

/************************************************************************/
char *getConfig(CONFIG *config, char *key)
{
   CONFIG *c;
   for(c=config; c!=NULL; NEXT(c))
   {
      if(!strcmp(c->key, key))
      {
         return(c->value);
      }
   }
   return(NULL);
}

/************************************************************************/
int writeConfig(char *cfgFile, CONFIG *config)
{
   CONFIG *c;
   FILE   *fp;
   
   if((fp=fopen(cfgFile, "w"))==NULL)
   {
      return(1);
   }
   else
   {
      for(c=config; c!=NULL; NEXT(c))
      {
         fprintf(fp, "%s=%s\n", c->key, c->value);
      }
      FCLOSE(fp);
   }

   return(0);
}
