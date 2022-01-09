#include "morpion2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int SIZE_LIGNES;     /* nombre de lignes */
int SIZE_COLONNES;   /* nombre de colonnes */
int SIZE_ALIGN;      /* taille de l'alignement pour gagner */


partie *cree_partie_initiale () {
   partie *retour=  calloc(1,sizeof(partie));
   retour->position= calloc(1,sizeof(damier));
   retour->position->nb_vide = SIZE_LIGNES*SIZE_COLONNES;
   retour->position->joueur = JNOIR;
//   retour->position->score = 0;
//   retour->num_coups = 0;
//   retour->position->num_coups = 0;
   retour->position_prec = retour->position_suiv = NULL;
   return retour;
}

/* calcul d'un score absolu (ou de sa modif) avec nbalign */
int calcule_score_align(int nbalign[][2],int evalmove) {
    int s=0, i;
    if (nbalign[SIZE_ALIGN-1][0]>0) return -SCORE_VICTORY;
    if (nbalign[SIZE_ALIGN-1][1]>0) return SCORE_VICTORY;
    if (evalmove) { /* il faut absolument réduire les SIZE_ALIGN-2 si possible */
       if (nbalign[SIZE_ALIGN-2][0]<0) return SCORE_VICTORY/2;
       if (nbalign[SIZE_ALIGN-2][1]<0) return -SCORE_VICTORY/2;
    }
    for (i=SIZE_ALIGN-2;i>=0;i--) {
         int u = nbalign[i][1]-nbalign[i][0];
         if (!evalmove || s!=0) { if (u>3) u=3; else if (u<-3) u=-3; }
         s += (2<<(2*i))*u;
    }
    return s;
}

/* fonction d'usage pour la fonction suivante */
void update_alignment(damier *d, int player, int sens, int modifie, int nbalign[][2], int nbdir, int l, int c) {
      int aplay = d->alignements[l][c][nbdir][player];
      int eplay = d->alignements[l][c][nbdir][1-player];
      if (modifie) {
        d->alignements[l][c][nbdir][player]+=sens;
      }
      if (sens==1) {
          if (eplay==0) {
              nbalign[aplay][player]++;
              if (aplay>0) nbalign[aplay-1][player]--;
          } else if (aplay==0) {
              nbalign[eplay-1][1-player]--;
          }
      } else {
          if (eplay==0) {
              nbalign[aplay-1][player]--;
              if (aplay>1) nbalign[aplay-2][player]++;
          } else if (aplay==1) nbalign[eplay-1][1-player]++;
      }
}

/* mise à jour des alignements, ou scoring d'un coup. fait un update des nb_align */
void update_alignments(damier *d, const mouvement *c, int sens, int modifie, int nbalign[][2]) {
   int player = (c->joueur == JNOIR ? 0 : 1);
   int i,j;
   for (i=0;i<SIZE_ALIGN;i++) {
      nbalign[i][0] = nbalign[i][1] = 0;
   }
   int bound_col = SIZE_COLONNES-SIZE_ALIGN;
   int bound_lig = SIZE_LIGNES-SIZE_ALIGN;
   /* horizontal */
   i = c->colonne; j=0;
   while (i>=0 && j<SIZE_ALIGN) {
       if (i<=bound_col)
           update_alignment(d,player,sens,modifie,nbalign,DIR_HORIZ,c->ligne,i);
       i--; j++;
   }
   /* vertical */
   i = c->ligne; j=0;
   while (i>=0 && j<SIZE_ALIGN) {
       if (i<=bound_lig)
           update_alignment(d,player,sens,modifie,nbalign,DIR_VERT,i,c->colonne);
       i--; j++;
   }
   /* diag1 */
   i = 0;
   while (c->ligne-i>=0 && c->colonne-i>=0 && i<SIZE_ALIGN) {
       if (c->colonne-i<=bound_col && c->ligne-i<=bound_lig)
            update_alignment(d,player,sens,modifie,nbalign,DIR_DIAG1,c->ligne-i,c->colonne-i);
       i++;
   }
   /* diag2 */
   i = 0;
   while (c->ligne-i>=0 && c->colonne+i<SIZE_COLONNES && i<SIZE_ALIGN) {
       if (c->ligne-i<=bound_lig && c->colonne+i>=SIZE_ALIGN-1)
          update_alignment(d,player,sens,modifie,nbalign,DIR_DIAG2,c->ligne-i,c->colonne+i);
       i++;
   }
   if (modifie) {
      for (i=0;i<SIZE_ALIGN;i++) {
        d->nb_alignements[i][0] += nbalign[i][0];
        d->nb_alignements[i][1] += nbalign[i][1];
      }
   }
}


void change_position(damier *d, const mouvement *c, int sens) {
    d->num_coups+=sens;
    d->grille[c->ligne][c->colonne]=(sens ==1 ? c->joueur : 0);
    d->nb_vide-=sens;
    d->joueur = -d->joueur;
    {
      int nbalign[MAX_ALIGN][2];
      update_alignments(d,c,sens,1,nbalign);
    }
}

/* fait correspondre p->num_coups et d->num_coups */
void actualise_partie(const partie *p, damier *d) {
    const partie *p2;
    if (p->num_coups == d->num_coups) return;
    if (p->num_coups>d->num_coups) {
        p2 = p->position_prec;
        while (p2 && p2->num_coups > d->num_coups)
            p2 = p2->position_prec;
        if (p2==NULL || p2->num_coups!=d->num_coups) {
            fprintf(stderr,"Erreur dans la structure du jeu. (actualise_partie, 1)\n");
            exit(EXIT_FAILURE);
        }
        while (p2!=p) {
            change_position(d,&(p2->coup_suivant),1);
            p2 = p2->position_suiv;
        }
    } else {
        p2=p;
        while (p2->position_suiv && p2->position_suiv->num_coups < d->num_coups) {
            p2 = p2->position_suiv;
        }
        if (p2->position_suiv==NULL || p2->position_suiv->num_coups!=d->num_coups) {
            fprintf(stderr,"Erreur dans la structure du jeu. (actualise_partie, 2)\n");
            exit(EXIT_FAILURE);
        }
        change_position(d,&(p2->coup_suivant),-1);
        while (p2!=p) {
            p2 = p2->position_prec;
            change_position(d,&(p2->coup_suivant),-1);
        }
    }
}

/* -1 : hors borne ou termine ou NULL        -2 : mauvais joueur
   -3 : position déjà occupée
   1 : ok */
int coup_possible (const partie *p, const mouvement *c) {
   if ((p==NULL) || (c==NULL)) return -1;
   actualise_partie(p,p->position);
   if (p->termine!=0) return -1;
   if ((c->ligne<0) || (c->colonne<0) || (c->ligne>=SIZE_LIGNES) || (c->colonne>=SIZE_COLONNES))
        return -1;
   if (p->position->joueur!= c->joueur) return -2;
   if (p->position->grille[c->ligne][c->colonne]!=0) return -3;
   return 1;
}

int remplit_liste_mouvements(const partie *p, liste_mouvements *lm) {
    int l,c,nbm=0;
    if (p->termine==1) return 0;
    actualise_partie(p,p->position);
    for (l=0;l<SIZE_LIGNES;l++) {
        for (c=0;c<SIZE_COLONNES;c++) {
            int nbalign[MAX_ALIGN][2];
            if (p->position->grille[l][c]!=0) continue;
            lm->liste[nbm].colonne=c;
            lm->liste[nbm].ligne=l;
            lm->liste[nbm].joueur=p->position->joueur;
            update_alignments(p->position,&(lm->liste[nbm]),1,0,nbalign);
            lm->liste[nbm].score = calcule_score_align(nbalign,1);
            nbm++;
        }
    }
    lm->nb_moves = nbm;
    return nbm;
}

/* calcule si la partie est terminée, et le score (selon les jeux, en fonction du dernier coup) */
int calcul_fin_partie(partie *p, mouvement *c) {

    if (p->position->nb_alignements[SIZE_ALIGN-1][0]>0) {
        p->termine=1;
        p->score = -SCORE_VICTORY;
        return 1;
    }
    if (p->position->nb_alignements[SIZE_ALIGN-1][1]>0) {
        p->termine=1;
        p->score=SCORE_VICTORY;
        return 1;
    }
    if (p->position->nb_vide==0) {
        p->score=0;
        p->termine=1;
        return 1;
    }
    {
        p->score = calcule_score_align(p->position->nb_alignements,0);
    }
    p->termine=0;
    return 0;
}


/* nouveau mouvement */
/* note : c->ligne==-1 : passe son tour */
partie *joue_coup_suivant(partie *p, mouvement *c) {
    int ok = coup_possible(p,c);    /* actualise si nécessaire */
    /* on vérifie, quand même, que le coup est valide */
    if (ok==-1) {
        fprintf(stderr,"out-of-bound move\n");
        exit(-1);
    }
    if ((ok< -1) || (ok==0)) {
        fprintf(stderr,"tentative de coup invalide : %d %d %d %d %d!\n",c->colonne, c->ligne,c->joueur,p->position->joueur,ok);
        exit(-1);
    }
    {
        partie *newpart;
        if (p->position_suiv==NULL)
        {
            p->position_suiv = (partie *) malloc(sizeof(partie));
            newpart = p->position_suiv;
            newpart->position_suiv=NULL;
            newpart->position_prec=p;
            newpart->num_coups = p->num_coups+1;
        } else { newpart = p->position_suiv; }
        newpart->position=p->position;
        newpart->coup_suivant.joueur=0;
        memcpy(&(p->coup_suivant),c,sizeof(mouvement));
        change_position(newpart->position,c,1);

        calcul_fin_partie(newpart,c);
        return newpart;
    }
}

partie *parcours_arriere(partie *p) {
    partie *pprec = p->position_prec;
    if (pprec==NULL || pprec->coup_suivant.joueur==0) return NULL;
    actualise_partie(pprec,p->position);
    return pprec;
}
partie *parcours_avant(partie *p) {
    if (p->coup_suivant.joueur==0) return NULL;
    actualise_partie(p->position_suiv,p->position);
    return p->position_suiv;
}

void bloque_avenir(partie *p) {
    actualise_partie(p,p->position);
    memset(&(p->coup_suivant),0,sizeof(mouvement));
}

partie *duplique_position_actuelle(partie *p) {
    partie * newpart = calloc(1,sizeof(partie));
    damier * newpos = calloc(1,sizeof(damier));
    actualise_partie(p,p->position);
    memcpy(newpart,p,sizeof(partie));
    memcpy(newpos,p->position,sizeof(damier));
    newpart->position_prec=NULL;
    newpart->position=newpos;
    bloque_avenir(newpart);
    newpart->position_suiv=NULL;
    return newpart;
}

void libere_partie(partie *p) {
     if (p->position_prec!=NULL) {
        fprintf(stderr,"libere_partie non appelé sur départ !\n");
        exit(EXIT_FAILURE);
     }
     free(p->position);
     partie *p2=p;
     while (p2!=NULL) {
        p = p2;
        p2 = p->position_suiv;
        free(p);
     }
}

int get_player(const partie *p) {
    int a = p->position->joueur;
    if (p->num_coups == p->position->num_coups)
        return a;
    int b = p->num_coups - p->position->num_coups;
    while (b>0) { a= -a; b--;}
    while (b<0) { a=-a; b++;}
    return a;
}
int get_score(const partie *p) { return p->score; }
int get_score_final(const partie *p) { return p->score * p->termine; }

int position_critique(const partie *p) {
   return (p->position->nb_alignements[SIZE_ALIGN-2][0]>=1 || p->position->nb_alignements[SIZE_ALIGN-2][1]>=1);
}

int comparecroissant(const void* pm1, const void* pm2) {
    mouvement *m1 = (mouvement *)pm1;
    mouvement *m2 = (mouvement *)pm2;
    return m1->score - m2->score;
}
int comparedecroissant(const void* pm1, const void* pm2) {
    mouvement *m1 = (mouvement *)pm1;
    mouvement *m2 = (mouvement *)pm2;
    return m2->score - m1->score;
}

void tri_liste_mouvements(liste_mouvements *lm, int order) {
  qsort(lm->liste,lm->nb_moves,sizeof(mouvement),(order==1 ? comparecroissant : comparedecroissant));
}
