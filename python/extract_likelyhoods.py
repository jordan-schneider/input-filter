""" Utility script to exact the likelyhood values from a run. """

import glob
import ntpath
import argparse


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description=
        'Processes parse tree data into (determiner, noun category, count) triplets.'
    )
    parser.add_argument(
        '-i',
        '--in-folder',
        default='./output/efcamdat/',
        help='Folder containing run outputs.')
    parser.add_argument(
        '-o',
        '--out-folder',
        default='./output/efcamdat/generated/',
        help='Path to folder to put output.')

    return parser.parse_args()


def main():
    args = get_flags()
    for file in glob.glob(f'{args.in_folder}/*.out'):
        in_file = open(file, 'r')
        in_file.readline()

        likelyhood_strings = list()
        for line in in_file.readlines():
            likelyhood = line.split(',')[-1]
            likelyhood = likelyhood[:-1]
            likelyhood_strings.append(likelyhood)

        file_base = ntpath.basename(file).split('.')[0]

        out_file = open(f'{args.out_folder}/likelyhoods_{file_base}.csv', 'w')
        out_file.writelines(','.join(likelyhood_strings))


if __name__ == "__main__":
    main()
