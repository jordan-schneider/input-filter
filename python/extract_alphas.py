""" Utility script to exact the alpha values from a run. """

import argparse
import glob
import ntpath
import pickle
from typing import Dict, List, Tuple


def get_flags():
    """ Parses the flags. """
    parser = argparse.ArgumentParser(
        description="Processes parse tree data into (determiner, noun category, count) triplets."
    )
    parser.add_argument(
        "-i",
        "--in-folder",
        default="./output/efcamdat/",
        help="Folder containing run outputs.",
    )
    parser.add_argument(
        "-o",
        "--out-folder",
        default="./output/efcamdat/generated/",
        help="Path to folder to put output.",
    )

    return parser.parse_args()


def main():
    args = get_flags()

    for file in glob.glob(f"{args.in_folder}/*.out"):
        in_file = open(file, "r")

        alphas_per_object: Dict[int, List[List[int]]] = dict()
        # Ignore two lines.
        in_file.readline()
        in_file.readline()
        for line in in_file.readlines():
            line = line.split("]],")[0]
            line = line[2:]
            alpha_strings = line.split("],[")
            for object_index, alpha_string in enumerate(alpha_strings):
                alpha = alpha_string.split(",")
                alpha_val: Tuple[int, int, int] = tuple(
                    [int(alpha_i) for alpha_i in alpha]
                )
                alpha_sequence = alphas_per_object.get(object_index, list())
                alpha_sequence.append(alpha_val)
                alphas_per_object[object_index] = alpha_sequence

        file_base = ntpath.basename(file).split(".")[0]

        out_file = open(f"{args.out_folder}/alphas_{file_base}.pickle", "wb")
        pickle.dump(alphas_per_object, out_file)


if __name__ == "__main__":
    main()
