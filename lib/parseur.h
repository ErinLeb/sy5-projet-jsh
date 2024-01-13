#ifndef PARSEUR_H
#define PARSEUR_H

#include<stdbool.h>

/* Appelle le parseur en indiquant si la commande sera executée au background ou non*/
extern void is_bg(char * cmd);
/* Effectue les redirections s'il y en a, appelle parseur pour traiter la commande puis remet les descripteurs par défaut */
extern void parseur_redirections(char *cmd, bool is_bg);
/* Traite la commande ppassée en paramètre */
extern void parseur(int argc, char ** argv, bool bg);

#endif