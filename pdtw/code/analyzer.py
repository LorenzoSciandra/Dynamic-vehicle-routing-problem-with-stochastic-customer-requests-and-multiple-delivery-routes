import pandas

BB_data = pandas.read_csv("./Results/BB/res.csv")
BR_data = pandas.read_csv("./Results/BR/res.csv")

BB_data.groupby("problem").agg({''})