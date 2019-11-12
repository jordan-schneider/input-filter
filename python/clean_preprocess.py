import argparse
from typing import Dict, List, Tuple


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description=
        'Processes (determiner, noun category, count) triplets into (determiner, mass_count, plural_count, singular_or_mass_count).'
    )
    parser.add_argument(
        '-i',
        '--in-file',
        default='./generated_data/det_cat_counts_efcamdat.csv',
        help='Path to input file')
    parser.add_argument(
        '-o',
        '--out-file',
        default='./data/counts/det_cat_counts_efcamdat_clean.csv',
        help='Path to input file')
    parser.add_argument(
        '--threshold',
        default=0,
        help='det-cat pairs with total count below this threshold are ignored.'
    )

    return parser.parse_args()


def main():
    args = get_flags()

    in_file = open(args.in_file)
    determiners: Dict[str, Tuple[int, int, int]] = dict()
    for line in in_file.readlines():
        det, cat, count = line.split(',')
        mass_count, plural_count, som_count = determiners.get(det, (0, 0, 0))
        if cat == "MASS":
            mass_count = int(count)
        elif cat == "PLURAL":
            plural_count = int(count)
        else:
            som_count = int(count)
        determiners[det] = (mass_count, plural_count, som_count)

    in_file.close()

    ordered: List[Tuple[str, Tuple[int, int, int]]] = list()
    for det, counts in determiners.items():
        ordered.insert(-1, (det, counts))

    def key(x):
        _, counts = x
        mass_count, plural_count, som_count = counts
        return mass_count + plural_count + som_count

    ordered.sort(key=key, reverse=True)

    out_file = open(args.out_file, 'w')

    out_file.write("det,MASS,PLURAL,SINGULAR_OR_MASS,TOTAL\n")

    for det, (mass_count, plural_count, som_count) in ordered:
        if mass_count + plural_count + som_count > int(args.threshold):
            out_file.write(
                f"{det},{mass_count},{plural_count},{som_count},{mass_count+plural_count+som_count}\n"
            )

    out_file.close()


if __name__ == "__main__":
    main()
