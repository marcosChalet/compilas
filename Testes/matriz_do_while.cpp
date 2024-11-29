🔢 i
🔢 j
🔢 k

🔢 matA[4:4]
🔢 matB[4:4]
🔢 matR[4:4]

i = 0
👇 {
    j = 0    
    👇  {
        k = 0
        matR[i:j] = 0
        👇 {
            matR[i:j] = matR[i:j] + matA[i:k] * matB[k:j]
            k = k + 1
        } 🔁 (k < 4)
        j = j + 1
    } 🔁 (j < 4)
    i = i + 1
} 🔁 (i < 4)
