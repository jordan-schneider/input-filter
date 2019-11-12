#!/bin/bash
if [ ! -f data/counts/det_cat_counts.csv ];
then
    INVALID=true
    python3 python/preprocess.py
fi

if [ ! -f data/counts/det_cat_counts_clean.csv ] || [ "$INVALID" = true ];
then
    python3 python/clean_preprocess.py

    echo "Check over data/counts/det_cat_counts_clean.csv for bad determiners e.g. xxx, m."
    read -n1 -r -p "Press space to continue..." key
fi

cut -d',' -f1 data/counts/det_cat_counts_clean.csv | tail -n +2 > data/dets.txt

python3 python/downsample.py
python3 python/downsample_each.py