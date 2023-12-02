#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#define UKURAN 4

struct permainan {
    int ubin[UKURAN][UKURAN];
    int skor;
};

enum {
    BAWAH,
    ATAS,
    KIRI,
    KANAN,
    GERAKAN
};

int posisi_acak(struct permainan* p)
{
    return rand() % UKURAN;
}

int nilai_acak(struct permainan* p)
{
    return (rand() % 10) ? 2 : 4;
}

void tampilkan(struct permainan* p)
{
    printf("--------------------------\n");
    for (int j = UKURAN - 1; j >= 0; --j) {
        printf("|");
        for (int i = 0; i < UKURAN; ++i) {
            if (p->ubin[i][j])
                printf("%4d  ", p->ubin[i][j]);
            else
                printf("      ");
        }
        printf("|\n");
    }
    printf("--------------------------\n");
}

void putar(struct permainan* p)
{
    struct permainan p2;
    for (int i = 0; i < UKURAN; ++i)
        for (int j = 0; j < UKURAN; ++j)
            p2.ubin[i][j] = p->ubin[j][i];
    *p = p2;
}

void balik(struct permainan* p)
{
    struct permainan p2;
    for (int i = 0; i < UKURAN; ++i)
        for (int j = 0; j < UKURAN; ++j)
            p2.ubin[i][j] = p->ubin[i][UKURAN - j - 1];
    *p = p2;
}

void mulai(struct permainan* p)
{
    struct permainan p2 = {};
    *p = p2;
    p->ubin[posisi_acak(p)][posisi_acak(p)] = nilai_acak(p);
    p->ubin[posisi_acak(p)][posisi_acak(p)] = nilai_acak(p);
}

void jatuh_kolom(int* a, int* b)
{
    int prev = 0;
    int j = 0;
    for (int i = 0; i < UKURAN; ++i) {
        if (a[i]) {
            if (a[i] == prev) {
                b[j - 1] *= 2;
                prev = 0;
            } else {
                b[j++] = a[i];
                prev = a[i];
            }
        }
    }
}

void jatuh(struct permainan* p)
{
    struct permainan p2 = {};
    for (int i = 0; i < UKURAN; ++i)
        jatuh_kolom(p->ubin[i], p2.ubin[i]);
    *p = p2;
}

int sama(struct permainan* a, struct permainan* b)
{
    for (int i = 0; i < UKURAN; ++i)
        for (int j = 0; j < UKURAN; ++j)
            if (a->ubin[i][j] != b->ubin[i][j])
                return 0;
    return 1;
}

int coba_jatuh(struct permainan* p)
{
    struct permainan p2 = *p;
    jatuh(p);
    if (sama(p, &p2))
        return 0;
    return 1;
}

void muncul(struct permainan* p)
{
    int i, j;
    while (1) {
        i = posisi_acak(p);
        j = posisi_acak(p);
        if (!p->ubin[i][j]) {
            p->ubin[i][j] = nilai_acak(p);
            return;
        }
    }
}

void gerak(struct permainan* p, int arah)
{
    if (arah / 2)
        putar(p);
    if (arah % 2)
        balik(p);
    if (coba_jatuh(p))
        muncul(p);
    if (arah % 2)
        balik(p);
    if (arah / 2)
        putar(p);
}

int baca_gerakan(void)
{
    char tombol[GERAKAN] = {'k','i','j','l'};
    int c;
    int i;
    while (isspace(c = getchar()));
    if (c == EOF)
        return c;
    for (i = 0; i < GERAKAN; ++i)
        if (c == tombol[i])
            return i;
    return EOF;
}

static struct termios cadangan;
static struct termios saat_ini;

void ambil_stdin(void)
{
    tcgetattr(STDIN_FILENO, &cadangan);
    saat_ini = cadangan;
    saat_ini.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &saat_ini);
}

void berikan_stdin(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &cadangan);
}

void perbarui_skor(struct permainan* p, int skor_lama)
{
    int skor_baru = 0;
    for (int i = 0; i < UKURAN; ++i)
        for (int j = 0; j < UKURAN; ++j)
            if (p->ubin[i][j] != 0)
                skor_baru += p->ubin[i][j];

    if (skor_baru != skor_lama)
        printf("\033[38;5;1mSKOR: %d\033[0m\n", skor_baru);
}

int permainan_berakhir(struct permainan* p)
{
    for (int i = 0; i < UKURAN - 1; ++i)
        for (int j = 0; j < UKURAN - 1; ++j)
            if (p->ubin[i][j] == p->ubin[i+1][j] || p->ubin[i][j] == p->ubin[i][j+1])
                return 0;
    return 1;
}


int main()
{
    int c;
    struct permainan p = { .skor = 0 };
    mulai(&p);
    tampilkan(&p);
    perbarui_skor(&p, p.skor);
    ambil_stdin();
    while ((c = baca_gerakan()) != EOF) {
        int skor_lama = p.skor;
        gerak(&p, c);
        tampilkan(&p);
        perbarui_skor(&p, skor_lama);

        if (permainan_berakhir(&p)) {
            printf("\033[38;5;1mPERMAINAN BERAKHIR!\033[0m\n");
            printf("\033[38;5;1mSKOR: %d\033[0m\n", p.skor);
            break;
        }
    }
    berikan_stdin();
    return 0;
}
