// Program C untuk permainan 2048
// Menggunakan library termios untuk input tanpa echo dan buffering
// Menggunakan ANSI escape codes untuk warna dan posisi kursor

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

// Konstanta untuk ukuran papan permainan
#define UKURAN_PAPAN 4

// Konstanta untuk warna ubin
#define RESET "\033[0m"
#define WARNA_MERAH "\033[38;5;1m"
#define WARNA_HIJAU "\033[38;5;2m"
#define WARNA_KUNING "\033[38;5;3m"
#define WARNA_BIRU "\033[38;5;4m"
#define WARNA_MAGENTA "\033[38;5;5m"
#define WARNA_SIAN "\033[38;5;6m"
#define WARNA_PUTIH "\033[38;5;7m"

// Konstanta untuk tombol input
#define ATAS 'i'
#define KIRI 'j'
#define BAWAH 'k'
#define KANAN 'l'
#define KELUAR 'q'

// Struct untuk menyimpan data permainan
struct DataPermainan {
    int ubin[UKURAN_PAPAN][UKURAN_PAPAN];
    int skor;
    char arah;
};

// Struct untuk menyimpan data skor
struct EntriSkor {
    char namaPengguna[20];
    int skor;
};

// Fungsi untuk mengacak posisi ubin kosong pada papan permainan
int AcakPosisi(struct DataPermainan* permainan){
    return rand() % UKURAN_PAPAN;
}

// Fungsi untuk mengembalikan warna sesuai dengan nilai ubin
char* DapatkanWarna(int nilai) {
    switch (nilai) {
        case 2: return WARNA_MERAH;
        case 4: return WARNA_HIJAU;
        case 8: return WARNA_KUNING;
        case 16: return WARNA_BIRU;
        case 32: return WARNA_MAGENTA;
        case 64: return WARNA_SIAN;
        case 128: return WARNA_PUTIH;
        case 256: return WARNA_MERAH;
        case 512: return WARNA_HIJAU;
        case 1024: return WARNA_KUNING;
        case 2048: return WARNA_BIRU;
        default: return RESET;
    }
}

// Fungsi untuk menampilkan papan permainan
void TampilkanPapan(struct DataPermainan* permainan) {
    system("clear");
    printf("Skor: %d\n", permainan->skor);
    printf("---------------------------\n");
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            if (permainan->ubin[i][j] == 0) {
                printf("|%*s| ", UKURAN_PAPAN, " ");
            } else {
                printf("%s|%*d| %s", DapatkanWarna(permainan->ubin[i][j]), UKURAN_PAPAN, permainan->ubin[i][j], RESET);
            }
        }
        printf("\n");
        printf("---------------------------\n");
    }
    printf("Tekan i, j, k, l untuk menggerakkan ubin ke atas, kiri, bawah, kanan.\n");
    printf("Tekan q untuk keluar dari permainan.\n");
}

// Fungsi untuk memutar papan permainan berlawanan jarum jam
void PutarPapan(struct DataPermainan* permainan) {
    int sementara[UKURAN_PAPAN][UKURAN_PAPAN];
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            sementara[i][j] = permainan->ubin[j][UKURAN_PAPAN - i - 1];
        }
    }
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            permainan->ubin[i][j] = sementara[i][j];
        }
    }
}

// Fungsi untuk membalik papan permainan secara horizontal
void BalikPapan(struct DataPermainan* permainan) {
    int sementara[UKURAN_PAPAN][UKURAN_PAPAN];
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            sementara[i][j] = permainan->ubin[i][UKURAN_PAPAN - j - 1];
        }
    }
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            permainan->ubin[i][j] = sementara[i][j];
        }
    }
}

// Fungsi untuk menggeser ubin ke kiri
void GeserKiri(struct DataPermainan* permainan) {
    int x, y;
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        x = 0;
        y = 1;
        while (y < UKURAN_PAPAN) {
            if (permainan->ubin[i][y] != 0) {
                if (permainan->ubin[i][x] == 0) {
                    permainan->ubin[i][x] = permainan->ubin[i][y];
                    permainan->ubin[i][y] = 0;
                } else if (permainan->ubin[i][x] == permainan->ubin[i][y]) {
                    permainan->ubin[i][x] *= 2;
                    permainan->skor += permainan->ubin[i][x] / 2;
                    permainan->ubin[i][y] = 0;
                    x++;
                } else {
                    x++;
                    if (x != y) {
                        permainan->ubin[i][x] = permainan->ubin[i][y];
                        permainan->ubin[i][y] = 0;
                    }
                }
            }
            y++;
        }
    }
}

// Fungsi untuk menggerakkan ubin sesuai dengan arah input
void GerakkanUbin(struct DataPermainan* permainan) {
    switch (permainan->arah) {
        case ATAS:
            PutarPapan(permainan);
            GeserKiri(permainan);
            PutarPapan(permainan);
            PutarPapan(permainan);
            PutarPapan(permainan);
            break;
        case KIRI:
            GeserKiri(permainan);
            break;
        case BAWAH:
            PutarPapan(permainan);
            PutarPapan(permainan);
            PutarPapan(permainan);
            GeserKiri(permainan);
            PutarPapan(permainan);
            break;
        case KANAN:
            BalikPapan(permainan);
            GeserKiri(permainan);
            BalikPapan(permainan);
            break;
    }
}

// Fungsi untuk mengecek apakah ada ubin kosong pada papan permainan
int CekUbinKosong(struct DataPermainan* permainan) {
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            if (permainan->ubin[i][j] == 0) {
                return 1;
            }
        }
    }
    return 0;
}

// Fungsi untuk mengecek apakah ada ubin yang bisa digabungkan pada papan permainan
int CekUbinGabungan(struct DataPermainan* permainan) {
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN - 1; j++) {
            if (permainan->ubin[i][j] == permainan->ubin[i][j + 1]) {
                return 1;
            }
        }
    }
    for (int i = 0; i < UKURAN_PAPAN - 1; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            if (permainan->ubin[i][j] == permainan->ubin[i + 1][j]) {
                return 1;
            }
        }
    }
    return 0;
}

// Fungsi untuk mengecek apakah permainan sudah selesai
int selesai(struct DataPermainan* permainan, char c) {
    if (c == 'Q' || c == 'q') {
        return 1;
    } else if (CekUbinKosong(permainan) || CekUbinGabungan(permainan)) {
        return 0;
    } else {
        return 1;
    }
}


// Fungsi untuk mengatur input tanpa echo dan buffering
void InputTanpaEcho(int echo) {
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
char BacaInput() {
    char c;
    InputTanpaEcho(0);
    c = getchar();
    InputTanpaEcho(1);
    return c;
}

// Fungsi untuk memulai permainan baru
void InisialisasiPermainan(struct DataPermainan* permainan) {
    for (int i = 0; i < UKURAN_PAPAN; i++) {
        for (int j = 0; j < UKURAN_PAPAN; j++) {
            permainan->ubin[i][j] = 0;
        }
    }
    
    permainan->skor = 0;

    int x, y;
    x = AcakPosisi(permainan);
    y = AcakPosisi(permainan);
    permainan->ubin[x][y] = (rand() % 10) ? 2 : 4;
    x = AcakPosisi(permainan);
    y = AcakPosisi(permainan);
    permainan->ubin[x][y] = (rand() % 10) ? 2 : 4;
}

// Fungsi untuk menampilkan menu utama
void tampilkanMenu() {
    system("clear");
    printf("===== MENU UTAMA =====\n");
    printf("Selamat datang di permainan 2048!\n");
    printf("Pilih salah satu dari pilihan berikut:\n");
    printf("1. Mulai permainan baru\n");
    printf("2. Tampilkan skor tertinggi\n");
    printf("3. Keluar dari program\n");
}   

// Fungsi untuk menampilkan skor tertinggi
void TampilkanSkorTertinggi() {
    FILE* fp = fopen("Top_Skor.txt", "r");
    if (fp == NULL) {
        printf("Tidak dapat membuka file scores.txt\n");
        return;
    }
    
    struct EntriSkor TopSkor[10];
    int n = 0;

    while (fscanf(fp, "%s %d", TopSkor[n].namaPengguna, &TopSkor[n].skor) == 2) {
        n++;
    }

    fclose(fp);

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (TopSkor[i].skor < TopSkor[j].skor) {
                struct EntriSkor temp = TopSkor[i];
                TopSkor[i] = TopSkor[j];
                TopSkor[j] = temp;
            }   
        }   
    }

    system("clear");
    printf("Skor tertinggi:\n");
    printf("| No | Nama Pengguna | Skor |\n");
    printf("|----|---------------|------|\n");
    for (int i = 0; i < n; i++) {
        printf("| %2d | %13s | %4d |\n", i + 1, TopSkor[i].namaPengguna, TopSkor[i].skor);
    }
}

// Fungsi untuk menyimpan skor ke file scores.txt
void SimpanSkor(struct DataPermainan* permainan) {
    InputTanpaEcho(1);
    FILE* fp = fopen("Top_Skor.txt", "a");
    if (fp == NULL) {
        printf("Tidak dapat membuka file scores.txt\n");
        return;
    }

    char namaPengguna[20];
    printf("Masukkan nama pengguna (maksimal 20 karakter): ");
    scanf("%s", namaPengguna);
    fprintf(fp, "%s %d\n", namaPengguna, permainan->skor);
    fclose(fp);
    InputTanpaEcho(0);
}

// Fungsi untuk memeriksa apakah input pengguna valid atau tidak
int valid(char c) {
    if (c == ATAS || c == KIRI || c == BAWAH || c == KANAN || c == KELUAR) {
        return 1;
    } else {
        return 0;
    }
}

// Fungsi untuk memeriksa apakah input pengguna berdampak atau tidak
int berdampak(struct DataPermainan* permainan, char c) {
    if (valid(c)) {
        int temp[UKURAN_PAPAN][UKURAN_PAPAN];

        for (int i = 0; i < UKURAN_PAPAN; i++) {
            for (int j = 0; j < UKURAN_PAPAN; j++) {
                temp[i][j] = permainan->ubin[i][j];
            }
        }

        GerakkanUbin(permainan);
        // Membandingkan papan permainan setelah dan sebelum input pengguna
        for (int i = 0; i < UKURAN_PAPAN; i++) {
            for (int j = 0; j < UKURAN_PAPAN; j++) {
                if (permainan->ubin[i][j] != temp[i][j]) {
                // Mengembalikan papan permainan ke keadaan sebelum input pengguna
                    for (int i = 0; i < UKURAN_PAPAN; i++) {
                        for (int j = 0; j < UKURAN_PAPAN; j++) {
                            permainan->ubin[i][j] = temp[i][j];
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
    struct DataPermainan permainan;
    char pilihan;
    do{
        tampilkanMenu();
        pilihan = BacaInput();
        switch (pilihan) {
            case '1':
                InisialisasiPermainan(&permainan);
                TampilkanPapan(&permainan);
                while (!selesai(&permainan, permainan.arah)) {
                    permainan.arah = BacaInput();
                    if (valid(permainan.arah)) {
                        if (berdampak(&permainan, permainan.arah)) {
                            GerakkanUbin(&permainan);
                            if (CekUbinKosong(&permainan)) {
                                int x, y;
                                do {
                                    x = AcakPosisi(&permainan);
                                    y = AcakPosisi(&permainan);
                                } while (permainan.ubin[x][y] != 0);
                                permainan.ubin[x][y] = (rand() % 10) ? 2 : 4;
                            }
                            TampilkanPapan(&permainan);
                        }
                    } else {
                        printf("Input tidak valid. Silakan coba lagi.\n");
                    }
                }
                printf("Permainan selesai!\n");
                SimpanSkor(&permainan);
                break;
            case '2':
                TampilkanSkorTertinggi();
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
