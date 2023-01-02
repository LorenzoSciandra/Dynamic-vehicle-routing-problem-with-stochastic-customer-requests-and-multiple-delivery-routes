import pandas
import numpy
import time
import matplotlib.pyplot as plt

BB_data = pandas.read_csv("./Results/BB/res.csv")
BR_data = pandas.read_csv("./Results/BR/res.csv")

print(BB_data[BB_data.columns[4]].sum() + "secondi di BB")
print(BR_data[BR_data.columns[4]].sum() + "secondi di BR")

BB_data = BB_data.groupby("problem").agg(
    {'cost': [numpy.mean, numpy.std], 'dist': [numpy.mean, numpy.std], 'time': [numpy.mean, numpy.std],
     'events': [numpy.mean, numpy.std], 'skipped': [numpy.mean, numpy.std]})
BB_data.columns = ['Mean Cost', 'Std Cost', 'Mean Dist', 'Std Dist', 'Mean Time', 'Std Time', 'Mean Events',
                   'Std Events', 'Mean Skipped', 'Std Skipped']
BB_data.reset_index(inplace=True)

BR_data = BR_data.groupby("problem").agg( {'cost': [numpy.mean, numpy.std], 'dist': [numpy.mean, numpy.std], 'time': [numpy.mean, numpy.std], 'events': [numpy.mean, numpy.std], 'skipped': [numpy.mean, numpy.std]})
BR_data.columns = ['Mean Cost', 'Std Cost', 'Mean Dist', 'Std Dist', 'Mean Time', 'Std Time', 'Mean Events', 'Std Events', 'Mean Skipped', 'Std Skipped']
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
        plt.errorbar(BB_data[BB_data.columns[0]], BB_data[BB_data.columns[i]], BB_data[BB_data.columns[i+1]], linestyle='None', marker='o', ecolor="red")

        # mettere label ai punti diventa brutto
        #for y in BB_data[BB_data.columns[i]].tolist():
        #    plt.annotate('%.2f' % y, xy=(BB_data[BB_data.columns[i]].tolist().index(y), y), xytext=(0, 0), textcoords='offset points')

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
            plt.errorbar(BR_data[BR_data.columns[0]], BR_data[BR_data.columns[i]], BR_data[BR_data.columns[i+1]], linestyle='None', marker='o', ecolor="red")

            # mettere label ai punti diventa brutto
            #for y in BR_data[BR_data.columns[i]]:
            #    plt.annotate('%.2f' % y, xy=(BR_data[BR_data.columns[i]].index(y), y), xytext=(0, 0), textcoords='offset points')

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
    while i < len(BR_data):
        print('1. Generate Next BB vs BR Plot')
        print('2. Exit')
        a = input()

        if a == '1':
            fig = plt.figure()
            j = 1
            while j < len(BB_data.columns):
                pos = int(320 + numpy.ceil(j/2))
                ax = fig.add_subplot(pos)
                x_axis = ["BB", "BR"]
                y_axis = [BB_data[BB_data.columns[j]][i], BR_data[BR_data.columns[j]][i]]
                y_axis_err = [BB_data[BB_data.columns[j+1]][i], BR_data[BB_data.columns[j+1]][i]]

                for y in y_axis:
                    plt.annotate('%.2f' % y, xy=(y_axis.index(y), y), xytext=(0, 0), textcoords='offset points')

                plt.errorbar(x_axis,y_axis, y_axis_err, linestyle='None', marker='o', ecolor="red")
                plt.title("BB vs BR on " + BB_data[BB_data.columns[0]][i])
                plt.xlabel("Algorithm")
                plt.ylabel(BB_data[BR_data.columns[j]].name)
                plt.tight_layout()
                j += 2
            plt.show()
        else:
            exit(0)
        i += 1
        time.sleep(0.3)
