import logging
import os
import pickle
from typing import Dict, List, Tuple

import matplotlib.pyplot as plt
import numpy as np

plots_prefix = "plots/"

# Mass, Plural, Singular or mass
Alpha = Tuple[int, int, int]
Delta = Tuple[float, float, float]

plt.rcParams.update({"font.size": 22})

alpha_order = [
    "A",
    "Many",
    "Which",
    "Mass only",
    "This",
    "Some",
    "The",
]

logging.basicConfig(level=logging.INFO)

determiner_groups = [
    ["a", "an", "another", "each", "every",],
    ["both", "these", "those",],
    ["which",],
    ["this", "that",],
    ["all"],
    ["any", "no", "some", "the", "what",],
]

all_alphas = [
    (0, 0, 1),
    (0, 1, 0),
    (0, 1, 1),
    (1, 0, 0),
    (1, 0, 1),
    (1, 1, 0),
    (1, 1, 1),
]

determiner_order = [det for det_group in determiner_groups for det in det_group]


def get_label(index: int, dets: List[str]) -> str:
    if len(dets) <= index:
        label = str(index)
    else:
        label = dets[index]
    return label


def alpha_to_int(alpha: Alpha) -> int:
    return int(alpha[0]) * 4 + int(alpha[1]) * 2 + int(alpha[2])


def get_alphas_per_object(dataset_path: str, dataset_name: str):
    alphas_per_object: Dict[int, List[Alpha]] = pickle.load(
        open(f"{dataset_path}/alphas_{dataset_name}.pickle", "rb")
    )

    alphas_per_object = {
        key: [(alpha[0], alpha[1], alpha[2]) for alpha in alphas]
        for key, alphas in alphas_per_object.items()
    }
    return alphas_per_object


def plot_frac_alphas(
    dataset_name: str,
    dataset_path: str,
    corpus_name: str,
    title: str,
    dets: List[str],
    correct_alpha_by_label: Dict[str, Alpha],
) -> None:
    """ Plots how often the sampled alpha was correct for each determiner"""
    alphas_per_object = get_alphas_per_object(dataset_path, dataset_name)

    freqs = {}
    n_dets = 0
    n_correct_dets = 0
    for object_index, alphas_of_object in alphas_per_object.items():
        label = get_label(object_index, dets)

        freqs[label] = sum(
            [
                sampled_alpha == correct_alpha_by_label[label]
                for sampled_alpha in alphas_of_object
            ]
        ) / len(alphas_of_object)

        max_frac = max(
            [
                sum([sampled_alpha == alpha for sampled_alpha in alphas_of_object])
                / len(alphas_of_object)
                for alpha in all_alphas
            ]
        )
        if freqs[label] >= max_frac:
            n_correct_dets += 1
        n_dets += 1
    logging.info(f"{n_correct_dets / n_dets * 100.0}% of determiners were correct.")

    # plt.title(title)

    gap_width = 0.5

    xticks = np.arange(len(freqs), dtype=np.float)
    i = 0
    for n_group, determiner_group in enumerate(determiner_groups):
        for _ in determiner_group:
            xticks[i] += 0.5 * n_group
            i += 1

    for i in range(len(determiner_groups)):
        plt.figure(figsize=(15, 5))

        on_groups = [determiner_groups[i]]
        off_groups = determiner_groups[0:i]
        if i < len(determiner_groups) - 1:
            off_groups += determiner_groups[i + 1 :]

        on_freqs = [freqs[det] for group in on_groups for det in group]
        off_freqs = [freqs[det] for group in off_groups for det in group]

        start_index = 0
        stop_index = 0
        for group in determiner_groups[:i]:
            start_index += len(group)
            stop_index += len(group)
        stop_index += len(on_freqs)

        on_ticks = xticks[start_index:stop_index]
        off_ticks = np.concatenate((xticks[0:start_index], xticks[stop_index:]))
        plt.bar(
            on_ticks, on_freqs, align="center", label="Correct", color="green",
        )
        plt.bar(
            on_ticks,
            [1 - freq for freq in on_freqs],
            align="center",
            bottom=on_freqs,
            label="Incorrect",
            color="#e06666",
        )
        plt.bar(
            off_ticks, off_freqs, align="center", color="grey",
        )
        plt.bar(
            off_ticks,
            [1 - freq for freq in off_freqs],
            align="center",
            bottom=off_freqs,
            color="darkgrey",
        )

        plt.plot(
            [-0.5, len(freqs) + gap_width * (len(determiner_groups) - 1) - 0.5],
            [1.0 / 7.0, 1.0 / 7.0],
            label="Chance",
            color="lightblue",
        )
        plt.xticks(xticks, determiner_order, rotation=90)
        # plt.xlabel("Determiner")
        # plt.ylabel("Probability of Correct Agreement Relationship")
        plt.legend(bbox_to_anchor=(1.25, 1), loc="upper right")
        plt.tight_layout()

        plt.savefig(
            os.path.join(plots_prefix, corpus_name, f"alphas_{dataset_name}_{i}")
        )
        plt.close()


def get_freqs(alphas_per_object, dets, correct_alpha_by_label):
    freqs = {}
    n_dets = 0
    n_correct_dets = 0
    for object_index, alphas_of_object in alphas_per_object.items():
        label = get_label(object_index, dets)

        freqs[label] = sum(
            [
                sampled_alpha == correct_alpha_by_label[label]
                for sampled_alpha in alphas_of_object
            ]
        ) / len(alphas_of_object)

        max_frac = max(
            [
                sum([sampled_alpha == alpha for sampled_alpha in alphas_of_object])
                / len(alphas_of_object)
                for alpha in all_alphas
            ]
        )
        if freqs[label] >= max_frac:
            n_correct_dets += 1
        n_dets += 1

    return freqs, n_correct_dets, n_dets


def plot_fracs(determiner_groups, xticks, freqs, i, gap_width):
    on_groups = [determiner_groups[i]]
    off_groups = determiner_groups[0:i]
    if i < len(determiner_groups) - 1:
        off_groups += determiner_groups[i + 1 :]

    on_freqs = [freqs[det] for group in on_groups for det in group]
    off_freqs = [freqs[det] for group in off_groups for det in group]

    start_index = 0
    stop_index = 0
    for group in determiner_groups[:i]:
        start_index += len(group)
        stop_index += len(group)
    stop_index += len(on_freqs)

    on_ticks = xticks[start_index:stop_index]
    off_ticks = np.concatenate((xticks[0:start_index], xticks[stop_index:]))
    plt.bar(
        on_ticks, on_freqs, align="center", label="Correct", color="green",
    )
    plt.bar(
        on_ticks,
        [1 - freq for freq in on_freqs],
        align="center",
        bottom=on_freqs,
        label="Incorrect",
        color="#e06666",
    )
    plt.bar(
        off_ticks, off_freqs, align="center", color="grey",
    )
    plt.bar(
        off_ticks,
        [1 - freq for freq in off_freqs],
        align="center",
        bottom=off_freqs,
        color="darkgrey",
    )

    plt.plot(
        [-0.5, len(freqs) + gap_width * (len(determiner_groups) - 1) - 0.5],
        [1.0 / 7.0, 1.0 / 7.0],
        label="Chance",
        color="lightblue",
    )


def compare_frac_alphas(
    dataset_name: str,
    dataset_paths: Tuple[str, str],
    dets: List[str],
    correct_alpha_by_label: Dict[str, Alpha],
) -> None:
    """ Plots how often the sampled alpha was correct for each determiner"""
    alphas_per_object_nonnative = get_alphas_per_object(dataset_paths[0], dataset_name)
    alphas_per_object_native = get_alphas_per_object(dataset_paths[1], dataset_name)

    native_freqs, _, _ = get_freqs(
        alphas_per_object_native, dets, correct_alpha_by_label
    )

    nonnative_freqs, _, _ = get_freqs(
        alphas_per_object_nonnative, dets, correct_alpha_by_label
    )

    gap_width = 0.5

    xticks = np.arange(len(native_freqs), dtype=np.float)
    i = 0
    for n_group, determiner_group in enumerate(determiner_groups):
        for _ in determiner_group:
            xticks[i] += 0.5 * n_group
            i += 1

    for i in range(len(determiner_groups)):
        plt.figure(figsize=(20, 8))

        ax = plt.subplot(211)
        plot_fracs(determiner_groups, xticks, native_freqs, i, gap_width)
        ax.set_xticklabels([])
        ax.tick_params(bottom=False)
        plt.legend(bbox_to_anchor=(1.2, 1), loc="upper right")

        plt.subplot(212)
        plot_fracs(determiner_groups, xticks, nonnative_freqs, i, gap_width)
        plt.xticks(xticks, determiner_order, rotation=90)

        # plt.xlabel("Determiner")
        # plt.ylabel("Probability of Correct Agreement Relationship")
        plt.tight_layout()

        plt.savefig(
            os.path.join(plots_prefix, "comparison", f"alphas_{dataset_name}_{i}")
        )
        plt.close()


def alphas_per_object_to_int(
    alpha_per_object: Dict[int, List[Alpha]]
) -> Dict[int, List[int]]:
    return {
        det: [alpha_to_int(alpha) for alpha in alphas]
        for det, alphas in alpha_per_object.items()
    }


def plot_alpha(
    corpus_name: str,
    dataset_name: str,
    dataset_path: str,
    dets: List[str],
    determiners: List[str],
    correct_alphas: Dict[str, Alpha] = None,
) -> None:
    alphas_per_object = pickle.load(
        open(f"{dataset_path}/alphas_{dataset_name}.pickle", "rb")
    )

    for object_index, int_alphas_of_object in alphas_per_object_to_int(
        alphas_per_object
    ).items():
        label = get_label(index=object_index, dets=dets)
        if label in determiners:

            # logging.info(label)
            # logging.info(alphas_per_object[object_index])

            plt.figure(figsize=(10, 5))

            N, bins, patches = plt.hist(
                [val - 1 for val in int_alphas_of_object],
                bins=list(range(8)),
                density=True,
                color="#e06666",
            )

            # plt.title(label)
            # plt.xlabel("Alpha")
            # plt.ylabel("Frequency")
            plt.xticks(np.arange(7, dtype=np.float) + 0.5, [""] * 7)
            # plt.xticks(
            #     [0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5],
            #     [("\n" * i) + alpha for i, alpha in enumerate(alpha_order)],
            # )
            plt.ylim((0, 1))

            if correct_alphas:
                correct_alpha = correct_alphas[label]
                alpha_val = alpha_to_int(correct_alpha)
                patches[alpha_val - 1].set_facecolor("g")
                patches[alpha_val - 1].set_label("Correct")
                if alpha_val == 1:
                    patches[1].set_label("Incorrect")
                else:
                    patches[0].set_label("Incorrect")

                plt.legend()

            plt.tight_layout()

            plt.savefig(
                os.path.join(plots_prefix, corpus_name, "alphas_per_det", label)
            )

            plt.close()


def read_csv(filename: str) -> np.ndarray:
    rows = open(filename).readline().split(",")
    return np.array([float(x) for x in rows])


def get_dist_pluralities(
    dataset_name: str, counts_path: str, corpus_name: str, dets: List[str]
) -> Tuple[float, Delta]:
    counts_file = open(
        os.path.join(counts_path, f"det_cat_counts_{corpus_name}_clean.csv")
    )
    counts_file.readline()

    counts = np.zeros(3, dtype=np.float)
    total_count = 0
    for line in counts_file.readlines():
        det, mass_str, plural_str, singular_str, _ = line.split(",")
        if det in dets:
            counts[0] += int(mass_str)
            counts[1] += int(plural_str)
            counts[2] += int(singular_str)

    fracs = counts / np.sum(counts)

    logging.info(f"det dist for {dataset_name} in {corpus_name} is {fracs}")

    return fracs


def get_attributed_noise(
    dataset_name: str,
    counts_path: str,
    corpus_name: str,
    alphas: Dict[str, Alpha],
    dets: List[str],
) -> Tuple[float, Delta]:
    counts_file = open(
        os.path.join(counts_path, f"det_cat_counts_{corpus_name}_clean.csv")
    )
    counts_file.readline()
    counts = {}
    total_count = 0
    for line in counts_file.readlines():
        det, mass_str, plural_str, singular_str, total_str = line.split(",")
        mass_count = int(mass_str)
        plural_count = int(plural_str)
        singular_count = int(singular_str)
        total = int(total_str)
        if det in dets:
            counts[det] = (mass_count, plural_count, singular_count)
            total_count += total

    noise_counts = np.zeros(3)

    for det in common_dets:
        alpha = alphas[det]
        for i, allowed in enumerate(alpha):
            if allowed == 0 and det in counts:
                noise_counts[i] += counts[det][i]

    noise_rate = np.sum(noise_counts) / total_count

    delta = noise_counts / np.sum(noise_counts)

    return noise_rate, delta


def get_estimated_noise(
    dataset_name: str, outputs_path: str
) -> Tuple[Tuple[float, float], Delta]:
    epsilons = read_csv(f"{outputs_path}/epsilons_{dataset_name}.csv")

    epsilon_mean = np.mean(epsilons)
    epsilon_std = np.std(epsilons)

    delta_ones = read_csv(f"{outputs_path}/delta_ones_{dataset_name}.csv")
    delta_twos = read_csv(f"{outputs_path}/delta_twos_{dataset_name}.csv")
    delta_threes = read_csv(f"{outputs_path}/delta_threes_{dataset_name}.csv")
    delta_one_mean = np.mean(delta_ones)
    delta_two_mean = np.mean(delta_twos)
    delta_three_mean = np.mean(delta_threes)

    return (
        (epsilon_mean, epsilon_std),
        (delta_one_mean, delta_two_mean, delta_three_mean),
    )


def get_modal_alphas(
    outputs_path: str, dataset_name: str, dets: List[str]
) -> Dict[str, Alpha]:
    alphas_per_object = get_alphas_per_object(outputs_path, dataset_name)

    modal_alpha: Dict[str, Alpha] = dict()
    for object_index, alphas_of_object in alphas_per_object.items():
        det = get_label(object_index, dets)
        total_sampled_alphas = len(alphas_of_object)
        alpha_fracs: Dict[Alpha, float] = dict()
        for alpha in all_alphas:
            alpha_fracs[alpha] = (
                sum([sampled_alpha == alpha for sampled_alpha in alphas_of_object])
                / total_sampled_alphas
            )

        max_frac = 0.0
        for alpha, frac in alpha_fracs.items():
            if frac > max_frac:
                max_frac = frac
                modal_alpha[det] = alpha

    return modal_alpha


def noise_stats(
    dataset_name: str,
    outputs_path: str,
    counts_path: str,
    corpus_name: str,
    true_alphas: Dict[str, Alpha],
    dets: List[str],
):
    (epsilon_mean, epsilon_std), delta_mean = get_estimated_noise(
        dataset_name=dataset_name, outputs_path=outputs_path
    )

    logging.info(
        f"Epsilons for {dataset_name} in {corpus_name} have a mean of {epsilon_mean} and standard deviation of {epsilon_std}"
    )
    logging.info(
        f"Deltas for {dataset_name} in {corpus_name} have a mean of ({delta_mean})"
    )

    true_noise_rate, true_delta = get_attributed_noise(
        dataset_name=dataset_name,
        counts_path=counts_path,
        corpus_name=corpus_name,
        alphas=true_alphas,
        dets=dets,
    )

    logging.info(
        f"True noise rate for {dataset_name} in {corpus_name} is {true_noise_rate}"
    )
    logging.info(f"True delta for {dataset_name} in {corpus_name} is {true_delta}")

    modal_alphas = get_modal_alphas(
        outputs_path=outputs_path, dataset_name=dataset_name, dets=dets
    )
    attributed_noise_rate, attributed_delta = get_attributed_noise(
        dataset_name=dataset_name,
        counts_path=counts_path,
        corpus_name=corpus_name,
        alphas=modal_alphas,
        dets=dets,
    )

    logging.info(
        f"Noise rate from inferred alphas for {dataset_name} in {corpus_name} is {attributed_noise_rate}"
    )
    logging.info(
        f"Deltas from inferred alphas for {dataset_name} in {corpus_name} is {attributed_delta}"
    )

    return (
        (epsilon_mean, epsilon_std, true_noise_rate, attributed_noise_rate),
        (delta_mean, tuple(true_delta)),
    )


def compute_ones(dataset_name: str, dataset_path: str) -> List[float]:
    n_ones: List[int] = [0, 0, 0]
    with open(f"{dataset_path}/alphas_{dataset_name}.pickle", "rb") as alphas_file:
        alphas_per_object = pickle.load(alphas_file)
        for _, sampled_alphas in alphas_per_object.items():
            for alpha in sampled_alphas:
                ones = sum(alpha)
                n_ones[ones - 1] += 1
    return [count / sum(n_ones) for count in n_ones]


def plot_systematization_true_alphas(true_alphas: Dict[str, Alpha]):
    n_allowed_types = np.zeros(3, dtype=np.int)
    for alpha in true_alphas.values():
        n_allowed_types[sum(alpha) - 1] += 1

    plt.bar(range(3), n_allowed_types / sum(n_allowed_types))
    plt.xlabel("Number of Allowed Noun Pluralities")
    plt.ylabel("Fraction")
    plt.xticks(range(3), [1, 2, 3])
    plt.ylim((0, 1))
    plt.tight_layout()
    plt.savefig(os.path.join(plots_prefix, "true_systematization"))
    plt.close()


efcamdat_common_dets = open("data/efcamdat/common_dets.txt", "r").readlines()
efcamdat_common_dets = [det[:-1] for det in efcamdat_common_dets]

enron_common_dets = open("data/enron/common_dets.txt", "r").readlines()
enron_common_dets = [det[:-1] for det in enron_common_dets]

assert enron_common_dets == efcamdat_common_dets

common_dets = efcamdat_common_dets

get_dist_pluralities(
    dataset_name="log_5000_common",
    counts_path="data/efcamdat/counts",
    corpus_name="efcamdat",
    dets=common_dets,
)
get_dist_pluralities(
    dataset_name="log_5000_common",
    counts_path="data/enron/counts",
    corpus_name="enron",
    dets=common_dets,
)

true_alphas_file = open("data/alphas.txt", "r")
true_alphas_file.readline()
true_alphas: Dict[str, Alpha] = {}
for line in true_alphas_file.readlines():
    det, mass, plural, singular = line.split(",")
    singular = singular.strip()
    true_alphas[det] = (
        int(mass == "1"),
        int(plural == "1"),
        int(singular == "1"),
    )

true_alphas = {det: true_alphas[det] for det in common_dets}

plot_frac_alphas(
    dataset_name="log_5000_common",
    dataset_path="output/efcamdat/generated",
    corpus_name="efcamdat",
    title="Model Performance On Non-native Input",
    dets=common_dets,
    correct_alpha_by_label=true_alphas,
)

# plot_frac_alphas(
#     dataset_name="log_5000_common",
#     dataset_path="output/enron/generated",
#     corpus_name="enron",
#     title="Model Performance On Native Input",
#     dets=common_dets,
#     correct_alpha_by_label=true_alphas,
# )

compare_frac_alphas(
    dataset_name="log_5000_common",
    dataset_paths=("output/efcamdat/generated", "output/enron/generated"),
    dets=common_dets,
    correct_alpha_by_label=true_alphas,
)


plt.bar(range(3), compute_ones("log_5000_common", "output/efcamdat/generated"))
plt.xlabel("Number of Allowed Noun Pluralities")
plt.ylabel("Fraction")
# plt.title("Degree of Systematization of Input Filter Model on Non-native Input")
plt.xticks(range(3), [1, 2, 3])
plt.ylim((0, 1))
plt.tight_layout()
plt.savefig(os.path.join(plots_prefix, "efcamdat", "systematization_log_5000_common"))
plt.close()

plt.bar(range(3), [0, 0, 1])
plt.xlabel("Number of Allowed Noun Pluralities")
plt.ylabel("Fraction")
# plt.title("Degree of Systematization of Naive Model on Non-native Input")
plt.xticks(range(3), [1, 2, 3])
plt.ylim((0, 1))
plt.tight_layout()
plt.savefig(os.path.join(plots_prefix, "no_systematization"))
plt.close()

plt.bar(range(3), compute_ones("log_5000_common", "output/enron/generated"))
plt.xlabel("Number of Allowed Noun Pluralities")
plt.ylabel("Fraction")
# plt.title("Degree of Systematization of Input Filter Model on Native Input")
plt.xticks(range(3), [1, 2, 3])
plt.ylim((0, 1))
plt.tight_layout()
plt.savefig(os.path.join(plots_prefix, "enron", "systematization_log_5000_common"))
plt.close()

plot_alpha(
    corpus_name="efcamdat",
    dataset_name="log_5000_common",
    dataset_path="output/efcamdat/generated",
    dets=common_dets,
    determiners=["a", "these", "which", "this", "all", "the", "both"],
    correct_alphas=true_alphas,
)


plot_alpha(
    corpus_name="enron",
    dataset_name="log_5000_common",
    dataset_path="output/enron/generated",
    dets=common_dets,
    determiners=["a", "these", "which", "this", "all", "the", "both"],
    correct_alphas=true_alphas,
)

noise_stats(
    dataset_name="log_5000_common",
    outputs_path="output/efcamdat/generated",
    counts_path="data/efcamdat/counts",
    corpus_name="efcamdat",
    true_alphas=true_alphas,
    dets=common_dets,
)
noise_stats(
    dataset_name="log_5000_common",
    outputs_path="output/enron/generated",
    counts_path="data/enron/counts",
    corpus_name="enron",
    true_alphas=true_alphas,
    dets=common_dets,
)

plot_systematization_true_alphas(true_alphas)
