#ifndef PARSEUR_H
#define PARSEUR_H

/* Récupère une ligne de l'entrée standard et appelle la commande y correspondant */
extern void parseur_redirections(char *);
extern void parseur(int argc, char *argv[]);

#endif