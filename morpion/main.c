#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include "morpion2.h"
#include "time.h"

/* num des joueurs : 0 : humain >0 : ordi */
int joueur1;
int joueur2;

int AI_ordi(partie *p, mouvement *resultat, int jr) {
   switch (jr) {
       case 1 : return AI_ordi_1(p,resultat);
       case 2 : return AI_ordi_2(p,resultat);
       case 3 : return AI_ordi_3(p,resultat);
       case 4 : return AI_ordi_4(p,resultat);
       case 5 : return AI_ordi_5(p,resultat);
       case 6 : return AI_ordi_6(p,resultat);
       case 7 : return AI_ordi_7(p,resultat);
       default : return AI_ordi_0(p,resultat);
   }
}

partie *jeu_ordi(partie *p, int jr) {
    partie *cp = duplique_position_actuelle(p);
    mouvement m;
    unsigned int starttemps = graph_GetTicks();
    int bscore = AI_ordi(cp,&m,jr);
    graph_printf(" score retourne : %d   en %ld ms",bscore,graph_GetTicks()-starttemps);
    printf(" score retourne : %d   en %lu ms\n",bscore,(unsigned long int) (graph_GetTicks()-starttemps));

    libere_partie(cp);
    if (coup_possible(p,&m)>0)
        return joue_coup_suivant(p,&m);
    fprintf(stderr,"Erreur : AI_ordi a retourné un coup invalide !!!\n");
    exit(EXIT_FAILURE);
}

int introduction() {
    int n;
    int statut;
    graph_printf("Jeu de morpion (variante n,m,k)");
    statut=-1;
    while (statut!=0 && statut!=1) {
      graph_printf("Voulez-vous (0) faire une partie (1) tester une A.I. ?");
      graph_scanf("%d",&statut);
    }
    if (statut==0) {
        n=-1;
        while (n<1 || n>MAX_SIZE_COLONNES) {
            graph_printf("Nombre colonnes (max %d) : ",MAX_SIZE_COLONNES);
            graph_scanf("%d",&n);
        }
        SIZE_COLONNES=n;
        n=-1;
        while (n<1 || n>MAX_SIZE_LIGNES) {
            graph_printf("Nombre lignes (max %d) : ",MAX_SIZE_LIGNES);
            graph_scanf("%d",&n);
        }
        SIZE_LIGNES=n;
        n=-1;
        while (n<1 || n>MAX_SIZE_LIGNES) {
            graph_printf("Nombre de pions à aligner (max %d) : ",MAX_ALIGN);
            graph_scanf("%d",&n);
        }
        SIZE_ALIGN=n;
        graph_printf("Le joueur noir commence.");
        n=-1;
        while (n<0 || n>8) {
           graph_printf("Joueur noir : (0) humain (1 à 8) A.I. ?");
           graph_scanf("%d",&n);
        }
        joueur1 = n;
        n=-1;
        while (n<0 || n>8) {
           graph_printf("Joueur blanc : (0) humain (1 à 8) A.I. ?");
           graph_scanf("%d",&n);
        }
        joueur2 = n;
    } else {
        n = -2;
        while (n<-1 || n>8) {
          graph_printf("Quelle A.I. souhaitez-vous tester (0 à 8) ?");
          graph_scanf("%d",&n);
        }
        joueur1 = n;
        n = -1;
        while (n<0 || n>9) {
          graph_printf("Jusqu'a quel fichier de test (0 à 9) ?");
          graph_scanf("%d",&n);
        }
        joueur2 = n;
    }
    return statut;
}

int test_AI_on_file(FILE *fichier,char *nomtest) {
      partie *p;
      int bon;
      char nom[50];
      char grille[MAX_SIZE_LIGNES][MAX_SIZE_COLONNES];
      fscanf(fichier,"%d%d%d",&SIZE_LIGNES, &SIZE_COLONNES, &SIZE_ALIGN);
      p = cree_partie_initiale();
      int tmpval;
      fscanf(fichier,"%d ",&tmpval);
      {
          int i,j;
          mouvement c;
          for (i=0;i<SIZE_LIGNES;i++) {
             fscanf(fichier,"%s",nom);
             for (j=0;j<SIZE_COLONNES;j++) {
                grille[i][j]=0;
                if (nom[j]=='B' || nom[j]=='N') {
                   c.joueur=(nom[j]=='B' ? JBLANC : JNOIR);
                   c.ligne=i; c.colonne=j;
                   change_position(p->position, &c, 1);
                   p->num_coups++;
                   calcul_fin_partie(p,&c);
                } else if (nom[j]=='s') {
                    grille[i][j]=1;
                }
             }
          }
      }
      p->position->joueur=tmpval;
      draw_grid();
      update_grille(p->position);
      {
          int i,score;
          mouvement c;
          Uint32 t, t_total=0;
          char answer[100];
          bon=0;
          for (i=0;i<5;i++) {
              t = graph_GetTicks();
              score=AI_ordi(p,&c,joueur1);
              printf("%s (essai %d) : (%d;%d) score %d\n",nomtest,i,c.ligne, c.colonne, score);
              t_total+=(graph_GetTicks()-t);
              if (grille[c.ligne][c.colonne]==1) bon++;
          }
          sprintf(answer,"%s : %d corrects (sur 5 essais) (score: %d) en %d ms",nomtest,bon,score,t_total);
          graph_puts(answer);
          puts(answer);
      }
      /* liberer la partie */
      libere_partie(p);
      return bon;
}

void test_AI() {
   FILE *fichier;
   char nom[15];
   int nb=0;
   while (nb<=joueur2) {
      sprintf(nom,"test%d.txt",nb);
      printf("Ouverture %s\n",nom);
      fichier = fopen(nom,"r");
      if (fichier==NULL) break;
      test_AI_on_file(fichier,nom);
      nb++;
   }
}

partie *jouer_coup(partie *p, int jr) {
    if (jr==0) { /* humain */
        int col,lig;
        mouvement m;
        m.joueur=p->position->joueur;
        graph_printf("Au joueur %s (humain).",m.joueur==JNOIR ? "noir" : "blanc");
        while (1) {
           get_click_grille(&col,&lig);
           if (p->position->grille[lig][col]!=0) continue;
           m.ligne=lig;
           m.colonne=col;
           if (coup_possible(p,&m)>0) {
                return joue_coup_suivant(p,&m);
           }
        }
    } else {
        graph_printf("Au joueur %s (AI %d).",p->position->joueur==JNOIR ? "noir" : "blanc", jr);
        return jeu_ordi(p,jr);
    }
}

int jouer_partie(partie *p) {
    update_grille(p->position);
    while (p!=NULL && p->termine<=0) {
        p = jouer_coup(p,joueur1);
        update_grille(p->position);
        graph_vide_pile_evenements();
        // int i; for (i=0;i<SIZE_ALIGN;i++) printf("%d,%d ", p->position->nb_alignements[i][0], p->position->nb_alignements[i][1]);
        // printf("\n");
        if (p==NULL || p->termine>0) break;
        p = jouer_coup(p,joueur2);
        update_grille(p->position);
        graph_vide_pile_evenements();
        // for (i=0;i<SIZE_ALIGN;i++) printf("%d,%d ", p->position->nb_alignements[i][0], p->position->nb_alignements[i][1]);
        // printf("\n");
    }
    if (p==NULL) {
        graph_printf("Erreur de partie !");
        return -1;
    }
    if (p->score>0) {
        graph_printf("Terminé. Le joueur blanc a gagné.");
    } else if (p->score<0) {
        graph_printf("Terminé. Le joueur noir a gagné.");
    } else {
        graph_printf("Match nul.");
    }
    return 0;
}


int graphics_main()
{
    char s[100];
    srand(time(NULL));
    int status = introduction();
    if (status==0) {
        partie *p = cree_partie_initiale();
        draw_grid();
        jouer_partie(p);
        libere_partie(p);
    } else {
        if (joueur1==-1) {
            for (joueur1=0;joueur1<9;joueur1++) {
                printf("AI %d\n",joueur1);
                test_AI();
            }
        }
        else
        test_AI();
    }
    graph_printf("Entree pour terminer...");
    graph_gets(s);
    return 0;
}
