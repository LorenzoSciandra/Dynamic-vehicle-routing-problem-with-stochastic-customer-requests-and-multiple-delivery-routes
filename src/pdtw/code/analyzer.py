import pandas
import numpy
import time
import matplotlib.pyplot as plt


BB_data = pandas.read_csv("./Results/BB/res.csv")
BR_data = pandas.read_csv("./Results/BR/res.csv")

print(str(BB_data[BB_data.columns[5]].sum()) + " secondi per BB")
print(str(BR_data[BR_data.columns[5]].sum())+ " secondi per BR")

BB_data = BB_data.groupby("problem_type").agg(
    {'cost': [numpy.mean, numpy.std], 'dist': [numpy.mean, numpy.std], 'time': [numpy.mean, numpy.std],
     'events': [numpy.mean, numpy.std], 'skipped': [numpy.mean, numpy.std]})
BB_data.columns = ['Mean Cost', 'Std Cost', 'Mean Dist', 'Std Dist', 'Mean Time', 'Std Time', 'Mean Events',
                   'Std Events', 'Mean Skipped', 'Std Skipped']
BB_data.reset_index(inplace=True)

BR_data = BR_data.groupby("problem_type").agg(
    {'cost': [numpy.mean, numpy.std], 'dist': [numpy.mean, numpy.std], 'time': [numpy.mean, numpy.std],
     'events': [numpy.mean, numpy.std], 'skipped': [numpy.mean, numpy.std]})
BR_data.columns = ['Mean Cost', 'Std Cost', 'Mean Dist', 'Std Dist', 'Mean Time', 'Std Time', 'Mean Events',
                   'Std Events', 'Mean Skipped', 'Std Skipped']
BR_data.reset_index(inplace=True)

print_br = False
print_vs = False
i = 1

while i < len(BB_data.columns) and not print_br:

    print('1. Generate Next BB Plot')
    print('2. Go to BR Plots')
    print('3. Exit')
    a = input()

    if a == '1':
        plt.errorbar(BB_data[BB_data.columns[0]], BB_data[BB_data.columns[i]], BB_data[BB_data.columns[i + 1]],
                     color="red", linestyle='None', marker='o', capsize=5, capthick=1, ecolor="black")

        # mettere label ai punti diventa brutto
        for y in BB_data[BB_data.columns[i]].tolist():
            plt.annotate('%.2f' % y, xy=(BB_data[BB_data.columns[i]].tolist().index(y), y), xytext=(2, 5),
                         textcoords='offset points', rotation=85)

        plt.title(BB_data[BB_data.columns[i]].name + " of BB")
        plt.xlabel("Problem Name")
        plt.ylabel(BB_data[BB_data.columns[i]].name)
        plt.xticks(rotation=90)
        plt.tick_params(axis='x', which='major')
        plt.tight_layout()
        plt.show()

    elif a == '2':
        print_br = True
    else:
        exit(0)

    i += 2
    time.sleep(0.3)

if i == len(BB_data.columns) or print_br:
    i = 1
    while i < len(BR_data.columns) and not print_vs:
        print('1. Generate Next BR Plot')
        print('2. Go to BB and BR differences')
        print('3. Exit')
        a = input()

        if a == '1':
            plt.errorbar(BR_data[BR_data.columns[0]], BR_data[BR_data.columns[i]], BR_data[BR_data.columns[i + 1]],
                         color="red", linestyle='None', marker='o', capsize=5, capthick=1, ecolor="black")

            # mettere label ai punti diventa brutto
            for y in BR_data[BR_data.columns[i]]:
                plt.annotate('%.2f' % y, xy=(BR_data[BR_data.columns[i]].index(y), y), xytext=(2, 5),
                             textcoords='offset points', rotation=85)

            plt.title(BR_data[BR_data.columns[i]].name + " of BR")
            plt.xlabel("Problem Name")
            plt.ylabel(BR_data[BR_data.columns[i]].name)
            plt.xticks(rotation=90)
            plt.tick_params(axis='x', which='major')
            plt.tight_layout()
            plt.show()

        elif a == '2':
            print_vs = True

        else:
            exit(0)

        i += 2
        time.sleep(0.3)

if i == len(BB_data.columns) or print_vs:
    i = 1
    while i < len(BR_data.columns):
        print('1. Generate Next BB vs BR Plot')
        print('2. Exit')
        a = input()

        if a == '1':
            x_br_list = [elem + " (BR)" for elem in list(BR_data[BR_data.columns[0]])]
            x_bb_list = [elem + " (BB)" for elem in list(BB_data[BB_data.columns[0]])]
            y_br_list = list(BR_data[BR_data.columns[i]])
            y_bb_list = list(BB_data[BB_data.columns[i]])
            br_errlist = list(BR_data[BR_data.columns[i + 1]])
            bb_errlist = list(BB_data[BB_data.columns[i + 1]])

            plt.figure(figsize=(8, 6))

            for j in range(len(x_bb_list)):
                plt.errorbar(x_br_list[j], y_br_list[j], br_errlist[j],
                             color="red", linestyle='None', marker='o', capsize=5, capthick=1, ecolor="purple")

                plt.annotate('%.2f' % y_br_list[j], xy=(x_br_list[j], y_br_list[j]), xytext=(2, 5),
                             textcoords='offset points', rotation=85)

                plt.errorbar(x_bb_list[j], y_bb_list[j], bb_errlist[j],
                             color="blue", linestyle='None', marker='x', capsize=5, capthick=1, ecolor="green")

                plt.annotate('%.2f' % y_bb_list[j], xy=(x_bb_list[j], y_bb_list[j]), xytext=(2, 5),
                             textcoords='offset points', rotation=85)

            plt.title("Comparison of " + BR_data[BR_data.columns[i]].name)
            plt.xlabel("Problem Name (Algorithm)")
            plt.ylabel(BR_data[BR_data.columns[i]].name)
            plt.xticks(rotation=75)
            plt.tick_params(axis='x', which='major')
            plt.tight_layout()
            plt.show()

        else:
            exit(0)

        i += 2
        time.sleep(0.3)
