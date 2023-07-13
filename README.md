# NuPBO

NuPBO is a local search solver for solving Pseudo-Boolean Optimization (PBO) problems. It is developed based on the LS-PBO solver [2] and shares the same instance format, which is the weighted conjunctive normal form (wecnf).

## Compilation

To compile NuPBO, navigate to the `source_code` directory and run the `make` command.

## Usage

NuPBO requires two mandatory parameters:
- The first parameter is the PBO instance file.
- The second parameter is the seed value.

We utilize runsolver to execute NuPBO. Please refer to `starexec_runsolver.sh` script for more details. This script takes three arguments:
- Argument 1: The PBO instance file.
- Argument 2: Seed value.
- Argument 3: Runtime limit in seconds.

Here is an example of running NuPBO using the `starexec_runsolver.sh` script:

```shell
./starexec_runsolver.sh ./2club200v15p5scn.wecnf 2 10
```

## Opb2wecnf Conversion
The opb2wecnf_source_code folder contains a program for converting standard OPB format files to the corresponding wecnf format.

## Output Explanation
- The output starting with 'o' represents the sum of weights of unsatisfied soft clauses in the wecnf format.
- 'c realmin' corresponds to the objective function value in the original OPB file.
- The output starting with 's' represents the solution in the OPB format, which corresponds one-to-one with the wecnf format.

## 
For more details, please refer to the original paper:

[1] Yi Chu, Shaowei Cai, Chuan Luo, Zhendong Lei, Cong Peng: Towards More Efficient Local Search for Pseudo-Boolean Optimization. CP 2023, To appear.

[2] Zhendong Lei, Shaowei Cai, Chuan Luo, Holger H. Hoos: Efficient Local Search for Pseudo Boolean Optimization. SAT 2021: 332-348