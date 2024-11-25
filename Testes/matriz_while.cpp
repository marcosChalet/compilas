int i
int j
int k

int matA[4:4]
int matB[4:4]
int matR[4:4]

i = 0
while (i < 4) {
    j = 0
    while (j < 4) {
        k = 0
        while (k < 4) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}