#ifndef CD_H
#define CD_H

/* Change le répertoire courant qui devient la racine */
extern int cd();

/*Change le répertoire courant par celui désigné par path */
extern int cd_with_arg(const char* path);

#endif