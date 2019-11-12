""" Downsamples the total observed counts to some range. """

from typing import List
import argparse
import os
import math


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description='Downsampled the cleaned counts to a uniform total.')
    parser.add_argument(
        '-i',
        '--in_file',
        default='./data/efcamdat/counts/det_cat_counts_efcamdat_clean.csv',
        help='File containing cleaned counts.')
    parser.add_argument(
        '-b', '--base', default='1.1', help='Base of the scaling logarithm.')

    return parser.parse_args()


def main():
    args = get_flags()
    base = float(args.base)
    # Compute total count over all determiners
    log_total: int = 0
    with open(args.in_file, 'r') as infile:
        # Ignore the header
        infile.readline()
        for line in infile.readlines():
            det, mass_s, plural_s, singular_or_mass_s, total_s = line.split(
                ',')
            log_total += math.log(int(total_s), base)
        infile.close()

    target: int
    for target in [
            10**2, 5 * 10**2, 10**3, 5 * 10**3, 10**4, 5 * 10**4, 10**5,
            5 * 10**5, 10**6
    ]:
        outfile_name = f'{os.path.dirname(args.in_file)}/log_downsampled_to_{target}.csv'
        lines: List[str] = list()
        with open(args.in_file, 'r') as infile, \
                open(outfile_name, 'w') as outfile:
            outfile.writelines(infile.readline())
            for line in infile.readlines():
                det, mass_s, plural_s, singular_or_mass_s, total_s = line.split(
                    ',')

                row_total = int(total_s)
                row_target = math.log(row_total, base) / log_total * target

                if row_target > row_total:
                    break

                mass: int = int(int(mass_s) * row_target / row_total)
                plural: int = int(int(plural_s) * row_target / row_total)
                singular_or_mass: int = int(
                    int(singular_or_mass_s) * row_target / row_total)
                if mass + plural + singular_or_mass > 0:
                    lines.append(','.join([
                        det,
                        str(mass),
                        str(plural),
                        str(singular_or_mass),
                        str(mass + plural + singular_or_mass)
                    ]) + "\n")
            outfile.writelines(lines)
            infile.close()
            outfile.close()


if __name__ == "__main__":
    main()
