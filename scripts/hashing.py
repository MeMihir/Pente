def ternary_array_hash(arr):
    p = 5  # choose a prime number as the base
    hash_value = 0
    n = len(arr)
    for i in range(n):
        hash_value = hash_value * p + (i + 1) * arr[i]
    return hash_value


if __name__ == "__main__":
    arr = list(map(int, input().split()))
    print(ternary_array_hash(arr))