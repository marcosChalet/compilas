int i
int j
int k

int matA[3:3]
int matB[3:3]
int matR[3:3]

for ( i = 0 ; i < 3; i = i + 1) {
    for ( j = 0 ; j < 3; j = j + 1) {
        matR[i:j] = 0
    }
}

for ( i = 0 ; i < 3; i = i + 1) {
    for ( j = 0 ; j < 3; j = j + 1) {
        for ( k = 0 ; k < 3; k = k + 1) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
        }
    }
}
