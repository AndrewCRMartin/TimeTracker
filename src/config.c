/* Simple config file reading */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "config.h"
#include "bioplib/macros.h"


/************************************************************************/
/* malloc's the directory name, so needs to be freed
 */
char *getConfigDirName(char *progName)
{
   char *dirName = NULL;
   char *home;
   int  fileNameLen = 0;

   home=getenv("HOME");
   fileNameLen += strlen(home);
   fileNameLen += strlen("/.config/");
   fileNameLen += strlen(progName) + 1;
   if((dirName = (char *)malloc(fileNameLen * sizeof(char)))!=NULL)
   {
      sprintf(dirName, "%s/.config/%s", home, progName);
      if(access(dirName, F_OK))  /* Doesn't exist */
         mkdir(dirName, 0755);
   }

   return(dirName);
}

/************************************************************************/
/* malloc's the file path, so needs to be freed
 */
char *getConfigFilePath(char *progName, char *filename)
{
   char *filePath = NULL;
   char *dirName;
   int fileNameLen = 0;

   if((dirName = getConfigDirName(progName)) != NULL)
   {
      fileNameLen += strlen(dirName);
      fileNameLen += strlen(filename);
      fileNameLen += 2; /* The / and the \0 */
      if((filePath = (char *)malloc(fileNameLen * sizeof(char)))!=NULL)
         sprintf(filePath, "%s/%s", dirName, filename);
      FREE(dirName);
   }

   return(filePath);
}




/************************************************************************/
CONFIG *readConfig(char *cfgFile)
{
   CONFIG *config = NULL,
          *c      = NULL,
          *c2     = NULL;
   FILE   *fp     = NULL;
   char   buffer[CONFIG_MAXBUFF],
          key[CONFIG_MAXKEY],
          value[CONFIG_MAXVAL],
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

            if((ptr = strchr(buffer, '='))!=NULL)
            {
               /* If the line contains an = sign, we have a single value*/

               /* Allocate item in linked list                          */
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
               c->mvalues = NULL;
               
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
            else if((ptr = strchr(buffer, '<'))!=NULL)
            {
               /* If the line contains a < sign, we have multiple values*/
               CONFIG_MVALUES *m;

               /* Copy from the next character into the value           */
               strncpy(value, ptr+1, CONFIG_MAXVAL-1);
               value[CONFIG_MAXVAL-1] = '\0';

               /* Terminate at the < sign and copy the first part into
                  the key
               */
               *ptr = '\0';
               strncpy(key, buffer, CONFIG_MAXKEY-1);
               key[CONFIG_MAXKEY-1] = '\0';

               /* If we have the key already, add this value to mvalues */
               if((c2=gotConfigKey(config, key))!=NULL)
               {
                  m = c2->mvalues;
                  LAST(m);
                  ALLOCNEXT(m, CONFIG_MVALUES);
                  if(m==NULL)
                  {
                     for(c2=config; c2!=NULL; NEXT(c2))
                        FREELIST(c2->mvalues, CONFIG_MVALUES);
                     FREELIST(config, CONFIG);
                     return(NULL);
                  }
                  strcpy(m->value, value);
               }
               else  /* It's a new key                                  */
               {
                  /* Allocate item in linked list                       */
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

                  c->value[0] = '\0';
                  
                  INIT(c->mvalues, CONFIG_MVALUES);
                  m = c->mvalues;

                  strcpy(c->key, key);
                  strcpy(m->value, value);
               }
            }
         }
         FCLOSE(fp);
      }
   }
   return(config);
}


/************************************************************************/
CONFIG *gotConfigKey(CONFIG *config, char *key)
{
   CONFIG *c = NULL;

   for(c=config; c!=NULL; NEXT(c))
   {
      if(!strcmp(c->key, key))
      {
         return(c);
      }
   }

   return(NULL);
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
   char *multi = NULL;
   for(c=config; c!=NULL; NEXT(c))
   {
      if(!strcmp(c->key, key))
      {
         if(c->mvalues == NULL)
         {
            return(c->value);
         }
         else  /* We have multiple values                               */
         {
            CONFIG_MVALUES *m;
            int totalLength = 0;
            /* Calculate the total length and allocate memory           */
            for(m=c->mvalues; m!=NULL; NEXT(m))
               totalLength += strlen(m->value) + 1;
            multi = (char *)malloc(totalLength+1 + sizeof(char));
            multi[0] = '\0';
            /* Create a | separated list as a string                    */
            for(m=c->mvalues; m!=NULL; NEXT(m))
            {
               strcat(multi, m->value);
               if(m->next != NULL)
                  strcat(multi, "|");
            }
            return(multi);
         }
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

#ifdef TEST
int main(int argc, char **argv)
{
   CONFIG *config, *c;
   char *multi;
   
   config = readConfig("./t/test.conf");
   for(c=config; c!=NULL; NEXT(c))
   {
      if(c->mvalues)
      {
         CONFIG_MVALUES *m;
         for(m=c->mvalues; m!=NULL; NEXT(m))
         {
            printf("%s : %s\n", c->key, m->value);
         }
      }
      else
      {
         printf("%s : %s\n", c->key, c->value);
      }
   }

   printf("vala : %s\n", getConfig(config, "vala"));
   multi = getConfig(config, "multi");
   printf("multi : %s\n", multi);
   return(0);
}
#endif
