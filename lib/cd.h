#ifndef CD_H
#define CD_H

/* Change le répertoire courant par la racine */
extern int cd();

/* Change le répertoire courant par celui désigné par path */
extern int cd_with_arg(char* path);

#endif