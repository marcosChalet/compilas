🔢 i
🔢 j
🔢 k

🔢 matA[4:4]
🔢 matB[4:4]
🔢 matR[4:4]

🧬 ( i = 0 ; i < 4; i = i + 1) {
    🧬 ( j = 0 ; j < 4; j = j + 1) {
        🧬 ( k = 0 ; k < 4; k = k + 1) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
        }
    }
}
