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

        delta_ones = list()
        delta_twos = list()
        delta_threes = list()
        first = True
        for line in in_file.readlines():
            if first:
                delta_1, delta_2, delta_3 = line.split(',')[2:5]
                first = False
            else:
                delta_1, delta_2, delta_3 = line.split(']],')[1].split(
                    ',')[1:4]
            delta_1 = delta_1[1:]  # Remove [
            delta_3 = delta_3[:-2]  # Remove ]
            delta_ones.append(delta_1)
            delta_twos.append(delta_2)
            delta_threes.append(delta_3)

        file_base = ntpath.basename(file).split('.')[0]

        delta_ones_file = open(f'{args.out_folder}/delta_ones_{file_base}.csv',
                               'w')

        delta_ones_file.writelines(','.join(delta_ones))

        delta_twos_file = open(f'{args.out_folder}/delta_twos_{file_base}.csv',
                               'w')

        delta_twos_file.writelines(','.join(delta_twos))

        delta_threes_file = open(
            f'{args.out_folder}/delta_threes_{file_base}.csv', 'w')
        delta_threes_file.writelines(','.join(delta_threes))


if __name__ == "__main__":
    main()
