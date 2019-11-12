""" Processes Wikipedia CoNLL-X parses into (determiner, noun category, count) triplets. """

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
        '--wikipedia-loc',
        default='./data/Wikipedia',
        help='Location of wikipedia data folder.')
    parser.add_argument(
        '-o',
        '--output',
        default='./generated_data/det_cat_counts_wiki.csv',
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


def extract_triplets(noun_classes, wikipedia_loc):
    """ Extracts triplets from a noun class dataframe and a xml tree parse context. """
    nouns = set(noun_classes.noun)
    l2_noun_det_pairs = dict()

    n_sentence = 0
    for filename in glob.glob(f'{wikipedia_loc}/parsed/*.gz'):
        #print(filename)
        wiki_file = gzip.open(filename, 'rt')
        wiki_file.readline()
        line = 'start'

        max_token = 2

        line = wiki_file.readline()
        while line.strip() != '</doc>':
            if line.strip() != '':
                #print(line)
                token_id, form, lemma, cpos, fpos, features, head, dep, phead, pdep = line.strip(
                ).split('\t')

                if max_token > int(token_id):  # If new sentence
                    n_sentence += 1
                    unfinished_pair = dict()

                max_token = int(token_id)

                if cpos is 'D' and fpos.strip() != 'EX':
                    # print(f'det: {form}, fpos: {fpos}')
                    unfinished_pair[head] = clean(form)

                if token_id in unfinished_pair and form in nouns:
                    det = unfinished_pair[token_id]
                    # print(
                    #    f'token_id: {token_id}, det: {det}, noun: {form}, fpos: {fpos}'
                    #)
                    if fpos in ['NN', 'NNP', 'NNS', 'NNPS']:
                        pair = (det, get_plurality(form, fpos, noun_classes))
                        l2_noun_det_pairs[pair] = l2_noun_det_pairs.get(
                            pair, 0) + 1

                        if det == 'a' and get_plurality(
                                form, fpos, noun_classes) == 'MASS':
                            print(f'det: a, noun: {form}')
                            print(line)
                            print(filename)

                    del unfinished_pair[token_id]

                if n_sentence % 100000 == 0:
                    print(
                        f"[{datetime.datetime.now()}] Sentence: {n_sentence}, pairs: {len(l2_noun_det_pairs.keys())}"
                    )

            line = wiki_file.readline()

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

    l2_noun_det_pairs = extract_triplets(noun_classes, args.wikipedia_loc)

    out_file = open(args.output, 'w')
    for (key, count) in l2_noun_det_pairs.items():
        det, noun = key
        out_file.write(det + "," + noun + "," + str(count) + "\n")
    out_file.close()


if __name__ == "__main__":
    main()
