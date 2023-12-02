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
    printf("\033[2J");
    printf("\033[H");
    printf("---------------------------\n");
    for (int j = UKURAN - 1; j >= 0; --j) {
        for (int i = 0; i < UKURAN; ++i) {
            if (p->ubin[i][j]) {
                switch (p->ubin[i][j]) {
                    case 2:
                        printf("\033[38;5;1m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 4:
                        printf("\033[38;5;9m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 8:
                        printf("\033[38;5;10m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 16:
                        printf("\033[38;5;11m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 32:
                        printf("\033[38;5;12m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 64:
                        printf("\033[38;5;13m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 128:
                        printf("\033[38;5;14m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 256:
                        printf("\033[38;5;15m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 512:
                        printf("\033[38;5;16m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 1024:
                        printf("\033[38;5;17m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    case 2048:
                        printf("\033[38;5;18m|%4d| \033[0m", p->ubin[i][j]);
                        break;
                    default:
                        printf("|%4d| ", p->ubin[i][j]);
                        break;
                }
            } else {
                printf("|    | ");
            }
        }
        printf("\n---------------------------\n");
    }
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

void perbarui_skor(struct permainan* p, int* skor_baru)
{
    *skor_baru = 0;
    for (int i = 0; i < UKURAN; ++i)
        for (int j = 0; j < UKURAN; ++j)
            if (p->ubin[i][j] != 0)
                *skor_baru += p->ubin[i][j];
}

int permainan_berakhir(struct permainan* p)
{
    for (int i = 0; i < UKURAN; ++i) {
        for (int j = 0; j < UKURAN; ++j) {
            if (p->ubin[i][j] == 0) {
                return 0; // Ada petak kosong, permainan tidak berakhir
            }
            if (i < UKURAN - 1 && p->ubin[i][j] == p->ubin[i+1][j]) {
                return 0; // Ada petak horizontal yang berdampingan dengan nilai yang sama
            }
            if (j < UKURAN - 1 && p->ubin[i][j] == p->ubin[i][j+1]) {
                return 0; // Ada petal yang berdampingan secara vertikal dengan nilai yang sama
            }
        }
    }
    return 1; // Tidak ada gerakan yang valid, permainan berakhir
}

void menu()
{
    printf("\033[38;5;1mMAIN MENU\033[0m\n");
    printf("1. New Game\n");
    printf("2. Display Score\n");
    printf("3. Exit\n");
}

int main()
{
    int c;
    int option;
    struct permainan p = { .skor = 0 };
    menu();

    do
    {
        printf("\033[38;5;1mEnter option (1-3): \033[0m");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            mulai(&p);
            tampilkan(&p);
            perbarui_skor(&p, &p.skor);
            printf("\033[38;5;1mSKOR: %d\033[0m\n", p.skor);
            ambil_stdin();
            while ((c = baca_gerakan()) != EOF)
            {
                int skor_lama = p.skor;
                gerak(&p, c);
                perbarui_skor(&p, &p.skor);
                tampilkan(&p);
                printf("\033[38;5;1mSKOR: %d\033[0m\n", p.skor);

                if (permainan_berakhir(&p))
                {
                    printf("\033[38;5;1mPERMAINAN BERAKHIR!\033[0m\n");
                    printf("\033[38;5;1mSKOR: %d\033[0m\n", p.skor);
                    break;
                }
            }
            berikan_stdin();
            break;
        case 2:
            printf("\033[38;5;1mDisplay Score: %d\033[0m\n", p.skor);
            break;
        case 3:
            printf("\033[38;5;1mExit\033[0m\n");
            break;
        default:
            printf("\033[38;5;1mInvalid option\033[0m\n");
            break;
        }

        menu();
    } while (option != 3);

    return 0;
}
