#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/sched.h>
#include<linux/pid.h>
//dev_t dev = 0;
static int pidin;
static int opti;
MODULE_PARM_DESC(pidin,"i");
MODULE_PARM_DESC(opti,"i2");
module_param(pidin,int,0);
module_param(opti,int,0);
//static struct class *dev_class;
//static struct cdev my_cdev;
static int DFS(struct task_struct *taskInput);
static int BFS(struct task_struct *taskInput);
//void DFS(struct task_struct *taskInput);
//static struct file_operations fops = 
//{
//	.owner	= THIS_MODULE,
//};
static int DFS(struct task_struct *taskInput){	
struct list_head *list;
list_for_each(list, &taskInput->children) {
struct task_struct *taskStru;
printk(KERN_INFO"here");
   taskStru = list_entry(list, struct task_struct, sibling);
   printk(KERN_INFO "PID: [%d] NAME: [%s]",taskStru->pid,taskStru->comm);
   DFS(taskStru);
}

return 0;
}
static int BFS(struct task_struct *taskInput){	
struct list_head *list;
list_for_each(list, &taskInput->children) {
struct task_struct *taskStru;
printk(KERN_INFO"here");
   taskStru = list_entry(list, struct task_struct, sibling);
   BFS(taskStru);
   printk(KERN_INFO "PID: [%d] NAME: [%s]",taskStru->pid,taskStru->comm);
}

return 0;
}

static int __init my_module_init(void){
	struct task_struct *taskParent;
	printk(KERN_INFO"Loading \n");
	taskParent=pid_task(find_vpid(pidin),PIDTYPE_PID);
	if(opti==0){
	BFS(taskParent);}
		if(opti==1){
	DFS(taskParent);}
	return 0;
}

void __exit rmmodule(void) {
	printk(KERN_INFO "Device driver is removed successfully...\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AslihanBegum");
MODULE_DESCRIPTION("The module");
module_init(my_module_init);
module_exit(rmmodule);

