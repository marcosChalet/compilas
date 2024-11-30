🔢 i
🔢 j
🔢 k

🔢 matA[4:4]
🔢 matB[4:4]
🔢 matR[4:4]

i = 0
🔁 (i < 4) {
    j = 0
    🔁 (j < 4) {
        k = 0
        🔁 (k < 4) {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}