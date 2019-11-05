from tkinter import *
from tkinter.ttk import *
import matplotlib.pyplot as plt 

tasks = []

class Task:
    def __init__(self, id):
        self.id = id

class App(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        Style().theme_use("default")
        
        self.master = master
        self.master.title("Homework 2")
 
        self.lbl_task = Label(self, text="Task")
        self.lbl_task.grid(row=0, column=0, padx=10, pady=10)
        
        self.ent_task = Entry(self)
        self.ent_task.grid(row=0, column=1, columnspan=2, padx=10, pady=10)
        
        self.lbl_path = Label(self, text="Path")
        self.lbl_path.grid(row=1, column=0, padx=10, pady=10)
        
        self.ent_path = Entry(self)
        self.ent_path.grid(row=1, column=1, columnspan=2, padx=10, pady=10)
        
        self.btn_add = Button(self, text="Add task", command=self.action_add)
        self.btn_add.grid(row=2, column=0,padx=10, pady=10)
        
        self.btn_save = Button(self, text="Save tasks", command=self.action_save)
        self.btn_save.grid(row=2, column=1, padx=10, pady=10)

        self.btn_load = Button(self, text="Load report", command=self.action_load)
        self.btn_load.grid(row=2, column=2, padx=10, pady=10)
        
        self.pack(fill=BOTH, expand=1)
    
    def action_add(self):
        tasks.append(Task(len(tasks) + 1))

    def action_save(self):
        fout = open(self.ent_path.get(), "w")
        for task in tasks:
            fout.write('{}\r\n'.format(task.id))
        fout.close()
    
    def action_load(self):
        table_header = ["Task #", "Starting time (ms)", "Finishing time (ms)"]
        table_data = []

        fin = open(self.ent_path.get(), "r")
        i = 0
        for line in fin:
            row = []
            task = line.split(",")
            for attr in task:
                row.append(attr.strip())
            if len(row) == len(table_header):
                table_data.append(row)
        print(table_data)
        fin.close()
        
        fig = plt.figure("Report", dpi=120)
        ax = fig.add_subplot(1, 1, 1)
        ax.axis("off")
        table = ax.table(colLabels=table_header, cellText=table_data, loc="center")
        table.set_fontsize(12)
        table.scale(1, 2)
        plt.show()

root = Tk()
root.resizable(False, False)
app = App(root)
root.mainloop() 
