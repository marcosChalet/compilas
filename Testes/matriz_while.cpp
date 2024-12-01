🔢 refResult[4:4]
🔢 matA[4:4]
🔢 matB[4:4]
🔢 matR[4:4]

👻 multiplicaMatriz(🔢 mat[4:4], 🔢 matAA[4:4], 🔢 matBB[4:4]) : 🔢{
    🔢 i
    🔢 j
    🔢 k
    i = 0
    🔁 (i < 4) {
        j = 0
        🔁 (j < 4) {
            k = 0
            🔁 (k < 4) {
                mat[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
                k = k + 1
            }
            j = j + 1
        }
        i = i + 1
    }

    🦋 mat
}

refResult = multiplicaMatriz(matR, matA, matB)
