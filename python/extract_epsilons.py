""" Utility script to exact the epsilon values from a run. """

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

        epsilon_strings = list()
        first = True
        for line in in_file.readlines():
            if first:
                epsilon_string = line.split(',')[1]
                first = False
            else:
                epsilon_string = line.split(']],')[1].split(',')[0]
            epsilon_strings.append(epsilon_string)

        file_base = ntpath.basename(file).split('.')[0]

        out_file = open(f'{args.out_folder}/epsilons_{file_base}.csv', 'w')
        out_file.writelines(','.join(epsilon_strings))


if __name__ == "__main__":
    main()
