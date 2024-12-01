🔢 refResult[4:4]
🔢 matA[4:4]
🔢 matB[4:4]
🔢 matR[4:4]

👻 multiplicaMatriz(🔢 mat[4:4], 🔢 matAA[4:4], 🔢 matBB[4:4]) : 🔢{
    🔢 i
    🔢 j
    🔢 k

    🧬 ( i = 0 ; i < 4; i = i + 1) {
        🧬 ( j = 0 ; j < 4; j = j + 1) {
            🧬 ( k = 0 ; k < 4; k = k + 1) {
                mat[i:j] = mat[i:j] + matA[i:k] * matB[k:j]
            }
        }
    }

    🦋 mat
}

refResult = multiplicaMatriz(matR, matA, matB)
