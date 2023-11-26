#ifndef EXIT_H
#define EXIT_H

/*Indique au programme qu'on sort du programme avec la valeur de retour de la dernière commande */
extern int exit_jsh();

/*Indique au programme qu'on sort du programme avec la valeur de retour passée en argument */
extern int exit_jsh_with_arg(int val);

#endif