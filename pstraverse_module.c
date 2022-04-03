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
#include<linux/ioctl.h>

dev_t dev = 0;
static int initD = 1;
#define RD_VALUE _IOR('a','b',int32_t*)
#define WR_VALUE _IOW('a','a',int32_t*)
static int pidin;
static int opti;
MODULE_PARM_DESC(pidin,"i");
MODULE_PARM_DESC(opti,"i2");
module_param(pidin,int,0);
module_param(opti,int,0);

int32_t newpid = 0;
int32_t newopt=1 ;
static int repeat = 0;

static int DFS(struct task_struct *taskInput);
static int BFS(struct task_struct *taskInput, int level);
static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg);
static void printBFS (void);

static struct class *dev_class;
static struct cdev my_cdev;
static char pidName [100][100];
static int levelArr[100];
static int size =0;
static int pidArr[100];
int maxLevel =0;

static struct file_operations fops = 
{
	.owner	= THIS_MODULE,
	.unlocked_ioctl=my_ioctl

};

static int DFS(struct task_struct *taskInput){	
	struct list_head *list;
	struct task_struct *taskStru;
	
	printk(KERN_INFO "PID: [%d] NAME: [%s]\n",taskInput->pid,taskInput->comm);
	list_for_each(list, &taskInput->children) {
	
		taskStru = list_entry(list, struct task_struct, sibling);
		DFS(taskStru);
	}
	
	return 0;

}
static int BFS(struct task_struct *taskInput, int level){	
	
	struct list_head *list;
	struct task_struct *taskStru;
	
	pidArr[size]=taskInput->pid;
	levelArr[size] = level;
	strcpy(pidName[size],taskInput->comm);
	size++;
	
	if(level > maxLevel) maxLevel=level;
	list_for_each(list, &taskInput->children) {
		taskStru = list_entry(list, struct task_struct, sibling);
		BFS(taskStru,level+1);
	}
	
	return 0;

}

static void printBFS (void){
	int i;
	int j;
	for(i=0; i<=maxLevel; i++){
		for (j=0; j<size; j++){
			if(levelArr[j]==i){
				printk(KERN_INFO "PID: [%d] NAME: [%s]\n",pidArr[j],pidName[j]);
			}
		}
	}
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
		BFS(taskParent,0);
		printBFS();
	}
	if(opti==1){
		DFS(taskParent);
	}
	return 0;


r_device: 
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
	}
	

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg){

switch(cmd){
/*case RD_VALUE:
	if(copy_to_user((int32_t *) arg,&pidin,sizeof(pidin)))
	printk(KERN_INFO "PRİNTED");
	break;
	*/
case WR_VALUE:

if(repeat ==  0){
	repeat ++;
      if(copy_from_user(&newpid ,(int32_t*) arg, sizeof(newpid)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
}
else if (repeat == 1){
	repeat = 0;
	if(copy_from_user(&newopt,  (int32_t*)arg, sizeof(newopt)))
	{
		pr_err("Data Write : Err!\n");
	}
	
                        	struct task_struct *taskParent;
				taskParent=pid_task(find_vpid(newpid),PIDTYPE_PID);
				printk(KERN_INFO "ROOTPID: [%d] NAME: [%s]\n",taskParent->pid,taskParent->comm);
                        	if(newopt==1){
				DFS(taskParent);
				
				}
				else if(newopt==0){
				
					BFS(taskParent,0);
					printBFS();
				
				}
}	
                        break;
}
	
return 0;
}

void __exit rmmodule(void) {
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device driver is removed successfully...\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AslihanBegum");
MODULE_DESCRIPTION("The module");
module_init(my_module_init);
module_exit(rmmodule);

