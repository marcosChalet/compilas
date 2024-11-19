int i
int j
int k

int matA[3:3]
int matB[3:3]
int matR[3:3]

i = 0
j = 0
k = 0

while (i < 3) {
    while (j < 3) {
        matR[i:j] = 0
        j = j + 1
    }
    i = i + 1
}

i = 0
while (i < 3) {
    j = 0
    while (j < 3) {
        k = 0
        while (k < 3) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}
