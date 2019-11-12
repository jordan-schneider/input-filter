#!/bin/bash
python3 python/extract_alphas.py -i $1 -o $2
python3 python/extract_deltas.py -i $1 -o $2
python3 python/extract_epsilons.py -i $1 -o $2
python3 python/extract_likelyhoods.py -i $1 -o $2