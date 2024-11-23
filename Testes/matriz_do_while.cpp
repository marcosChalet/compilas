int i
int j
int k

int matA[3:3]
int matB[3:3]
int matR[3:3]

i = 0
do {
    j = 0    
    do  {
        k = 0
        matR[i:j] = 0
        do {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        } while (k < 3)
        j = j + 1
    } while (j < 3)
    i = i + 1
} while (i < 3)
