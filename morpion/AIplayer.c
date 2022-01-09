/* fichier contenant les joueurs IA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "morpion2.h"

/**** joueur aléatoire ****/
int AI_ordi_0(partie *p, mouvement *resultat) {
   liste_mouvements lm;
   int nb;
   /* on vérifie si le jeu n'est pas terminé. Normalement AI_ordi ne devrait pas être appelé dans ce cas */
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appelé sur partie terminée ?\n");
      exit(EXIT_FAILURE);
   }
   /* recupération de la liste des mouvements dans lm (structure) */
   remplit_liste_mouvements(p,&lm);
   /* joueur aléatoire : on tire au sort le numéro du mouvement à jouer parmi le nombre total de mouvements */
   nb = rand()%GET_NB_MOUVEMENT(lm);
   /* pour "retourner" le mouvement, on le copie sur le pointeur resultat */
   COPIE_MOUVEMENT(lm,nb,resultat);
   /* le score retourné est 0 : jeu aléatoire, on n'utilise pas ce score */
   return 0;
}

/**** meilleur coup local ****/
int AI_ordi_1(partie *p, mouvement *resultat) {
   liste_mouvements lm;
   /* on v‚rifie si le jeu n'est pas termin‚. Normalement AI_ordi ne devrait pas ˆtre appel‚ dans ce cas */
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie termin‚e ?\n");
      exit(EXIT_FAILURE);
   }
   /* recup‚ration de la liste des mouvements dans lm (structure) */
   remplit_liste_mouvements(p,&lm);
   /* meilleur mouvement */
   int bscore = -SCORE_VICTORY-1; /* meilleur score (initialise a pire que le meilleur score possible) */
   int nb_bscore=0; /* nombre de "meilleurs scores" */
   int bmove = -1; /* meilleur mouvement */
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        /* pour chaque mouvement, on calcule la nouvelle position */
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc = get_score(np)*get_player(p); /* "Score" _relatif_ au joueur qui joue maintenant */
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               /* calcul pour avoir aleatoirement le meilleur mouvement, inutile si on ne s'interesse qu'au score */
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   /* pour "retourner" le mouvement, on le copie sur le pointeur resultat */
   COPIE_MOUVEMENT(lm,bmove,resultat);
   /* le score retourne est le meilleur score calcule */
   return bscore;
}

int negamax(partie *d){
    liste_mouvements lm;
    if(est_termine(d))return get_score(d)*get_player(d);
    remplit_liste_mouvements(d,&lm);
    int sc=-SCORE_VICTORY;
    for(int i=0; i<GET_NB_MOUVEMENT(lm);i++){
        partie *f=joue_coup_suivant(d,GET_MOUVEMENT(lm,i));
        int sc2=-negamax(f);
        if(sc<sc2)sc=sc2;
    }
    return sc;
}


/**** analyse complete du jeu ****/
int AI_ordi_2(partie *p, mouvement *resultat) {
    liste_mouvements lm;
   /* on verifie si le jeu n'est pas termine. Normalement AI_ordi ne devrait pas etre appel‚ dans ce cas */
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie terminee ?\n");
      exit(EXIT_FAILURE);
   }
   /* recuperation de la liste des mouvements dans lm (structure) */
   remplit_liste_mouvements(p,&lm);
   //int sco;
   int bscore =-SCORE_VICTORY-1; //scorebest /* meilleur score (initialise a pire que le meilleur score possible) */
   int nb_bscore=0; //nbbest  /* nombre de "meilleurs scores" */
   int bmove = -1; //best  /* meilleur mouvement */
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        /* pour chaque mouvement, on calcule la nouvelle position */
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc=-negamax(np);
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               /* calcul pour avoir aleatoirement le meilleur mouvement, inutile si on ne s'interesse qu'au score */
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   /* pour "retourner" le mouvement, on le copie sur le pointeur resultat */
   COPIE_MOUVEMENT(lm,bmove,resultat);
   /* le score retourne est le meilleur score calcule */
   return bscore;
}

int negamax2(partie *d, int s){
    liste_mouvements lm;
    remplit_liste_mouvements(d,&lm);
    if(est_termine(d) || s==0)return get_score(d)*get_player(d);
    int sc=-SCORE_VICTORY;
    for(int i=0; i<GET_NB_MOUVEMENT(lm);i++){
        partie *f=joue_coup_suivant(d,GET_MOUVEMENT(lm,i));
        int sc2=-negamax2(f,s-1);
        if(sc<sc2)sc=sc2;
    }
    return sc;
}


/**** analyse partielle du jeu ***/
int AI_ordi_3(partie *p, mouvement *resultat) {
    liste_mouvements lm;
   /* on verifie si le jeu n'est pas termine. Normalement AI_ordi ne devrait pas etre appel‚ dans ce cas */
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie terminee ?\n");
      exit(EXIT_FAILURE);
   }
   /* recuperation de la liste des mouvements dans lm (structure) */
   remplit_liste_mouvements(p,&lm);
   //int sco;
   int bscore =-SCORE_VICTORY-1; /* meilleur score (initialise a pire que le meilleur score possible) */
   int nb_bscore=0; /* nombre de "meilleurs scores" */
   int bmove = -1; /* meilleur mouvement */
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        /* pour chaque mouvement, on calcule la nouvelle position */
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc=-negamax2(np,4);
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               /* calcul pour avoir aleatoirement le meilleur mouvement, inutile si on ne s'interesse qu'au score */
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   /* pour "retourner" le mouvement, on le copie sur le pointeur resultat */
   COPIE_MOUVEMENT(lm,bmove,resultat);
   /* le score retourne est le meilleur score calcule */
   return bscore;
}

int negamax_elag(partie *d, int s, int a, int b){
    liste_mouvements lm;
    remplit_liste_mouvements(d,&lm);
    if(est_termine(d) || s==0)return get_score(d)*get_player(d);
    int sc=-SCORE_VICTORY;
    for(int i=0; i<GET_NB_MOUVEMENT(lm);i++){
        partie *f=joue_coup_suivant(d,GET_MOUVEMENT(lm,i));
        int sc2=-negamax_elag(f,s-1,-b,-a);
        if(sc<sc2)sc=sc2;
        if(sc>a)a=sc;
        if(a>=b) return sc;
    }
    return sc;
}



/**** analyse partielle du jeu avec decroissance des scores gagnants ***/
int AI_ordi_4(partie *p, mouvement *resultat) {
    liste_mouvements lm;
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie terminee ?\n");
      exit(EXIT_FAILURE);
   }
   remplit_liste_mouvements(p,&lm);
   int a=-SCORE_VICTORY-1;
   int b=SCORE_VICTORY+1;
   int bscore =-SCORE_VICTORY-1;
   int nb_bscore=0;
   int bmove = -1;
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc=-negamax_elag(np,4,a,b);
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   COPIE_MOUVEMENT(lm,bmove,resultat);
   return bscore;
}

int negamax_elag2(partie *d, int s, int a, int b){
    liste_mouvements lm;
    if(est_termine(d) || s==0)return get_score(d)*get_player(d);
    remplit_liste_mouvements(d,&lm);
    tri_liste_mouvements(&lm,-get_player(d));
    int sc=-SCORE_VICTORY;
    for(int i=0; i<GET_NB_MOUVEMENT(lm);i++){
        partie *f=joue_coup_suivant(d,GET_MOUVEMENT(lm,i));
        int sc2=-negamax_elag(f,s-1,-b,-a);
        if(sc<sc2)sc=sc2;
        if(sc>a)a=sc;
        if(a>=b) return sc;
    }
    return sc;
}


/**** analyse partielle du jeu avec decroissance des scores gagnants et alpha-beta ***/
int AI_ordi_5(partie *p, mouvement *resultat) {
    liste_mouvements lm;
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie terminee ?\n");
      exit(EXIT_FAILURE);
   }
   remplit_liste_mouvements(p,&lm);
   int a=-SCORE_VICTORY-1;
   int b=SCORE_VICTORY+1;
   int bscore =-SCORE_VICTORY-1;
   int nb_bscore=0;
   int bmove = -1;
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc=-negamax_elag2(np,4,a,b);
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   COPIE_MOUVEMENT(lm,bmove,resultat);
   return bscore;
}

int negamax3(partie *d, int s){
    liste_mouvements lm;
    if(est_termine(d) || s==0)return get_score(d)*get_player(d);
    remplit_liste_mouvements(d,&lm);
    int sc=-SCORE_VICTORY;
    for(int i=0; i<GET_NB_MOUVEMENT(lm);i++){
        partie *f=joue_coup_suivant(d,GET_MOUVEMENT(lm,i));
        int sc2=-negamax3(f,s-1);
        if(sc2>(SCORE_VICTORY/2) || -sc2>(SCORE_VICTORY/2)){
            if(sc2<0)sc2=-sc2;
            sc2=sc2/2;
        }
        if(sc<sc2)sc=sc2; //max
    }
    return sc;
}

/**** analyse partielle du jeu avec decroissance des scores gagnants et alpha-beta efficace ***/
int AI_ordi_6(partie *p, mouvement *resultat) {
   liste_mouvements lm;
   if (est_termine(p)) {
      fprintf(stderr,"AI_ordi appel‚ sur partie terminee ?\n");
      exit(EXIT_FAILURE);
   }
   remplit_liste_mouvements(p,&lm);
   int bscore =-SCORE_VICTORY-1;
   int nb_bscore=0;
   int bmove = -1;
   for (int i=0;i<GET_NB_MOUVEMENT(lm);i++) {
        partie *np = joue_coup_suivant(p,GET_MOUVEMENT(lm,i));
        int sc=-negamax3(np,6);
        if (sc>bscore) {
            bscore = sc;
            nb_bscore=1;
            bmove=i;
        } else if (sc==bscore) {
               nb_bscore++;
               if (rand()%nb_bscore==0) bmove=i;
        }
   }
   COPIE_MOUVEMENT(lm,bmove,resultat);
   return bscore;
}

/**** analyse partielle du jeu avec decroissance des scores gagnants et alpha-beta efficace  et coups critiques ***/
int AI_ordi_7(partie *p, mouvement *resultat){
    return 0;
}

