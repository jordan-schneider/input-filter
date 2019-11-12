""" Processes English L2 parse tree data into (determiner, noun category, count) triplets. """
import argparse
import datetime
import gc
import xml.etree.ElementTree as ElementTree

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
        '--efcamdatfile',
        default='./data/EFCAMDAT2_parse.xml',
        help='Location of EFCAMDAT2 xml file.')
    parser.add_argument(
        '-o',
        '--output',
        default='./generated_data/det_cat_counts_efcamdat.csv',
        help='Path to output file.')

    return parser.parse_args()


def clean(string):
    """ Cleans the string by making the case uniform and removing spaces. """
    if string:
        return string.strip().lower()
    return ''


def extract_triplets(noun_classes, context):
    """ Extracts triplets from a noun class dataframe and a xml tree parse context. """
    nouns = set(noun_classes.noun)
    l2_noun_det_pairs = dict()

    n_sentence = 0
    for event, elem in context:
        # <sentence> is the start of annotated text
        if event == 'end' and elem.tag == 'sentence':
            n_sentence += 1
            # Each of the children is a token with tagged attributes
            for token in elem:
                # If we are currently looking at a determiner
                if token.attrib['pos'] in ['DT', 'CD', 'WDT']:
                    head = elem[int(token.attrib['head']) - 1]
                    # If the head element is actually a noun and we have record of it
                    if clean(head.text) in nouns:
                        if head.attrib['pos'] in ['NN',
                                                  'NNP']:  # Singular or Mass
                            if noun_classes[noun_classes.noun == clean(
                                    head.text)].iloc[0].countable:
                                pair = (clean(token.text), 'SINGULAR_OR_MASS')
                            else:
                                pair = (clean(token.text), 'MASS')
                        elif head.attrib['pos'] in ['NNS', 'NNPS']:  # Plural
                            pair = (clean(token.text), 'PLURAL')
                        l2_noun_det_pairs[pair] = l2_noun_det_pairs.get(
                            pair, 0) + 1
            if n_sentence % 100000 == 0:
                print(
                    f"[{datetime.datetime.now()}] Sentence: {n_sentence}, pairs: {len(l2_noun_det_pairs.keys())}"
                )
        if elem.tag != 'token':
            elem.clear()
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

    # get an iterable
    context = ElementTree.iterparse(args.efcamdatfile)

    l2_noun_det_pairs = extract_triplets(noun_classes, context)

    out_file = open(args.output, 'w')
    for (key, count) in l2_noun_det_pairs.items():
        det, noun = key
        out_file.write(det + "," + noun + "," + str(count) + "\n")
    out_file.close()


if __name__ == "__main__":
    main()
