#define CONFIG_MAXKEY 20
#define CONFIG_MAXVAL 160
#define CONFIG_MAXBUFF 240
typedef struct _config
{
   char key[CONFIG_MAXKEY],
        value[CONFIG_MAXVAL];
   struct _config *next;
}  CONFIG;

/************************************************************************/
CONFIG *readConfig(char *cfgFile);
CONFIG *setConfig(CONFIG *config, char *key, char *value);
char *getConfig(CONFIG *config, char *key);
int writeConfig(char *cfgFile, CONFIG *config);
