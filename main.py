import time
import random
import datetime as dt
import matplotlib.pyplot as plt
import matplotlib.animation as ani
from tkinter import *
from tkinter.ttk import *
from tkinter import messagebox as mb
from threading import Thread

class ConsoleAppender(Thread):
    def __init__(self, console):
        Thread.__init__(self)
        self.console = console
        
    def run(self):
        fin = open("console.csv", "w+")
        fin.close()

        fin = open("console.csv", "r+")

        while 1:
            end = fin.tell()
            line = fin.readline()
            if not line:
                time.sleep(1)
                fin.seek(end)
            else:
                self.console.insert(END, line)
                
        fin.close()

class Plot:
    def __init__(self):
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.xs = []
        self.ys = []

    def update(self, i):
        fin = open("console.csv", "w+")
        fin.close()
        
        try:
            fin = open("graph.csv", "r+")
            line = fin.readline()

            self.xs.clear()
            self.ys.clear()

            while line:
                coords = line.replace("\n", "").split(",")
        
                if(len(coords) == 2):
                    self.xs.append(coords[0])
                    self.ys.append(coords[1])

                    self.xs = self.xs[-10:]
                    self.ys = self.ys[-10:]

                line = fin.readline()
            
            plt.title("FreeRTOS Sporadic Server - RM Scheduling")
            plt.ylabel("Capacity")

            self.ax.clear()
            self.ax.plot(self.xs, self.ys)
        finally:
                fin.close()

    def start(self):
        anim = ani.FuncAnimation(self.fig, self.update, interval=1000)
        plt.show()

class Task:
    def __init__(self):
        self.name = "Task " + str(len(tasks) + 1)
        self.time_compute = 0
        self.time_period = 0
        self.job = None
        self.added = False

    def __eq__(self, other):
        if not isinstance(other, Task):
            return False
        return self.name == other.name
 
    def __ne__(self, other):
        return not self == other
    
    def __str__(self):
        return self.name

class App(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        Style().theme_use("default")
        self.master = master
        self.master.title("Scheduler Configurator")

    def layout_refresh(self):
        for widget in self.winfo_children():
            widget.destroy()

        # Task
        self.lbl_task = Label(self, text="Task")
        self.lbl_task.grid(row=0, column=0, padx=10, pady=10)
        self.cmb_task = Combobox(self, values=tasks)
        self.cmb_task.grid(row=0, column=1, padx=10, pady=10)
        if current_task.added:
            self.cmb_task.current(tasks.index(current_task))
        self.cmb_task.bind("<<ComboboxSelected>>", self.action_select_task)

        # Job
        self.lbl_job = Label(self, text="Job")
        self.lbl_job.grid(row=1, column=0)
        self.cmb_job = Combobox(self, values=jobs)
        self.cmb_job.grid(row=1, column=1, padx=10, pady=10)
        self.cmb_job.current(jobs.index(current_job))
        self.cmb_job.bind("<<ComboboxSelected>>", self.action_select_job)
 
        # Compute time
        self.lbl_time_compute = Label(self, text="Compute time (ticks)")
        self.lbl_time_compute.grid(row=2, column=0, padx=10, pady=10)
        self.ent_time_compute = Entry(self)
        self.ent_time_compute.grid(row=2, column=1, padx=10, pady=10)
        self.cmb_job.bind("<<ComboboxSelected>>", self.action_select_job)
 
        # Period time
        self.lbl_time_period = Label(self, text="Period time (ticks)")
        self.lbl_time_period.grid(row=3, column=0, padx=10, pady=10)
        self.ent_time_period = Entry(self)
        self.ent_time_period.grid(row=3, column=1, padx=10, pady=10)

        # Server capacity time
        self.lbl_time_server_capacity = Label(self, text="Server capacity time (ticks)")
        self.lbl_time_server_capacity.grid(row=4, column=0, padx=10, pady=10)
        self.ent_time_server_capacity = Entry(self)
        self.ent_time_server_capacity.grid(row=4, column=1, padx=10, pady=10)
        self.cmb_job.bind("<<ComboboxSelected>>", self.action_select_job)
 
        # Server period time
        self.lbl_time_server_period = Label(self, text="Server period time (ticks)")
        self.lbl_time_server_period.grid(row=5, column=0, padx=10, pady=10)
        self.ent_time_server_period = Entry(self)
        self.ent_time_server_period.grid(row=5, column=1, padx=10, pady=10)
        
        # Add task
        self.btn_add_task = Button(self, text="Add task", command=self.action_add)
        self.btn_add_task.grid(row=6, column=0, padx=10, pady=10)

        # Remove task
        self.btn_remove_task = Button(self, text="Remove task", command=self.action_remove)
        self.btn_remove_task.grid(row=6, column=1, padx=10, pady=10)

        # Save server
        self.btn_save = Button(self, text="Save server", command=self.action_save)
        self.btn_save.grid(row=8, column=0, padx=10, pady=10)

        # Max util
        self.btn_max_util = Button(self, text="Max util", command=self.action_max)
        self.btn_max_util.grid(row=8, column=1, padx=10, pady=10)

         # Upload batch
        self.btn_upload = Button(self, text="Upload batch", command=self.action_upload)
        self.btn_upload.grid(row=9, column=0, padx=10, pady=10)

        # Show plot
        self.btn_show = Button(self, text="Show plot", command=self.action_show)
        self.btn_show.grid(row=9, column=1, padx=10, pady=10)
       
        self.txt_serial = Text(self)
        self.txt_serial.grid(row=10, column=0, columnspan=2, padx=10, pady=10)
        ConsoleAppender(self.txt_serial).start()

        self.pack(fill=BOTH, expand=1)

    def action_select_task(self, event):
        global current_task
        for task in tasks:
            if task.name == self.cmb_task.get():
                current_task = task
                self.layout_refresh()
                self.ent_time_compute.delete(0, END)
                self.ent_time_compute.insert(0, str(current_task.time_compute))
                self.ent_time_period.delete(0, END)
                self.ent_time_period.insert(0, str(current_task.time_period))
                break

    def action_select_job(self, event):
        global current_job
        for job in jobs:
            if job == self.cmb_job.get():
                current_job = job
                self.layout_refresh()
                break

    def action_add(self):
        global current_task
        current_task.name = self.cmb_task.get()
        current_task.time_compute = self.ent_time_compute.get()
        current_task.time_period = self.ent_time_period.get()
        current_task.added = True
        for job in jobs:
            if job == self.cmb_job.get():
                current_task.job = job
                break
        i = -1
        found = False
        for task in tasks:
            i += 1
            if task == current_task:
                tasks[i] = current_task
                found = True
                break
        if not found:
            tasks.append(current_task)
        current_task = Task()
        self.layout_refresh()

    def action_remove(self):
        global current_task
        current_task.name = self.cmb_task.get()
        tasks.remove(current_task) 
        current_task = Task()
        self.layout_refresh()

    def action_max(self):
        mb.showinfo("Maximum utilization", "Capacity={}\nPeriod={}".format(0, 0))
   
    def action_upload(self):
        fout = open("/Users/Lazar/Desktop/batch.txt", "w")
        fout.write(str(len(tasks)) + "\r\n")
        for task in tasks:
            fout.write("{name},{compute},{period},{job}".format(
                name=task.name,
                compute=task.time_compute,
                period=task.time_period,
                job=task.job))
            fout.write("\r\n")
        fout.close()

    def action_save(self):
        fout = open("/Users/Lazar/Desktop/server.txt", "w")
        fout.write("{capacity},{period}\r\n".format(
            capacity=self.ent_time_server_capacity.get(),
            period=self.ent_time_server_period.get()))
        fout.close()
 
    def action_show(self):
        plot = Plot()
        plot.start()

root = Tk()
root.resizable(False, False)
app = App(root)

tasks = []
jobs = [
    "PrintLetters",
    "PrintNumbers",
    "PrintSymbols"
]

current_task = Task()
current_job = jobs[0]

app.layout_refresh()
root.mainloop()
