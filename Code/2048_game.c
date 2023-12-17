// Program C untuk permainan 2048
// Menggunakan library termios untuk input tanpa echo dan buffering
// Menggunakan ANSI escape codes untuk warna dan posisi kursor

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

// Konstanta untuk ukuran papan permainan
#define UKURAN 4

// Konstanta untuk warna ubin
#define RESET "\033[0m"
#define RED "\033[38;5;1m"
#define GREEN "\033[38;5;2m"
#define YELLOW "\033[38;5;3m"
#define BLUE "\033[38;5;4m"
#define MAGENTA "\033[38;5;5m"
#define CYAN "\033[38;5;6m"
#define WHITE "\033[38;5;7m"

// Konstanta untuk lebar ubin
#define WIDTH 4

// Konstanta untuk tombol input
#define UP 'i'
#define LEFT 'j'
#define DOWN 'k'
#define RIGHT 'l'
#define QUIT 'q'

// Struct untuk menyimpan data permainan
struct permainan {
    int ubin[UKURAN][UKURAN];
    int skor;
    char arah;
};

// Struct untuk menyimpan data skor
struct ScoreEntry {
    char username[20];
    int score;
};

// Fungsi untuk mengacak posisi ubin kosong pada papan permainan
int posisi_acak(struct permainan* p){
  return rand() % UKURAN;
}

// Fungsi untuk mengembalikan warna sesuai dengan nilai ubin
char* warna(int n) {
    switch (n) {
        case 2: return RED;
        case 4: return GREEN;
        case 8: return YELLOW;
        case 16: return BLUE;
        case 32: return MAGENTA;
        case 64: return CYAN;
        case 128: return WHITE;
        case 256: return RED;
        case 512: return GREEN;
        case 1024: return YELLOW;
        case 2048: return BLUE;
        default: return RESET;
    }
}

// Fungsi untuk menampilkan papan permainan
void tampil_papan(struct permainan* p) {
    system("clear");
    printf("Skor: %d\n", p->skor);
    printf("---------------------------\n");
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            if (p->ubin[i][j] == 0) {
                printf("|%*s| ", WIDTH, " ");
            } else {
                printf("%s|%*d| %s", warna(p->ubin[i][j]), WIDTH, p->ubin[i][j], RESET);
            }
        }
        printf("\n");
        printf("---------------------------\n");
    }
    printf("Tekan i, j, k, l untuk menggerakkan ubin ke atas, kiri, bawah, kanan.\n");
    printf("Tekan q untuk keluar dari permainan.\n");
}

// Fungsi untuk memutar papan permainan berlawanan jarum jam
void putar(struct permainan* p) {
    int temp[UKURAN][UKURAN];
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            temp[i][j] = p->ubin[j][UKURAN - i - 1];
        }
    }
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            p->ubin[i][j] = temp[i][j];
        }
    }
}

// Fungsi untuk membalik papan permainan secara horizontal
void balik(struct permainan* p) {
    int temp[UKURAN][UKURAN];
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            temp[i][j] = p->ubin[i][UKURAN - j - 1];
        }
    }
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            p->ubin[i][j] = temp[i][j];
        }
    }
}

// Fungsi untuk menggeser ubin ke kiri
void geser_kiri(struct permainan* p) {
    int x, y;
    for (int i = 0; i < UKURAN; i++) {
        x = 0;
        y = 1;
        while (y < UKURAN) {
            if (p->ubin[i][y] != 0) {
                if (p->ubin[i][x] == 0) {
                    p->ubin[i][x] = p->ubin[i][y];
                    p->ubin[i][y] = 0;
                } else if (p->ubin[i][x] == p->ubin[i][y]) {
                    p->ubin[i][x] *= 2;
                    p->skor += p->ubin[i][x];
                    p->ubin[i][y] = 0;
                    x++;
                } else {
                    x++;
                    if (x != y) {
                        p->ubin[i][x] = p->ubin[i][y];
                        p->ubin[i][y] = 0;
                    }
                }
            }
            y++;
        }
    }
}

// Fungsi untuk menggerakkan ubin sesuai dengan arah input
void gerak(struct permainan* p) {
    switch (p->arah) {
        case UP:
            putar(p);
            geser_kiri(p);
            putar(p);
            putar(p);
            putar(p);
            break;
        case LEFT:
            geser_kiri(p);
            break;
        case DOWN:
            putar(p);
            putar(p);
            putar(p);
            geser_kiri(p);
            putar(p);
            break;
        case RIGHT:
            balik(p);
            geser_kiri(p);
            balik(p);
            break;
    }
}

// Fungsi untuk mengecek apakah ada ubin kosong pada papan permainan
int ada_ubin_kosong(struct permainan* p) {
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            if (p->ubin[i][j] == 0) {
                return 1;
            }
        }
    }
    return 0;
}

// Fungsi untuk mengecek apakah ada ubin yang bisa digabungkan pada papan permainan
int ada_ubin_gabung(struct permainan* p) {
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN - 1; j++) {
            if (p->ubin[i][j] == p->ubin[i][j + 1]) {
                return 1;
            }
        }
    }
    for (int i = 0; i < UKURAN - 1; i++) {
        for (int j = 0; j < UKURAN; j++) {
            if (p->ubin[i][j] == p->ubin[i + 1][j]) {
                return 1;
             }
         }
    }
    return 0;
}

// Fungsi untuk mengecek apakah permainan sudah selesai
int selesai(struct permainan* p) {
    if (ada_ubin_kosong(p) || ada_ubin_gabung(p)) {
        return 0;
    } else {
        return 1;
    }
}

// Fungsi untuk mengatur input tanpa echo dan buffering
void input_tanpa_echo(int echo) {
    static struct termios oldt, newt;
    if (echo) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    } else {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
}

// Fungsi untuk membaca input dari pengguna
char baca_input() {
    char c;
    input_tanpa_echo(0);
    c = getchar();
    input_tanpa_echo(1);
    return c;
}

// Fungsi untuk memulai permainan baru
void mulai_baru(struct permainan* p) {
    for (int i = 0; i < UKURAN; i++) {
        for (int j = 0; j < UKURAN; j++) {
            p->ubin[i][j] = 0;
        }
    }
    
    p->skor = 0;

    int x, y;
    x = posisi_acak(p);
    y = posisi_acak(p);
    p->ubin[x][y] = (rand() % 10) ? 2 : 4;
    x = posisi_acak(p);
    y = posisi_acak(p);
    p->ubin[x][y] = (rand() % 10) ? 2 : 4;
}

// Fungsi untuk menampilkan menu utama
void tampil_menu() {
    system("clear");
    printf("===== MENU UTAMA =====\n");
    printf("Selamat datang di permainan 2048!\n");
    printf("Pilih salah satu dari pilihan berikut:\n");
    printf("1. Mulai permainan baru\n");
    printf("2. Tampilkan skor tertinggi\n");
    printf("3. Keluar dari program\n");
}   

// Fungsi untuk menampilkan skor tertinggi
void tampil_skor() {
    FILE* fp = fopen("scores.txt", "r");
    if (fp == NULL) {
        printf("Tidak dapat membuka file scores.txt\n");
        return;
    }
    
    struct ScoreEntry scores[10];
    int n = 0;

    while (fscanf(fp, "%s %d", scores[n].username, &scores[n].score) == 2) {
        n++;
    }

    fclose(fp);

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (scores[i].score < scores[j].score) {
                struct ScoreEntry temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }   
        }   
    }

    system("clear");
    printf("Skor tertinggi:\n");
    printf("| No | Nama Pengguna | Skor |\n");
    printf("|----|---------------|------|\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %13s | %4d |\n", i + 1, scores[i].username, scores[i].score);
    }
}

// Fungsi untuk menyimpan skor ke file scores.txt
void simpan_skor(struct permainan* p) {
    input_tanpa_echo(1);
    FILE* fp = fopen("scores.txt", "a");
    if (fp == NULL) {
        printf("Tidak dapat membuka file scores.txt\n");
        return;
    }

    char username[20];
    printf("Masukkan nama pengguna (maksimal 20 karakter): ");
    scanf("%s", username);
    fprintf(fp, "%s %d\n", username, p->skor);
    fclose(fp);
    input_tanpa_echo(0);
}

// Fungsi untuk memeriksa apakah input pengguna valid atau tidak
int valid(char c) {
  if (c == UP || c == LEFT || c == DOWN || c == RIGHT || c == QUIT) {
    return 1;
  } else {
    return 0;
  }
}

// Fungsi untuk memeriksa apakah input pengguna berdampak atau tidak
int berdampak(struct permainan* p, char c) {
  if (valid(c)) {
    int temp[UKURAN][UKURAN];

    for (int i = 0; i < UKURAN; i++) {
      for (int j = 0; j < UKURAN; j++) {
        temp[i][j] = p->ubin[i][j];
      }
    }

    gerak(p);
    // Membandingkan papan permainan setelah dan sebelum input pengguna
    for (int i = 0; i < UKURAN; i++) {
      for (int j = 0; j < UKURAN; j++) {
        if (p->ubin[i][j] != temp[i][j]) {
        // Mengembalikan papan permainan ke keadaan sebelum input pengguna
          for (int i = 0; i < UKURAN; i++) {
            for (int j = 0; j < UKURAN; j++) {
              p->ubin[i][j] = temp[i][j];
            }
          }
          return 1;
        }
      }
    }
    return 0;
  } else {
    return 0;
  }
}

int main() {
  srand(time(NULL));
  struct permainan p;
  char pilihan;
  do{
    tampil_menu();
    pilihan = baca_input();
    switch (pilihan) {
      case '1':
        mulai_baru(&p);
        tampil_papan(&p);
        while (!selesai(&p)) {
          p.arah = baca_input();
          if (valid(p.arah)) {
            if (berdampak(&p, p.arah)) {
              gerak(&p);
              if (ada_ubin_kosong(&p)) {
                int x, y;
                do {
                  x = posisi_acak(&p);
                  y = posisi_acak(&p);
                } while (p.ubin[x][y] != 0);
                p.ubin[x][y] = (rand() % 10) ? 2 : 4;
              }
              tampil_papan(&p);
            }
          } else {
            printf("Input tidak valid. Silakan coba lagi.\n");
          }
        }
        printf("Permainan selesai!\n");
        simpan_skor(&p);
        break;
      case '2':
        tampil_skor();
        printf("\nTekan enter untuk kembali ke menu utama.\n");
        getchar();
        break;
      case '3':
        printf("Terima kasih telah bermain!\n");
        return 0;
      default:
        printf("Pilihan tidak valid. Silakan coba lagi.\n");
    }
  } while (pilihan != 3);
  return 0;
}
