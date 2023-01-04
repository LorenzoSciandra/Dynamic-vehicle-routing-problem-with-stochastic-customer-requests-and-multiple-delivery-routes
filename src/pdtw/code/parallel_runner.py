print("PROGRAM NOT WORKING")
exit(1)

import os
import re
import subprocess
import threading
import time
from multiprocessing.pool import ThreadPool
from typing import Pattern, List, TextIO

number_of_scenarios_to_run: int = 10

all_scenarios: list[str] = os.listdir("../instances/sddp/stacy/Homogeneous/")
regex: Pattern[str] = re.compile("[A-Za-z0-9]+_[A-Za-z0-9]+_[A-Za-z0-9]+_hom_1_actual")
problems: list[str] = [s.removesuffix("_actual") for s in all_scenarios if regex.match(s)]

template: str = """----------------------------------------
DEFAULT
----------------------------------------

>> GENERATOR
INSTANCE_TYPE		Stacy

>> PROBLEM
PROBLEM_NAME		%(problem)s
ACTUAL_SCENARIO_ID	%(scenario_id)d
TW_TYPE			d1
"""


def run_bb(problem: str, scenario_id: int):
    try:
        thread_id = threading.get_ident()
        print(problem + " " + str(scenario_id))

        config_file: TextIO = open(f"../instances/sddp/stacy/config/temp{thread_id}.dat", "x")
        config_file.write(template % {"problem": problem, "scenario_id": scenario_id})
        config_file.close()

        subprocess.call(
            f"./cmake-build-debug-wsl/branch_and_bound_solver ../instances/sddp/stacy/config/temp{thread_id}.dat")
    except Exception as e:
        print("Error: " + str(e))


pool = ThreadPool(processes=10)

for problem_instance in problems:
    for scenario_instance in range(1, number_of_scenarios_to_run + 1):
        pool.apply_async(run_bb, kwds={"problem": problem_instance, "scenario_id": scenario_instance})

# def run_bb(config: dict[str, int]):
#     subprocess.Popen()
#
# with ThreadPool(processes=10) as pool:
#     pool.apply_async()
#
# for problem in problem_instances:
#     for scenario_instance in range(1, number_of_scenarios_to_run + 1):
#         pool.apply_async(run_bb)

# config_file: TextIO = open("../instances/sddp/stacy/config/temp.dat", "w")
# config_file.write(template % {"problem": s, "scenario_id": n})
# config_file.close()

# os.system(
#     "./cmake-build-debug-wsl/branch_and_bound_solver ../instances/sddp/stacy/config/temp.dat"
# )

# os.system(
#     "./cmake-build-debug-wsl/branch_and_regret_solver ../instances/sddp/stacy/config/temp.dat"
# )

pool.close()
pool.join()
