int i
int j
int k

int matA[4:4]
int matB[4:4]
int matR[4:4]

for ( i = 0 ; i < 4; i = i + 1) {
    for ( j = 0 ; j < 4; j = j + 1) {
        for ( k = 0 ; k < 4; k = k + 1) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
        }
    }
}
