import datetime
import os
import re
from typing import Pattern, List, TextIO

number_of_scenarios_to_run: int = 10

all_scenarios: list[str] = os.listdir("../instances/sddp/stacy/Homogeneous/")
regex: Pattern[str] = re.compile("[A-Za-z0-9]+_[A-Za-z0-9]+_[A-Za-z0-9]+_hom_1_actual")
problem_instances: list[str] = [s.removesuffix("_actual") for s in all_scenarios if regex.match(s)]

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

bb_timedelta = datetime.timedelta(0)
br_timedelta = datetime.timedelta(0)

for s in problem_instances:
    for n in range(1, number_of_scenarios_to_run + 1):
        config_file: TextIO = open("../instances/sddp/stacy/config/temp.dat", "w")
        config_file.write(template % {"problem": s, "scenario_id": n})
        config_file.close()

        start = datetime.datetime.now()
        os.system(
            "./cmake-build-debug-wsl/branch_and_bound_solver ../instances/sddp/stacy/config/temp.dat"
        )
        bb_timedelta += datetime.datetime.now() - start

        start = datetime.datetime.now()

        os.system(
            "./cmake-build-debug-wsl/branch_and_regret_solver ../instances/sddp/stacy/config/temp.dat"
        )
        br_timedelta += datetime.datetime.now() - start

print(f"Computation took {str(bb_timedelta+br_timedelta)}, BB: {str(bb_timedelta)}, BR: {str(br_timedelta)}")
