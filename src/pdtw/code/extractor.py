import re
import os
import pandas
from typing import Pattern, Match, TextIO

pattern: str = "[a-zA-Z]* Cost:([0-9.]*) Dist:([0-9.]*).*Time:([0-9.]*) Events:([0-9]*).*\n*Skipped: ([0-9]*)"

regex: Pattern[str] = re.compile(pattern, re.MULTILINE)

dirs = ["./Results/BB/", "./Results/BR/"]

for dire in dirs:
    all_BB_txts = os.listdir(dire)

    all_results = []

    for filename in all_BB_txts:

        file: TextIO = open(dire + filename, "r")

        results: Match[str] | None = regex.search(file.read())

        if results is None:
            print(dire + filename + " does not contain any matchable result!")
        else:
            parsed_result_list = filename.removesuffix(".txt").rsplit("_", 1)
            parsed_result_list[0] = parsed_result_list[0].rsplit("_")[0] + "_" + parsed_result_list[0].rsplit("_")[1] + "_" + parsed_result_list[0].rsplit("_")[2]
            parsed_result = tuple(parsed_result_list)
            for i in range(1, results.lastindex + 1):
                parsed_result += (results.group(i),)

            all_results.append(parsed_result)

    df = pandas.DataFrame(all_results, columns=["problem", "instance", "cost", "dist", "time", "events", "skipped"])
    df.to_csv(dire+"res.csv", sep=',', encoding='utf-8', index=False)
