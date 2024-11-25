int i int j int k float vet[10] bool flag
int mat[10:20]

i = 0
j = 9
k = 1
flag = false
mat[0:0] = 21

if (mat[0:0] > 10) {
    mat[0:0] = 5
}

do
{
    while (i < k)
        i = i + 1

    // testa operadores logicos
    while (j > k && j <= 5 || true && !flag)
        j = j - 1

    if (vet[i] < vet[j])
    {
        vet[i] = vet[i - 1] * 2.0
        vet[j] = vet[j + 1] / 2.0
    }
} 
while (i < j)