import argparse
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import StrMethodFormatter, FuncFormatter
import numpy as np
import math
import time

# Command-Line Argument Parsing
#=================================================================
def build_arg_parser():
    RESULT_DEFAULT_PATH = "evaluation"
    PLOTTING_TYPE_DEFAULT = "threads"
    EXPORT_PDF_DEFAULT = False

    parser = argparse.ArgumentParser()

    porthelp = "Result data path"
    if RESULT_DEFAULT_PATH:
        resultkwargs = { "default": RESULT_DEFAULT_PATH,
                "help": porthelp + " Default: {}".format(RESULT_DEFAULT_PATH) }
    else:
        resultkwargs = { "required": True,
                "help": porthelp + " Required." }
    parser.add_argument("-r", "--result", type=str, **resultkwargs)
    
    parser.add_argument("--pdf", action='store_true', help="Export plotting as PDF. Default: {}".format(EXPORT_PDF_DEFAULT))
    
    parser.add_argument("-t", "--type", nargs='?', default=PLOTTING_TYPE_DEFAULT,
            help="Draw chart based on the desired type (e.g., by threads or duration).  Default: {}".format(PLOTTING_TYPE_DEFAULT))

    return parser

# Plotting object to draw charts
#=================================================================
class PlotData:
    def __init__(self, result_data, draw_type, export_pdf):
        self.result_data = result_data
        self.draw_type = draw_type
        self.export_pdf = export_pdf
    
    def __generate_name(self, type):
        timestr = "figure_{}_{}".format(time.strftime("%Y%m%d_%H%M%S"), type)
        return timestr
        
    def __formatted_number(self, num):
        millnames = ['', 'K', 'M', 'B', 'T']

        millidx = max(0, min(len(millnames) - 1,
           int(math.floor(0
              if num == 0
              else math.log10(abs(num)) / 3))))

        return '{:.0f}{}'.format(num / 10 ** (3 * millidx), millnames[millidx])

    def __kilos(self, x, pos):
        return '%1.0f' % (x/1000)

    def __formatted_label(self, x):
        return f"{self.__formatted_number(x)} queries"
    
    def __read_csv(self, path):
        df = pd.read_csv(path, names=['Threads', 'Duration', 'Queries', 'Operations', 'Energy', 'Effiency'], header=None)
        return df
        
    def __get_data(self, original_data):
        result =  original_data.astype(int)
        result = list(map(int, result))
        return result
            
    def __draw(self, field):        
        marker_map = ["s", "^", "o", "x", "2", "."]
        color_map = ['b', 'g', 'r', 'k', 'm', 'y', 'c']

        df = self.__read_csv(self.result_data)

        sortedData = df.sort_values(by=['Queries'])
        myData = sortedData["Queries"].unique()
        
        for i in range (len(myData)):
            filter = df["Queries"]==myData[i]
            newData = df.where(filter, inplace = False)
            
            newData = newData.dropna()

            x = self.__get_data(newData[field])
            
            y = newData['Effiency']
            y = list(map(float, y))
            
            plt.plot(x, y, color=color_map[i], marker=marker_map[i], linestyle='solid', linewidth=1, markersize=5, label= self.__formatted_label(myData[i]))
            plt.xlabel(field)
            plt.ylabel("Energy effiency (Kops/Joule)")
            
            plt.gca().yaxis.set_major_formatter(FuncFormatter(self.__kilos))
            plt.gca().xaxis.set_major_formatter(StrMethodFormatter('{x:,.0f}'))

        plt.xticks(x)
        leg = plt.legend()
        leg.get_frame().set_edgecolor('black')
        leg.get_frame().set_linewidth(0.5)
        plt.grid(visible=True, which='major', color='#dddddd', linestyle='-')
        
        if (self.export_pdf):
            plt.savefig(f'{self.__generate_name(field.lower())}.pdf')  
        else:
            plt.show()
       
    def plot(self):
        if (self.draw_type == "threads"):
            self.__draw("Threads")
        elif (self.draw_type == "duration"):
            self.__draw("Duration")
        else:
            print("Please provide a corrct plotting type. It should between 'threads' and 'duration'.")
        
        
# Main function (Entry Point)
#=================================================================
if __name__ == "__main__":
    parser = build_arg_parser()
    args = parser.parse_args()

    plotdata_obj = PlotData(args.result, args.type, args.pdf)
    plotdata_obj.plot()