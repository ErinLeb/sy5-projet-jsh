Dans ce fichier sont données des explications concernant l'architecture, le fonctionnement et les choix faits au cours de ce projet.

Voici un sommaire de ce que l'on y trouve : 
- Hiérarchie générale de dossiers
- Fonctionnement général
- Variables globales
- Prompt
- Parseur
- Redirections
- Commandes internes
  - cd
  - ?
  - pwd
  - exit
- Commandes externes
- Jobs
    - Structure des jobs
    - Kill
- Gestion des signaux

## Hiérarchie générale de dossiers 
Les fichiers de configuration et ceux qui assurent le bon fonctionnement du projet sont accessibles à la racine du dossier. Les fichiers de code correspondant à des tâches spécifiques (commandes internes et externes, kill, prompt, jobs, parseur) se situent dans le dossier `src`. Leurs library headers (`.h`) correspondant se situent dans le dossier `lib`. Le programme se lance dans le fichier `main.c`.  
Enfin, en plus des tests fournis dans `Projet_test.sh`, le dossier `tests` contient un fichier de test compilable avec `make test` et qui regroupe les tests que nous avons utilisé pour s'assurer que nos commandes fonctionnent bien.

# Fonctionnement général 
Le principe de `jsh` est de traiter les demandes de l'utilisateur. On distingue plusieurs rôles : 
* l'affichage d'une invite de commande (prompt), indiquant le répertoire courant et le nombre de jobs en cours
* l'interprétation des lignes écrites par l'utilisateur et l'exécution des commandes correspondantes.
* l'affichage des jobs en cours qui ont changé d'état, pour tenir informé l'utilisateur.

Ainsi, dans le fichier `main.c` on retrouve la structure globale dont on a besoin pour faire tourner le programme : définition et initialisation des variables d'environnement, ouverture du répertoire courant, puis en boucle : affichage du prompt et lecture de l'entrée utilisateur, envoyée au parseur, mise à jour de l'état des jobs, jusqu'à la fin du programme.


## Variables globales
Les variables qu'il est nécessaire de garder en mémoire sont stockées dans `lib/env.h`. On y retrouve notamment : 
- des constantes indiquant les valeurs maximales prises par la taille du prompt `PROMPT`, la taille des paths `PATH_MAX` et le nombre de jobs `NBR_MAX_JOBS`. 
- un tableau d'entiers `default_fd` à initialiser avec les descripteurs de fichiers standards (entrée, sortie, sortie erreur) pour les remettre à leur valeur par défaut après des redirections.
- des chaines de caractères pour pouvoir se déplacer dans l'arborescence de fichiers : `path_courant`, `path_precedent`. Celles-ci sont initialisées dans `main.c` à l'aide de `getcwd`.
- des informations concernant les jobs en cours : le nombre `cmp_jobs`, le tableau de leurs pgids `jobs_suivis` et un tableau de booléens utile pour savoir quels identifiants sont déjà pris et lesquels sont libres d'être attribués à de nouveaux jobs `id_taken`.
- un entier pour la valeur de retour de la dernière commande effectuée.
- un booléen `appel_exit` qui est utilisé dans le cas où l'on veut sortir du programme et qu'il reste des jobs en cours.
- un booléen `boucle` qui indique si le programme tourne toujours ou si on en sort.
  
## Prompt
Dans `main.c`, on commence par allouer de la mémoire pour une chaine de caractères `prompt_char` de la taille définie `PROMPT`, ensuite, à chaque tour de boucle, on appelle la fonction `prompt` avec cette chaine en argument. Celle-ci se charge de créer, à partir du nombre de jobs en cours et du *path* du répertoire courant, la chaine de caractères qui respecte les normes exigées du prompt, en taille et en couleur, et la met dans `prompt_char` puis la renvoie. C'est l'appel à `readline` dans le main qui affiche le prompt car on lui passe `prompt_char` en argument.

## Parseur
Une fois le prompt affiché, le programme attend avec un appel à `readline` une saisie de l'utilisateur qui sera stockée dans la chaine de caractères `ligne_cmd`. Il l'ajoute ensuite à l'historique de commandes avec la fonction `add_history` et la traite. Pour cela, `ligne_cmd` est passée en argument à la fonction `parseur_redirections`. 
#### Redirections
`parseur_redirections` coupe la ligne de commande en mots et si un symbole de redirection est rencontré, le changement est effectué avec le fichier donné juste après. Si plusieurs redirections sont données sur le même canal, les fichiers sont créés si besoin mais seul le dernier fichier reste ouvert et est utilisé pour la redirection. Les canaux qui ont été modifiés sont enregistrés pour pouvoir les remettre à leurs fichiers par défaut à la fin du traitement de la commande ou en cas d'erreur grâçe aux `goto maj_default`. En revanche, tant qu'un symbole de redirection n'est pas détecté, le mot courant traité est ajouté au tableau de chaines de caractères `argv` qui représente la commande à traiter. Une fois toutes les redirections traitées, on a récupéré  la commande complète dans `argv` qui est passée à la fonction `parseur`. On donne également à ces deux fonctions un booléen permettant de savoir si la commande à traiter doit être exécutée à l'avant ou à l'arrière plan. Sa valeur est déterminée dans la fonction `is_bg`.

#### Commandes
`parseur` analyse le premier mot du tableau `argv` et distingue plusieurs cas :
- s'il s'agit d'une commande interne (`cd`, `?`, `pwd`, `exit`, `jobs`, `kill`, `bg`, `fg`) : si c'est le cas et si les arguments sont corrects, le traitement de la commande est délégué à la fonction associée, et la valeur de retour est mise à jour.
- si la commande n'a pas été trouvée parmi ces possibilité, il s'agit d'une commande externe (ou d'une commande erronée) donc on commence par vérifier si elle doit être lancée au background (si `&` est à la fin de la ligne, `bool bg` est mis à `true`) puis on délègue le traitement à `cmd_ext`.

## Commandes internes
Les commandes internes renvoient 0 si leur exécution s'est bien déroulée, 1 sinon.

#### cd
Selon le nombre d'arguments, deux fonctions de `cd.c` peuvent être appelées : 
- si un argument est donné, à l'aide de `chdir`, on change le répertoire courant avec celui dont le chemin est indiqué par l'argument. On sauvegarde dans `path_precedent` l'ancienne valeur de `path_courant` et on met à jour `path_courant` à l'aide de `getcwd`.
- si `-` est donné comme argument, on change le répertoire courant avec la valeur de `path_precedent` puis on met à jour les valeurs de `path_precedent` et de `path_courant`.
- si aucun argument n'est donné, alors la fonction `cd` récupère la variable d'environnement `HOME` et modifie les paths du répertoire courant et précédent.
  
#### ?
Cette commande est traitée par la fonction `last_output` dans le fichier du même nom. Elle affiche la valeur de `val_retour`.

#### pwd
Cette commande est traitée par la fonction `pwd` dans le fichier du même nom. Elle affiche `path_courant`.

#### exit
Si l'utilisateur écrit seulement `exit` sans autre argument, alors `parseur` appelle `exit_jsh`, qui elle_même appelle `exit_jsh_with_arg` avec comme argument `val_retour` par défaut.

Si c'est la première fois qu'on appelle `exit_jsh_with_arg`, la fonction met le booléen `appel_exit` à true puis *s'il n'y a pas de jobs en cours* passe également la valeur de `boucle` à false et retourne l'argument qui lui est passé en argument pour arrêter la boucle du main et sortir du programme avec cette valeur de retour. Dans le cas ou il reste des jobs en cours, cette fonction échoue (affiche un message d'erreur). Si c'est la deuxième fois consécutive qu'on appelle la fonction, alors tous les jobs reçoivent le signal `SIGHUP` et la valeur de `boucle` est bien modifiée à false. L'appel à n'importe quelle autre fonction que `exit` change la valeur de `appel_exit` à false.

Le cas où l'utilisateur utilise `CTRL+D` pour terminer le programme est traité dans `parseur_redirections`, quand readline lui passe une chaîne de caractères `NULL` : `exit_jsh` est appelé dans ce cas.

## Commandes externes
La fonction `cmd_ext` prend en argument un entier `argc` et un tableau de chaines de caractères `argv` qui correspondent à la commande à effectuer et à son nombre d'argument. Elle prend également un booléen `bg` pour déterminer si la commande doit être exécutée à l'avant ou à l'arrière plan. Et enfin elle prend un pid_t `pid` qui est permet de savoir si c'est le père ou le fils issu du fork dans le parseur qui appelle la fonction.
- dans le cas du fils, on commence par remettre la gestion des signaux par défaut pour que les signaux ignorés par jsh ne le soient pas par ces fils qui exécutent des commandes. Puis on appelle `execvp` de manière à tenir compte du contenu de `PATH` et si l'*exec* échoue (car la commande n'existe pas par exemple), la valeur renvoyée est 1.
Un nouveau groupe de processus lui est attribué avec `setpgid`.
- dans le cas du père, on crée un job au nom de la commande (on concatène le tableau en argument) avec le pid du fils. On met à jour les différentes variables en lien avec les jobs en cours (le compteur, le tableau des *pid*). 
  - si le fils doit être exécuté au *background*, alors on met à jour son statut. Et on l'affiche si celui a changé depuis la dernière fois qu'il a été affiché. Le père n'attend pas son fils. On peut donc exécuter une nouvelle commande dans jsh.
  - sinon, on permet au fils d'écrire dans le terminal avec `tcsetpgrp` et le père attend son fils, jusqu'à ce que son statut change. On le met alors à jour. S'il est terminé on le supprime en redonnant la main à jsh sur le terminal de contrôle avec `tcsetpgrp`. Si il n'est pas terminé il devient un processus en *background*.

## Jobs
### Structure des jobs
Pour gérer les jobs dans JSH, le programme dispose de variables, évoquées dans la partie `Variables d'environnement`, ainsi que des fonctions dans `jobs.c`. Celles-ci manipulent la structure définie dans `jobs.h` par `struct job`. Elle réunit les caractéristiques suivantes : le numéro du job `id`, le `pgid`, le pid des processus du job `pid_proc`, le statut des processus du job `status_proc`, la commande exécutée par chaque processus du job `cmd_proc`, le nombre de processus du job `nb_processus`, le nom de la commande donnée par l'utilisateur `cmd`, le booléen `afficher_save` qui sert à l'initialisation du job pour afficher son état, le statut du job `jobstatus`. Ce dernier est défini par l'`enum JobStatus` et peut prendre les valeurs suivantes : JOB_RUNNING, JOB_DONE, JOB_STOPPED, JOB_KILLED, JOB_DETACHED (entiers de 0 à 4). 

Pour créer un job, la fonction `new_job` a besoin du pgid et de la commande correspondante. Elle alloue la mémoire nécessaire à la structure du job et attribue un `id` selon ceux disponibles.

Pour ajouter un processus à un job, la fonction `add_proc_to_job` le pid du job à ajouter et le pgid du groupe dans lequel on veut l'ajouter. Elle met donc à jour les variables du job correspondant avec les valeurs du processus.

Pour supprimer un job, la fonction `suppression_job` supprime le job correspondant à la place donnée par l'argument dans le tableau des pids des jobs, et met à jour les variables nécessaires.

Pour changer le statut d'un job, la fonction `set_status` prend un job `j` et un booléen `bg` pour savoir si le wait doit être bloquant ou non et vérifie le statut de tous les processus du job et met à jour le statut du job en fonction.

### Affichage des jobs en cours
Le programme a besoin d'afficher l'état des jobs en cours à deux moments différents, encodés par deux fonctions différentes : quand l'utilisateur le demande avec `jobs`, à chaque entrée de l'utilisateur, pour le maintenir informé des changements d'état avec `check_jobs_info`, appelé à la fin de chaque boucle du main.

`jobs` fait le tour des jobs (dans l'ordre du tableau de leurs pid) en appelant `waitpid` de manière non bloquante pour récupérer leurs informations et mettre à jour leur statut (avec `set_status`). L'état est affiché en fonction de celui-ci. Le job est supprimé s'il est dans un des états suivants : DONE, KILLED, DETACHED. Si l'option -t est ajouté, alors on affiche arborescence des processus, à l'aide du ficher proc.

`check_jobs_info` n'affiche que les jobs qui ont changé d'état depuis le dernier affichage. (Pour cela, pour chaque job, on regarde le résultat de l'appel non bloquant à `waitpid`, sauf s'il s'agit d'un job que l'on vient de créer (l'appel à waitpid vient déjà d'être fait mais on veut quand même avoir l'info visuelle) donc si `afficher_save` est vrai)

### Kill
Le parseur vérifie si l'argument passé à `kill` correspond à un job (`%` devant le numéro) ou à un processus et appelle `kill_job` ou `kill_proc` avec un numéro de signal. 
Les fonctions `kill_term_job` et `kill_term_proc` appellent ces fonctions avec le numéro de signal `SIGTERM`.

#### Gestion des signaux
C'est dans le main avant le lancement de la boucle principale que les signaux `SIGINT`, `SIGTERM`, `SIGTTIN`, `SIGQUIT`, `SIGTTOU` et `SIGTSTP` sont ignorés pour `jsh`. Ce comportement est modifié et remis au comportement par défaut pour les processus fils exécutant des commandes dans `commandes_externes.c` (entre le `fork` et l'`exec` dans `cmd_ext`)