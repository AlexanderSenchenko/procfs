#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

// static int procfs_show(struct seq_file *, void *);
// static int procfs_open(struct inode *, struct  file *);
static int procfs_open(struct inode *, struct file *);
static int procfs_release(struct inode *, struct file *);
static ssize_t procfs_read(struct file *, char *, size_t, loff_t *);
static ssize_t procfs_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "procfs"
#define BUF_LEN 80

static int dev_open = 0;

static char msg[BUF_LEN];
static char *msg_ptr;
static int size_msg;

static char write_msg[BUF_LEN];

static const struct file_operations fops = {
  .owner    = THIS_MODULE,
  .open     = procfs_open,
  .read     = procfs_read,
  .write    = procfs_write,
  .release  = procfs_release
};

int __init procfs_init(void)
{
  int rv = 0;
  printk(KERN_INFO "Trying to create /proc/%s:\n", DEVICE_NAME);

  if (proc_create(DEVICE_NAME, 0666, NULL, &fops) == NULL) {
    rv = -ENOMEM;
    printk(KERN_INFO "Error: Could not initialize /proc/%s\n", DEVICE_NAME);
  } else {
    printk(KERN_INFO "Success!\n");
  }

  return rv;
}

void __exit procfs_exit(void)
{
  remove_proc_entry(DEVICE_NAME, NULL);
  printk(KERN_INFO "/proc/%s removed\n", DEVICE_NAME);
}

#if 1
static int procfs_open(struct inode *inode, struct  file *file)
{
  static int counter = 0;
  if (dev_open)
    return -EBUSY;
  
  dev_open++;
  size_msg = sprintf(msg, "I already told you %d times Hello world!\n%c", counter++, '\0');
  msg_ptr = msg;

  try_module_get(THIS_MODULE);

  return SUCCESS;
}
#else
static int procfs_show(struct seq_file *m, void *v)
{
  seq_printf(m, "Hello proc!\n");
  return 0;
}

static int procfs_open(struct inode *inode, struct  file *file)
{
  return single_open(file, procfs_show, NULL);
}
#endif

static int procfs_release(struct inode *inode, struct file *file)
{
  dev_open--;

  module_put(THIS_MODULE); 

  return 0;
}

static ssize_t procfs_read(struct file *filp,
                           char __user *buf, size_t count,
                           loff_t *ppos)
{
  return simple_read_from_buffer(buf, count, ppos, msg_ptr, size_msg);
}

static ssize_t procfs_write(struct file *filp,
                            const char __user *buf, size_t len,
                            loff_t *ppos)
{
  ssize_t ret;

  ret = simple_write_to_buffer(write_msg, BUF_LEN, ppos, buf, len);
  if (ret < 0)
    return ret;

  if (ret < BUF_LEN)
    write_msg[ret] = '\0';

  printk(KERN_ALERT "msg: %s\n", write_msg);

  return ret;
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");