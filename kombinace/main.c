#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VSTUPNI_SOUBOR "startovni_listina_kombinace_zeny.txt"
#define VYSTUPNI_SOUBOR "vysledkova_listina_kombinace_zeny.txt"
#define MAX_RADKA 100

typedef struct {
    int minuty;
    int sekundy;
    int setiny;
} CAS;

typedef struct {
    int startCislo;
    char jmeno[20];
    char prijmeni[30];
    char stat[4];
    CAS sjezd;
    CAS slalom;
} ZAVODNICE;

int casNaSetiny(CAS t) {
    return (t.minuty * 60 + t.sekundy) * 100 + t.setiny;
}

CAS setinyNaCas(int s) {
    CAS t;
    t.minuty = s / 6000;
    t.sekundy = (s % 6000) / 100;
    t.setiny = s % 100;
    return t;
}

CAS soucetCasu(CAS a, CAS b) {
    return setinyNaCas(casNaSetiny(a) + casNaSetiny(b));
}

CAS rozdilCasu(CAS a, CAS b) {
    return setinyNaCas(casNaSetiny(a) - casNaSetiny(b));
}

void vymen(ZAVODNICE *a, ZAVODNICE *b) {
    ZAVODNICE tmp = *a;
    *a = *b;
    *b = tmp;
}

void trideni(ZAVODNICE *pole, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int casA = casNaSetiny(soucetCasu(pole[j].sjezd, pole[j].slalom));
            int casB = casNaSetiny(soucetCasu(pole[j + 1].sjezd, pole[j + 1].slalom));
            if (casA > casB) {
                vymen(&pole[j], &pole[j + 1]);
            }
        }
    }
}

CAS nactiCas(const char *retezec) {
    CAS t;
    sscanf(retezec, "%d:%d,%d", &t.minuty, &t.sekundy, &t.setiny);
    return t;
}

ZAVODNICE *nactiSoubor(int *pocet) {
    FILE *f = fopen(VSTUPNI_SOUBOR, "r");
    if (!f) {
        printf("Nelze otevřít vstupní soubor.\n");
        return NULL;
    }

    ZAVODNICE *pole = NULL;
    int idx = 0;
    char radek[MAX_RADKA];

    while (fgets(radek, MAX_RADKA, f)) {
        if (idx == 0) { idx++; continue; } // přeskočit hlavičku
        ZAVODNICE *tmp = realloc(pole, (idx) * sizeof(ZAVODNICE));
        if (!tmp) {
            printf("Chyba alokace paměti.\n");
            free(pole);
            fclose(f);
            return NULL;
        }
        pole = tmp;

        char sjezdStr[20], slalomStr[20];
        sscanf(radek, "%d %s %s %s %s %s",
               &pole[idx - 1].startCislo,
               pole[idx - 1].jmeno,
               pole[idx - 1].prijmeni,
               pole[idx - 1].stat,
               sjezdStr, slalomStr);

        pole[idx - 1].sjezd = nactiCas(sjezdStr);
        pole[idx - 1].slalom = nactiCas(slalomStr);

        idx++;
    }

    fclose(f);
    *pocet = idx - 1;
    return pole;
}

void vypisStartovniListina(ZAVODNICE *z, int pocet) {
    printf("A L P S K E   L Y Z O V A N I  -  K O M B I N A C E  Z E N Y\n");
    printf("-----------------------------------------------------------\n");
    printf("poradi jmeno           prijmeni       stat  sjezd   slalom\n");
    for (int i = 0; i < pocet; i++) {
        printf("%2d.    %-14s %-15s %-4s %d:%02d,%02d   %d:%02d\n",
               z[i].startCislo,
               z[i].jmeno,
               z[i].prijmeni,
               z[i].stat,
               z[i].sjezd.minuty, z[i].sjezd.sekundy, z[i].sjezd.setiny,
               z[i].slalom.minuty, z[i].slalom.sekundy, z[i].slalom.setiny);
    }
    printf("\n");
}

void ulozVysledky(ZAVODNICE *z, int n) {
    FILE *f = fopen(VYSTUPNI_SOUBOR, "w");
    if (!f) {
        printf("Nelze zapsat do vystupniho souboru.\n");
        return;
    }

    fprintf(f, "A L P S K E   L Y Z O V A N I  -  K O M B I N A C E  Z E N Y\n");
    fprintf(f, "-----------------------------------------------------------\n");
    fprintf(f, "poradi jmeno           prijmeni        stat sjezd   slalom  celkem ztrata\n");

    CAS casVitezky = soucetCasu(z[0].sjezd, z[0].slalom);

    for (int i = 0; i < n; i++) {
        CAS celkem = soucetCasu(z[i].sjezd, z[i].slalom);
        CAS ztrata = rozdilCasu(celkem, casVitezky);

        fprintf(f, "%2d.    %-14s %-15s %-4s %d:%02d,%02d %d:%02d,%02d %d:%02d,%02d",
                i + 1,
                z[i].jmeno,
                z[i].prijmeni,
                z[i].stat,
                z[i].sjezd.minuty, z[i].sjezd.sekundy, z[i].sjezd.setiny,
                z[i].slalom.minuty, z[i].slalom.sekundy, z[i].slalom.setiny,
                celkem.minuty, celkem.sekundy, celkem.setiny);

        if (i == 0) {
            fprintf(f, "\n");
        } else {
            fprintf(f, "   +%d:%02d,%02d\n", ztrata.minuty, ztrata.sekundy, ztrata.setiny);
        }
    }

    fclose(f);
    printf("Soubor %s byl vytvoren.\n", VYSTUPNI_SOUBOR);
}

int main() {
    int pocet;
    ZAVODNICE *data = nactiSoubor(&pocet);
    if (!data) return 1;

    vypisStartovniListina(data, pocet);
    trideni(data, pocet);
    ulozVysledky(data, pocet);

    free(data);
    return 0;
}
