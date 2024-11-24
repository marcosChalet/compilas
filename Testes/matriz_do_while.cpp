int i
int j
int k

int matA[4:4]
int matB[4:4]
int matR[4:4]

i = 0
do {
    j = 0    
    do  {
        k = 0
        matR[i:j] = 0
        do {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        } while (k < 4)
        j = j + 1
    } while (j < 4)
    i = i + 1
} while (i < 4)
