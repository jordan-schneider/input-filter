""" Downsamples the total observed counts to some range. """

import argparse
import os


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description='Downsampled the cleaned counts to a uniform total.')
    parser.add_argument(
        '-i', '--in_files', help='Comma separated list of files to join.')

    return parser.parse_args()


def main():
    args = get_flags()
    files = args.in_files.split(',')
    dicts = []
    for file in files:
        with open(file, 'r') as infile:
            counts = {}
            lines = infile.readlines()
            for line in lines[1:]:
                det, n1, n2, n3, n4 = line.split(',')
                counts[det] = (n1, n2, n3, n4)
            dicts += [counts]

    common_dets = set(dicts[0].keys())
    for counts in dicts:
        common_dets = common_dets.intersection(counts.keys())

    for i, file in enumerate(files):
        counts = dicts[i]
        folder = os.path.dirname(file)
        basename = os.path.basename(file)
        filename = basename.split('.')[0]
        outfile_name = f'{folder}/{filename}_common.csv'
        with open(outfile_name, 'w') as outfile:
            outfile.write('det,MASS,PLURAL,SINGULAR_OR_MASS,TOTAL\n')
            for det in common_dets:
                n1, n2, n3, n4 = counts[det]
                outfile.write(f'{det},{n1},{n2},{n3},{n4}')


if __name__ == "__main__":
    main()
