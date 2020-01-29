from tkinter import *
from tkinter.ttk import *
import matplotlib.pyplot as plt 

class Resource:
    def __init__(self):
        self.name = "Resource " + str(len(resources) + 1)
        self.delay = 0
        
    def __eq__(self, other):
        if not isinstance(other, Resource):
            return False
        return self.name == other.name

    def __ne__(self, other):
        return not self == other

    def __str__(self):
        return self.name

class Job:
    def __init__(self, name):
        self.name = name
        self.resources = []
        for i in range(len(resources)):
            self.resources.append(IntVar())
        
    def __eq__(self, other):
        if not isinstance(other, Job):
            return False
        return self.name == other.name

    def __ne__(self, other):
        return not self == other

    def __str__(self):
        return self.name

class Heuristic:
    def __init__(self, name):
        self.name = name
        self.order = -1
    
    def __eq__(self, other):
        if not isinstance(other, Job):
            return False
        return self.name == other.name

    def __ne__(self, other):
        return not self == other

    def __str__(self):
        return self.name

class Task:
    def __init__(self):
        self.name = "Task " + str(len(tasks) + 1)
        self.time_start = 0
        self.time_compute = 0
        self.time_deadline = 0
        self.job = None
        self.precedence = []
        self.added = False
        for i in range(len(tasks)):
            self.precedence.append(IntVar())

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
        self.master.title("Batch Editor")

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

        # Start time
        self.lbl_time_start = Label(self, text="Start time (ticks)")
        self.lbl_time_start.grid(row=2, column=0, padx=10, pady=10)
        self.ent_time_start = Entry(self)
        self.ent_time_start.grid(row=2, column=1, padx=10, pady=10)
 
        # Compute time
        self.lbl_time_compute = Label(self, text="Compute time (ticks)")
        self.lbl_time_compute.grid(row=3, column=0, padx=10, pady=10)
        self.ent_time_compute = Entry(self)
        self.ent_time_compute.grid(row=3, column=1, padx=10, pady=10)
        self.cmb_job.bind("<<ComboboxSelected>>", self.action_select_job)

        # Start time
        self.lbl_time_start = Label(self, text="Start time (ticks)")
        self.lbl_time_start.grid(row=2, column=0, padx=10, pady=10)
        self.ent_time_start = Entry(self)
        self.ent_time_start.grid(row=2, column=1, padx=10, pady=10)
 
        # Compute time
        self.lbl_time_compute = Label(self, text="Compute time (ticks)")
        self.lbl_time_compute.grid(row=3, column=0, padx=10, pady=10)
        self.ent_time_compute = Entry(self)
        self.ent_time_compute.grid(row=3, column=1, padx=10, pady=10)
  
        # Deadline time
        self.lbl_time_deadline = Label(self, text="Deadline time (ticks)")
        self.lbl_time_deadline.grid(row=4, column=0, padx=10, pady=10)
        self.ent_time_deadline = Entry(self)
        self.ent_time_deadline.grid(row=4, column=1, padx=10, pady=10)
        
        # Precendence
        if len(tasks) > 0:
            self.lbl_precedence = Label(self, text="Precendence")
            self.lbl_precedence.grid(row=5, column=0, columnspan=2, padx=10, pady=10)
        self.frm_precedence = Frame(self)
        self.frm_precedence.grid(row=6, column=0, columnspan=2)
        self.precedence_refresh()
 
        # Resources
        if len(resources) > 0:
            self.lbl_resources = Label(self, text="Resources")
            self.lbl_resources.grid(row=7, column=0, columnspan=2, padx=10, pady=10)
        self.frm_resources = Frame(self)
        self.frm_resources.grid(row=8, column=0, columnspan=2)
        self.resources_refresh()

        # Add task
        self.btn_add_task = Button(self, text="Add task", command=self.action_add_task)
        self.btn_add_task.grid(row=9, column=0, columnspan=2, padx=10, pady=10)

        # Resource name
        self.lbl_resource_name = Label(self, text="Resource name")
        self.lbl_resource_name.grid(row=10, column=0, padx=10, pady=10)
        self.ent_resource_name = Entry(self)
        self.ent_resource_name.grid(row=10, column=1, padx=10, pady=10)

        # Resource delay
        self.lbl_resource_delay = Label(self, text="Resource delay (ticks)")
        self.lbl_resource_delay.grid(row=11, column=0, padx=10, pady=10)
        self.ent_resource_delay = Entry(self)
        self.ent_resource_delay.grid(row=11, column=1, padx=10, pady=10)

        # Add resource
        self.btn_add_resource = Button(self, text="Add resource", command=self.action_add_resource)
        self.btn_add_resource.grid(row=12, column=0, columnspan=2, padx=10, pady=10)

        # Heuristic name
        self.lbl_heuristic_name = Label(self, text="Heuristic")
        self.lbl_heuristic_name.grid(row=13, column=0)
        self.cmb_heuristic_name = Combobox(self, values=heuristics)
        self.cmb_heuristic_name.grid(row=13, column=1, padx=10, pady=10)
        self.cmb_heuristic_name.current(heuristics.index(current_heuristic))
        self.cmb_heuristic_name.bind("<<ComboboxSelected>>", self.action_select_heuristic_name)

        # Heuristic order
        self.lbl_heuristic_order = Label(self, text="Heuristic order")
        self.lbl_heuristic_order.grid(row=14, column=0)
        self.cmb_heuristic_order = Combobox(self, values=list(range(len(heuristics))))
        self.cmb_heuristic_order.grid(row=14, column=1, padx=10, pady=10)
        self.cmb_heuristic_order.current(current_heuristic.order)
        self.cmb_heuristic_order.bind("<<ComboboxSelected>>", self.action_select_heuristic_order)

        # Path
        self.lbl_path = Label(self, text="Path")
        self.lbl_path.grid(row=15, column=0, padx=10, pady=10)
        self.ent_path = Entry(self)
        self.ent_path.grid(row=15, column=1, padx=10, pady=10)

        # Save batch
        self.btn_save = Button(self, text="Save batch", command=self.action_save)
        self.btn_save.grid(row=16, column=0, padx=10, pady=10)

        # Load report
        self.btn_load = Button(self, text="Load report", command=self.action_load)
        self.btn_load.grid(row=16, column=1, padx=10, pady=10)
        
        self.pack(fill=BOTH, expand=1)
   
    def precedence_refresh(self):
        if len(tasks) == 0:
            return
        i = -1
        for task in tasks:
            i += 1
            self.cbn = Checkbutton(self.frm_precedence, text=task.name, variable=current_task.precedence[i])
            self.cbn.grid(row=int(i/10), column=i%10)
 
    def resources_refresh(self):
        if len(resources) == 0:
            return
        i = -1
        for resource in resources:
            i += 1
            self.cbn = Checkbutton(self.frm_resources, text=resource.name, variable=current_job.resources[i])
            self.cbn.grid(row=int(i/10), column=i%10)
    
    def action_select_task(self, event):
        global current_task
        for task in tasks:
            if task.name == self.cmb_task.get():
                current_task = task
                self.layout_refresh()
                break

    def action_select_job(self, event):
        global current_job
        for job in jobs:
            if job.name == self.cmb_job.get():
                current_job = job
                self.layout_refresh()
                break

    def action_select_heuristic_name(self, event):
        global current_heuristic
        for heuristic in heuristics:
            if heuristic.name == self.cmb_heuristic_name.get():
                current_heuristic = heuristic
                current_heuristic.order = 0
                self.layout_refresh()
                break

    def action_select_heuristic_order(self, event):
        current_heuristic.order = int(self.cmb_heuristic_order.get())
        self.layout_refresh()

    def action_add_task(self):
        global current_task
        current_task.name = self.cmb_task.get()
        current_task.time_start = self.ent_time_start.get()
        current_task.time_compute = self.ent_time_compute.get()
        current_task.time_deadline = self.ent_time_deadline.get()
        current_task.added = True

        for job in jobs:
            if job.name == self.cmb_job.get():
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
            for task in tasks:
                task.precedence.append(IntVar())

        current_task = Task()
    
        self.layout_refresh()
    
    def action_add_resource(self):
        resource = Resource()
        resource.name = self.ent_resource_name.get()
        resource.delay = self.ent_resource_delay.get()

        i = -1
        found = False
        for other in resources:
            i += 1
            if other == resource:
                resources[i] = resource
                found = True
                break
        if not found:
            resources.append(resource)
            for job in jobs:
                job.resources.append(IntVar())
        
        self.layout_refresh()
    
    def action_save(self):
        fout = open(self.ent_path.get(), "w")
        # Resources
        fout.write(str(len(resources)) + "\r\n")
        for resource in resources:
            fout.write("{name},{delay}\r\n".format(
                name=resource.name,
                delay=resource.delay))
        # Jobs
        fout.write(str(len(jobs)) + "\r\n")
        for job in jobs:
            fout.write(job.name)
            for i in range(len(tasks)):
                if job.resources[i].get() == 1:
                    fout.write("," + str(i))
            fout.write("\r\n")
        # Heuristics
        heuristics_sorted = sorted(heuristics, key=lambda h: h.order)
        heuristics_sorted = [h for h in heuristics_sorted if h.order > 0]
        fout.write(str(len(heuristics_sorted)) + "\r\n")
        for heuristic in heuristics_sorted:
            if heuristic.order < 0:
                continue
            fout.write("{name},{order}\r\n".format(
                name=heuristic.name,
                order=heuristic.order))
        # Tasks
        fout.write(str(len(tasks)) + "\r\n")
        for task in tasks:
            fout.write("{name},{start},{compute},{deadline},{job}".format(
                name=task.name,
                start=task.time_start,
                compute=task.time_compute,
                deadline=task.time_deadline,
                job=jobs.index(task.job)))
            for i in range(len(tasks)):
                if task.precedence[i].get() == 1:
                    fout.write("," + str(i))
            fout.write("\r\n")
        fout.close()
    
    def action_load(self):
        table_header = ["Task", "Timestamp (ticks)"]
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

tasks = []
resources = []
jobs = [
    Job("PrintLetters"),
    Job("PrintNumbers"),
    Job("PrintSymbols")
]
heuristics = [
    Heuristic("FCFS"),
    Heuristic("SJF"),
    Heuristic("EDF"),
    Heuristic("ESTF"),
    Heuristic("EDFSJF"),
    Heuristic("EDFESTF"),
    Heuristic("E")
]

current_task = Task()
current_job = jobs[0]
current_heuristic = heuristics[0]
current_heuristic.order = 0

app.layout_refresh()
root.mainloop() 
