from tkinter import *
from tkinter.ttk import *
import matplotlib.pyplot as plt 

class Resource:
    def __init__(self):
        self.name = "Resource " + str(len(resources))
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
        self.name = "Task " + str(len(tasks))
        self.time_start = 0
        self.time_compute = 0
        self.time_deadline = 0
        self.job = None
        self.precedence = []
 
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
        self.layout_refresh()

    def layout_refresh(self):
        for widget in self.winfo_children():
    	    widget.destroy()

        # Task
        self.lbl_task = Label(self, text="Task")
        self.lbl_task.grid(row=0, column=0, padx=10, pady=10)
        self.cmb_task = Combobox(self, values=tasks)
        self.cmb_task.grid(row=0, column=1, padx=10, pady=10)
        if current_task >= 0:
            self.cmb_task.current(current_task)
        self.cmb_task.bind("<<ComboboxSelected>>", self.action_select_task)

        # Job
        self.lbl_job = Label(self, text="Job")
        self.lbl_job.grid(row=1, column=0)
        self.cmb_job = Combobox(self, values=jobs)
        self.cmb_job.grid(row=1, column=1, padx=10, pady=10)
        if current_job >= 0:
            self.cmb_job.current(current_job)
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

        # Path
        self.lbl_path = Label(self, text="Path")
        self.lbl_path.grid(row=13, column=0, padx=10, pady=10)
        self.ent_path = Entry(self)
        self.ent_path.grid(row=13, column=1, padx=10, pady=10)

        # Save batch
        self.btn_save = Button(self, text="Save batch", command=self.action_save)
        self.btn_save.grid(row=14, column=0, padx=10, pady=10)

        # Load report
        self.btn_load = Button(self, text="Load report", command=self.action_load)
        self.btn_load.grid(row=14, column=1, padx=10, pady=10)
        
        self.pack(fill=BOTH, expand=1)
   
    def precedence_refresh(self):
        i = 0
        for task in tasks:
            i += 1
            var = IntVar()
            self.cbn_test = Checkbutton(self.frm_precedence, text=task.name, variable=var)
            self.cbn_test.grid(row=int(i/11), column=i%11)
 
    def resources_refresh(self):
        i = 0
        for resource in resources:
            i += 1
            var = IntVar()
            self.cbn_test = Checkbutton(self.frm_resources, text=resource.name, variable=var)
            self.cbn_test.grid(row=int(i/11), column=i%11)

    def action_select_task(self, event):
        global current_task
        i = 0
        for task in tasks:
            i += 1
            if task.name == self.cmb_task.get():
                current_task = i - 1
                self.layout_refresh()
                break

    def action_select_job(self, event):
        global current_job
        i = 0
        for job in jobs:
            i += 1
            if job.name == self.cmb_job.get():
                current_job = i - 1
                self.layout_refresh()
                break

    def action_add_task(self):
        task = Task()
        task.name = self.cmb_task.get()
        task.job = self.cmb_job.get()
        
        found = None
        for other in tasks:
            if other == task:
                found = other
                break
        if found is not None:
            tasks.remove(found)
        tasks.append(task)
        
        self.layout_refresh()
    
    def action_add_resource(self):
        resource = Resource()
        resource.name = self.ent_resource_name.get()
        resource.delay = self.ent_resource_delay.get()
        
        found = None
        for other in resources:
            if other == resource:
                found = other
                break
        if found is not None:
            resources.remove(found)
        resources.append(resource)
        
        self.layout_refresh()
    
    def action_save(self):
        fout = open(self.ent_path.get(), "w")
        for task in tasks:
            fout.write('{},{}\r\n'.format(task.id, task.parents))
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

tasks = []
resources = []

jobs = [
    Job("Job 1"),
    Job("Job 2"),
    Job("Job 3")
]

current_task = -1
current_job = -1

root = Tk()
root.resizable(False, False)
app = App(root)
root.mainloop() 
