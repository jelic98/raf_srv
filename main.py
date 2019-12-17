import time
import random
import serial
import datetime as dt
from queue import Queue
import matplotlib.pyplot as plt
import matplotlib.animation as ani
from tkinter import *
from tkinter.ttk import *
from tkinter import messagebox as mb
from threading import Thread

PATH_CONSOLE = "io/console.csv"
PATH_GRAPH = "io/graph.csv"

class SerialThread(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.arduino = serial.Serial("/dev/cu.usbmodem14101", baudrate=9600, timeout=3)
        time.sleep(3)

    def run(self):
        while(True):
            if not cmd_q.empty():
                cmd = cmd_q.get()
                self.arduino.write(cmd.encode())
                fconsole = open(PATH_CONSOLE, "a+")
                fconsole.write("<<< {}\n".format(cmd))
                fconsole.close()

            response = self.arduino.readline().decode("ascii").replace("\r\n", "\n")
            
            if response[0] == "G": # Graph
                coords = response.split(",")
                coords.pop(0)
        
                fgraph = open(PATH_GRAPH, "a+")
                fgraph.write(",".join(coords))
                fgraph.close()
            elif response[0] == "U": # Utilization
                cfg = response.split(",")
                cfg_capacity = cfg[1]
                cfg_period = cfg[2]

                mb.showinfo("Maximum utilization", "Capacity={}\nPeriod={}".format(cfg_capacity, cfg_period))
            else: # Console
                fconsole = open(PATH_CONSOLE, "a+")
                fconsole.write(response.replace("\n", r"\n") + "\n")
                fconsole.close()

class ConsoleThread(Thread):
    def __init__(self, console):
        Thread.__init__(self)
        self.console = console
        
    def run(self):
        fin = open(PATH_CONSOLE, "w+")
        fin.close()
        fin = open(PATH_CONSOLE, "r+")

        while True:
            end = fin.tell()
            line = fin.readline()
            if not line:
                time.sleep(1)
                fin.seek(end)
            else:
                self.console.insert(END, line)
                self.console.see("end")
                
class Plot:
    def __init__(self):
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.xs = []
        self.ys = []

    def update(self, i): 
        try:
            fin = open(PATH_CONSOLE, "w+")
            fin.close()
            self.xs.clear()
            self.ys.clear()

            line = fin.readline()
            
            while line:
                coords = line.replace("\n", "").split(",")
        
                if(len(coords) == 2):
                    self.xs.append(coords[0])
                    self.ys.append(coords[1])

                line = fin.readline()
            
            plt.title("FreeRTOS Sporadic Server - RM Scheduling")
            plt.ylabel("Capacity")
            
            self.xs = self.xs[-10:]
            self.ys = self.ys[-10:]

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
        
        # Add periodic task
        self.btn_add_task = Button(self, text="Add task to batch", command=self.action_tap)
        self.btn_add_task.grid(row=6, column=0, padx=10, pady=10)

        # Remove periodic task
        self.btn_remove_task = Button(self, text="Remove task from batch", command=self.action_trp)
        self.btn_remove_task.grid(row=6, column=1, padx=10, pady=10)

        # Join batch
        self.btn_upload = Button(self, text="Upload batch", command=self.action_bjp)
        self.btn_upload.grid(row=8, column=0, padx=10, pady=10)
        
        # Add sporadic task
        self.btn_sporadic = Button(self, text="Add sporadic task", command=self.action_tas)
        self.btn_sporadic.grid(row=8, column=1, padx=10, pady=10)

        # Configure server
        self.btn_save = Button(self, text="Configure server", command=self.action_sc)
        self.btn_save.grid(row=9, column=0, padx=10, pady=10)

        # Maximum utilization
        self.btn_max_util = Button(self, text="Max utilization", command=self.action_smu)
        self.btn_max_util.grid(row=9, column=1, padx=10, pady=10)

        # Show graph
        self.btn_show = Button(self, text="Show graph", command=self.action_show)
        self.btn_show.grid(row=10, column=0, columnspan=2, padx=10, pady=10)
       
        # Console
        self.txt_console = Text(self)
        self.txt_console.grid(row=11, column=0, columnspan=2, padx=10, pady=10)
        
        SerialThread().start()
        ConsoleThread(self.txt_console).start()

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

    def action_tap(self):
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
        self.cmd_push("TAP,{name},{compute},{period},{job}\n".format(
            name=current_task.name,
            compute=current_task.time_compute,
            period=current_task.time_period,
            job=current_task.job))
        current_task = Task()
        self.layout_refresh()
    
    def action_tas(self): 
        global current_task
        current_task.name = self.cmb_task.get()
        self.cmd_push("TAS,{name},{compute},{job}\n".format(
            name=current_task.name,
            compute=current_task.time_compute,
            job=current_task.job))

    def action_trp(self):
        global current_task
        current_task.name = self.cmb_task.get()
        self.cmd_push("TRP,{name}\n".format(name=current_task.name))
        tasks.remove(current_task)
        current_task = Task()
        self.layout_refresh()

    def action_smu(self):
        self.cmd_push("SMU\n")
   
    def action_bjp(self):
        self.cmd_push("BJP\n")

    def action_sc(self):
        self.cmd_push("SC,{capacity},{period}\n".format(
            capacity=self.ent_time_server_capacity.get(),
            period=self.ent_time_server_period.get()))
 
    def action_show(self):
        plot = Plot()
        plot.start()

    def cmd_push(self, cmd):
        cmd_q.put(cmd)

cmd_q = Queue()

root = Tk()
root.resizable(False, False)
app = App(root)

tasks = []
jobs = [
    "Printer",
]

current_task = Task()
current_job = jobs[0]

app.layout_refresh()
root.mainloop()
