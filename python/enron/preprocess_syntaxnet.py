""" Processes syntaxnet CoNLL format parses into (determiner, noun category, count) triplets. """

import argparse
import datetime
import glob
import gzip

import pandas as pd


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description=
        'Processes parse tree data into (determiner, noun category, count) triplets.'
    )
    parser.add_argument(
        '--ncfile',
        default='./data/CELEX/nouns.txt',
        help='File containing noun countability info.')
    parser.add_argument(
        '-i',
        '--input',
        default='./data/',
        help='Path to the CoLNN formatted input file.')
    parser.add_argument(
        '-o',
        '--output',
        default='./generated_data/det_cat_counts_syntaxnet.csv',
        help='Path to output file.')

    return parser.parse_args()


def clean(string):
    """ Cleans the string by making the case uniform and removing spaces. """
    if string:
        return string.strip().lower()
    return ''


def get_plurality(noun: str, fpos: str, noun_classes):
    if fpos in ['NN', 'NNP']:  # Singular or Mass
        if noun_classes[noun_classes.noun == clean(noun)].iloc[0].countable:
            plurality = 'SINGULAR_OR_MASS'
        else:
            plurality = 'MASS'
    elif fpos in ['NNS', 'NNPS']:  # Plural
        plurality = 'PLURAL'
    return plurality


def extract_triplets(noun_classes, in_path):
    """ Extracts triplets from a noun class dataframe and a xml tree parse context. """
    nouns = set(noun_classes.noun)
    l2_noun_det_pairs = dict()

    n_sentence = 0
    in_file = open(in_path, 'r')

    max_token = 2

    for i, line in enumerate(in_file):
        if line.strip() != '':
            token_id, form, lemma, cpos, fpos, features, head, dep, phead, pdep = line.strip(
            ).split('\t')

            if max_token > int(token_id):  # If new sentence
                n_sentence += 1
                unfinished_pair = dict()
                if n_sentence % 100000 == 0:
                    print(
                        f"[{datetime.datetime.now()}] Sentence: {n_sentence}, pairs: {len(l2_noun_det_pairs.keys())}"
                    )

            max_token = int(token_id)

            if cpos == 'DET' and fpos.strip() != 'EX':
                unfinished_pair[head] = clean(form)

            if token_id in unfinished_pair and form in nouns:
                det = unfinished_pair[token_id]
                if fpos in ['NN', 'NNP', 'NNS', 'NNPS']:
                    pair = (det, get_plurality(form, fpos, noun_classes))
                    l2_noun_det_pairs[pair] = l2_noun_det_pairs.get(pair,
                                                                    0) + 1
                del unfinished_pair[token_id]

    return l2_noun_det_pairs


def main():
    """ Reads the data files and extracts the (determiner, noun class, count) triplets. """
    args = get_flags()

    noun_classes_per_sense = pd.read_csv(
        args.ncfile, sep='\\', names=['noun', 'countable', 'uncountable'])
    noun_classes_per_sense['noun'] = [
        clean(noun) for noun in noun_classes_per_sense.noun
    ]
    noun_classes = noun_classes_per_sense.groupby(
        'noun',
        as_index=False).aggregate(lambda group: any([x == 'Y' for x in group]))

    l2_noun_det_pairs = extract_triplets(noun_classes, args.input)

    out_file = open(args.output, 'w')
    for (key, count) in l2_noun_det_pairs.items():
        det, noun = key
        out_file.write(det + "," + noun + "," + str(count) + "\n")
    out_file.close()


if __name__ == "__main__":
    main()
