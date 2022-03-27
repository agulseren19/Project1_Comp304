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

static struct class *dev_class;
static struct cdev my_cdev;

static struct file_operations fops = 
{
	.owner	= THIS_MODULE,
	.unlocked_ioctl=my_ioctl

};

static int DFS(struct task_struct *taskInput){	
	struct list_head *list;
	list_for_each(list, &taskInput->children) {
		struct task_struct *taskStru;
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
		taskStru = list_entry(list, struct task_struct, sibling);
		BFS(taskStru);
		printk(KERN_INFO "PID: [%d] NAME: [%s]",taskStru->pid,taskStru->comm);
	}

	return 0;
}

static int __init my_module_init(void){
	if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) < 0) {
		printk(KERN_INFO"Cannot allocate the major number...\n");
	}

	printk(KERN_INFO"Major = %d Minor =  %d..\n", MAJOR(dev),MINOR(dev));

	/* creating cdev structure*/
	cdev_init(&my_cdev, &fops);

	/* adding character device to the system */
	if((cdev_add(&my_cdev, dev, 1)) < 0) {
		goto r_class;
	}	 

	/* creating struct class */
	if((dev_class =  class_create(THIS_MODULE, "my_class")) == NULL) {
		goto r_class;
	}

	/* creating device */

	if((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL) {
		goto r_device;
	}

	struct task_struct *taskParent;
	printk(KERN_INFO"Loading \n");
	taskParent=pid_task(find_vpid(pidin),PIDTYPE_PID);
	printk(KERN_INFO "ROOTPID: [%d] NAME: [%s]",taskParent->pid,taskParent->comm);
	if(opti==0){
		BFS(taskParent);}
	if(opti==1){
		DFS(taskParent);}
	return 0;
}
static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg){
switch(cmd){
case RD_VALUE:
	if(copy_to_user((int32_t *) arg,&pidin,sizeof(pidin)))
}

void __exit rmmodule(void) {
	printk(KERN_INFO "Device driver is removed successfully...\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AslihanBegum");
MODULE_DESCRIPTION("The module");
module_init(my_module_init);
module_exit(rmmodule);

