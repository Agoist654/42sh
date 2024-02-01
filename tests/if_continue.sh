for var in 1 2 3;do
    if break; then
        echo food
    fi
done


for big in 4 5 6; do
    for var in 1 2 3;do
        if continue; then
            echo food
        fi
    done
    echo big
done

for big in 4 5 6; do
    for var in 1 2 3;do
        if continue 2; then
            echo food
        fi
    done
    echo big
done
