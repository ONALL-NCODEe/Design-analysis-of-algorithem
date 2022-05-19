#include <stdio.h>
#include <malloc.h>

#define MAX_N 20

typedef struct {
    char tenDV[MAX_N];
    float TL, GT, DG;
    int SL, PA;
} DoVat;

DoVat *readFrom_File(char *filepath, float *W, int *n) {
    FILE *f = fopen(filepath, "r");

    fscanf(f, "%f", W); // Xac dinh trong luong cua do vat
    DoVat *dsdv = (DoVat*)malloc(sizeof(DoVat));

    int i = 0;
    while (!feof(f))
    {
        fscanf(f, "%f%f%[^\n]", &dsdv[i].TL, &dsdv[i].GT, &dsdv[i].tenDV);
        dsdv[i].DG = dsdv[i].GT / dsdv[i].TL;
        dsdv[i].PA = 0;
        i++;
        dsdv = (DoVat*)realloc(dsdv, sizeof(DoVat) * (i+1));
    }
    *n = i;
    fclose(f);

    return dsdv;
}

void Swap(DoVat *x, DoVat *y) {
    DoVat Temp = *x;
    *x = *y;
    *y = Temp;
}

void BubbleSort(DoVat *dsdv, int n) {
    int i, j;
    for (i = 0;  i <= n-2; i++)
    {
        for (j = n-1; j >= i+1; j--)
            if (dsdv[j].DG > dsdv[j-1].DG)
                Swap(&dsdv[j], &dsdv[j-1]);
    }
}

void in_DSDV(DoVat *dsdv, int n, float W) {
    int i;
    float TTL = 0.0, TGT = 0.0;

    printf("Trong luong CBL = %-9.2f\n", W);
    printf("+---+--------------------+---------+---------+---------+-----------+\n");
	printf("|STT|     Ten Do Vat     |T. Luong | Gia Tri | Don gia | Phuong an |\n");
    printf("|---|--------------------|---------|---------|---------|-----------|\n");
    for (i = 0; i < n; i++)
    {
        printf("|%2d |%-20s|%9.2f|%9.2f|%9.2f|%6d     |\n", 
                i+1, dsdv[i].tenDV, dsdv[i].TL, dsdv[i].GT, dsdv[i].DG, dsdv[i].PA);
        TTL = TTL + dsdv[i].PA * dsdv[i].TL;
        TGT = TGT + dsdv[i].PA * dsdv[i].GT;
    }
    printf("+---+--------------------+---------+---------+---------+-----------+\n");

    printf("Phuong an theo thu tu DG giam: X(");
    for (i = 0; i < n; i++)
    {
        i < n-1
        ? printf("%d,", dsdv[i].PA)
        : printf("%d)", dsdv[i].PA);
    }
    printf("\nTTL = %-9.2f", TTL);
    printf("\nTGT = %-9.2f\n", TGT);
}

// Tinh cac dai luong tai nut goc
void root_node(float W, float *V, float *CT, float *GLNTT, float *TGT, float DG_MAX) {
    *TGT = 0;
    *V = W;
    *CT = *V * DG_MAX;
    *GLNTT = 0.0;
}

void cap_nhat_GLNTT(float TGT, float *GLNTT, int x[], DoVat *dsdv, int n) {
    int i;
    if (*GLNTT < TGT)
    {
        *GLNTT = TGT;
        for (i = 0; i < n; i++)
            dsdv[i].PA = x[i];
    }
}

int min(int a, int b) {
    return a < b ? a : b;
}

void Branch_and_Bound(int i, float *TGT, float *CT, float *V, float *GLNTT, int x[], DoVat *dsdv, int n) {
    int j, yk; // j: so do vat duoc chon, yk: do vat lon nhat co the chon
    yk = *V / dsdv[i].TL;

    for (j = yk; j >= 0; j--) 
    {  
        /**
         * Xet tat ca ca kha nang co the phan nahnh theo so luong do vat
         * Ung voi mot gia tri cua j la mot nut tren cay
         * Tinh 3 dai luong cua mot nut trong
         */ 
        *TGT = *TGT + j * dsdv[i].GT;
        *V = *V - j * dsdv[i].TL;
        *CT = *TGT + *V * dsdv[i+1].DG; 

        if (*CT > *GLNTT) // TH chua cat tia (Dk: CT <= GLNTT)
        {
            x[i] = j;
            i == n-1 || *V == 0 // Da xet het tat ca cac do vat hoac balo da day
            ? cap_nhat_GLNTT(*TGT, GLNTT, x, dsdv, n)
            : Branch_and_Bound(i+1, TGT, CT, V, GLNTT, x, dsdv, n);
        }
        // Quay lui xet nut khac
        x[i] = 0;
        *TGT = *TGT - j * dsdv[i].GT; // Tra lai TGT nut cha
        *V = *V + j * dsdv[i].TL;     // Tra lai TTL nut cha
    }
}

int main() {
	int n; 			// luu so luong do vat
	float W, CT, TGT, V, GLNTT; 
    /**
     * W Trong luong cua balo
     * CT Can Tren
     * TGT Luu Tong gia tri cua cac vat da duoc chon tai moi nut
     * V Luu Trong luong con lai cua Ba lo tai moi nut
     * GLNTT Luu Gia lon nhat tam thoi 
    */
    DoVat *dsdv = readFrom_File((char*)"./CaiBalo1.txt", &W, &n);

    int x[n]; // Luu phuong an tot nhat tam thoi

    printf("\nPhuong an Cai Ba lo 1 dung thuat toan Branch and Bound: \n");
    BubbleSort(dsdv, n);
    root_node(W, &V, &CT, &GLNTT, &TGT, dsdv[0].DG);
    Branch_and_Bound(0, &TGT, &CT, &V, &GLNTT, x, dsdv, n);
    in_DSDV(dsdv, n, W);

    free(dsdv);
    return 0;
}