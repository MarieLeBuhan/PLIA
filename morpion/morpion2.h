#ifndef MORPION2_H_INCLUDED
#define MORPION2_H_INCLUDED

#define MAX_SIZE_LIGNES 12
#define MAX_SIZE_COLONNES 12
#define MAX_ALIGN 8
#define JNOIR  (-1)         /* rond */
#define JBLANC  (1)         /* croix */

#define SCORE_VICTORY 10000000

/*** description des alignements */
#define NB_DIRECTIONS 4
#define DIR_HORIZ     0
#define DIR_VERT      1
#define DIR_DIAG1     2
#define DIR_DIAG2     3
#define ALIGN_OFFSET  0x10

/* "constantes" globales, réellement définies dans main.c */
extern int SIZE_LIGNES;     /* nombre de lignes */
extern int SIZE_COLONNES;   /* nombre de colonnes */
extern int SIZE_ALIGN;      /* taille de l'alignement pour gagner */


/* description d'un jeu */
typedef struct st_damier {
    int num_coups;                                      /* pour vérifier adéquation damier/partie */
    char grille[MAX_SIZE_LIGNES][MAX_SIZE_COLONNES];    /* grille contenant les pions */
    char alignements[MAX_SIZE_LIGNES][MAX_SIZE_COLONNES][NB_DIRECTIONS][2];   /* alignements */
    int nb_alignements[MAX_ALIGN][2];
    unsigned int nb_vide;                               /* nombre de cases vides */
    char joueur;                                        /* joueur en cours */
} damier;

/* mouvement */
typedef struct st_mouvement {
    char joueur;
    int ligne, colonne;
    int score;
} mouvement;


typedef struct st_lmouvements {
    mouvement liste[MAX_SIZE_LIGNES*MAX_SIZE_COLONNES];
    int nb_moves;
} liste_mouvements;

/* partie */
typedef struct st_partie {
    damier *position;
    int num_coups;
    int score;
    int termine; /* 0 : rien de spécial ; 1 : terminé ; -1 : ne peut pas jouer */
    mouvement coup_suivant; /* coup_suivant.joueur = 0 => position_suiv pointe sur une "fausse" partie */
    struct st_partie *position_prec, *position_suiv;
} partie;

partie *cree_partie_initiale ();  /* crée un jeu initial */
int get_player(const partie *p);
int get_score(const partie *p);
int get_score_final(const partie *p);
int position_critique(const partie *p);
int coup_possible (const partie *p, const mouvement *c);
int remplit_liste_mouvements(const partie *p, liste_mouvements *lm);
void tri_liste_mouvements(liste_mouvements *lm, int order);
partie *joue_coup_suivant(partie *p, mouvement *c); /* joue un coup sur le jeu en cours */
partie *parcours_arriere(partie *p); /* retourne la position precedence */
partie *parcours_avant(partie *p); /* retourne la position suivante, si elle correspond à quelque chose */
partie *parcours_present(partie *p); /* retourne la position "maximale" en avant */
void bloque_avenir(partie *p); /* reinitialise la partie au coup actuel, détruisant les coups suivants */
partie *duplique_position_actuelle(partie *p); /* duplique la position actuelle, isolée */
static inline int est_termine(partie *p) { return p->termine==1; }
#define GET_NB_MOUVEMENT(lm)            (lm.nb_moves)
#define GET_SCORE_MOUVEMENT(lm,nb)      (lm.liste[nb].score)
#define GET_MOUVEMENT(lm,nb)            (&(lm.liste[nb]))
#define COPIE_MOUVEMENT(lm,nb,m)        (memcpy((m),(&(lm.liste[nb])),sizeof(mouvement)))
void libere_partie(partie *p);

/* seulement pour main.c */
void change_position(damier *d, const mouvement *c, int sens);
int calcul_fin_partie(partie *p, mouvement *c);

/* declaration pour AIplayer.c */
int AI_ordi_0(partie *p, mouvement *resultat);
int AI_ordi_1(partie *p, mouvement *resultat);
int AI_ordi_2(partie *p, mouvement *resultat);
int AI_ordi_3(partie *p, mouvement *resultat);
int AI_ordi_4(partie *p, mouvement *resultat);
int AI_ordi_5(partie *p, mouvement *resultat);
int AI_ordi_6(partie *p, mouvement *resultat);
int AI_ordi_7(partie *p, mouvement *resultat);


#endif // MORPION2_H_INCLUDED
