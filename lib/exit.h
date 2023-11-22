#ifndef EXIT_H
#define EXIT_H

/*Indique au programme qu'on sort du programme avec la valeur de retour de la dernière commande */
extern void exit_jsh();

/*Indique au programme qu'on sort du programme avec la valeur de retour passée en argument */
extern void exit_jsh_with_arg(int val);

#endif