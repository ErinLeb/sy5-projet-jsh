#ifndef PARSEUR_H
#define PARSEUR_H

/* coupe une chaîne de caractères (ligne de commande) en mots (commande + arguments)*/
//extern char** cutPath(char *);

/* récupère une ligne de l'entrée standard et appelle la commande y correspondant*/
extern void parseur(char *);

#endif