#define CONFIG_MAXKEY 20
#define CONFIG_MAXVAL 160
#define CONFIG_MAXBUFF 240
typedef struct _config_mvalues
{
   char value[CONFIG_MAXVAL];
   struct _config_mvalues *next;
}  CONFIG_MVALUES;
   
typedef struct _config
{
   char key[CONFIG_MAXKEY],
        value[CONFIG_MAXVAL];
   CONFIG_MVALUES *mvalues;
   struct _config *next;
}  CONFIG;


/************************************************************************/
CONFIG *readConfig(char *cfgFile);
CONFIG *setConfig(CONFIG *config, char *key, char *value);
char *getConfig(CONFIG *config, char *key);
int writeConfig(char *cfgFile, CONFIG *config);
char *getConfigDirName(char *progName);
char *getConfigFilePath(char *progName, char *filename);
CONFIG *gotConfigKey(CONFIG *config, char *key);
