#ifndef PARSEUR_H
#define PARSEUR_H

/* Effectue les redirections s'il y en a, appelle parseur pour traiter la commande puis remet les descripteurs par défaut */
extern void parseur_redirections(char *);
/* Traite la commande ppassée en paramètre */
void parseur(int argc, char **argv);

#endif